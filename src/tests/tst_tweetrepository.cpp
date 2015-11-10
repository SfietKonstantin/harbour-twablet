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

#include <gtest/gtest.h>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <tweetrepositorycontainer.h>
#include "mockqueryexecutor.h"
#include "testrepositorylistener.h"

using testing::Return;
using testing::_;

static std::ostream & operator<<(std::ostream &os, const TestRepositoryListener<Tweet>::Data &data)
{
    std::stringstream insertedIds {};
    for (auto it = std::begin(data.insertedIds); it != std::end(data.insertedIds); ++it) {
        if (it != std::begin(data.insertedIds)) {
            insertedIds << ",";
        }
        insertedIds << it->toLocal8Bit().data();
    }

    os << "(status:" << data.status << ", insertion_type:" << data.insertionType
       << ", inserted_ids:" << insertedIds.str() << ", updated_id:"
       << data.updatedId.toLocal8Bit().data() << ", index:" << data.index1 << ")";
    return os;
}

class tweetrepository: public testing::Test, protected TestRepositoryListener<Tweet>
{
public:
    explicit tweetrepository()
        : account(QLatin1String("test"), QLatin1String("test"), QLatin1String("test"),
                  QByteArray("test"), QByteArray("test"))
    {
        queryExecutor = new MockQueryExecutor();
        repository.reset(new TweetRepositoryContainer(IQueryExecutor::ConstPtr(queryExecutor)));
    }
protected:
    MockQueryExecutor *queryExecutor {nullptr};
    Account account;
    std::unique_ptr<TweetRepositoryContainer> repository {nullptr};
};

TEST_F(tweetrepository, SupportedTypes)
{
    {
        const TweetRepositoryQuery query {TweetRepositoryQuery::Home, Query::Parameters()};
        repository->referenceQuery(account, query);
        const std::set<Query> queries (repository->referencedQueries(account));
        EXPECT_EQ(queries.size(), 1);
        EXPECT_TRUE(queries.find(query) != std::end(queries));
    }
    {
        const TweetRepositoryQuery query {TweetRepositoryQuery::Mentions, Query::Parameters()};
        repository->referenceQuery(account, query);
        const std::set<Query> queries (repository->referencedQueries(account));
        EXPECT_EQ(queries.size(), 2);
        EXPECT_TRUE(queries.find(query) != std::end(queries));
    }
    {
        const TweetRepositoryQuery query {TweetRepositoryQuery::Search, Query::Parameters{{"q", "test"}}};
        repository->referenceQuery(account, query);
        const std::set<Query> queries (repository->referencedQueries(account));
        EXPECT_EQ(queries.size(), 3);
        EXPECT_TRUE(queries.find(query) != std::end(queries));
    }
    {
        const TweetRepositoryQuery query {TweetRepositoryQuery::Favorites, Query::Parameters{{"user_id", "123"}}};
        repository->referenceQuery(account, query);
        const std::set<Query> queries (repository->referencedQueries(account));
        EXPECT_EQ(queries.size(), 4);
        EXPECT_TRUE(queries.find(query) != std::end(queries));
    }
    {
        const TweetRepositoryQuery query {TweetRepositoryQuery::UserTimeline, Query::Parameters{{"user_id", "123"}}};
        repository->referenceQuery(account, query);
        const std::set<Query> queries (repository->referencedQueries(account));
        EXPECT_EQ(queries.size(), 5);
        EXPECT_TRUE(queries.find(query) != std::end(queries));
    }
}

TEST_F(tweetrepository, UnsupportedTypes)
{
    {
        const UserRepositoryQuery query {UserRepositoryQuery::Friends, Query::Parameters{{"user_id", "123"}}};
        repository->referenceQuery(account, query);
        const std::set<Query> queries (repository->referencedQueries(account));
        EXPECT_EQ(queries.size(), 0);
        EXPECT_TRUE(queries.find(query) == std::end(queries));
    }
    {
        const UserRepositoryQuery query {UserRepositoryQuery::Followers, Query::Parameters{{"user_id", "123"}}};
        repository->referenceQuery(account, query);
        const std::set<Query> queries (repository->referencedQueries(account));
        EXPECT_EQ(queries.size(), 0);
        EXPECT_TRUE(queries.find(query) == std::end(queries));
    }
}

