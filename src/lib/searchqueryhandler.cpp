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

#include "searchqueryhandler.h"
#include <QtCore/QUrl>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QLoggingCategory>
#include "tweet.h"

SearchQueryHandler::SearchQueryHandler(const Query::Arguments &arguments)
    : AbstractTweetQueryHandler()
{
    auto qIt = arguments.find(QLatin1String("q"));
    if (qIt != std::end(arguments)) {
        m_query = qIt->second;
    }
    auto resultTypeIt = arguments.find(QLatin1String("result_type"));
    if (resultTypeIt != std::end(arguments)) {
        const QString &resultType = resultTypeIt->second;
        if (resultType == QLatin1String("mixed")) {
            m_resultType = "mixed";
        } else if (resultType == QLatin1String("recent")) {
            m_resultType = "recent";
        } else if (resultType == QLatin1String("popular")) {
            m_resultType = "popular";
        }
    }
    qCDebug(QLoggingCategory("search-query-handler")) << "Searching using q:" << m_query
                                                      << "result_type:" << m_resultType;
}

QString SearchQueryHandler::path() const
{
    return QLatin1String{"search/tweets.json"};
}

AbstractTweetQueryHandler::Parameters SearchQueryHandler::commonParameters() const
{
    Parameters returned {
        {"count", QByteArray::number(100)},
        {"include_entities", "true"},
        {"q", QUrl::toPercentEncoding(m_query)}
    };
    if (!m_resultType.isEmpty()) {
        returned.insert({"result_type", m_resultType});
    }
    return returned;
}

bool SearchQueryHandler::treatReply(RequestType requestType, const QByteArray &data,
                                    std::vector<Tweet> &items, QString &errorMessage,
                                    IQueryHandler::Placement &placement)
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
    return AbstractTweetQueryHandler::treatReply(requestType, tweets, items, placement);
}

