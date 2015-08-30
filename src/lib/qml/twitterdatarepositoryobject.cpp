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

#include "twitterdatarepositoryobject.h"
#include "twitterquery.h"
#include <QtCore/QVariantMap>

TwitterDataRepositoryObject::TwitterDataRepositoryObject(QObject *parent)
    : QObject(parent)
{
    m_loadSaveManager.load(m_users);
    m_loadSaveManager.load(m_layouts);
    for (const Layout &layout : m_layouts) {
        Q_UNUSED(layout)
        m_tweetRepositories.emplace_back();
    }
}

TwitterUserRepository & TwitterDataRepositoryObject::users()
{
    return m_users;
}

LayoutRepository & TwitterDataRepositoryObject::layouts()
{
    return m_layouts;
}

TwitterTweetRepository & TwitterDataRepositoryObject::tweets(int layoutIndex)
{
    return m_tweetRepositories[layoutIndex];
}

void TwitterDataRepositoryObject::addUser(const QString &name, const QString &userId,
                                          const QString &screenName,
                                          const QString &token, const QString &tokenSecret)
{
    m_users.add(TwitterUser(name, userId, screenName, token.toLocal8Bit(),
                            tokenSecret.toLocal8Bit()));
    m_loadSaveManager.save(m_users);
}

void TwitterDataRepositoryObject::removeUser(int index)
{
    m_users.remove(index);
    m_loadSaveManager.save(m_users);
}

void TwitterDataRepositoryObject::addLayout(const QString &name, int userIndex, int queryType,
                                            const QVariantMap &arguments)
{
    if (userIndex < 0 || std::begin(m_users) + userIndex >= std::end(m_users)) {
        return;
    }
    const TwitterUser &user = *(std::begin(m_users) + userIndex);

    switch (queryType) {
    case TwitterQuery::Timeline:
        break;
    default:
        return;
        break;
    }

    TwitterQuery::Arguments queryArguments {};
    for (const QString &key : arguments.keys()) {
        queryArguments.push_back(std::make_pair(key, arguments.value(key).toString()));
    }

    m_layouts.add(Layout(name, user.userId(), TwitterQuery(static_cast<TwitterQuery::Type>(queryType),
                                                           std::move(queryArguments))));
    m_tweetRepositories.emplace_back();
    m_loadSaveManager.save(m_layouts);
    refresh();
}

void TwitterDataRepositoryObject::refresh()
{

    for (int i = 0; i < m_layouts.count(); ++i) {
        const Layout &layout {*(m_layouts.begin() + i)};
        const TwitterUser &user {m_users.find(layout.userId())};
        if (!user.isNull()) {
            m_tweetsCentralRepository.query(layout.name(), user, layout.query(), m_tweetRepositories[i]);
        }
    }
}
