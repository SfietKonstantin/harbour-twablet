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
#include "private/timeutil.h"
#include <set>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

Tweet::Tweet(const QJsonObject &json)
{
    // Use the retweeted status when possible
    QJsonObject tweet {json};
    QJsonObject retweetedTweet {json.value(QLatin1String("retweeted_status")).toObject()};
    QJsonObject displayedTweet {tweet};
    if (!retweetedTweet.isEmpty()) {
        displayedTweet = retweetedTweet;

        // Adding the retweeting user when retweeting
        m_retweetingUser = std::move(User(tweet.value(QLatin1String("user")).toObject()));
    }

    m_id = std::move(tweet.value(QLatin1String("id_str")).toString());
    m_originalId = std::move(displayedTweet.value(QLatin1String("id_str")).toString());
    m_text = std::move(displayedTweet.value(QLatin1String("text")).toString());
    m_favoriteCount = displayedTweet.value(QLatin1String("favorite_count")).toInt();
    m_favorited = displayedTweet.value(QLatin1String("favorited")).toBool();
    m_retweetCount = displayedTweet.value(QLatin1String("retweet_count")).toInt();
    m_retweeted = displayedTweet.value(QLatin1String("retweeted")).toBool();
    m_inReplyTo = std::move(displayedTweet.value(QLatin1String("in_reply_to_status_id")).toString());
    m_source = std::move(tweet.value(QLatin1String("source")).toString());
    m_timestamp = std::move(private_util::fromUtc(displayedTweet.value(QLatin1String("created_at")).toString()));
    m_user = std::move(User(displayedTweet.value(QLatin1String("user")).toObject()));

    QJsonObject entities {displayedTweet.value(QLatin1String("entities")).toObject()};
    m_entities = Entity::create(entities);
}

bool Tweet::isValid() const
{
    return !m_id.isEmpty();
}

QString Tweet::id() const
{
    return m_id;
}

QString Tweet::originalId() const
{
    return m_originalId;
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

void Tweet::setFavorited(bool faviroted)
{
    m_favorited = faviroted;
}

int Tweet::retweetCount() const
{
    return m_retweetCount;
}

bool Tweet::isRetweeted() const
{
    return m_retweeted;
}

void Tweet::setRetweeted(bool retweeted)
{
    m_retweeted = retweeted;
}

QString Tweet::inReplyTo() const
{
    return m_inReplyTo;
}

QString Tweet::source() const
{
    return m_source;
}

std::vector<Entity::Ptr> Tweet::entities() const
{
    return m_entities;
}

