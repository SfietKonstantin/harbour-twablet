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

#include "usertimelinequeryhandler.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QUrl>
#include "tweet.h"

UserTimelineQueryHandler::UserTimelineQueryHandler(const Query::Arguments &arguments)
{
    auto userIdIt = arguments.find(QLatin1String("user_id"));
    if (userIdIt != std::end(arguments)) {
        m_userId = userIdIt->second;
    }

    auto excludeRepliesIt = arguments.find(QLatin1String("exclude_replies"));
    if (excludeRepliesIt != std::end(arguments)) {
        m_excludeReplies = excludeRepliesIt->second;
    }

    auto includeRtsIt = arguments.find(QLatin1String("include_rts"));
    if (includeRtsIt != std::end(arguments)) {
        m_includeRts = includeRtsIt->second;
    }
}

void UserTimelineQueryHandler::createRequest(QString &path, std::map<QByteArray, QByteArray> &parameters) const
{
    path = QLatin1String("statuses/user_timeline.json");

    parameters.insert({"count", QByteArray::number(200)});
    parameters.insert({"trim_user", "false"});
    parameters.insert({"include_entities", "true"});
    parameters.insert({"user_id", QUrl::toPercentEncoding(m_userId)});
    parameters.insert({"exclude_replies", QUrl::toPercentEncoding(m_excludeReplies)});
    parameters.insert({"include_rts", QUrl::toPercentEncoding(m_includeRts)});
    if (!m_sinceId.isEmpty()) {
        parameters.insert({"since_id", QUrl::toPercentEncoding(m_sinceId)});
    }
}

bool UserTimelineQueryHandler::treatReply(const QByteArray &data, std::vector<Tweet> &items,
                                          QString &errorMessage, Placement &placement)
{
    QJsonParseError error {-1, QJsonParseError::NoError};
    QJsonDocument document {QJsonDocument::fromJson(data, &error)};
    if (error.error != QJsonParseError::NoError) {
        errorMessage = error.errorString();
        placement = Discard;
        return false;
    }

    const QJsonArray &tweets (document.array());
    items.reserve(tweets.size());
    for (const QJsonValue &tweet : tweets) {
        if (tweet.isObject()) {
            items.emplace_back(tweet.toObject());
        }
    }

    if (!items.empty()) {
        m_sinceId = std::begin(items)->id();
    }

    placement = Prepend;
    return true;
}