TEST_F(tweetrepository, RefDeref)
{
    EXPECT_CALL(*queryExecutor, makeError(_, _, _)).WillRepeatedly(Return(QNetworkReply::NoError));
    EXPECT_CALL(*queryExecutor, makeErrorMessage(_, _, _)).WillRepeatedly(Return(QString()));
    EXPECT_CALL(*queryExecutor, makeReply(QByteArray{"statuses/home_timeline.json"}, _, _))
            .Times(2).WillRepeatedly(Return(QByteArray()));
    EXPECT_CALL(*queryExecutor, makeReply(QByteArray{"statuses/mentions_timeline.json"}, _, _))
            .Times(1).WillRepeatedly(Return(QByteArray()));

    {
        TweetRepositoryQuery query {TweetRepositoryQuery::Home, Query::Parameters()};
        repository->referenceQuery(account, query);
        const std::set<Query> queries (repository->referencedQueries(account));
        EXPECT_EQ(queries.size(), 1);
        EXPECT_TRUE(queries.find(query) != std::end(queries));
    }
    {
        TweetRepositoryQuery query {TweetRepositoryQuery::Mentions, Query::Parameters()};
        repository->referenceQuery(account, query);
        const std::set<Query> queries (repository->referencedQueries(account));
        EXPECT_EQ(queries.size(), 2);
        EXPECT_TRUE(queries.find(query) != std::end(queries));
    }

    repository->refresh();

    {
        TweetRepositoryQuery query {TweetRepositoryQuery::Mentions, Query::Parameters()};
        repository->dereferenceQuery(account, query);
        const std::set<Query> queries (repository->referencedQueries(account));
        EXPECT_EQ(queries.size(), 1);
        EXPECT_TRUE(queries.find(query) == std::end(queries));
    }

    repository->refresh();
}

