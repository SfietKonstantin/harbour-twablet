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

#include "layout.h"

Layout::Layout(const QString &name, const QString &userId, TweetListQuery &&query)
    : m_name{name}, m_accountUserId{userId}, m_query{std::move(query)}
{
}

bool Layout::isValid() const
{
    return (!m_name.isEmpty() && !m_accountUserId.isEmpty() && m_query.isValid());
}

QString Layout::name() const
{
    return m_name;
}

void Layout::setName(const QString &name)
{
    m_name = name;
}

QString Layout::accountUserId() const
{
    return m_accountUserId;
}

void Layout::setAccountUserId(const QString &userId)
{
    m_accountUserId = userId;
}

TweetListQuery Layout::query() const
{
    return m_query;
}

void Layout::setQuery(TweetListQuery &&query)
{
    m_query = std::move(query);
}

int Layout::unread() const
{
    return m_unread;
}

void Layout::setUnread(int unread)
{
    m_unread = unread;
}
