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

#include "layoutobject.h"

LayoutObject::LayoutObject(const Layout &layout, QObject *parent)
    : QObject(parent), m_layout{layout}
{
}

LayoutObject * LayoutObject::create(const Layout &layout, QObject *parent)
{
    return new LayoutObject(layout, parent);
}

QString LayoutObject::name() const
{
    return m_layout.name();
}

int LayoutObject::unread() const
{
    return m_layout.unread();
}

QueryObject::Type LayoutObject::queryType() const
{
    return static_cast<QueryObject::Type>(m_layout.query().type());
}

void LayoutObject::update(const Layout &other)
{
    if (m_layout.name() != other.name()) {
        m_layout.setName(other.name());
        emit nameChanged();
    }

    if (m_layout.query() != other.query()) {
        Query::Type oldType = m_layout.query().type();
        m_layout.setQuery(other.query());
        if (m_layout.query().type() != oldType) {
            emit typeChanged();
        }
    }

    if (m_layout.unread() != other.unread()) {
        m_layout.setUnread(other.unread());
        emit unreadChanged();
    }
}
