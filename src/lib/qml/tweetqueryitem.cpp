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

#include "tweetqueryitem.h"
#include "private/twitterqueryutil.h"
#include "accountobject.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

namespace qml
{

TweetQueryItem::TweetQueryItem(QObject *parent)
    : AbstractQueryItem(parent)
{
}

QString TweetQueryItem::tweetId() const
{
    return m_tweetId;
}

void TweetQueryItem::setTweetId(const QString &tweetId)
{
    if (m_tweetId != tweetId) {
        m_tweetId = tweetId;
        emit tweetIdChanged();
    }
}

TweetObject * TweetQueryItem::data() const
{
    return m_data.get();
}

void TweetQueryItem::setFavorited(bool favorited)
{
    if (m_data == nullptr) {
        return;
    }

    Tweet tweet {std::move(m_data->data())};
    tweet.setFavorited(favorited);
    m_data->update(tweet);
}

void TweetQueryItem::setRetweeted()
{
    if (m_data == nullptr) {
        return;
    }

    Tweet tweet {std::move(m_data->data())};
    tweet.setRetweeted(true);
    m_data->update(tweet);
}

bool TweetQueryItem::isQueryValid() const
{
    return !m_tweetId.isEmpty();
}

QNetworkReply * TweetQueryItem::createQuery(const Account &account) const
{
    QByteArray path {"statuses/show.json"};
    std::map<QByteArray, QByteArray> parameters {
        {"id", QUrl::toPercentEncoding(m_tweetId)},
        {"trim_user", "false"},
        {"include_entities", "true"}
    };

    return private_util::TwitterQueryUtil::get(network(), path, parameters, account);
}

void TweetQueryItem::handleReply(const QByteArray &reply, QNetworkReply::NetworkError networkError,
                                 const QString &errorMessage)
{
    Q_UNUSED(reply)
    Q_UNUSED(errorMessage)
    if (networkError != QNetworkReply::NoError) {
        setStatusAndErrorMessage(Error, tr("Failed to retrieve tweet"));
        return;
    }

    QJsonParseError error {-1, QJsonParseError::NoError};
    QJsonDocument document {QJsonDocument::fromJson(reply, &error)};
    if (error.error != QJsonParseError::NoError) {
        setStatusAndErrorMessage(Error, tr("Failed to retrieve tweet"));
        return;
    }

    Tweet tweet {document.object()};
    m_data.reset(TweetObject::create(tweet));
    emit dataChanged();
}

}
