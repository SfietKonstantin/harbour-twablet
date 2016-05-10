/*
 * Copyright (C) 2014 Lucien XU <sfietkonstantin@free.fr>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * "Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * The names of its contributors may not be used to endorse or promote
 *     products derived from this software without specific prior written
 *     permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 */

#include "tweetrepositorycontainer.h"
#include "private/accountutil.h"
#include "private/debughelper.h"
#include "private/repositoryquerycallback.h"
#include "private/twitterqueryutil.h"
#include "repositoryqueryhandlerfactory.h"
#include <QtCore/QLoggingCategory>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtNetwork/QNetworkReply>

static const QLoggingCategory logger {"tweet-repository-container"};

TweetRepositoryContainer::TweetRepositoryContainer(IQueryExecutor::ConstPtr &&queryExecutor)
    : m_queryExecutor(std::move(queryExecutor))
{
    Q_ASSERT_X(m_queryExecutor, "TweetRepositoryContainer", "NULL query executor");
}

TweetRepository * TweetRepositoryContainer::repository(const Account &account, const Query &query)
{
    auto it = m_mapping.find(ContainerKey{Account{account}, Query{query}});
    if (it == std::end(m_mapping)) {
        return nullptr;
    }
    return &(it->second.repository);
}

void TweetRepositoryContainer::referenceQuery(const Account &account, const Query &query)
{
    Data *data {getMappingData(ContainerKey{Account{account}, Query{query}})};
    if (data == nullptr) {
        return;
    }
    ++data->refcount;

    qCDebug(logger) << "Refcount info:";
    for (const auto &it : m_mapping) {
        qCDebug(logger) << "  For query" << it.first.query() << "Refcount:" << it.second.refcount
                     << "Observers:" << it.second.repository.listeners().size();
    }
}

void TweetRepositoryContainer::dereferenceQuery(const Account &account, const Query &query)
{
    Data *data {getMappingData(ContainerKey{Account{account}, Query{query}})};
    if (data == nullptr) {
        return;
    }
    --data->refcount;
    if (data->refcount == 0) {
        m_mapping.erase(ContainerKey{Account{account}, Query{query}});
    }

    qCDebug(logger) << "Refcount info:";
    for (const auto &it : m_mapping) {
        qCDebug(logger) << "  For query" << it.first.query() << "Refcount:" << it.second.refcount;
    }
}

std::set<Query> TweetRepositoryContainer::referencedQueries(const Account &account) const
{
    std::set<Query> returned;
    for (auto it = std::begin(m_mapping); it != std::end(m_mapping); ++it) {
        if (it->first.account().userId() == account.userId()) {
            returned.insert(it->first.query());
        }
    }
    return returned;
}

void TweetRepositoryContainer::refresh()
{
    for (auto it = std::begin(m_mapping); it != std::end(m_mapping); ++it) {
        load(it->first, it->second, IRepositoryQueryHandler<Tweet>::Refresh);
    }
}

void TweetRepositoryContainer::refresh(const Account &account, const Query &query)
{
    auto it = m_mapping.find(ContainerKey{Account{account}, Query{query}});
    if (it != std::end(m_mapping)) {
        load(it->first, it->second, IRepositoryQueryHandler<Tweet>::Refresh);
    } else {
        qCWarning(logger) << "refresh: cannot perform load";
        qCWarning(logger) << "  Account:" << account.userId();
        qCWarning(logger) << "  Query:" << query;
    }
}

void TweetRepositoryContainer::loadMore(const Account &account, const Query &query)
{
    auto it = m_mapping.find(ContainerKey{Account{account}, Query{query}});
    if (it != std::end(m_mapping)) {
        load(it->first, it->second, IRepositoryQueryHandler<Tweet>::LoadMore);
    } else {
        qCWarning(logger) << "loadmore: cannot perform load";
        qCWarning(logger) << "  Account:" << account.userId();
        qCWarning(logger) << "  Query:" << query;
    }
}

Tweet TweetRepositoryContainer::tweet(const QString &id) const
{
    auto it = m_data.find(id);
    if (it != std::end(m_data)) {
        return it->second;
    }
    return Tweet();
}

void TweetRepositoryContainer::updateTweet(const Tweet &tweet)
{
    auto it = m_data.find(tweet.id());
    if (it != std::end(m_data)) {
        it->second = tweet;
    }

    for (auto it = std::begin(m_mapping); it != std::end(m_mapping); ++it) {
        TweetRepository &repository = it->second.repository;
        for (int i = 0; i < repository.size(); ++i) {
            const Tweet &currentTweet {*(std::begin(repository) + i)};
            if (currentTweet.id() == tweet.id()) {
                repository.update(i, std::move(Tweet(tweet)));
            }
        }
    }
}

void TweetRepositoryContainer::load(const ContainerKey &key, Data &mappingData,
                                  IRepositoryQueryHandler<Tweet>::RequestType requestType)
{
    if (mappingData.loading) {
        qCDebug(logger) << "Already loading:" << key;
        return;
    }

    qCDebug(logger) << "Load:" << key;
    qCDebug(logger) << "Refcount info:";
    for (const auto &it : m_mapping) {
        qCDebug(logger) << "  For query" << it.first.query() << "Refcount:" << it.second.refcount
                        << "Observers:" << it.second.repository.listeners().size();
    }

    mappingData.loading = true;

    QByteArray path {key.query().path()};
    Query::Parameters parameters (key.query().parameters());
    const Query::Parameters &additionalParameters (mappingData.handler->additionalParameters(requestType));
    parameters.insert(std::begin(additionalParameters), std::end(additionalParameters));

    qCDebug(logger) << "Request:" << path << parameters;
    mappingData.repository.start();

    m_queryExecutor->execute(key.query().requestType(), path, parameters, key.account(), [this, &mappingData, requestType](QIODevice &reply, QNetworkReply::NetworkError error, const QString &errorMessage) {
        std::vector<Tweet> items {};
        private_util::RepositoryQueryCallback<Tweet> callback {
            requestType,
            *mappingData.handler,
            mappingData.repository,
            items,
            mappingData.loading
        };
        callback(reply, error, errorMessage);
        for (const Tweet &tweet : items) {
            m_data.emplace(tweet.id(), tweet);
            qCDebug(logger) << "Adding tweet with id" << tweet.id();
        }
    });
}

TweetRepositoryContainer::Data * TweetRepositoryContainer::getMappingData(const ContainerKey &key)
{
    auto it = m_mapping.find(key);
    if (it != std::end(m_mapping)) {
        return &(it->second);
    }

    if (!private_util::isAccountValid(key.account())) {
        return nullptr;
    }

    IRepositoryQueryHandler<Tweet>::Ptr handler {RepositoryQueryHandlerFactory::createTweet(key.query())};
    if (!handler) {
        return nullptr;
    }
    return &(m_mapping.emplace(key, Data{std::move(handler)}).first->second);
}

TweetRepositoryContainer::Data::Data(IRepositoryQueryHandler<Tweet>::Ptr &&inputHandler)
    : handler(std::move(inputHandler))
{
}
