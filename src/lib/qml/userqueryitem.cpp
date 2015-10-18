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

#include "userqueryitem.h"
#include "private/twitterqueryutil.h"
#include "accountobject.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

namespace qml
{

UserQueryItem::UserQueryItem(QObject *parent)
    : AbstractQueryItem(parent)
{
}

QString UserQueryItem::userId() const
{
    return m_userId;
}

void UserQueryItem::setUserId(const QString &userId)
{
    if (m_userId != userId) {
        m_userId = userId;
        emit userIdChanged();
    }
}

UserObject * UserQueryItem::user() const
{
    return m_user.get();
}

bool UserQueryItem::isQueryValid() const
{
    return !m_userId.isEmpty();
}

QNetworkReply * UserQueryItem::createQuery() const
{
    QByteArray path {"users/show.json"};
    std::map<QByteArray, QByteArray> parameters {{"user_id", QUrl::toPercentEncoding(m_userId)}};

    return private_util::TwitterQueryUtil::get(network(), path, parameters, account()->data());
}

void UserQueryItem::handleReply(const QByteArray &reply, QNetworkReply::NetworkError networkError,
                                const QString &errorMessage)
{
    Q_UNUSED(errorMessage)
    if (networkError != QNetworkReply::NoError) {
        return;
    }

    QJsonParseError error {-1, QJsonParseError::NoError};
    QJsonDocument document {QJsonDocument::fromJson(reply, &error)};
    if (error.error != QJsonParseError::NoError) {
        setStatusAndErrorMessage(Error, tr("Internal error"));
        return;
    }

    const QJsonObject &user {document.object()};
    m_user.reset(UserObject::create(User(user)));
    emit userChanged();
}

}
