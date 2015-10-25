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

#ifndef DATAREPOSITORYOBJECTMAP_H
#define DATAREPOSITORYOBJECTMAP_H

#include <QtCore/QtGlobal>

#include "repository.h"
#include "datarepositoryobject.h"
#include "iaccountrepositorycontainerobject.h"
#include "ilayoutcontainerobject.h"
#include "itweetrepositorycontainerobject.h"
#include "iuserrepositorycontainerobject.h"
#include "tweetlistquerywrapperobject.h"
#include "userlistquerywrapperobject.h"

class Account;
class Layout;
class Tweet;
class User;

namespace qml
{

class DataRepositoryObject;
template<class T> class DataRepositoryObjectMap;
template<> class DataRepositoryObjectMap<Account>
{
public:
    static void getQueryInfo(QObject *queryWrapper, QString &accountUserId, Query &query)
    {
        Q_UNUSED(queryWrapper)
        Q_UNUSED(accountUserId)
        Q_UNUSED(query)
    }
    static Repository<Account> * get(QObject &object, const QString &accountUserId, const Query &query)
    {
        Q_UNUSED(accountUserId)
        Q_UNUSED(query)
        IAccountRepositoryContainerObject *accountContainer = qobject_cast<IAccountRepositoryContainerObject *>(&object);
        return accountContainer != nullptr ? &(accountContainer->accountRepository()) : nullptr;
    }
    static void addListener(Repository<Account> &repository, IRepositoryListener<Account> &listener,
                            QObject *object, const QString &accountUserId, const Query &query)
    {
        Q_UNUSED(object)
        Q_UNUSED(accountUserId)
        Q_UNUSED(query)
        repository.addListener(listener);
    }
    static void removeListener(Repository<Account> &repository, IRepositoryListener<Account> &listener,
                               QObject *object, const QString &accountUserId, const Query &query)
    {
        Q_UNUSED(object)
        Q_UNUSED(accountUserId)
        Q_UNUSED(query)
        repository.removeListener(listener);
    }
};
template<> class DataRepositoryObjectMap<Layout>
{
public:
    static void getQueryInfo(QObject *queryWrapper, QString &accountUserId, Query &query)
    {
        Q_UNUSED(queryWrapper)
        Q_UNUSED(accountUserId)
        Q_UNUSED(query)
    }
    static Repository<Layout> * get(QObject &object, const QString &accountUserId, const Query &query)
    {
        Q_UNUSED(accountUserId)
        Q_UNUSED(query)
        ILayoutContainerObject *layoutContainer = qobject_cast<ILayoutContainerObject *>(&object);
        return layoutContainer != nullptr ? &(layoutContainer->layouts()) : nullptr;
    }
    static void addListener(Repository<Layout> &repository, IRepositoryListener<Layout> &listener,
                            QObject *object, const QString &accountUserId, const Query &query)
    {
        Q_UNUSED(object)
        Q_UNUSED(accountUserId)
        Q_UNUSED(query)
        repository.addListener(listener);
    }
    static void removeListener(Repository<Layout> &repository, IRepositoryListener<Layout> &listener,
                               QObject *object, const QString &accountUserId, const Query &query)
    {
        Q_UNUSED(object)
        Q_UNUSED(accountUserId)
        Q_UNUSED(query)
        repository.removeListener(listener);
    }
};

template<class T> class DataRepositoryObjectMap;
template<> class DataRepositoryObjectMap<Tweet>
{
public:
    static void getQueryInfo(QObject *queryWrapper, QString &accountUserId, Query &query)
    {
        TweetListQueryWrapperObject *queryObject = qobject_cast<TweetListQueryWrapperObject *>(queryWrapper);
        if (queryObject == nullptr) {
            accountUserId.clear();
            query = Query{};
        } else {
            accountUserId = queryObject->accountUserId();
            query = queryObject->query();
        }
    }
    static Repository<Tweet> * get(QObject &object, const QString &accountUserId, const Query &query)
    {
        IAccountRepositoryContainerObject *accountContainer = qobject_cast<IAccountRepositoryContainerObject *>(&object);
        ITweetRepositoryContainerObject *tweetContainer = qobject_cast<ITweetRepositoryContainerObject *>(&object);

        if (accountContainer == nullptr || tweetContainer == nullptr || !query.isValid()) {
            return nullptr;
        }

        const Account &account {accountContainer->account(accountUserId)};
        tweetContainer->referenceTweetListQuery(account, query);
        return tweetContainer->tweetRepository(account, query);
    }
    static void addListener(Repository<Tweet> &repository, IRepositoryListener<Tweet> &listener,
                            QObject *object, const QString &accountUserId, const Query &query)
    {
        Q_UNUSED(object)
        Q_UNUSED(accountUserId)
        Q_UNUSED(query)
        repository.addListener(listener);
    }
    static void removeListener(Repository<Tweet> &repository, IRepositoryListener<Tweet> &listener,
                               QObject *object, const QString &accountUserId, const Query &query)
    {
        IAccountRepositoryContainerObject *accountContainer = qobject_cast<IAccountRepositoryContainerObject *>(object);
        ITweetRepositoryContainerObject *tweetContainer = qobject_cast<ITweetRepositoryContainerObject *>(object);

        if (accountContainer != nullptr && tweetContainer != nullptr && query.isValid()) {
            const Account &account {accountContainer->account(accountUserId)};
            tweetContainer->dereferenceTweetListQuery(account, query);
        }

        repository.removeListener(listener);
    }
};

template<> class DataRepositoryObjectMap<User>
{
public:
    static void getQueryInfo(QObject *queryWrapper, QString &accountUserId, Query &query)
    {
        UserListQueryWrapperObject *queryObject = qobject_cast<UserListQueryWrapperObject *>(queryWrapper);
        if (queryObject == nullptr) {
            accountUserId.clear();
            query = Query{};
        } else {
            accountUserId = queryObject->accountUserId();
            query = queryObject->query();
        }
    }
    static Repository<User> * get(QObject &object, const QString &accountUserId, const Query &query)
    {
        IAccountRepositoryContainerObject *accountContainer = qobject_cast<IAccountRepositoryContainerObject *>(&object);
        IUserRepositoryContainerObject *userContainer = qobject_cast<IUserRepositoryContainerObject *>(&object);

        if (accountContainer == nullptr || userContainer == nullptr || !query.isValid()) {
            return nullptr;
        }

        const Account &account {accountContainer->account(accountUserId)};
        userContainer->referenceUserListQuery(account, query);
        return userContainer->userRepository(account, query);
    }
    static void addListener(Repository<User> &repository, IRepositoryListener<User> &listener,
                            QObject *object, const QString &accountUserId, const Query &query)
    {
        Q_UNUSED(object)
        Q_UNUSED(accountUserId)
        Q_UNUSED(query)
        repository.addListener(listener);
    }
    static void removeListener(Repository<User> &repository, IRepositoryListener<User> &listener,
                               QObject *object, const QString &accountUserId, const Query &query)
    {
        IAccountRepositoryContainerObject *accountContainer = qobject_cast<IAccountRepositoryContainerObject *>(object);
        IUserRepositoryContainerObject *userContainer = qobject_cast<IUserRepositoryContainerObject *>(object);

        if (accountContainer != nullptr && userContainer != nullptr && query.isValid()) {
            const Account &account {accountContainer->account(accountUserId)};
            userContainer->dereferenceUserListQuery(account, query);
        }

        repository.removeListener(listener);
    }
};

}

#endif // DATAREPOSITORYOBJECTMAP_H

