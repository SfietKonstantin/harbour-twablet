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

#ifndef TWEETREPOSITORYCONTAINER_H
#define TWEETREPOSITORYCONTAINER_H

#include <map>
#include "account.h"
#include "containerkey.h"
#include "globals.h"
#include "query.h"
#include "tweetrepository.h"
#include "irepositoryqueryhandler.h"
#include "iqueryexecutor.h"

class TweetRepositoryContainer
{
public:
    explicit TweetRepositoryContainer(IQueryExecutor::ConstPtr &&queryExecutor);
    DISABLE_COPY_DEFAULT_MOVE(TweetRepositoryContainer);
    TweetRepository * repository(const Account &account, const Query &query);
    void referenceQuery(const Account &account, const Query &query);
    void dereferenceQuery(const Account &account, const Query &query);
    std::set<Query> referencedQueries(const Account &account) const;
    void refresh();
    void refresh(const Account &account, const Query &query);
    void loadMore(const Account &account, const Query &query);
    Tweet tweet(const QString &id) const;
    void updateTweet(const Tweet &tweet);
private:
    struct Data
    {
        explicit Data(IRepositoryQueryHandler<Tweet>::Ptr &&inputHandler);
        bool loading {false};
        TweetRepository repository {};
        int refcount {0};
        IRepositoryQueryHandler<Tweet>::Ptr handler {};
    };
    void load(const ContainerKey &key, Data &mappingData,
              IRepositoryQueryHandler<Tweet>::RequestType requestType);
    Data * getMappingData(const ContainerKey &key);
    IQueryExecutor::ConstPtr m_queryExecutor {nullptr};
    std::map<QString, Tweet> m_data {};
    std::map<ContainerKey, Data> m_mapping {};
};

#endif // TWEETREPOSITORYCONTAINER_H
