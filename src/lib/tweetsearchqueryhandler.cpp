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

#include "tweetsearchqueryhandler.h"
#include "private/listqueryhandlerutil.h"
#include <QtCore/QUrl>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>

TweetSearchQueryHandler::TweetSearchQueryHandler()
{
}

IListQueryHandler<Tweet>::Ptr TweetSearchQueryHandler::create()
{
    return Ptr(new TweetSearchQueryHandler());
}

Query::Parameters TweetSearchQueryHandler::additionalParameters(RequestType requestType) const
{
    Query::Parameters returned {};

    switch (requestType) {
    case Refresh:
        if (!m_sinceId.isEmpty()) {
            returned.emplace("since_id", QUrl::toPercentEncoding(m_sinceId));
        }
        break;
    case LoadMore:
        if (!m_maxId.isEmpty()) {
            returned.emplace("max_id", QUrl::toPercentEncoding(m_maxId));
        }
        break;
    default:
        break;
    }
    return returned;
}

bool TweetSearchQueryHandler::treatReply(RequestType requestType, const QByteArray &data,
                                       std::vector<Tweet> &items, QString &errorMessage,
                                       Placement &placement)
{
    QJsonParseError error {-1, QJsonParseError::NoError};
    QJsonDocument document {QJsonDocument::fromJson(data, &error)};
    if (error.error != QJsonParseError::NoError) {
        errorMessage = error.errorString();
        placement = Discard;
        return false;
    }

    const QJsonObject &root (document.object());
    const QJsonArray tweets (root.value(QLatin1String("statuses")).toArray());
    return private_util::treatTweetReply(requestType, tweets, items, placement,
                                         m_sinceId, m_maxId);
}
