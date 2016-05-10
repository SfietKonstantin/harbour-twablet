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

#ifndef ITEMQUERYCONTAINER_H
#define ITEMQUERYCONTAINER_H

#include <map>
#include <set>
#include "account.fwd.h"
#include "containerkey.h"
#include "globals.h"
#include "iitemqueryhandler.h"
#include "iqueryexecutor.h"
#include "iitemlistener.h"
#include "query.h"

class User;
class Tweet;
class ItemQueryContainer
{
public:
    explicit ItemQueryContainer(IQueryExecutor::ConstPtr queryExecutor);
    DISABLE_COPY_DEFAULT_MOVE(ItemQueryContainer);
    bool executeQuery(const Account &account, const Query &query,
                      IItemListener<Tweet> &listener);
    bool executeQuery(const Account &account, const Query &query,
                      IItemListener<User> &listener);
private:
    template<class T> class Data
    {
    public:
        Data(typename IItemQueryHandler<T>::Ptr &&inputHandler);
        typename IItemQueryHandler<T>::Ptr handler;
        std::set<IItemListener<T> *> listeners;
        bool loading {false};
    };
    template<class T> using QueryMap = std::map<ContainerKey, Data<T>>;
    template<class T> Data<T> * getMappingData(const ContainerKey &key);
    template<class T> bool doExecuteQuery(const Account &account, const Query &query,
                                          IItemListener<T> *listeners);
    IQueryExecutor::ConstPtr m_queryExecutor {nullptr};
    QueryMap<Tweet> m_tweetQueries {};
    QueryMap<User> m_userQueries {};
    template<class T> friend class ItemQueryContainerPrivate;
};

#endif // ITEMQUERYCONTAINER_H
