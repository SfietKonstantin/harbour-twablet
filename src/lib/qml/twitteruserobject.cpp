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

#include "twitteruserobject.h"

TwitterUserObject::TwitterUserObject(const TwitterUser &twitterUser, QObject *parent)
    : QObject(parent), m_twitterUser(twitterUser)
{
}

TwitterUserObject * TwitterUserObject::create(const TwitterUser &twitterUser, QObject *parent)
{
    return new TwitterUserObject(twitterUser, parent);
}

QString TwitterUserObject::name() const
{
    return m_twitterUser.name();
}

QString TwitterUserObject::userId() const
{
    return m_twitterUser.userId();
}

QString TwitterUserObject::screenName() const
{
    return m_twitterUser.screenName();
}

QByteArray TwitterUserObject::token() const
{
    return m_twitterUser.token();
}

QByteArray TwitterUserObject::tokenSecret() const
{
    return m_twitterUser.tokenSecret();
}

void TwitterUserObject::setName(const QString &name)
{
    if (m_twitterUser.name() != name) {
        m_twitterUser.setName(name);
        emit nameChanged();
    }
}
