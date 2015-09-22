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
#include "urlentity.h"
#include "mediaentity.h"
#include "usermentionentity.h"
#include "hashtagentity.h"
#include <set>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

template <class T>
static void checkAndInsert(const QJsonValue &value, std::vector<Entity::Ptr> &returned,
                           std::set<QString> &inserted)
{
    Entity::Ptr entity {new T(value.toObject())};
    if (inserted.find(entity->text()) == std::end(inserted)) {
        returned.push_back(entity);
        inserted.insert(entity->text());
    }
}

std::vector<Entity::Ptr> Entity::create(const QJsonObject &json)
{
    std::vector<Entity::Ptr> returned {};
    std::set<QString> inserted {};
    const QJsonArray &media (json.value(QLatin1String("media")).toArray());
    for (const QJsonValue &value : media) {
        checkAndInsert<MediaEntity>(value, returned, inserted);
    }

    const QJsonArray &urls (json.value(QLatin1String("urls")).toArray());
    for (const QJsonValue &value : urls) {
        checkAndInsert<UrlEntity>(value, returned, inserted);
    }

    const QJsonArray &users (json.value(QLatin1String("user_mentions")).toArray());
    for (const QJsonValue &value : users) {
        checkAndInsert<UserMentionEntity>(value, returned, inserted);
    }

    const QJsonArray &hashtags (json.value(QLatin1String("hashtags")).toArray());
    for (const QJsonValue &value : hashtags) {
        checkAndInsert<HashtagEntity>(value, returned, inserted);
    }

    const QJsonArray &extended (json.value(QLatin1String("extended_entities")).toArray());
    for (const QJsonValue &value : extended) {
        checkAndInsert<MediaEntity>(value, returned, inserted);
    }

    return returned;
}
