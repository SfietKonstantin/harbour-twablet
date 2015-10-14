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

#include "tweetcentralrepository.h"
#include "hometimelinequeryhandler.h"
#include "mentionstimelinequeryhandler.h"
#include "searchqueryhandler.h"
#include "favoritesqueryhandler.h"
#include "usertimelinequeryhandler.h"
#include "private/twitterqueryutil.h"
#include "private/repositoryprocesscallback.h"
#include <QtCore/QLoggingCategory>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtNetwork/QNetworkReply>

TweetCentralRepository::TweetCentralRepository(IQueryExecutor::Ptr queryExecutor)
    : m_queryExecutor(std::move(queryExecutor))
{
    Q_ASSERT_X(m_queryExecutor, "TweetCentralRepository", "NULL query executor");
}

TweetRepository * TweetCentralRepository::repository(const Account &account, const Query &query)
{
    auto it = m_mapping.find(MappingKey{account, query});
    if (it == std::end(m_mapping)) {
        return nullptr;
    }
    return &(it->second.repository);
}

void TweetCentralRepository::referenceQuery(const Account &account, const Query &query)
{
    MappingData *data {getMappingData(account, query)};
    if (data == nullptr) {
        return;
    }
    ++data->refcount;

    for (const auto &it : m_mapping) {
        qCDebug(QLoggingCategory("tweet-central-repository")) << "For query" << it.first.query.type()
                                                              << "Refcount:" << it.second.refcount;
    }
}

void TweetCentralRepository::dereferenceQuery(const Account &account, const Query &query)
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
        qCDebug(QLoggingCategory("tweet-central-repository")) << "For query" << it.first.query.type()
                                                              << "Refcount:" << it.second.refcount;
    }
}

std::set<Query> TweetCentralRepository::referencedQueries(const Account &account) const
{
    std::set<Query> returned;
    for (auto it = std::begin(m_mapping); it != std::end(m_mapping); ++it) {
        if (it->first.account.userId() == account.userId()) {
            returned.insert(it->first.query);
        }
    }
    return returned;
}

void TweetCentralRepository::refresh()
{
    for (auto it = std::begin(m_mapping); it != std::end(m_mapping); ++it) {
        load(it->first, it->second, IListQueryHandler<Tweet>::Refresh);
    }
}

void TweetCentralRepository::refresh(const Account &account, const Query &query)
{
    auto it = m_mapping.find(MappingKey{account, query});
    if (it != std::end(m_mapping)) {
        load(it->first, it->second, IListQueryHandler<Tweet>::Refresh);
    }
}

void TweetCentralRepository::loadMore(const Account &account, const Query &query)
{
    auto it = m_mapping.find(MappingKey{account, query});
    if (it != std::end(m_mapping)) {
        load(it->first, it->second, IListQueryHandler<Tweet>::LoadMore);
    }
}

Tweet TweetCentralRepository::tweet(const QString &id) const
{
    auto it = m_data.find(id);
    if (it != std::end(m_data)) {
        return it->second;
    }
    return Tweet();
}

void TweetCentralRepository::updateTweet(const Tweet &tweet)
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

void TweetCentralRepository::load(const MappingKey &key, MappingData &mappingData,
                                  IListQueryHandler<Tweet>::RequestType requestType)
{
    if (mappingData.loading) {
        return;
    }

    mappingData.loading = true;

    QString path {};
    std::map<QByteArray, QByteArray> parameters {};
    mappingData.handler->createRequest(requestType, path, parameters);
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

TweetCentralRepository::MappingData * TweetCentralRepository::getMappingData(const Account &account, const Query &query)
{
    auto it = m_mapping.find(MappingKey(account, query));
    if (it != std::end(m_mapping)) {
        return &(it->second);
    }

    switch (query.type()) {
    case Query::Home:
    {
        std::unique_ptr<IListQueryHandler<Tweet>> handler {new HomeTimelineQueryHandler()};
        return &(m_mapping.emplace(MappingKey{account, query}, MappingData{std::move(handler)}).first->second);
        break;
    }
    case Query::Mentions:
    {
        std::unique_ptr<IListQueryHandler<Tweet>> handler {new MentionsTimelineQueryHandler()};
        return &(m_mapping.emplace(MappingKey{account, query}, MappingData{std::move(handler)}).first->second);
        break;
    }
    case Query::Search:
    {
        std::unique_ptr<IListQueryHandler<Tweet>> handler {new SearchQueryHandler(query.parameters())};
        return &(m_mapping.emplace(MappingKey{account, query}, MappingData{std::move(handler)}).first->second);
        break;
    }
    case Query::Favorites:
    {
        std::unique_ptr<IListQueryHandler<Tweet>> handler {new FavoritesQueryHandler(query.parameters())};
        return &(m_mapping.emplace(MappingKey{account, query}, MappingData{std::move(handler)}).first->second);
        break;
    }
    case Query::UserTimeline:
    {
        std::unique_ptr<IListQueryHandler<Tweet>> handler {new UserTimelineQueryHandler(query.parameters())};
        return &(m_mapping.emplace(MappingKey{account, query}, MappingData{std::move(handler)}).first->second);
        break;
    }
    default:
        qCWarning(QLoggingCategory("tweet-central-repository")) << "Unhandled type" << query.type();
        return nullptr;
        break;
    }
}

bool TweetCentralRepository::MappingKeyComparator::operator()(const MappingKey &first, const MappingKey &second) const
{
    return first.account.userId() == second.account.userId() ? (first.query < second.query)
                                                             : (first.account.userId() < second.account.userId());
}

TweetCentralRepository::MappingData::MappingData(std::unique_ptr<IListQueryHandler<Tweet>> &&inputHandler)
    : handler(std::move(inputHandler))
{
}

TweetCentralRepository::MappingKey::MappingKey(const Account &inputAccount, const Query &inputQuery)
    : account(std::move(inputAccount)), query(std::move(inputQuery))
{
}
