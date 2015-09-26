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
#include "loadsavemanager.h"
#include "accountrepository.h"
#include "layoutrepository.h"
#include "tweetcentralrepository.h"

class AccountObject;
class DataRepositoryObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool hasAccounts READ hasAccounts NOTIFY hasAccountsChanged)
public:
    explicit DataRepositoryObject(QObject *parent = 0);
    bool hasAccounts() const;
    AccountRepository & accounts();
    LayoutRepository & layouts();
    TweetRepository & tweets(const Layout &layout);
    const Layout * temporaryLayout(int index) const;
    bool isTemporaryLayoutValid(int index) const;
signals:
    void hasAccountsChanged();
public slots:
    // Accounts
    int addAccount(const QString &name, const QString &userId, const QString &screenName,
                    const QString &token, const QString &tokenSecret);
    void updateAccountName(int index, const QString &name);
    void removeAccount(int index);
    // Layouts
    void addLayout(const QString &name, int accountIndex, int queryType, const QVariantMap &arguments);
    void addLayout(const QString &name, AccountObject *account, int queryType, const QVariantMap &arguments);
    void addDefaultLayouts(int accountIndex, const QString &homeName, bool enableHomeTimeline,
                           const QString &mentionsName, bool enableMentionsTimeline);
    void updateLayoutName(int index, const QString &name);
    void updateLayoutUnread(int index, int unread);
    void removeLayout(int index);
    void refresh();
    // Temporary layout
    int addTemporaryLayout(AccountObject *account, int queryType, const QVariantMap &arguments);
    void removeTemporaryLayout(int index);
    void clearTemporary();
    void refreshTemporary(int index);
private:
    Account accountFromId(const QString &userId) const;
    void refresh(const Layout &layout);
    void addLayout(const QString &name, const QString &userId, int queryType, const QVariantMap &arguments);
    bool addLayoutCheckAccount(int accountIndex, QString &userId);
    bool addLayoutCheckQuery(int queryType, const QVariantMap &arguments,
                             Query::Arguments &queryArguments) const;
    void insertRepository(const Layout &layout);
    void insertRepository();
    void removeLayoutFromRepositories(const Layout &layout);
    void removeLayoutFromRepositories(int index);

    LoadSaveManager m_loadSaveManager {};
    AccountRepository m_accounts {};
    std::map<QString, const Account &> m_accountsMapping {};
    LayoutRepository m_layouts {};
    TweetCentralRepository m_tweetsCentralRepository {};
    std::map<int, Layout> m_temporaryLayouts {};
    int m_temporaryLayoutsIndex {0};
};

#endif // DATAREPOSITORYOBJECT_H
