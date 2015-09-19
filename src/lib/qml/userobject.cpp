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

#include "userobject.h"
#include <QtCore/QJsonArray>

UserObject::UserObject(const User &data, QObject *parent)
    : QObject(parent), m_data{data}
{
    initializeUrl();
}

UserObject * UserObject::create(const User &data, QObject *parent)
{
    return new UserObject(data, parent);
}

bool UserObject::isValid() const
{
    return m_data.isValid();
}

QString UserObject::id() const
{
    return m_data.id();
}

QString UserObject::name() const
{
    return m_data.name();
}

QString UserObject::screenName() const
{
    return m_data.screenName();
}

QString UserObject::description() const
{
    return m_data.description();
}

QString UserObject::location() const
{
    return m_data.location();
}

QString UserObject::displayUrl() const
{
    return m_displayUrl;
}

QString UserObject::url() const
{
    return m_url;
}

bool UserObject::isProtected() const
{
    return m_data.isProtected();
}

bool UserObject::isFollowing() const
{
    return m_data.isFollowing();
}

int UserObject::statusesCount() const
{
    return m_data.statusesCount();
}

int UserObject::followersCount() const
{
    return m_data.followersCount();
}

int UserObject::friendsCount() const
{
    return m_data.friendsCount();
}

int UserObject::listedCount() const
{
    return m_data.listedCount();
}

int UserObject::favouritesCount() const
{
    return m_data.favouritesCount();
}

QString UserObject::imageUrl() const
{
    return m_data.imageUrl();
}

QString UserObject::bannerUrl() const
{
    return m_data.bannerUrl();
}

int UserObject::tweetsPerDay() const
{
    qint64 days {m_data.createdAt().daysTo(QDateTime::currentDateTime())};
    return static_cast<double>(m_data.statusesCount()) / static_cast<double>(days);
}

void UserObject::initializeUrl()
{
    QJsonObject urlEntities (m_data.entities().value(QLatin1String("url")).toObject());
    const QJsonArray &urls (urlEntities.value(QLatin1String("urls")).toArray());
    if (urls.count() != 1) {
        return;
    }

    const QJsonObject &url (urls.first().toObject());
    const QJsonArray &indices (url.value(QLatin1String("indices")).toArray());
    if (indices.count() != 2) {
        return;
    }

    int first = indices[0].toInt();
    int second = indices[1].toInt();
    int size = second - first;

    if (first != 0 || size != m_data.url().size()) {
        return;
    }

    m_url = std::move(url.value(QLatin1String("expanded_url")).toString());
    m_displayUrl = std::move(url.value(QLatin1String("display_url")).toString());
}
