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

#include "datarepositoryobject.h"
#include "query.h"
#include "accountobject.h"
#include <QtCore/QVariantMap>
#include <QtCore/QLoggingCategory>

namespace qml
{

DataRepositoryObject::DataRepositoryObject(QObject *parent)
    : QObject(parent)
{
    m_loadSaveManager.load(m_accounts);
    for (const Account &account : m_accounts) {
        m_accountsMapping.emplace(account.userId(), account);
    }
    m_loadSaveManager.load(m_layouts);
    for (const Layout &layout : m_layouts) {
        insertRepository(layout);
    }
}

bool DataRepositoryObject::hasAccounts() const
{
    return m_accounts.empty();
}

AccountRepository & DataRepositoryObject::accounts()
{
    return m_accounts;
}

LayoutRepository & DataRepositoryObject::layouts()
{
    return m_layouts;
}

TweetRepository & DataRepositoryObject::tweets(const Layout &layout)
{
    return m_tweetsCentralRepository.repository(accountFromId(layout.userId()), layout.query());
}

const Layout * DataRepositoryObject::temporaryLayout(int index) const
{
    auto it = m_temporaryLayouts.find(index);
    if (it == std::end(m_temporaryLayouts)) {
        return nullptr;
    }

    return &(it->second);
}

bool DataRepositoryObject::isTemporaryLayoutValid(int index) const
{
    return m_temporaryLayouts.find(index) != std::end(m_temporaryLayouts);
}

bool DataRepositoryObject::isUserRepositoryValid(int index) const
{
    return m_userCentralRepository.isValid(index);
}

UserRepository & DataRepositoryObject::user(int index)
{
    return m_userCentralRepository.repository(index);
}

int DataRepositoryObject::addAccount(const QString &name, const QString &userId,
                                     const QString &screenName,
                                     const QString &token, const QString &tokenSecret)
{
    bool oldHasAccounts = hasAccounts();
    Account &addedAccount (m_accounts.append(Account(name, userId, screenName, token.toLocal8Bit(),
                                                     tokenSecret.toLocal8Bit())));
    m_accountsMapping.emplace(addedAccount.userId(), addedAccount);
    m_loadSaveManager.save(m_accounts);

    if (hasAccounts() != oldHasAccounts) {
        emit hasAccountsChanged();
    }

    return m_accounts.size() - 1;
}

void DataRepositoryObject::updateAccountName(int index, const QString &name)
{
    if (index < 0 || index >= m_accounts.size()) {
        return;
    }
    Account account {*(std::begin(m_accounts) + index)};
    account.setName(name);
    m_accounts.update(index, std::move(account));
    m_loadSaveManager.save(m_accounts);
}

void DataRepositoryObject::removeAccount(int index)
{
    if (index < 0 || index >= m_accounts.size()) {
        return;
    }

    bool oldHasAccounts = hasAccounts();
    const QString userId {(std::begin(m_accounts) + index)->userId()};
    m_accountsMapping.erase(userId);
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
        removeLayoutFromRepositories(i);
    }
    m_loadSaveManager.save(m_layouts);

    if (hasAccounts() != oldHasAccounts) {
        emit hasAccountsChanged();
    }
}

void DataRepositoryObject::addLayout(const QString &name, int accountIndex, int queryType,
                                     const QVariantMap &parameters)
{
    QString userId {};
    if (!addLayoutCheckAccount(accountIndex, userId)) {
        return;
    }

    addLayout(name, userId, queryType, parameters);
}

void DataRepositoryObject::addLayout(const QString &name, AccountObject *account, int queryType,
                                     const QVariantMap &parameters)
{
    if (account == nullptr) {
        return;
    }

    addLayout(name, account->userId(), queryType, parameters);
}

void DataRepositoryObject::addDefaultLayouts(int accountIndex, const QString &homeName,
                                             bool enableHomeTimeline, const QString &mentionsName,
                                             bool enableMentionsTimeline)
{
    QString userId {};
    if (!addLayoutCheckAccount(accountIndex, userId)) {
        return;
    }

    if (enableHomeTimeline) {
        m_layouts.append(Layout(homeName, userId, Query(Query::Home, Query::Parameters())));
        insertRepository();
    }
    if (enableMentionsTimeline) {
        m_layouts.append(Layout(mentionsName, userId, Query(Query::Mentions, Query::Parameters())));
        insertRepository();
    }
    m_loadSaveManager.save(m_layouts);
}

void DataRepositoryObject::updateLayoutName(int index, const QString &name)
{
    if (index < 0 || index >= m_layouts.size()) {
        return;
    }
    Layout layout {*(std::begin(m_layouts) + index)};
    layout.setName(name);
    m_layouts.update(index, std::move(layout));
    m_loadSaveManager.save(m_layouts);
}

void DataRepositoryObject::updateLayoutUnread(int index, int unread)
{
    if (index < 0 || index >= m_layouts.size()) {
        return;
    }
    Layout layout {*(std::begin(m_layouts) + index)};
    layout.setUnread(unread);
    m_layouts.update(index, std::move(layout));
}

void DataRepositoryObject::removeLayout(int index)
{
    if (index < 0 || index >= m_layouts.size()) {
        return;
    }

    removeLayoutFromRepositories(index);
    m_loadSaveManager.save(m_layouts);
}

void DataRepositoryObject::refresh()
{
    m_tweetsCentralRepository.refresh();
}

void DataRepositoryObject::loadMore(int layoutIndex)
{
    if (layoutIndex < 0 || layoutIndex >= m_layouts.count()) {
        return;
    }

    const Layout &layout {*(std::begin(m_layouts) + layoutIndex)};
    m_tweetsCentralRepository.loadMore(accountFromId(layout.userId()), layout.query());
}

