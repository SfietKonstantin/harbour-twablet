/*
 * Copyright (C) 2015 Lucien XU <sfietkonstantin@free.fr>
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

#include "twitteraccountobject.h"

TwitterAccountObject::TwitterAccountObject(const TwitterAccount &twitterAccount, QObject *parent)
    : QObject(parent), m_twitterAccount(twitterAccount)
{
}

TwitterAccountObject * TwitterAccountObject::create(const TwitterAccount &twitterAccount, QObject *parent)
{
    return new TwitterAccountObject(twitterAccount, parent);
}

QString TwitterAccountObject::name() const
{
    return m_twitterAccount.name();
}

QString TwitterAccountObject::userId() const
{
    return m_twitterAccount.userId();
}

QString TwitterAccountObject::screenName() const
{
    return m_twitterAccount.screenName();
}

QByteArray TwitterAccountObject::token() const
{
    return m_twitterAccount.token();
}

QByteArray TwitterAccountObject::tokenSecret() const
{
    return m_twitterAccount.tokenSecret();
}

void TwitterAccountObject::update(const TwitterAccount &other)
{
    if (m_twitterAccount.name() != other.name()) {
        m_twitterAccount.setName(other.name());
        emit nameChanged();
    }
}