TEST_F(tweetrepository, ErrorManagement)
{
    // First send a network error
    // Then send a twitter error (rate limit)
    // Then send a malformatted reply
    // Then send a correct data
    EXPECT_CALL(*queryExecutor, makeError(_, _, _)).Times(4)
            .WillOnce(Return(QNetworkReply::ContentNotFoundError))
            .WillOnce(Return(QNetworkReply::ContentOperationNotPermittedError))
            .WillOnce(Return(QNetworkReply::NoError))
            .WillOnce(Return(QNetworkReply::NoError));
    EXPECT_CALL(*queryExecutor, makeErrorMessage(_, _, _)).WillRepeatedly(Return(QString()));

    QJsonObject twitterRateLimit {};
    {
        QJsonArray errors {};
        QJsonObject error {};
        error.insert(QLatin1String{"code"}, 88);
        errors.append(error);
        twitterRateLimit.insert(QLatin1String{"errors"}, errors);
    }
    QJsonArray twitterReply {};
    {
        QJsonObject tweet {};
        tweet.insert(QLatin1String{"id_str"}, QLatin1String("3"));
        tweet.insert(QLatin1String{"text"}, QLatin1String("Test text 3"));
        tweet.insert(QLatin1String{"favorite_count"}, 0);
        tweet.insert(QLatin1String{"favorited"}, false);
        tweet.insert(QLatin1String{"retweet_count"}, 0);
        tweet.insert(QLatin1String{"retweeted"}, false);
        tweet.insert(QLatin1String{"source"}, QLatin1String{"Test source 3"});
        tweet.insert(QLatin1String{"created_at"}, QLatin1String{"Tue Aug 28 21:16:23 +0000 2012"});

        QJsonObject user {};
        user.insert(QLatin1String{"id_str"}, QLatin1String{"10"});
        user.insert(QLatin1String{"name"}, QLatin1String{"Test user 10"});
        user.insert(QLatin1String{"screen_name"}, QLatin1String{"test_user_10"});
        user.insert(QLatin1String{"description"}, QLatin1String{"Test description 10"});
        user.insert(QLatin1String{"protected"}, false);
        user.insert(QLatin1String{"following"}, true);
        user.insert(QLatin1String{"statuses_count"}, 123);
        user.insert(QLatin1String{"followers_count"}, 456);
        user.insert(QLatin1String{"friends_count"}, 789);
        user.insert(QLatin1String{"listed_count"}, 1234);
        user.insert(QLatin1String{"favourites_count"}, 5678);
        user.insert(QLatin1String{"profile_image_url_https"}, QLatin1String{"http://a0.twimg.com/profile_images/730275945/oauth-dancer_normal.jpg"});
        user.insert(QLatin1String{"created_at"}, QLatin1String{"Wed Mar 03 19:37:35 +0000 2010"});

        tweet.insert(QLatin1String{"user"}, user);
        twitterReply.append(tweet);
    }
    {
        QJsonObject tweet {};
        tweet.insert(QLatin1String{"id_str"}, QLatin1String("2"));
        tweet.insert(QLatin1String{"text"}, QLatin1String("Test text 2"));
        tweet.insert(QLatin1String{"favorite_count"}, 88);
        tweet.insert(QLatin1String{"favorited"}, true);
        tweet.insert(QLatin1String{"retweet_count"}, 77);
        tweet.insert(QLatin1String{"retweeted"}, false);
        tweet.insert(QLatin1String{"source"}, QLatin1String{"Test source 2"});
        tweet.insert(QLatin1String{"created_at"}, QLatin1String{"Wed Mar 07 22:23:19 +0000 2007"});

        QJsonObject user {};
        user.insert(QLatin1String{"id_str"}, QLatin1String{"9"});
        user.insert(QLatin1String{"name"}, QLatin1String{"Test user 9"});
        user.insert(QLatin1String{"screen_name"}, QLatin1String{"test_user_9"});
        user.insert(QLatin1String{"description"}, QLatin1String{"Test description 9"});
        user.insert(QLatin1String{"protected"}, true);
        user.insert(QLatin1String{"following"}, false);
        user.insert(QLatin1String{"statuses_count"}, 12);
        user.insert(QLatin1String{"followers_count"}, 34);
        user.insert(QLatin1String{"friends_count"}, 56);
        user.insert(QLatin1String{"listed_count"}, 78);
        user.insert(QLatin1String{"favourites_count"}, 90);
        user.insert(QLatin1String{"profile_image_url_https"}, QLatin1String{"http://a0.twimg.com/profile_images/1270234259/raffi-headshot-casual_normal.png"});
        user.insert(QLatin1String{"created_at"}, QLatin1String{"Sun Aug 19 14:24:06 +0000 2007"});

        tweet.insert(QLatin1String{"user"}, user);
        twitterReply.append(tweet);
    }
    EXPECT_CALL(*queryExecutor, makeReply(_, _, _)).Times(4)
            .WillOnce(Return(QByteArray()))
            .WillOnce(Return(QJsonDocument(twitterRateLimit).toJson()))
            .WillOnce(Return(QByteArray("{")))
            .WillOnce(Return(QByteArray(QJsonDocument(twitterReply).toJson())));

    TweetRepositoryQuery query {TweetRepositoryQuery::Home, Query::Parameters()};
    repository->referenceQuery(account, query);
    const std::set<Query> queries (repository->referencedQueries(account));
    EXPECT_EQ(queries.size(), 1);
    EXPECT_TRUE(queries.find(query) != std::end(queries));

    TweetRepository *homeTimeline {repository->repository(account, query)};
    EXPECT_TRUE(homeTimeline != nullptr);
    homeTimeline->addListener(*this);

    repository->refresh();
    EXPECT_EQ(data.size(), 2);
    EXPECT_EQ(data.at(0), Data(Data::createLoading()));
    EXPECT_EQ(data.at(1), Data(Data::createError()));

    repository->refresh();
    EXPECT_EQ(data.size(), 4);
    EXPECT_EQ(data.at(2), Data(Data::createLoading()));
    EXPECT_EQ(data.at(3), Data(Data::createError()));

    repository->refresh();
    EXPECT_EQ(data.size(), 6);
    EXPECT_EQ(data.at(4), Data(Data::createLoading()));
    EXPECT_EQ(data.at(5), Data(Data::createError()));

    repository->refresh();
    EXPECT_EQ(data.size(), 9);
    EXPECT_EQ(data.at(6), Data(Data::createLoading()));
    EXPECT_EQ(data.at(7), Data(Data::createPrepend({QLatin1String("3"), QLatin1String("2")})));
    EXPECT_EQ(data.at(8), Data(Data::createIdle()));
}
