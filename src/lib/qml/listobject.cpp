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

#include "listobject.h"

namespace qml
{

ListObject::ListObject(const List &data, QObject *parent)
    : QObject(parent), m_data{data}
{
    m_user.reset(UserObject::create(m_data.user(), this));
}

ListObject * ListObject::create(const List &data, QObject *parent)
{
    return new ListObject(data, parent);
}

bool ListObject::isValid() const
{
    return m_data.isValid();
}

QString ListObject::id() const
{
    return m_data.id();
}

QString ListObject::name() const
{
    return m_data.name();
}

QString ListObject::slug() const
{
    return m_data.slug();
}

QString ListObject::fullName() const
{
    return m_data.fullName();
}

QString ListObject::description() const
{
    return m_data.description();
}

UserObject * ListObject::user() const
{
    return m_user.get();
}

QString ListObject::mode() const
{
    return m_data.mode();
}

bool ListObject::isFollowing() const
{
    return m_data.isFollowing();
}

int ListObject::memberCount() const
{
    return m_data.memberCount();
}

int ListObject::subscriberCount() const
{
    return m_data.subscriberCount();
}

QString ListObject::uri() const
{
    return m_data.uri();
}

List ListObject::data() const
{
    return m_data;
}

void ListObject::update(const List &other)
{
    Q_UNUSED(other)
}

}

