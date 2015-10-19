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
#include "private/debughelper.h"
#include "private/repositoryprocesscallback.h"
#include "private/twitterqueryutil.h"
#include "listqueryhandlerfactory.h"
#include <QtCore/QLoggingCategory>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtNetwork/QNetworkReply>

TweetRepositoryContainer::TweetRepositoryContainer(IQueryExecutor::ConstPtr &&queryExecutor)
    : m_queryExecutor(std::move(queryExecutor))
{
    Q_ASSERT_X(m_queryExecutor, "TweetRepositoryContainer", "NULL query executor");
}

TweetRepository * TweetRepositoryContainer::repository(const Account &account, const Query &query)
{
    auto it = m_mapping.find(MappingKey{account, query});
    if (it == std::end(m_mapping)) {
        return nullptr;
    }
    return &(it->second.repository);
}

void TweetRepositoryContainer::referenceQuery(const Account &account, const Query &query)
{
    MappingData *data {getMappingData(account, query)};
    if (data == nullptr) {
        return;
    }
    ++data->refcount;

    for (const auto &it : m_mapping) {
        qCDebug(QLoggingCategory("tweet-central-repository")) << "For query" << it.first.query.path()
                                                              << "Refcount:" << it.second.refcount;
    }
}

void TweetRepositoryContainer::dereferenceQuery(const Account &account, const Query &query)
{
    MappingData *data {getMappingData(account, query)};
    if (data == nullptr) {
        return;
    }
    --data->refcount;
    if (data->refcount == 0) {
        m_mapping.erase(MappingKey{account, query});
    }

    for (const auto &it : m_mapping) {
        qCDebug(QLoggingCategory("tweet-central-repository")) << "For query" << it.first.query.path()
                                                              << "Refcount:" << it.second.refcount;
    }
}

std::set<Query> TweetRepositoryContainer::referencedQueries(const Account &account) const
{
    std::set<Query> returned;
    for (auto it = std::begin(m_mapping); it != std::end(m_mapping); ++it) {
        if (it->first.account.userId() == account.userId()) {
            returned.insert(it->first.query);
        }
    }
    return returned;
}

void TweetRepositoryContainer::refresh()
{
    for (auto it = std::begin(m_mapping); it != std::end(m_mapping); ++it) {
        load(it->first, it->second, IListQueryHandler<Tweet>::Refresh);
    }
}

void TweetRepositoryContainer::refresh(const Account &account, const Query &query)
{
    auto it = m_mapping.find(MappingKey{account, query});
    if (it != std::end(m_mapping)) {
        load(it->first, it->second, IListQueryHandler<Tweet>::Refresh);
    }
}

void TweetRepositoryContainer::loadMore(const Account &account, const Query &query)
{
    auto it = m_mapping.find(MappingKey{account, query});
    if (it != std::end(m_mapping)) {
        load(it->first, it->second, IListQueryHandler<Tweet>::LoadMore);
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

void TweetRepositoryContainer::load(const MappingKey &key, MappingData &mappingData,
                                  IListQueryHandler<Tweet>::RequestType requestType)
{
    if (mappingData.loading) {
        return;
    }

    mappingData.loading = true;

    QByteArray path {key.query.path()};
    Query::Parameters parameters (key.query.parameters());
    const Query::Parameters &additionalParameters (mappingData.handler->additionalParameters(requestType));
    parameters.insert(std::begin(additionalParameters), std::end(additionalParameters));

    qCDebug(QLoggingCategory("tweet-central-repository")) << "Request:" << path << parameters;
    mappingData.repository.start();

    m_queryExecutor->execute(path, parameters, key.account, [this, &mappingData, requestType](QIODevice &reply, QNetworkReply::NetworkError error, const QString &errorMessage) {
        std::vector<Tweet> items {};
        private_util::RepositoryProcessCallback<Tweet> callback {
            requestType,
            *mappingData.handler,
            mappingData.repository,
            items,
            mappingData.loading
        };
        callback(reply, error, errorMessage);
        for (const Tweet &tweet : items) {
            m_data.emplace(tweet.id(), tweet);
            qCDebug(QLoggingCategory("tweet-central-repository")) << "Adding tweet with id" << tweet.id();
        }
    });
}

TweetRepositoryContainer::MappingData * TweetRepositoryContainer::getMappingData(const Account &account, const Query &query)
{
    auto it = m_mapping.find(MappingKey(account, query));
    if (it != std::end(m_mapping)) {
        return &(it->second);
    }

    IListQueryHandler<Tweet>::Ptr handler {ListQueryHandlerFactory::createTweet(query)};
    if (!handler) {
        return nullptr;
    }
    return &(m_mapping.emplace(MappingKey{account, query}, MappingData{std::move(handler)}).first->second);
}

bool TweetRepositoryContainer::MappingKeyComparator::operator()(const MappingKey &first, const MappingKey &second) const
{
    return first.account.userId() == second.account.userId() ? (first.query < second.query)
                                                             : (first.account.userId() < second.account.userId());
}

TweetRepositoryContainer::MappingData::MappingData(IListQueryHandler<Tweet>::Ptr &&inputHandler)
    : handler(std::move(inputHandler))
{
}

TweetRepositoryContainer::MappingKey::MappingKey(const Account &inputAccount, const Query &inputQuery)
    : account(std::move(inputAccount)), query(std::move(inputQuery))
{
}
