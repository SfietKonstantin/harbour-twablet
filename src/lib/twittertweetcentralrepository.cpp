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

#include "twittertweetcentralrepository.h"
#include "itwitterqueryhandler.h"
#include "twittertimelinequeryhandler.h"
#include "private/twitterqueryutil.h"
#include <QtCore/QLoggingCategory>
#include <QtNetwork/QNetworkReply>

TwitterTweetCentralRepository::TwitterTweetCentralRepository()
    : m_network{new QNetworkAccessManager()}
{
}

void TwitterTweetCentralRepository::query(const QString &layoutName, const TwitterUser &user,
                                          const TwitterQuery &query,
                                          TwitterTweetRepository &repository)
{
    // Create the request
    ITwitterQueryHandler * handler = createQueryHandler(query);
    if (handler == nullptr) {
        return;
    }

    QString path {};
    std::map<QString, QString> parameters {};
    handler->createRequest(path, parameters);
    QNetworkReply *reply = m_network->get(TwitterQueryUtil::createGetRequest(path, parameters, user));
    repository.start();

    connect(reply, &QNetworkReply::finished, this, [this, layoutName, handler, reply, &repository]() {
        QObjectPtr<QNetworkReply> replyPtr {reply};
        std::unique_ptr<ITwitterQueryHandler> handlerPtr {handler};

        if (replyPtr->error() != QNetworkReply::NoError) {
            qCWarning(QLoggingCategory("twitter-tweet-central-repository")) << "Error happened during request for layout" << layoutName;
            qCWarning(QLoggingCategory("twitter-tweet-central-repository")) << "Error code:" << reply->error();
            qCWarning(QLoggingCategory("twitter-tweet-central-repository")) << "Error message (Qt):" << reply->errorString();
            qCWarning(QLoggingCategory("twitter-tweet-central-repository")) << "Error message (Twitter):" << reply->readAll();
            repository.error(tr("Network error. Please try again later."));
            return;
        }

        std::vector<TwitterTweet> items {};
        QString errorMessage {};
        if (!handlerPtr->treatReply(reply->readAll(), items, errorMessage)) {
            qCWarning(QLoggingCategory("twitter-tweet-central-repository")) << "Error happened during request for layout" << layoutName;
            qCWarning(QLoggingCategory("twitter-tweet-central-repository")) << "Parsing error: " << errorMessage;
            repository.error(tr("Internal error"));
            return;
        } else {
            for (const TwitterTweet &tweet : items) {
                m_data.emplace(tweet.id(), tweet);
            }
            qDebug(QLoggingCategory("twitter-tweet-central-repository")) << "New data available for layout" << layoutName << ". Count:" << items.size();
            repository.add(items);
            repository.finish();
        }
    });
}

ITwitterQueryHandler * TwitterTweetCentralRepository::createQueryHandler(const TwitterQuery &query)
{
    switch (query.type()) {
    case TwitterQuery::Timeline:
        return new TwitterTimelineQueryHandler();
        break;
    default:
        return nullptr;
        break;
    }
}
