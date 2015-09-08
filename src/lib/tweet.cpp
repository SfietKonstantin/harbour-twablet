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

#include "tweet.h"
#include <set>
#include <QtCore/QLocale>
#include <QtCore/QJsonArray>

Tweet::Tweet(const QJsonObject &json)
{
    // Use the retweeted status when possible
    QJsonObject tweet {json};
    QJsonObject retweetedTweet {json.value(QLatin1String("retweeted_status")).toObject()};
    if (!retweetedTweet.isEmpty()) {
        tweet = retweetedTweet;

        // Adding the retweeting user when retweeting
        m_retweetingUser = std::move(User(json.value(QLatin1String("user")).toObject()));
    }

    m_id = tweet.value(QLatin1String("id_str")).toString();
    m_text = tweet.value(QLatin1String("text")).toString();
    m_favoriteCount = tweet.value(QLatin1String("favorite_count")).toInt();
    m_favorited = tweet.value(QLatin1String("favorited")).toBool();
    m_retweetCount = tweet.value(QLatin1String("retweet_count")).toInt();
    m_retweeted = tweet.value(QLatin1String("retweeted")).toBool();
    m_inReplyTo = tweet.value(QLatin1String("in_reply_to_status_id")).toString();
    m_source = tweet.value(QLatin1String("source")).toString();

    QString createdAtString = tweet.value(QLatin1String("created_at")).toString();
    QLocale locale (QLocale::English, QLocale::UnitedStates);
    m_timestamp = locale.toDateTime(createdAtString, QLatin1String("ddd MMM dd HH:mm:ss +0000 yyyy"));
    m_user = std::move(User(tweet.value(QLatin1String("user")).toObject()));

    QJsonObject entities {tweet.value(QLatin1String("entities")).toObject()};

    QJsonArray media (entities.value(QLatin1String("media")).toArray());
    for (const QJsonValue &medium : media) {
        m_media.emplace_back(medium.toObject());
    }
    QJsonArray extendedEntities (entities.value(QLatin1String("extended_entities")).toArray());
    for (const QJsonValue &medium : extendedEntities) {
        m_media.emplace_back(medium.toObject());
    }
}

bool Tweet::isValid() const
{
    return !m_id.isEmpty();
}

QString Tweet::id() const
{
    return m_id;
}

QString Tweet::text() const
{
    return m_text;
}

User Tweet::user() const
{
    return m_user;
}

User Tweet::retweetingUser() const
{
    return m_retweetingUser;
}

QDateTime Tweet::timestamp() const
{
    return m_timestamp;
}

int Tweet::favoriteCount() const
{
    return m_favoriteCount;
}

bool Tweet::isFavorited() const
{
    return m_favorited;
}

int Tweet::retweetCount() const
{
    return m_retweetCount;
}

bool Tweet::isRetweeted() const
{
    return m_retweeted;
}

QString Tweet::inReplyTo() const
{
    return m_inReplyTo;
}

QString Tweet::source() const
{
    return m_source;
}

std::vector<Media> Tweet::media() const
{
    return m_media;
}

