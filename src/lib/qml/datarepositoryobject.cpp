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
#include <QtCore/QVariantMap>
#include "private/conversionutil.h"
#include "private/networkqueryexecutor.h"
#include "accountobject.h"
#include "query.h"
#include "querytypeobject.h"
#include "querywrappervisitor.h"

namespace qml
{

DataRepositoryObject::DataRepositoryObject(QObject *parent)
    : QObject(parent)
    , m_network(new QNetworkAccessManager())
    , m_tweetRepositoryContainer(private_util::NetworkQueryExecutor::create(*m_network))
    , m_userRepositoryContainer(private_util::NetworkQueryExecutor::create(*m_network))
    , m_listRepositoryContainer(private_util::NetworkQueryExecutor::create(*m_network))
    , m_itemQueryContainer(private_util::NetworkQueryExecutor::create(*m_network))
{
    m_loadSaveManager.load(m_accounts);
    for (const Account &account : m_accounts) {
        m_accountsMapping.emplace(account.userId(), account);
    }
    m_loadSaveManager.load(m_layouts);
    for (const Layout &layout : m_layouts) {
        m_tweetRepositoryContainer.referenceQuery(account(layout.accountUserId()), layout.query());
    }
}

bool DataRepositoryObject::hasAccounts() const
{
    return m_accounts.empty();
}

AccountRepository & DataRepositoryObject::accountRepository()
{
    return m_accounts;
}

LayoutRepository & DataRepositoryObject::layouts()
{
    return m_layouts;
}

TweetRepository * DataRepositoryObject::tweetRepository(const Account &account, const Query &query)
{
    return m_tweetRepositoryContainer.repository(account, query);
}

void DataRepositoryObject::referenceTweetRepositoryQuery(const Account &account, const Query &query)
{
    m_tweetRepositoryContainer.referenceQuery(account, query);
}

void DataRepositoryObject::dereferenceTweetRepositoryQuery(const Account &account, const Query &query)
{
    m_tweetRepositoryContainer.dereferenceQuery(account, query);
}

UserRepository * DataRepositoryObject::userRepository(const Account &account, const Query &query)
{
    return m_userRepositoryContainer.repository(account, query);
}

void DataRepositoryObject::referenceUserRepositoryQuery(const Account &account, const Query &query)
{
    return m_userRepositoryContainer.referenceQuery(account, query);
}

void DataRepositoryObject::dereferenceUserRepositoryQuery(const Account &account, const Query &query)
{
    return m_userRepositoryContainer.dereferenceQuery(account, query);
}

ListRepository * DataRepositoryObject::listRepository(const Account &account, const Query &query)
{
    return m_listRepositoryContainer.repository(account, query);
}

void DataRepositoryObject::referenceListRepositoryQuery(const Account &account, const Query &query)
{
    return m_listRepositoryContainer.referenceQuery(account, query);
}

void DataRepositoryObject::dereferenceListRepositoryQuery(const Account &account, const Query &query)
{
    return m_listRepositoryContainer.dereferenceQuery(account, query);
}

Account DataRepositoryObject::account(const QString &accountUserId) const
{
    auto it = m_accountsMapping.find(accountUserId);
    if (it == std::end(m_accountsMapping)) {
        return Account{};
    }
    return it->second;
}

ItemQueryContainer * DataRepositoryObject::itemQueryContainer()
{
    return &m_itemQueryContainer;
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
    const QString accountUserId {(std::begin(m_accounts) + index)->userId()};
    m_accountsMapping.erase(accountUserId);
    m_accounts.remove(index);
    m_loadSaveManager.save(m_accounts);

    std::vector<int> removedIndexes;
    for (int i = 0; i < m_layouts.count(); ++i) {
        if ((std::begin(m_layouts) + i)->accountUserId() == accountUserId) {
            removedIndexes.push_back(i);
        }
    }
    std::sort(std::begin(removedIndexes), std::end(removedIndexes), [](int first, int second) { return first > second; });

    for (int i : removedIndexes) {
        dereferenceLayoutTweetList(i);
    }
    m_loadSaveManager.save(m_layouts);

    if (hasAccounts() != oldHasAccounts) {
        emit hasAccountsChanged();
    }
}

void DataRepositoryObject::addLayout(const QString &name, int accountIndex, int queryType,
                                     const QVariantMap &parameters)
{
    QString accountUserId {};
    if (!addLayoutCheckAccount(accountIndex, accountUserId)) {
        return;
    }

    addLayout(name, accountUserId, queryType, parameters);
}

void DataRepositoryObject::addLayout(const QString &name, const QString &accountUserId,
                                     int queryType, const QVariantMap &parameters)
{
    TweetRepositoryQuery query {
        static_cast<TweetRepositoryQuery::Type>(queryType),
        std::move(private_util::convertParameters(parameters))
    };
    if (!query.isValid()) {
        return;
    }

    m_tweetRepositoryContainer.referenceQuery(account(accountUserId), query);
    m_layouts.append(std::move(Layout(name, accountUserId, std::move(query))));
    m_loadSaveManager.save(m_layouts);
    refresh();
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
        TweetRepositoryQuery query {TweetRepositoryQuery::Home, TweetRepositoryQuery::Parameters()};
        m_tweetRepositoryContainer.referenceQuery(account(userId), query);
        m_layouts.append(Layout{homeName, userId, std::move(query)});
    }
    if (enableMentionsTimeline) {
        TweetRepositoryQuery query {TweetRepositoryQuery::Mentions, TweetRepositoryQuery::Parameters()};
        m_tweetRepositoryContainer.referenceQuery(account(userId), query);
        m_layouts.append(Layout{mentionsName, userId, std::move(query)});
    }
    m_loadSaveManager.save(m_layouts);
    refresh();
}

