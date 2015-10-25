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

#ifndef QUERYTYPEOBJECT_H
#define QUERYTYPEOBJECT_H

#include <QtCore/QObject>
#include "query.h"

namespace qml
{

class QueryTypeObject : public QObject
{
    Q_OBJECT
    Q_ENUMS(TweetListType)
    Q_ENUMS(UserListType)
    Q_ENUMS(TweetItemType)
    Q_ENUMS(UserItemType)
public:
    enum TweetListType
    {
        InvalidTweetList = TweetListQuery::Invalid,
        Home = TweetListQuery::Home,
        Mentions = TweetListQuery::Mentions,
        Search = TweetListQuery::Search,
        Favorites = TweetListQuery::Favorites,
        UserTimeline = TweetListQuery::UserTimeline,
    };
    enum UserListType
    {
        InvalidUserList = UserListQuery::Invalid,
        Friends = UserListQuery::Friends,
        Followers = UserListQuery::Followers
    };
    enum TweetItemType
    {
        InvalidTweetItem = TweetItemQuery::Invalid,
        ShowTweet = TweetItemQuery::Show,
        StatusUpdate = TweetItemQuery::StatusUpdate,
        Favorite = TweetItemQuery::Favorite,
        Unfavorite = TweetItemQuery::Unfavorite,
        Retweet = TweetItemQuery::Retweet
    };
    enum UserItemType
    {
        InvalidUserItem = UserItemQuery::Invalid,
        ShowUser = UserItemQuery::Show,
        Follow = UserItemQuery::Follow,
        Unfollow = UserItemQuery::Unfollow
    };

    DISABLE_COPY_DISABLE_MOVE(QueryTypeObject);
private:
    explicit QueryTypeObject(QObject *parent = 0);
};

}

#endif // QUERYTYPEOBJECT_H
