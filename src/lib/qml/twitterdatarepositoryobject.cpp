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
#include "query.h"
#include <QtCore/QVariantMap>

TwitterDataRepositoryObject::TwitterDataRepositoryObject(QObject *parent)
    : QObject(parent)
{
    m_loadSaveManager.load(m_accounts);
    m_loadSaveManager.load(m_layouts);
    for (const Layout &layout : m_layouts) {
        m_tweetRepositories.emplace(layout, std::move(TweetRepository()));
    }
}

bool TwitterDataRepositoryObject::hasAccounts() const
{
    return (std::begin(m_accounts) != std::end(m_accounts));
}

AccountRepository & TwitterDataRepositoryObject::accounts()
{
    return m_accounts;
}

LayoutRepository & TwitterDataRepositoryObject::layouts()
{
    return m_layouts;
}

TweetRepository & TwitterDataRepositoryObject::tweets(const Layout &layout)
{
    return m_tweetRepositories[layout];
}

int TwitterDataRepositoryObject::addAccount(const QString &name, const QString &userId,
                                            const QString &screenName,
                                            const QString &token, const QString &tokenSecret)
{
    bool oldHasAccounts = hasAccounts();
    m_accounts.append(Account(name, userId, screenName, token.toLocal8Bit(),
                            tokenSecret.toLocal8Bit()));
    m_loadSaveManager.save(m_accounts);

    if (hasAccounts() != oldHasAccounts) {
        emit hasAccountsChanged();
    }

    return std::end(m_accounts) - std::begin(m_accounts) - 1;
}

void TwitterDataRepositoryObject::updateAccountName(int index, const QString &name)
{
    if (index < 0 || index >= std::end(m_accounts) - std::begin(m_accounts)) {
        return;
    }
    Account account {*(std::begin(m_accounts) + index)};
    account.setName(name);
    m_accounts.update(index, std::move(account));
    m_loadSaveManager.save(m_accounts);
}

void TwitterDataRepositoryObject::removeAccount(int index)
{
    if (index < 0 || index >= std::end(m_accounts) - std::begin(m_accounts)) {
        return;
    }

    bool oldHasAccounts = hasAccounts();
    const QString userId {(std::begin(m_accounts) + index)->userId()};
    m_accounts.remove(index);
    m_loadSaveManager.save(m_accounts);

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

    if (hasAccounts() != oldHasAccounts) {
        emit hasAccountsChanged();
    }
}

void TwitterDataRepositoryObject::addLayout(const QString &name, int accountIndex, int queryType,
                                            const QVariantMap &arguments)
{
    if (accountIndex < 0 || accountIndex >= std::end(m_accounts) - std::begin(m_accounts)) {
        return;
    }
    const Account &account = *(std::begin(m_accounts) + accountIndex);

    switch (queryType) {
    case Query::Home:
        break;
    case Query::Mentions:
        break;
    default:
        return;
        break;
    }

    Query::Arguments queryArguments {};
    for (const QString &key : arguments.keys()) {
        queryArguments.push_back(std::make_pair(key, arguments.value(key).toString()));
    }

    m_layouts.append(Layout(name, account.userId(), Query(static_cast<Query::Type>(queryType),
                                                           std::move(queryArguments))));
    m_tweetRepositories.emplace(*(std::end(m_layouts) - 1), TweetRepository());
    m_loadSaveManager.save(m_layouts);
    refresh();
}

void TwitterDataRepositoryObject::addDefaultLayouts(int accountIndex, const QString &homeName,
                                                    bool enableHomeTimeline, const QString &mentionsName,
                                                    bool enableMentionsTimeline)
{
    if (accountIndex < 0 || accountIndex >= std::end(m_accounts) - std::begin(m_accounts)) {
        return;
    }

    const Account &account = *(std::begin(m_accounts) + accountIndex);

    if (enableHomeTimeline) {
        m_layouts.append(Layout(homeName, account.userId(), Query(Query::Home,
                                                                         Query::Arguments())));
        m_tweetRepositories.emplace(*(std::end(m_layouts) - 1), TweetRepository());
    }
    if (enableMentionsTimeline) {
        m_layouts.append(Layout(mentionsName, account.userId(), Query(Query::Mentions,
                                                                             Query::Arguments())));
        m_tweetRepositories.emplace(*(std::end(m_layouts) - 1), TweetRepository());
    }
    m_loadSaveManager.save(m_layouts);
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

void TwitterDataRepositoryObject::updateLayoutUnread(int index, int unread)
{
    if (index < 0 || index >= std::end(m_layouts) - std::begin(m_layouts)) {
        return;
    }
    Layout layout {*(std::begin(m_layouts) + index)};
    layout.setUnread(unread);
    m_layouts.update(index, std::move(layout));
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
        const Account &account {m_accounts.find(layout.userId())};
        if (!account.isNull()) {
            m_tweetsCentralRepository.query(account, layout.query(), it->second);
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

    const Query &firstQuery {first.query()};
    const Query &secondQuery {second.query()};

    if (firstQuery.type() < secondQuery.type()) {
        return true;
    }

    if (firstQuery.type() > secondQuery.type()) {
        return false;
    }

    return firstQuery.arguments() < secondQuery.arguments();
}
