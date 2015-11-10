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

#include "list.h"
#include <QtCore/QJsonObject>
#include "private/timeutil.h"


List::List(const QJsonObject &json)
{
    m_id = std::move(json.value(QLatin1String("id_str")).toString());
    m_name = std::move(json.value(QLatin1String("name")).toString());
    m_slug = std::move(json.value(QLatin1String("slut")).toString());
    m_fullName = std::move(json.value(QLatin1String("full_name")).toString());
    m_description = std::move(json.value(QLatin1String("description")).toString());
    m_user = std::move(User(json.value(QLatin1String("user")).toObject()));
    m_mode = std::move(json.value(QLatin1String("mode")).toString());
    m_following = json.value(QLatin1String("following")).toBool();
    m_memberCount = json.value(QLatin1String("member_count")).toInt();
    m_subscriberCount = json.value(QLatin1String("subscriber_count")).toInt();
    m_uri = std::move(json.value(QLatin1String("uri")).toString());
    m_createdAt = std::move(private_util::fromUtc(json.value(QLatin1String("created_at")).toString()));
}

bool List::isValid() const
{
    return !m_id.isEmpty();
}

QString List::id() const
{
    return m_id;
}

QString List::name() const
{
    return m_name;
}

QString List::slug() const
{
    return m_slug;
}

QString List::fullName() const
{
    return m_fullName;
}

QString List::description() const
{
    return m_description;
}

User List::user() const
{
    return m_user;
}

QString List::mode() const
{
    return m_mode;
}

bool List::isFollowing() const
{
    return m_following;
}

int List::memberCount() const
{
    return m_memberCount;
}

int List::subscriberCount() const
{
    return m_subscriberCount;
}

QString List::uri() const
{
    return m_uri;
}
