/*
 * Copyright (C) 2015 Lucien XU <sfietkonstantin@free.fr>
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

#include "twittertimelinequery.h"
#include "twittertweet.h"
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

TwitterTimelineQuery::TwitterTimelineQuery(QObject *parent)
    : TwitterModelQuery(parent)
{
}

QString TwitterTimelineQuery::name() const
{
    return tr("Timeline");
}

TwitterModelItem::Type TwitterTimelineQuery::type() const
{
    return TwitterModelItem::Tweet;
}

void TwitterTimelineQuery::createRequest(QString &path, std::map<QString, QString> &parameters) const
{
    path = QLatin1String("statuses/home_timeline.json");
    parameters.insert({QLatin1String("count"), QString::number(200)});
}

bool TwitterTimelineQuery::treatReply(const QByteArray &data, QList<TwitterModelItem *> &items, QString &errorMessage, QObject *parent) const
{
    QJsonParseError error {-1, QJsonParseError::NoError};
    QJsonDocument document {QJsonDocument::fromJson(data, &error)};
    if (error.error != QJsonParseError::NoError) {
        errorMessage = error.errorString();
        return false;
    }

    QJsonArray tweets {document.array()};
    for (const QJsonValue &tweet : tweets) {
        if (tweet.isObject()) {
            items.append(TwitterTweet::create(tweet.toObject(), parent));
        }
    }
    return true;
}
