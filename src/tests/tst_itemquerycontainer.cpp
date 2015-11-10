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
#include <itemquerycontainer.h>
#include "mockqueryexecutor.h"
#include "mockitemlistener.h"

using testing::Return;
using testing::_;

class itemquerycontainer: public testing::Test
{
public:
    explicit itemquerycontainer()
        : account(QLatin1String("test"), QLatin1String("test"), QLatin1String("test"), QByteArray(), QByteArray())
    {
        queryExecutor = new MockQueryExecutor();
        container.reset(new ItemQueryContainer(IQueryExecutor::ConstPtr(queryExecutor)));
    }
protected:
    MockQueryExecutor *queryExecutor {nullptr};
    Account account;
    std::unique_ptr<ItemQueryContainer> container {nullptr};
};

TEST_F(itemquerycontainer, Base)
{
    EXPECT_CALL(*queryExecutor, makeError(_, _, _)).WillRepeatedly(Return(QNetworkReply::NoError));
    EXPECT_CALL(*queryExecutor, makeErrorMessage(_, _, _)).WillRepeatedly(Return(QString()));
    EXPECT_CALL(*queryExecutor, makeReply(QByteArray{"favorites/create.json"}, _, _))
            .Times(1).WillRepeatedly(Return(QByteArray("{}")));

    MockItemListener listener;
    EXPECT_CALL(listener, onStart()).Times(1);
    EXPECT_CALL(listener, onError(_)).Times(0);
    EXPECT_CALL(listener, handleFinish(_)).Times(1);

    TweetItemQuery query {TweetItemQuery::Favorite, Query::Parameters{{"id", "0"}}};
    container->executeQuery(account, query, listener);
}

