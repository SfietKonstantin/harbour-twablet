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

UserObject::UserObject(const User &data, QObject *parent)
    : QObject(parent), m_data{data}
{
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

QString UserObject::url() const
{
    return m_data.url();
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
