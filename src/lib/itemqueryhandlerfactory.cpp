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

#include "itemqueryhandlerfactory.h"
#include <QtCore/QLoggingCategory>
#include "private/debughelper.h"
#include "tweetitemqueryhandler.h"
#include "useritemqueryhandler.h"
#include "query.h"

static const QLoggingCategory logger {"item-query-handler-factory"};

IItemQueryHandler<Tweet>::Ptr ItemQueryHandlerFactory::createTweetItem(const Query &query)
{
    if (query.path() == TweetItemQuery::pathFromType(TweetItemQuery::Show)
        || query.path() == TweetItemQuery::pathFromType(TweetItemQuery::StatusUpdate)
        || query.path() == TweetItemQuery::pathFromType(TweetItemQuery::Favorite)
        || query.path() == TweetItemQuery::pathFromType(TweetItemQuery::Unfavorite)
        || query.path() == TweetItemQuery::pathFromType(TweetItemQuery::Retweet)) {
        return TweetItemQueryHandler::create();
    } else {
        qCWarning(logger) << "Cannot create query handler from query" << query;
        return IItemQueryHandler<Tweet>::Ptr();
    }
}

IItemQueryHandler<User>::Ptr ItemQueryHandlerFactory::createUserItem(const Query &query)
{
    if (query.path() == UserItemQuery::pathFromType(UserItemQuery::Show)
        || query.path() == UserItemQuery::pathFromType(UserItemQuery::Follow)
        || query.path() == UserItemQuery::pathFromType(UserItemQuery::Unfollow)) {
        return UserItemQueryHandler::create();
    } else {
        qCWarning(logger) << "Cannot create query handler from query" << query;
        return IItemQueryHandler<User>::Ptr();
    }
}

