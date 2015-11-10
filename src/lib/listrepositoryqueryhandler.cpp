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

#include "listrepositoryqueryhandler.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QUrl>
#include "private/repositoryqueryhandlerutil.h"

ListRepositoryQueryHandler::ListRepositoryQueryHandler()
{
}

IRepositoryQueryHandler<List>::Ptr ListRepositoryQueryHandler::create()
{
    return Ptr(new ListRepositoryQueryHandler());
}

Query::Parameters ListRepositoryQueryHandler::additionalParameters(RequestType requestType) const
{
    Q_UNUSED(requestType);
    Query::Parameters returned {};

    switch (requestType) {
    case Refresh:
        Q_ASSERT_X(false, "ListRepositoryQueryHandler", "Refreshed is not implemented for List");
        break;
    case LoadMore:
        if (!m_nextCursor.isEmpty()) {
            returned.emplace("cursor", QUrl::toPercentEncoding(m_nextCursor));
        }
        break;
    }
    return returned;
}

bool ListRepositoryQueryHandler::treatReply(RequestType requestType, const QByteArray &data,
                                            std::vector<List> &items, QString &errorMessage,
                                            Placement &placement)
{
    Q_ASSERT_X(requestType == LoadMore, "ListRepositoryQueryHandler", "Refreshed is not implemented for List");
    QJsonParseError error {-1, QJsonParseError::NoError};
    QJsonDocument document {QJsonDocument::fromJson(data, &error)};
    if (error.error != QJsonParseError::NoError) {
        errorMessage = error.errorString();
        placement = Discard;
        return false;
    }

    const QJsonObject &root (document.object());
    const QJsonArray &lists (root.value(QLatin1String("lists")).toArray());
    items.reserve(lists.size());
    for (const QJsonValue &list : lists) {
        if (list.isObject()) {
            items.emplace_back(list.toObject());
        }
    }

    if (!items.empty()) {
        m_nextCursor = root.value(QLatin1String("next_cursor_str")).toString();
        placement = Append;
    }
    return true;
}
