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
#include "private/conversionutil.h"

namespace qml
{

LayoutObject::LayoutObject(const Layout &data, QObject *parent)
    : QObject(parent), m_data{data}
    , m_query{new TweetListQueryWrapperObject(data.accountUserId(), data.query())}
    , m_parameters{private_util::convertParametersBack(data.query().parameters())}
{
}

LayoutObject * LayoutObject::create(const Layout &data, QObject *parent)
{
    return new LayoutObject(data, parent);
}

QString LayoutObject::name() const
{
    return m_data.name();
}

QString LayoutObject::accountUserId() const
{
    return m_data.accountUserId();
}

int LayoutObject::unread() const
{
    return m_data.unread();
}

QueryTypeObject::TweetListType LayoutObject::queryType() const
{
    return static_cast<QueryTypeObject::TweetListType>(m_data.query().type());
}

QObject * LayoutObject::query() const
{
    return m_query.get();
}

QVariantMap LayoutObject::parameters() const
{
    return m_parameters;
}

void LayoutObject::update(const Layout &other)
{
    if (m_data.name() != other.name()) {
        m_data.setName(other.name());
        emit nameChanged();
    }

    bool hasQueryChanged {false};

    if (m_data.accountUserId() != other.accountUserId()) {
        m_data.setAccountUserId(other.accountUserId());
        hasQueryChanged = true;
        emit accountUserIdChanged();
    }

    if (m_data.query() != other.query()) {
        TweetListQuery::Type oldType = m_data.query().type();
        m_data.setQuery(other.query());
        m_query->setQuery(other.query());
        if (m_data.query().type() != oldType) {
            emit queryTypeChanged();
        }
        QVariantMap parameters = private_util::convertParametersBack(m_data.query().parameters());
        if (m_parameters != parameters) {
            m_parameters = parameters;
            emit parametersChanged();
        }
        hasQueryChanged = true;
    }

    if (hasQueryChanged) {
        m_query.reset(new TweetListQueryWrapperObject(m_data.accountUserId(), m_data.query()));
        emit queryChanged();
    }

    if (m_data.unread() != other.unread()) {
        m_data.setUnread(other.unread());
        emit unreadChanged();
    }
}

}
