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

void TweetCentralRepository::query(const Account &account, const Query &query,
                                   TweetRepository &repository)
{
    // Create the request
    IQueryHandler * handler = getQueryHandler(query);
    if (handler == nullptr) {
        return;
    }

    QString path {};
    std::map<QByteArray, QByteArray> parameters {};
    handler->createRequest(path, parameters);
    QNetworkReply *reply {TwitterQueryUtil::get(*m_network, path, parameters, account)};
    repository.start();

    connect(reply, &QNetworkReply::finished, this, [this, account, query, handler, reply, &repository]() {
        QObjectPtr<QNetworkReply> replyPtr {reply};

        if (replyPtr->error() != QNetworkReply::NoError) {
            qCWarning(QLoggingCategory("tweet-central-repository")) << "Error happened during request for layout" << account.userId() << query.type();
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
                        repository.error(tr("Twitter rate limit exceeded. Please try again later."));
                        return;
                    }
                }
            }

            repository.error(tr("Network error. Please try again later."));
            return;
        }

        std::vector<Tweet> items {};
        QString errorMessage {};
        IQueryHandler::Placement placement {IQueryHandler::Discard};
        if (!handler->treatReply(reply->readAll(), items, errorMessage, placement)) {
            qCWarning(QLoggingCategory("tweet-central-repository")) << "Error happened during request for layout" << account.userId() << query.type();
            qCWarning(QLoggingCategory("tweet-central-repository")) << "Parsing error: " << errorMessage;
            repository.error(tr("Internal error"));
            return;
        } else {
            for (const Tweet &tweet : items) {
                m_data.emplace(tweet.id(), tweet);
            }
            qDebug(QLoggingCategory("tweet-central-repository")) << "New data available for layout" << account.userId() << query.type() << ". Count:" << items.size();
            switch (placement) {
            case IQueryHandler::Append:
                repository.append(items);
                break;
            case IQueryHandler::Prepend:
                repository.prepend(items);
                break;
            case IQueryHandler::Discard:
                break;
            }
            repository.finish();
        }
    });
}

IQueryHandler * TweetCentralRepository::getQueryHandler(const Query &query)
{
    auto it = m_queries.find(query);
    if (it != std::end(m_queries)) {
        return it->second.get();
    }

    switch (query.type()) {
    case Query::Home:
    {
        std::unique_ptr<IQueryHandler> handler {new HomeTimelineQueryHandler()};
        return m_queries.emplace(query, std::move(handler)).first->second.get();
        break;
    }
    case Query::Mentions:
    {
        std::unique_ptr<IQueryHandler> handler {new MentionsTimelineQueryHandler()};
        return m_queries.emplace(query, std::move(handler)).first->second.get();
        break;
    }
    case Query::Search:
    {
        std::unique_ptr<IQueryHandler> handler {new SearchQueryHandler(query.arguments())};
        return m_queries.emplace(query, std::move(handler)).first->second.get();
        break;
    }
    case Query::Favorites:
    {
        std::unique_ptr<IQueryHandler> handler {new FavoritesQueryHandler(query.arguments())};
        return m_queries.emplace(query, std::move(handler)).first->second.get();
        break;
    }
    case Query::UserTimeline:
    {
        std::unique_ptr<IQueryHandler> handler {new UserTimelineQueryHandler(query.arguments())};
        return m_queries.emplace(query, std::move(handler)).first->second.get();
        break;
    }
    default:
        return nullptr;
        break;
    }
}


bool TweetCentralRepository::QueryComparator::operator()(const Query &first,
                                                                       const Query &second) const
{
    if (first.type() < second.type()) {
        return true;
    }
    if (first.type() > second.type()) {
        return false;
    }
    return first.arguments() < second.arguments();
}
