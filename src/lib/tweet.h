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

#ifndef TWEET_H
#define TWEET_H

#include <QtCore/QString>
#include <QtCore/QDateTime>
#include "globals.h"
#include "user.h"
#include "media.h"

class Tweet
{
public:
    explicit Tweet() = default;
    explicit Tweet(const QJsonObject &json);
    DEFAULT_COPY_DEFAULT_MOVE(Tweet);
    bool isValid() const;
    QString id() const;
    QString text() const;
    User user() const;
    User retweetingUser() const;
    QDateTime timestamp() const;
    int favoriteCount() const;
    bool isFavorited() const;
    int retweetCount() const;
    bool isRetweeted() const;
    QString inReplyTo() const;
    QString source() const;
    std::vector<Media> media() const;
private:
    QString m_id {};
    QString m_text {};
    User m_user {};
    User m_retweetingUser {};
    QDateTime m_timestamp {};
    int m_favoriteCount {};
    bool m_favorited {};
    int m_retweetCount {};
    bool m_retweeted {};
    QString m_inReplyTo {};
    QString m_source {};
    std::vector<Media> m_media {};
};

#endif // TWEET_H
