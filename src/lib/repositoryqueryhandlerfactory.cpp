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

#include "repositoryqueryhandlerfactory.h"
#include "tweetrepositoryqueryhandler.h"
#include "tweetsearchqueryhandler.h"
#include "userrepositoryqueryhandler.h"
#include "listrepositoryqueryhandler.h"

IRepositoryQueryHandler<Tweet>::Ptr RepositoryQueryHandlerFactory::createTweet(const Query &query)
{
    if (query.path() == TweetRepositoryQuery::pathFromType(TweetRepositoryQuery::Home)
        || query.path() == TweetRepositoryQuery::pathFromType(TweetRepositoryQuery::Mentions)
        || query.path() == TweetRepositoryQuery::pathFromType(TweetRepositoryQuery::Favorites)
        || query.path() == TweetRepositoryQuery::pathFromType(TweetRepositoryQuery::UserTimeline)) {
        return TweetRepositoryQueryHandler::create();
    } else if (query.path() == TweetRepositoryQuery::pathFromType(TweetRepositoryQuery::Search)) {
        return TweetSearchQueryHandler::create();
    } else {
        return IRepositoryQueryHandler<Tweet>::Ptr();
    }
}

IRepositoryQueryHandler<User>::Ptr RepositoryQueryHandlerFactory::createUser(const Query &query)
{
    if (query.path() == UserRepositoryQuery::pathFromType(UserRepositoryQuery::Friends)
        || query.path() == UserRepositoryQuery::pathFromType(UserRepositoryQuery::Followers)) {
        return UserRepositoryQueryHandler::create();
    } else {
        return IRepositoryQueryHandler<User>::Ptr();
    }
}

IRepositoryQueryHandler<List>::Ptr RepositoryQueryHandlerFactory::createList(const Query &query)
{
    if (query.path() == ListRepositoryQuery::pathFromType(ListRepositoryQuery::Subscriptions)
        || query.path() == ListRepositoryQuery::pathFromType(ListRepositoryQuery::Ownerships)
        || query.path() == ListRepositoryQuery::pathFromType(ListRepositoryQuery::Memberships)) {
        return ListRepositoryQueryHandler::create();
    } else {
        return IRepositoryQueryHandler<List>::Ptr();
    }
}
