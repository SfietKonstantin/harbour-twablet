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

#include "entity.h"
#include <map>
#include <set>
#include <QtCore/QJsonArray>
#include "private/maputil.h"
#include "urlentity.h"
#include "mediaentity.h"
#include "usermentionentity.h"
#include "hashtagentity.h"

template <class T>
static void insertEntity(const QJsonValue &value, std::map<QString, Entity::Ptr> &map,
                         std::vector<QString> &texts)
{
    Entity::Ptr entity {new T(value.toObject())};
    if (!private_util::hasValue(map, entity->text())) {
        texts.push_back(entity->text());
    }
    map.emplace(entity->text(), entity);
}

template <class T>
static void overrideEntity(const QJsonValue &value, std::map<QString, Entity::Ptr> &map,
                           Entity::List &extended)
{
    Entity::Ptr entity {new T(value.toObject())};
    map.erase(entity->text());
    extended.push_back(entity);
}

Entity::List Entity::create(const QJsonObject &json, const QJsonObject &extendedJson)
{
    List extended {};
    List returned {};
    std::vector<QString> texts {};
    std::map<QString, Entity::Ptr> map {};
    const QJsonArray &media (json.value(QLatin1String("media")).toArray());
    for (const QJsonValue &value : media) {
        insertEntity<MediaEntity>(value, map, texts);
    }

    const QJsonArray &urls (json.value(QLatin1String("urls")).toArray());
    for (const QJsonValue &value : urls) {
        insertEntity<UrlEntity>(value, map, texts);
    }

    const QJsonArray &users (json.value(QLatin1String("user_mentions")).toArray());
    for (const QJsonValue &value : users) {
        insertEntity<UserMentionEntity>(value, map, texts);
    }

    const QJsonArray &hashtags (json.value(QLatin1String("hashtags")).toArray());
    for (const QJsonValue &value : hashtags) {
        insertEntity<HashtagEntity>(value, map, texts);
    }

    const QJsonArray &extendedMedia (extendedJson.value(QLatin1String("media")).toArray());
    for (const QJsonValue &value : extendedMedia) {
        overrideEntity<MediaEntity>(value, map, extended);
    }

    for (const QString &text : texts) {
        auto it = map.find(text);
        if (it != std::end(map)) {
            returned.push_back(it->second);
        }
    }
    for (Entity::Ptr entity : extended) {
        returned.push_back(entity);
    }

    return returned;
}
