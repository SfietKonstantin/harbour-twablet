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

#include "private/itemquerycallback.h"
#include "itemquerycontainer.h"
#include "itemqueryhandlerfactory.h"
#include "tweet.h"
#include "user.h"

template<class T> class ItemQueryHandlerCreator;
template<> class ItemQueryHandlerCreator<Tweet>
{
public:
    static IItemQueryHandler<Tweet>::Ptr createHandler(const Query &query)
    {
        return ItemQueryHandlerFactory::createTweetItem(query);
    }
};

template<> class ItemQueryHandlerCreator<User>
{
public:
    static IItemQueryHandler<User>::Ptr createHandler(const Query &query)
    {
        return ItemQueryHandlerFactory::createUserItem(query);
    }
};

template<class T> class ItemQueryContainerPrivate;
template<> class ItemQueryContainerPrivate<Tweet>
{
public:
    static typename ItemQueryContainer::QueryMap<Tweet> & getQueries(ItemQueryContainer &container)
    {
        return container.m_tweetQueries;
    }
};

template<> class ItemQueryContainerPrivate<User>
{
public:
    static typename ItemQueryContainer::QueryMap<User> & getQueries(ItemQueryContainer &container)
    {
        return container.m_userQueries;
    }
};

ItemQueryContainer::ItemQueryContainer(IQueryExecutor::ConstPtr queryExecutor)
    : m_queryExecutor(std::move(queryExecutor))
{
    Q_ASSERT_X(m_queryExecutor, "TweetRepositoryContainer", "NULL query executor");
}

bool ItemQueryContainer::executeQuery(const Account &account, const Query &query,
                                      IItemListener<Tweet> &listener)
{
    return doExecuteQuery<Tweet>(account, query, &listener);
}

bool ItemQueryContainer::executeQuery(const Account &account, const Query &query,
                                      IItemListener<User> &listener)
{
    return doExecuteQuery<User>(account, query, &listener);
}

template<class T>
ItemQueryContainer::Data<T> * ItemQueryContainer::getMappingData(const ContainerKey &key)
{
    QueryMap<T> &queries (ItemQueryContainerPrivate<T>::getQueries(*this));
    auto it = queries.find(key);
    if (it != std::end(queries)) {
        return &(it->second);
    }

    typename IItemQueryHandler<T>::Ptr handler {ItemQueryHandlerCreator<T>::createHandler(key.query())};

    if (!handler) {
        return nullptr;
    }
    return &(queries.emplace(key, Data<T>{std::move(handler)}).first->second);
}

template<class T>
bool ItemQueryContainer::doExecuteQuery(const Account &account, const Query &query,
                                        IItemListener<T> *listener)
{
    Data<T> *data {getMappingData<T>(ContainerKey{Account{account}, Query{query}})};
    if (data == nullptr) {
        return false;
    }
    data->listeners.insert(listener);

    for (IItemListener<T> *listener : data->listeners) {
        listener->onStart();
    }

    if (data->loading) {
        return true;
    }

    data->loading = true;

    QByteArray path {query.path()};
    Query::Parameters parameters (query.parameters());

    m_queryExecutor->execute(query.requestType(), path, parameters, account, [this, data, account, query](QIODevice &reply, QNetworkReply::NetworkError error, const QString &errorMessage) {
        private_util::ItemQueryCallback<T> callback {
            *data->handler,
            data->listeners
        };
        callback(reply, error, errorMessage);
        QueryMap<T> &queries = ItemQueryContainerPrivate<T>::getQueries(*this);
        queries.erase(ContainerKey{Account{account}, Query{query}});
    });
    return true;
}

template<class T>
ItemQueryContainer::Data<T>::Data(typename IItemQueryHandler<T>::Ptr &&inputHandler)
    : handler(std::move(inputHandler))
{
}
