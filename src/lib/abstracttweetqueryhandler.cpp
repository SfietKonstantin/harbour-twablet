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

#include "abstracttweetqueryhandler.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QUrl>
#include "tweet.h"

AbstractTweetQueryHandler::AbstractTweetQueryHandler()
{
}

void AbstractTweetQueryHandler::createRequest(RequestType requestType, QString &outPath,
                                              Parameters &outParameters) const
{
    outPath = path();
    outParameters = std::move(commonParameters());
    switch (requestType) {
    case Refresh:
        if (!m_sinceId.isEmpty()) {
            outParameters.emplace("since_id", QUrl::toPercentEncoding(m_sinceId));
        }
        break;
    case LoadMore:
        if (!m_maxId.isEmpty()) {
            outParameters.emplace("max_id", QUrl::toPercentEncoding(m_maxId));
        }
        break;
    }

}

bool AbstractTweetQueryHandler::treatReply(RequestType requestType, const QByteArray &data,
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

    return treatReply(requestType, document.array(), items, placement);
}

bool AbstractTweetQueryHandler::treatReply(RequestType requestType, const QJsonArray &data,
                                           std::vector<Tweet> &items, Placement &placement)
{
    items.reserve(data.size());
    for (const QJsonValue &tweet : data) {
        if (tweet.isObject()) {
            items.emplace_back(tweet.toObject());
        }
    }

    QString sinceId = std::begin(items)->id();
    ulong maxId = (std::end(items) - 1)->id().toULong();
    QString maxIdStr = QString::number(maxId - 1);
    if (!items.empty()) {
        switch (requestType) {
        case Refresh:
            m_sinceId = std::move(sinceId);
            if (m_maxId.isEmpty()) {
                m_maxId = std::move(maxIdStr);
            }
            placement = Prepend;
            break;
        case LoadMore:
            if (m_sinceId.isEmpty()) {
                m_sinceId = std::move(sinceId);
            }
            m_maxId = std::move(maxIdStr);
            placement = Append;
            break;
        }

    }
    return true;
}
