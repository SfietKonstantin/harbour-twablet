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

#include "tweetobject.h"
#include <QtCore/QRegularExpression>

TweetObject::TweetObject(const Tweet &data, QObject *parent)
    : QObject(parent), m_data{std::move(data)}, m_user{nullptr}
{
    m_user = UserObject::create(m_data.user(), this);
    if (m_data.retweetingUser().isValid()) {
        m_retweetingUser = UserObject::create(m_data.retweetingUser(), this);
    }
    QRegularExpression urlParser {QLatin1String("<a[^>]*>([^<]*)</a>")};
    QRegularExpressionMatch match {urlParser.match(m_data.source())};
    m_sourceName = match.captured(1);
    m_media.reset(MediaModel::create(m_data.entities(), this));
}

TweetObject * TweetObject::create(const Tweet &data, QObject *parent)
{
    return new TweetObject(data, parent);
}

QString TweetObject::id() const
{
    return m_data.id();
}

QString TweetObject::originalId() const
{
    return m_data.originalId();
}

QString TweetObject::text() const
{
    return m_data.text();
}

UserObject * TweetObject::user() const
{
    return m_user;
}

UserObject * TweetObject::retweetingUser() const
{
    return m_retweetingUser;
}

QDateTime TweetObject::timestamp() const
{
    return m_data.timestamp();
}

int TweetObject::favoriteCount() const
{
    return m_data.favoriteCount();
}

bool TweetObject::isFavorited() const
{
    return m_data.isFavorited();
}

int TweetObject::retweetCount() const
{
    return m_data.retweetCount();
}

bool TweetObject::isRetweeted() const
{
    return m_data.isRetweeted();
}

QString TweetObject::inReplyTo() const
{
    return m_data.inReplyTo();
}

QString TweetObject::source() const
{
    return m_data.source();
}

QString TweetObject::sourceName() const
{
    return m_sourceName;
}

MediaModel * TweetObject::media() const
{
    return m_media.get();
}

Tweet TweetObject::data() const
{
    return m_data;
}

void TweetObject::update(const Tweet &other)
{
    Q_UNUSED(other)
    if (m_data.isFavorited() != other.isFavorited()) {
        m_data.setFavorited(other.isFavorited());
        emit favoritedChanged();
    }

    if (m_data.isRetweeted() != other.isRetweeted()) {
        m_data.setRetweeted(other.isRetweeted());
        emit retweetedChanged();
    }
}