int DataRepositoryObject::addTemporaryLayout(AccountObject *account, int queryType, const QVariantMap &paramters)
{
    if (account == nullptr) {
        return -1;
    }

    Query::Parameters queryParameters {};
    if (!addLayoutCheckQuery(queryType, paramters, queryParameters)) {
        return -1;
    }

    int index = m_temporaryLayoutsIndex;
    ++m_temporaryLayoutsIndex;
    m_temporaryLayouts.emplace(index, Layout(QString(), account->userId(), std::move(Query(static_cast<Query::Type>(queryType),
                                                                                           std::move(queryParameters)))));
    auto it = m_temporaryLayouts.find(index);
    insertRepository(it->second);
    refresh(it->second);
    return index;
}

void DataRepositoryObject::removeTemporaryLayout(int index)
{
    if (index < 0) {
        return;
    }

    auto it = m_temporaryLayouts.find(index);
    if (it == std::end(m_temporaryLayouts)) {
        return;
    }

    removeLayoutFromRepositories(it->second);
    m_temporaryLayouts.erase(it);
}

void DataRepositoryObject::clearTemporary()
{
    for (auto it = std::begin(m_temporaryLayouts); it != std::end(m_temporaryLayouts); ++it) {
        removeLayoutFromRepositories(it->second);
    }
    m_temporaryLayouts.clear();
}

void DataRepositoryObject::refreshTemporary(int index)
{
    auto it = m_temporaryLayouts.find(index);
    if (it == std::end(m_temporaryLayouts)) {
        return;
    }
    refresh(it->second);
}

int DataRepositoryObject::addUser(AccountObject *account, int queryType, const QVariantMap &parameters)
{
    if (account == nullptr) {
        return -1;
    }

    Query::Parameters queryParameters {};
    if (!addUserCheckQuery(queryType, parameters, queryParameters)) {
        return -1;
    }

    return m_userCentralRepository.addRepository(account->data(), std::move(Query(static_cast<Query::Type>(queryType),
                                                                                  std::move(queryParameters))));
}

void DataRepositoryObject::removeUser(int index)
{
    m_userCentralRepository.removeRepository(index);
}

void DataRepositoryObject::userLoadMore(int index)
{
    m_userCentralRepository.loadMore(index);
}

void DataRepositoryObject::setTweetRetweeted(const QString &tweetId)
{
    Tweet tweet = m_tweetsCentralRepository.tweet(tweetId);
    if (tweet.isValid()) {
        tweet.setRetweeted(true);
        m_tweetsCentralRepository.updateTweet(tweet);
    }
}

void DataRepositoryObject::setTweetFavorited(const QString &tweetId, bool favorited)
{
    Tweet tweet = m_tweetsCentralRepository.tweet(tweetId);
    if (tweet.isValid()) {
        tweet.setFavorited(favorited);
        m_tweetsCentralRepository.updateTweet(tweet);
    }
}

Account DataRepositoryObject::accountFromId(const QString &userId) const
{
    return m_accountsMapping.at(userId);
}

void DataRepositoryObject::refresh(const Layout &layout)
{
    const Account &account {accountFromId(layout.userId())};
    m_tweetsCentralRepository.refresh(account, layout.query());
}

void DataRepositoryObject::addLayout(const QString &name, const QString &userId, int queryType,
                                     const QVariantMap &parameters)
{
    Query::Parameters queryParameters {};
    if (!addLayoutCheckQuery(queryType, parameters, queryParameters)) {
        return;
    }

    m_layouts.append(Layout(name, userId, Query(static_cast<Query::Type>(queryType),
                                                std::move(queryParameters))));
    insertRepository();
    m_loadSaveManager.save(m_layouts);
    refresh();
}

bool DataRepositoryObject::addLayoutCheckAccount(int accountIndex, QString &userId)
{
    if (accountIndex < 0 || accountIndex >= m_accounts.size()) {
        return false;
    }
    const Account &account = *(std::begin(m_accounts) + accountIndex);
    userId = account.userId();
    return true;
}

bool DataRepositoryObject::addLayoutCheckQuery(int queryType, const QVariantMap &parameters,
                                               Query::Parameters &queryParametrs) const
{
    switch (queryType) {
    case Query::Home:
        break;
    case Query::Mentions:
        break;
    case Query::Search:
        break;
    case Query::Favorites:
        break;
    case Query::UserTimeline:
        break;

    default:
        return false;
        break;
    }

    for (const QString &key : parameters.keys()) {
        queryParametrs.emplace(key, parameters.value(key).toString());
    }

    return true;
}

bool DataRepositoryObject::addUserCheckQuery(int queryType, const QVariantMap &parameters,
                                             Query::Parameters &queryParameters) const
{
    switch (queryType) {
    case Query::Friends:
        break;
    case Query::Followers:
        break;
    default:
        return false;
        break;
    }

    for (const QString &key : parameters.keys()) {
        queryParameters.emplace(key, parameters.value(key).toString());
    }

    return true;
}

void DataRepositoryObject::insertRepository(const Layout &layout)
{
    m_tweetsCentralRepository.refQuery(accountFromId(layout.userId()), layout.query());
}

void DataRepositoryObject::insertRepository()
{
    insertRepository(*(std::end(m_layouts) - 1));
}

void DataRepositoryObject::removeLayoutFromRepositories(const Layout &layout)
{
    m_tweetsCentralRepository.derefQuery(accountFromId(layout.userId()), layout.query());
}

void DataRepositoryObject::removeLayoutFromRepositories(int index)
{
    const Layout &layout {*(std::begin(m_layouts) + index)};
    removeLayoutFromRepositories(layout);
    m_layouts.remove(index);
}

}