void DataRepositoryObject::updateLayout(int index, const QString &name, int accountIndex,
                                        int queryType, const QVariantMap &parameters)
{
    if (index < 0 || index >= m_layouts.size()) {
        return;
    }

    QString accountUserId {};
    if (!addLayoutCheckAccount(accountIndex, accountUserId)) {
        return;
    }

    TweetRepositoryQuery query {
        static_cast<TweetRepositoryQuery::Type>(queryType),
        std::move(private_util::convertParameters(parameters))
    };

    const Layout &oldLayout {*(std::begin(m_layouts) + index)};
    m_tweetRepositoryContainer.dereferenceQuery(account(oldLayout.accountUserId()), oldLayout.query());

    Layout layout {name, accountUserId, std::move(query)};
    m_tweetRepositoryContainer.referenceQuery(account(layout.accountUserId()), layout.query());

    m_layouts.update(index, std::move(layout));
    m_loadSaveManager.save(m_layouts);
    refresh();
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

    dereferenceLayoutTweetList(index);
    m_loadSaveManager.save(m_layouts);
}

void DataRepositoryObject::moveLayout(int from, int to)
{
    m_layouts.move(from, to);
    m_loadSaveManager.save(m_layouts);
}

void DataRepositoryObject::refresh()
{
    m_tweetRepositoryContainer.refresh();
}

