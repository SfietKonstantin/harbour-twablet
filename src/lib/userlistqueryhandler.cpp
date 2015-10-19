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

#include "userlistqueryhandler.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QUrl>
#include "private/listqueryhandlerutil.h"

UserListQueryHandler::UserListQueryHandler()
{
}

IListQueryHandler<User>::Ptr UserListQueryHandler::create()
{
    return Ptr(new UserListQueryHandler());
}

Query::Parameters UserListQueryHandler::additionalParameters(RequestType requestType) const
{
    Q_UNUSED(requestType);
    Query::Parameters returned {};

    switch (requestType) {
    case Refresh:
        Q_ASSERT_X(false, "UserListQueryHandler", "Refreshed is not implemented for User");
        break;
    case LoadMore:
        if (!m_nextCursor.isEmpty()) {
            returned.emplace("cursor", QUrl::toPercentEncoding(m_nextCursor));
        }
        break;
    }
    return returned;
}

bool UserListQueryHandler::treatReply(RequestType requestType, const QByteArray &data,
                                      std::vector<User> &items, QString &errorMessage,
                                      Placement &placement)
{
    Q_ASSERT_X(requestType == LoadMore, "UserListQueryHandler", "Refreshed is not implemented for User");
    QJsonParseError error {-1, QJsonParseError::NoError};
    QJsonDocument document {QJsonDocument::fromJson(data, &error)};
    if (error.error != QJsonParseError::NoError) {
        errorMessage = error.errorString();
        placement = Discard;
        return false;
    }

    const QJsonObject &root (document.object());
    const QJsonArray &users (root.value(QLatin1String("users")).toArray());
    items.reserve(users.size());
    for (const QJsonValue &user : users) {
        if (user.isObject()) {
            items.emplace_back(user.toObject());
        }
    }

    if (!items.empty()) {
        m_nextCursor = QString::number(root.value(QLatin1String("next_cursor")).toInt());
        placement = Append;
    }
    return true;
}
