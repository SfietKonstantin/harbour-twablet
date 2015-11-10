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

#ifndef DATAREPOSITORYOBJECT_H
#define DATAREPOSITORYOBJECT_H

#include <QtCore/QObject>
#include "qobjectutils.h"
#include "loadsavemanager.h"
#include "accountrepository.h"
#include "layoutrepository.h"
#include "tweetrepositorycontainer.h"
#include "userrepositorycontainer.h"
#include "listrepositorycontainer.h"
#include "iaccountrepositorycontainerobject.h"
#include "ilayoutcontainerobject.h"
#include "itweetrepositorycontainerobject.h"
#include "iuserrepositorycontainerobject.h"
#include "ilistrepositorycontainerobject.h"
#include "iitemquerycontainerobject.h"

namespace qml
{

class AccountObject;
class TweetModelQueryWrapperObject;
class DataRepositoryObject
        : public QObject
        , public IAccountRepositoryContainerObject
        , public ILayoutContainerObject
        , public ITweetRepositoryContainerObject
        , public IUserRepositoryContainerObject
        , public IListRepositoryContainerObject
        , public IItemQueryContainerObject
{
    Q_OBJECT
    Q_PROPERTY(bool hasAccounts READ hasAccounts NOTIFY hasAccountsChanged)
    Q_INTERFACES(qml::IAccountRepositoryContainerObject)
    Q_INTERFACES(qml::ILayoutContainerObject)
    Q_INTERFACES(qml::ITweetRepositoryContainerObject)
    Q_INTERFACES(qml::IUserRepositoryContainerObject)
    Q_INTERFACES(qml::IListRepositoryContainerObject)
    Q_INTERFACES(qml::IItemQueryContainerObject)
public:
    explicit DataRepositoryObject(QObject *parent = 0);
    bool hasAccounts() const;
    AccountRepository & accountRepository() override;
    LayoutRepository & layouts() override;
    TweetRepository * tweetRepository(const Account &account, const Query &query) override;
    void referenceTweetRepositoryQuery(const Account &account, const Query &query) override;
    void dereferenceTweetRepositoryQuery(const Account &account, const Query &query) override;
    UserRepository * userRepository(const Account &account, const Query &query) override;
    void referenceUserRepositoryQuery(const Account &account, const Query &query) override;
    void dereferenceUserRepositoryQuery(const Account &account, const Query &query) override;
    ListRepository * listRepository(const Account &account, const Query &query) override;
    void referenceListRepositoryQuery(const Account &account, const Query &query) override;
    void dereferenceListRepositoryQuery(const Account &account, const Query &query) override;
    Account account(const QString &accountUserId) const override;
    ItemQueryContainer * itemQueryContainer() override;
signals:
    void hasAccountsChanged();
public slots:
    // Accounts
    int addAccount(const QString &name, const QString &userId, const QString &screenName,
                    const QString &token, const QString &tokenSecret);
    void updateAccountName(int index, const QString &name);
    void removeAccount(int index);
    // Layouts
    void addLayout(const QString &name, int accountIndex, int queryType,
                   const QVariantMap &parameters);
    void addLayout(const QString &name, const QString &accountUserId, int queryType,
                   const QVariantMap &parameters);
    void addDefaultLayouts(int accountIndex, const QString &homeName, bool enableHomeTimeline,
                           const QString &mentionsName, bool enableMentionsTimeline);
    void updateLayout(int index, const QString &name, int accountIndex, int queryType,
                      const QVariantMap &parameters);
    void updateLayoutUnread(int index, int unread);
    void removeLayout(int index);
    void moveLayout(int from, int to);
    void refresh();
    void refresh(QObject *query);
    void loadMore(QObject *query);
    // Action on tweets
    void setTweetRetweeted(const QString &tweetId);
    void setTweetFavorited(const QString &tweetId, bool favorited);
private:
    bool addLayoutCheckAccount(int accountIndex, QString &userId);
    void dereferenceLayoutTweetList(int index);

    QObjectPtr<QNetworkAccessManager> m_network {nullptr};
    LoadSaveManager m_loadSaveManager {};
    AccountRepository m_accounts {};
    std::map<QString, const Account &> m_accountsMapping {};
    LayoutRepository m_layouts {};
    TweetRepositoryContainer m_tweetRepositoryContainer;
    UserRepositoryContainer m_userRepositoryContainer;
    ListRepositoryContainer m_listRepositoryContainer;
    ItemQueryContainer m_itemQueryContainer;
};

}

#endif // DATAREPOSITORYOBJECT_H