void DataRepositoryObject::refresh(QObject *query)
{
    IQueryWrapperObject *queryWrapper = qobject_cast<IQueryWrapperObject *>(query);
    if (queryWrapper == nullptr) {
        return;
    }

    class RefreshVisitor: public QueryWrapperVisitor
    {
    public:
        explicit RefreshVisitor(DataRepositoryObject &parent,
                                TweetRepositoryContainer &tweetRepositoryContainer,
                                UserRepositoryContainer &userRepositoryContainer,
                                ListRepositoryContainer &listRepositoryContainer)
            : m_parent(parent), m_tweetRepositoryContainer(tweetRepositoryContainer)
            , m_userRepositoryContainer(userRepositoryContainer)
            , m_listRepositoryContainer(listRepositoryContainer)
        {
        }
        void visitTweetModelQuery(const TweetModelQueryWrapperObject &wrapperObject) override
        {
            const Account &account {m_parent.account(wrapperObject.accountUserId())};
            m_tweetRepositoryContainer.refresh(account, wrapperObject.query());
        }
        void visitUserModelQuery(const UserModelQueryWrapperObject &wrapperObject) override
        {
            const Account &account {m_parent.account(wrapperObject.accountUserId())};
            m_userRepositoryContainer.refresh(account, wrapperObject.query());
        }
        void visitListModelQuery(const ListModelQueryObject &wrapperObject) override
        {
            const Account &account {m_parent.account(wrapperObject.accountUserId())};
            m_listRepositoryContainer.refresh(account, wrapperObject.query());
        }
    private:
        DataRepositoryObject &m_parent;
        TweetRepositoryContainer &m_tweetRepositoryContainer;
        UserRepositoryContainer &m_userRepositoryContainer;
        ListRepositoryContainer &m_listRepositoryContainer;
    };
    RefreshVisitor visitor {
        *this,
        m_tweetRepositoryContainer,
        m_userRepositoryContainer,
        m_listRepositoryContainer
    };
    queryWrapper->accept(visitor);
}

void DataRepositoryObject::loadMore(QObject *query)
{
    IQueryWrapperObject *queryWrapper = qobject_cast<IQueryWrapperObject *>(query);
    if (queryWrapper == nullptr) {
        return;
    }

    class LoadMoreVisitor: public QueryWrapperVisitor
    {
    public:
        explicit LoadMoreVisitor(DataRepositoryObject &parent,
                                 TweetRepositoryContainer &tweetRepositoryContainer,
                                 UserRepositoryContainer &userRepositoryContainer,
                                 ListRepositoryContainer &listRepositoryContainer)
            : m_parent(parent), m_tweetRepositoryContainer(tweetRepositoryContainer)
            , m_userRepositoryContainer(userRepositoryContainer)
            , m_listRepositoryContainer(listRepositoryContainer)
        {
        }
        void visitTweetModelQuery(const TweetModelQueryWrapperObject &wrapperObject) override
        {
            const Account &account {m_parent.account(wrapperObject.accountUserId())};
            m_tweetRepositoryContainer.loadMore(account, wrapperObject.query());
        }
        void visitUserModelQuery(const UserModelQueryWrapperObject &wrapperObject) override
        {
            const Account &account {m_parent.account(wrapperObject.accountUserId())};
            m_userRepositoryContainer.loadMore(account, wrapperObject.query());
        }
        void visitListModelQuery(const ListModelQueryObject &wrapperObject) override
        {
            const Account &account {m_parent.account(wrapperObject.accountUserId())};
            m_listRepositoryContainer.loadMore(account, wrapperObject.query());
        }
    private:
        DataRepositoryObject &m_parent;
        TweetRepositoryContainer &m_tweetRepositoryContainer;
        UserRepositoryContainer &m_userRepositoryContainer;
        ListRepositoryContainer &m_listRepositoryContainer;
    };
    LoadMoreVisitor visitor {
        *this,
        m_tweetRepositoryContainer,
        m_userRepositoryContainer,
        m_listRepositoryContainer
    };
    queryWrapper->accept(visitor);
}

void DataRepositoryObject::setTweetRetweeted(const QString &tweetId)
{
    Tweet tweet = m_tweetRepositoryContainer.tweet(tweetId);
    if (tweet.isValid()) {
        tweet.setRetweeted(true);
        m_tweetRepositoryContainer.updateTweet(tweet);
    }
}

void DataRepositoryObject::setTweetFavorited(const QString &tweetId, bool favorited)
{
    Tweet tweet = m_tweetRepositoryContainer.tweet(tweetId);
    if (tweet.isValid()) {
        tweet.setFavorited(favorited);
        m_tweetRepositoryContainer.updateTweet(tweet);
    }
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

void DataRepositoryObject::dereferenceLayoutTweetList(int index)
{
    const Layout &layout {*(std::begin(m_layouts) + index)};
    m_tweetRepositoryContainer.dereferenceQuery(account(layout.accountUserId()), layout.query());
    m_layouts.remove(index);
}

}
