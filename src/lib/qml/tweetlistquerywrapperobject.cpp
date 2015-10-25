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

#include "tweetlistquerywrapperobject.h"
#include "private/conversionutil.h"
#include "querywrappervisitor.h"

namespace qml
{

TweetListQueryWrapperObject::TweetListQueryWrapperObject(QObject *parent)
    : QObject(parent)
{
}

TweetListQueryWrapperObject::TweetListQueryWrapperObject(const QString accountUserId,
                                                         const TweetListQuery &query,
                                                         QObject *parent)
    : QObject(parent), m_accountUserId(accountUserId), m_query(query)
{
}

QString TweetListQueryWrapperObject::accountUserId() const
{
    return m_accountUserId;
}

void TweetListQueryWrapperObject::setAccountUserId(const QString &accountUserId)
{
    if (m_accountUserId != accountUserId) {
        m_accountUserId = accountUserId;
        emit accountUserIdChanged();
    }
}


Query TweetListQueryWrapperObject::query() const
{
    return m_query;
}

void TweetListQueryWrapperObject::setQuery(TweetListQuery &&query)
{
    m_query = std::move(query);
}

QueryTypeObject::TweetListType TweetListQueryWrapperObject::type() const
{
    return m_type;
}

void TweetListQueryWrapperObject::setType(QueryTypeObject::TweetListType type)
{
    if (m_type != type) {
        m_type = type;
        updateQuery();
        emit typeChanged();
    }
}

QVariantMap TweetListQueryWrapperObject::parameters() const
{
    return m_parameters;
}

void TweetListQueryWrapperObject::setParameters(const QVariantMap &parameters)
{
    if (m_parameters != parameters) {
        m_parameters = parameters;
        updateQuery();
        emit parametersChanged();
    }
}

void TweetListQueryWrapperObject::accept(QueryWrapperVisitor &visitor) const
{
    visitor.visitTweetListQuery(*this);
}

void TweetListQueryWrapperObject::updateQuery()
{
    if (convertedType() != TweetListQuery::Invalid) {
        setQuery(TweetListQuery(convertedType(), private_util::convertParameters(m_parameters)));
    }
}

TweetListQuery::Type TweetListQueryWrapperObject::convertedType() const
{
    return static_cast<TweetListQuery::Type>(m_type);
}

}
