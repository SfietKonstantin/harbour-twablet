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

#include "user.h"
#include "private/timeutil.h"

User::User(const QJsonObject &json)
{
    m_id = json.value(QLatin1String("id_str")).toString();
    m_name = json.value(QLatin1String("name")).toString();
    m_screenName = json.value(QLatin1String("screen_name")).toString();
    m_description = json.value(QLatin1String("description")).toString();
    m_location = json.value(QLatin1String("location")).toString();
    m_url = json.value(QLatin1String("url")).toString();
    m_protected = json.value(QLatin1String("protected")).toBool();
    m_following = json.value(QLatin1String("following")).toBool();
    m_statusesCount = json.value(QLatin1String("statuses_count")).toInt();
    m_followersCount = json.value(QLatin1String("followers_count")).toInt();
    m_friendsCount = json.value(QLatin1String("location")).toInt();
    m_listedCount = json.value(QLatin1String("listed_count")).toInt();
    m_favouritesCount = json.value(QLatin1String("favourites_count")).toInt();
    m_imageUrl = json.value(QLatin1String("profile_image_url_https")).toString();
    m_bannerUrl = json.value(QLatin1String("profile_banner_url")).toString();
    m_createdAt = std::move(fromUtc(json.value(QLatin1String("created_at")).toString()));
    m_entities = std::move(json.value(QLatin1String("entities")).toObject());
}

bool User::isValid() const
{
    return !m_id.isEmpty();
}

QString User::id() const
{
    return m_id;
}

QString User::name() const
{
    return m_name;
}

QString User::screenName() const
{
    return m_screenName;
}

QString User::description() const
{
    return m_description;
}

QString User::location() const
{
    return m_location;
}

QString User::url() const
{
    return m_url;
}

bool User::isProtected() const
{
    return m_protected;
}

bool User::isFollowing() const
{
    return m_following;
}

int User::statusesCount() const
{
    return m_statusesCount;
}

int User::followersCount() const
{
    return m_followersCount;
}

int User::friendsCount() const
{
    return m_friendsCount;
}

int User::listedCount() const
{
    return m_listedCount;
}

int User::favouritesCount() const
{
    return m_favouritesCount;
}

QString User::imageUrl() const
{
    return m_imageUrl;
}

QString User::bannerUrl() const
{
    return m_bannerUrl;
}

QDateTime User::createdAt() const
{
    return m_createdAt;
}

QJsonObject User::entities() const
{
    return m_entities;
}
