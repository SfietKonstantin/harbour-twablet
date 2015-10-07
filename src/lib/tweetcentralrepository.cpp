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
#include <QtCore/QLoggingCategory>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtNetwork/QNetworkReply>

TweetCentralRepository::TweetCentralRepository()
    : m_network{new QNetworkAccessManager()}
{
}

TweetRepository & TweetCentralRepository::repository(const Account &account, const Query &query)
{
    return m_mapping.at(MappingKey{account, query}).repository;
}

void TweetCentralRepository::refQuery(const Account &account, const Query &query)
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

void TweetCentralRepository::derefQuery(const Account &account, const Query &query)
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

void TweetCentralRepository::refresh()
{
    for (auto it = std::begin(m_mapping); it != std::end(m_mapping); ++it) {
        load(it->first, it->second, IQueryHandler<Tweet>::Refresh);
    }
}

void TweetCentralRepository::refresh(const Account &account, const Query &query)
{
    auto it = m_mapping.find(MappingKey{account, query});
    if (it != std::end(m_mapping)) {
        load(it->first, it->second, IQueryHandler<Tweet>::Refresh);
    }
}

void TweetCentralRepository::loadMore(const Account &account, const Query &query)
{
    auto it = m_mapping.find(MappingKey{account, query});
    if (it != std::end(m_mapping)) {
        load(it->first, it->second, IQueryHandler<Tweet>::LoadMore);
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
                                  IQueryHandler<Tweet>::RequestType requestType)
{
    class LoadingLock
    {
    public:
        explicit LoadingLock(MappingData &mappingData)
            : m_mappingData(mappingData)
        {
        }
        ~LoadingLock()
        {
            m_mappingData.loading = false;
        }
    private:
        MappingData &m_mappingData;
    };

    if (mappingData.loading) {
        return;
    }

    mappingData.loading = true;

    QString path {};
    std::map<QByteArray, QByteArray> parameters {};
    mappingData.handler->createRequest(requestType, path, parameters);
    qCDebug(QLoggingCategory("tweet-central-repository")) << "Request:" << path << parameters;

    QNetworkReply *reply {private_util::TwitterQueryUtil::get(*m_network, path, parameters, key.account)};
    mappingData.repository.start();

    QObject::connect(reply, &QNetworkReply::finished, [this, &key, &mappingData, reply, requestType]() {
        QObjectPtr<QNetworkReply> replyPtr {reply};
        LoadingLock loadingLock {mappingData};

        if (replyPtr->error() != QNetworkReply::NoError) {
            qCWarning(QLoggingCategory("tweet-central-repository")) << "Error happened during request for layout" << key.account.userId() << key.query.type();
            qCWarning(QLoggingCategory("tweet-central-repository")) << "Error code:" << reply->error();
            qCWarning(QLoggingCategory("tweet-central-repository")) << "Error message (Qt):" << reply->errorString();
            const QByteArray &data {reply->readAll()};
            qCWarning(QLoggingCategory("tweet-central-repository")) << "Error message (Twitter):" << data;

            // Check if Twitter sent us an issue
            QJsonDocument document {QJsonDocument::fromJson(data)};
            if (document.isObject()) {
                const QJsonObject &object {document.object()};
                const QJsonArray &array {object.value(QLatin1String("errors")).toArray()};
                if (array.count() == 1) {
                    const QJsonObject &firstError {array.first().toObject()};
                    if (firstError.value(QLatin1String("code")).toInt() == 88) {
                        mappingData.repository.error(QObject::tr("Twitter rate limit exceeded. Please try again later."));
                        return;
                    }
                }
            }

            mappingData.repository.error(QObject::tr("Network error. Please try again later."));
            return;
        }

        std::vector<Tweet> items {};
        QString errorMessage {};
        IQueryHandler<Tweet>::Placement placement {IQueryHandler<Tweet>::Discard};
        bool returned = mappingData.handler->treatReply(requestType, reply->readAll(), items, errorMessage, placement);
        if (!returned) {
            qCWarning(QLoggingCategory("tweet-central-repository")) << "Error happened during request for layout" << key.account.userId() << key.query.type();
            qCWarning(QLoggingCategory("tweet-central-repository")) << "Parsing error: " << errorMessage;
            mappingData.repository.error(QObject::tr("Internal error"));
            return;
        } else {
            for (const Tweet &tweet : items) {
                m_data.emplace(tweet.id(), tweet);
                qCDebug(QLoggingCategory("tweet-central-repository")) << "Adding tweet with id" << tweet.id();
            }
            qCDebug(QLoggingCategory("tweet-central-repository")) << "New data available for layout" << key.account.userId() << key.query.type() << ". Count:" << items.size();
            switch (placement) {
            case IQueryHandler<Tweet>::Append:
                mappingData.repository.append(items);
                break;
            case IQueryHandler<Tweet>::Prepend:
                mappingData.repository.prepend(items);
                break;
            case IQueryHandler<Tweet>::Discard:
                break;
            }
            mappingData.repository.finish();
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
        std::unique_ptr<IQueryHandler<Tweet>> handler {new HomeTimelineQueryHandler()};
        return &(m_mapping.emplace(MappingKey{account, query}, MappingData{std::move(handler)}).first->second);
        break;
    }
    case Query::Mentions:
    {
        std::unique_ptr<IQueryHandler<Tweet>> handler {new MentionsTimelineQueryHandler()};
        return &(m_mapping.emplace(MappingKey{account, query}, MappingData{std::move(handler)}).first->second);
        break;
    }
    case Query::Search:
    {
        std::unique_ptr<IQueryHandler<Tweet>> handler {new SearchQueryHandler(query.parameters())};
        return &(m_mapping.emplace(MappingKey{account, query}, MappingData{std::move(handler)}).first->second);
        break;
    }
    case Query::Favorites:
    {
        std::unique_ptr<IQueryHandler<Tweet>> handler {new FavoritesQueryHandler(query.parameters())};
        return &(m_mapping.emplace(MappingKey{account, query}, MappingData{std::move(handler)}).first->second);
        break;
    }
    case Query::UserTimeline:
    {
        std::unique_ptr<IQueryHandler<Tweet>> handler {new UserTimelineQueryHandler(query.parameters())};
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
    if (first.account.userId() < second.account.userId()) {
        return true;
    }

    if (first.account.userId() > second.account.userId()) {
        return false;
    }

    if (first.query.type() < second.query.type()) {
        return true;
    }
    if (first.query.type() > second.query.type()) {
        return false;
    }
    return first.query.parameters() < second.query.parameters();
}

TweetCentralRepository::MappingData::MappingData(std::unique_ptr<IQueryHandler<Tweet>> &&inputHandler)
    : handler(std::move(inputHandler))
{
}

TweetCentralRepository::MappingKey::MappingKey(const Account &inputAccount, const Query &inputQuery)
    : account(std::move(inputAccount)), query(std::move(inputQuery))
{
}
