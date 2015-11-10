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

#ifndef TWEETOBJECT_H
#define TWEETOBJECT_H

#include <QtCore/QObject>
#include "tweet.h"
#include "userobject.h"
#include "mediamodel.h"
#include "quotedtweetobject.h"

namespace qml
{

class TweetObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString originalId READ originalId CONSTANT)
    Q_PROPERTY(QString text READ text CONSTANT)
    Q_PROPERTY(qml::UserObject * user READ user CONSTANT)
    Q_PROPERTY(qml::UserObject * retweetingUser READ retweetingUser CONSTANT)
    Q_PROPERTY(QDateTime timestamp READ timestamp CONSTANT)
    Q_PROPERTY(int favoriteCount READ favoriteCount CONSTANT)
    Q_PROPERTY(bool favorited READ isFavorited NOTIFY favoritedChanged)
    Q_PROPERTY(int retweetCount READ retweetCount CONSTANT)
    Q_PROPERTY(bool retweeted READ isRetweeted NOTIFY retweetedChanged)
    Q_PROPERTY(QString inReplyTo READ inReplyTo CONSTANT)
    Q_PROPERTY(QString source READ source CONSTANT)
    Q_PROPERTY(QString sourceName READ sourceName CONSTANT)
    Q_PROPERTY(qml::MediaModel * media READ media CONSTANT)
    Q_PROPERTY(qml::QuotedTweetObject * quotedStatus READ quotedStatus CONSTANT)
public:
    DISABLE_COPY_DISABLE_MOVE(TweetObject);
    static TweetObject * create(const Tweet &data, QObject *parent = 0);
    QString id() const;
    QString originalId() const;
    QString text() const;
    UserObject * user() const;
    UserObject * retweetingUser() const;
    QDateTime timestamp() const;
    int favoriteCount() const;
    bool isFavorited() const;
    int retweetCount() const;
    bool isRetweeted() const;
    QString inReplyTo() const;
    QString source() const;
    QString sourceName() const;
    MediaModel * media() const;
    QuotedTweetObject * quotedStatus() const;
    Tweet data() const;
    void update(const Tweet &other);
signals:
    void favoritedChanged();
    void retweetedChanged();
private:
    explicit TweetObject(const Tweet &data, QObject *parent = 0);
    Tweet m_data {};
    QObjectPtr<UserObject> m_user {nullptr};
    QObjectPtr<UserObject> m_retweetingUser {nullptr};
    QString m_sourceName {};
    QObjectPtr<MediaModel> m_media {nullptr};
    QObjectPtr<QuotedTweetObject> m_quotedStatus {nullptr};
};

}

#endif // TWEETOBJECT_H
