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

#include "listqueryhandlerutil.h"
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

namespace private_util
{

bool treatTweetReply(IListQueryHandler<Tweet>::RequestType requestType,
                     const QJsonArray &data, std::vector<Tweet> &items,
                     IListQueryHandler<Tweet>::Placement &placement,
                     QString &sinceId, QString &maxId)
{
    items.reserve(data.size());
    for (const QJsonValue &item : data) {
        if (item.isObject()) {
            items.emplace_back(item.toObject());
        }
    }

    QString newSinceId = !items.empty() ? std::begin(items)->id() : QString();
    quint64 newMaxId = items.empty() ? 0 : (std::end(items) - 1)->id().toULongLong();
    QString newMaxIdStr = newMaxId > 0 ? QString::number(newMaxId - 1) : QString();
    if (!items.empty()) {
        switch (requestType) {
        case IListQueryHandler<Tweet>::Refresh:
            sinceId = std::move(newSinceId);
            if (maxId.isEmpty()) {
                maxId = std::move(newMaxIdStr);
            }
            placement = IListQueryHandler<Tweet>::Prepend;
            break;
        case IListQueryHandler<Tweet>::LoadMore:
            if (sinceId.isEmpty()) {
                sinceId = std::move(newSinceId);
            }
            maxId = std::move(newMaxIdStr);
            placement = IListQueryHandler<Tweet>::Append;
            break;
        }

    }
    return true;
}

}
