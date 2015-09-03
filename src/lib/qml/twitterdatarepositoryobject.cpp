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
        m_tweetRepositories.emplace(layout, std::move(TwitterTweetRepository()));
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

TwitterTweetRepository & TwitterDataRepositoryObject::tweets(const Layout &layout)
{
    return m_tweetRepositories[layout];
}

void TwitterDataRepositoryObject::addUser(const QString &name, const QString &userId,
                                          const QString &screenName,
                                          const QString &token, const QString &tokenSecret)
{
    m_users.append(TwitterUser(name, userId, screenName, token.toLocal8Bit(),
                            tokenSecret.toLocal8Bit()));
    m_loadSaveManager.save(m_users);
}

void TwitterDataRepositoryObject::updateUserName(int index, const QString &name)
{
    if (index < 0 || index >= std::end(m_users) - std::begin(m_users)) {
        return;
    }
    TwitterUser user {*(std::begin(m_users) + index)};
    user.setName(name);
    m_users.update(index, std::move(user));
    m_loadSaveManager.save(m_users);
}

void TwitterDataRepositoryObject::removeUser(int index)
{
    if (index < 0 || index >= std::end(m_users) - std::begin(m_users)) {
        return;
    }

    const QString userId {(std::begin(m_users) + index)->userId()};
    m_users.remove(index);
    m_loadSaveManager.save(m_users);

    std::vector<int> removedIndexes;
    for (int i = 0; i < m_layouts.count(); ++i) {
        if ((std::begin(m_layouts) + i)->userId() == userId) {
            removedIndexes.push_back(i);
        }
    }
    std::sort(std::begin(removedIndexes), std::end(removedIndexes), [](int first, int second) { return first > second; });

    for (int i : removedIndexes) {
        m_layouts.remove(i);
    }
    m_loadSaveManager.save(m_layouts);
}

void TwitterDataRepositoryObject::addLayout(const QString &name, int userIndex, int queryType,
                                            const QVariantMap &arguments)
{
    if (userIndex < 0 || userIndex >= std::end(m_users) - std::begin(m_users)) {
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

    m_layouts.append(Layout(name, user.userId(), TwitterQuery(static_cast<TwitterQuery::Type>(queryType),
                                                           std::move(queryArguments))));
    m_tweetRepositories.emplace(*(std::end(m_layouts) - 1), TwitterTweetRepository());
    m_loadSaveManager.save(m_layouts);
    refresh();
}

void TwitterDataRepositoryObject::updateLayoutName(int index, const QString &name)
{
    if (index < 0 || index >= std::end(m_layouts) - std::begin(m_layouts)) {
        return;
    }
    Layout layout {*(std::begin(m_layouts) + index)};
    layout.setName(name);
    m_layouts.update(index, std::move(layout));
    m_loadSaveManager.save(m_layouts);
}

void TwitterDataRepositoryObject::removeLayout(int index)
{
    if (index < 0 || index >= std::end(m_layouts) - std::begin(m_layouts)) {
        return;
    }

    m_layouts.remove(index);
    m_loadSaveManager.save(m_layouts);
}

void TwitterDataRepositoryObject::refresh()
{
    for (auto it = std::begin(m_tweetRepositories); it != std::end(m_tweetRepositories); ++it) {
        const Layout &layout {it->first};
        const TwitterUser &user {m_users.find(layout.userId())};
        if (!user.isNull()) {
            m_tweetsCentralRepository.query(user, layout.query(), it->second);
        }
    }
}

bool TwitterDataRepositoryObject::LayoutComparator::operator()(const Layout &first,
                                                               const Layout &second) const
{
    if (first.userId() < second.userId()) {
        return true;
    }
    if (first.userId() > second.userId()) {
        return false;
    }

    const TwitterQuery &firstQuery {first.query()};
    const TwitterQuery &secondQuery {first.query()};

    if (firstQuery.type() < secondQuery.type()) {
        return true;
    }

    if (firstQuery.type() > secondQuery.type()) {
        return false;
    }

    return firstQuery.arguments() < secondQuery.arguments();
}
