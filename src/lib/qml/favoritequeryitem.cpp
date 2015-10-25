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

#include "favoritequeryitem.h"
#include "private/twitterqueryutil.h"
#include "accountobject.h"

namespace qml
{

FavoriteQueryItem::FavoriteQueryItem(QObject *parent)
    : AbstractQueryItem(parent)
{
}

QString FavoriteQueryItem::tweetId() const
{
    return m_tweetId;
}

void FavoriteQueryItem::setTweetId(QString tweetId)
{
    if (m_tweetId != tweetId) {
        m_tweetId = tweetId;
        emit tweetIdChanged();
    }
}

bool FavoriteQueryItem::isFavorited() const
{
    return m_favorited;
}

void FavoriteQueryItem::setFavorited(bool favorited)
{
    if (m_favorited != favorited) {
        m_favorited = favorited;
        emit favoritedChanged();
    }
}

bool FavoriteQueryItem::isQueryValid() const
{
    return !m_tweetId.isEmpty();
}

QNetworkReply * FavoriteQueryItem::createQuery(const Account &account) const
{
    QByteArray path {};
    if (m_favorited) {
        path = std::move(QByteArray("favorites/destroy.json"));
    } else {
        path = std::move(QByteArray("favorites/create.json"));
    }
    std::map<QByteArray, QByteArray> parameters {{"id", QUrl::toPercentEncoding(m_tweetId)}};

    return private_util::TwitterQueryUtil::post(network(), path, {}, parameters, account);
}

void FavoriteQueryItem::handleReply(const QByteArray &reply,
                                    QNetworkReply::NetworkError networkError,
                                    const QString &errorMessage)
{
    Q_UNUSED(reply)
    Q_UNUSED(errorMessage)
    if (networkError != QNetworkReply::NoError) {
        if (networkError == QNetworkReply::ContentOperationNotPermittedError) {
            setStatusAndErrorMessage(Error, tr("Favoriting is not allowed."));
        } else if (networkError == QNetworkReply::ContentNotFoundError) {
            setStatusAndErrorMessage(Error, tr("This tweet is not in your favorites"));
        }
    }
}

}
