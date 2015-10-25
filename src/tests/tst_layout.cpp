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
#include <qml/datarepositoryobject.h>
#include <qml/layoutmodel.h>
#include "debughelpers.h"

TEST(layout, Layout)
{
    Query::Parameters parameters {
        {"user_id", "12345"},
        {"exclude_replies", "true"},
        {"include_rts", "false"}
    };
    TweetListQuery query {TweetListQuery::UserTimeline, std::move(parameters)};
    EXPECT_TRUE(query.isValid());

    Layout layout {
        QLatin1String("Layout name"),
        QLatin1String("userId"),
        std::move(TweetListQuery(query)) // Copy query
    };

    EXPECT_TRUE(layout.isValid());
    EXPECT_EQ(layout.query(), query);
    EXPECT_EQ(layout.unread(), 0);

    Layout movedLayout {std::move(layout)};
    EXPECT_FALSE(layout.isValid());
    EXPECT_TRUE(movedLayout.isValid());
    EXPECT_TRUE(movedLayout.query().isValid());
    EXPECT_EQ(movedLayout.name(), QLatin1String("Layout name"));
    EXPECT_EQ(movedLayout.accountUserId(), QLatin1String("userId"));
    EXPECT_EQ(movedLayout.query(), query);
    EXPECT_EQ(movedLayout.unread(), 0);

    movedLayout.setName(QLatin1String("New layout"));
    EXPECT_EQ(movedLayout.name(), QLatin1String("New layout"));
    movedLayout.setAccountUserId(QLatin1String("1"));
    EXPECT_EQ(movedLayout.accountUserId(), QLatin1String("1"));

    Query::Parameters newParameters {
        {"user_id", "123"}
    };
    TweetListQuery newQuery {TweetListQuery::Favorites, std::move(newParameters)};
    movedLayout.setQuery(std::move(TweetListQuery(newQuery))); // Copy query
    EXPECT_EQ(movedLayout.query(), newQuery);
    movedLayout.setUnread(123);
    EXPECT_EQ(movedLayout.unread(), 123);
}

TEST(layout, LayoutRepository)
{
    LayoutRepository repository {};
    for (int i = 0; i < 4; ++i) {
        TweetListQuery query {TweetListQuery::Home, Query::Parameters()};
        repository.append(Layout(QString::number(i + 1), QString(), std::move(query)));
    }
    EXPECT_EQ((std::begin(repository) + 0)->name(), QString::number(1));
    EXPECT_EQ((std::begin(repository) + 1)->name(), QString::number(2));
    EXPECT_EQ((std::begin(repository) + 2)->name(), QString::number(3));
    EXPECT_EQ((std::begin(repository) + 3)->name(), QString::number(4));
    EXPECT_TRUE(std::begin(repository) + 4 == std::end(repository));
    repository.remove(1);
    EXPECT_EQ((std::begin(repository) + 0)->name(), QString::number(1));
    EXPECT_EQ((std::begin(repository) + 1)->name(), QString::number(3));
    EXPECT_EQ((std::begin(repository) + 2)->name(), QString::number(4));
    EXPECT_TRUE(std::begin(repository) + 3 == std::end(repository));
}

static qml::LayoutObject * getObject(qml::LayoutModel &model, int index)
{
    return model.data(model.index(index), qml::LayoutModel::LayoutRole).value<qml::LayoutObject *>();
}

TEST(layout, LayoutModel)
{
    qml::DataRepositoryObject repositoryObject {};
    LayoutRepository &repository (repositoryObject.layouts());

    for (int i = 0; i < 3; ++i) {
        TweetListQuery query {TweetListQuery::Home, std::move(Query::Parameters())};
        repository.append(Layout(QString::number(i + 1), QString(), std::move(query)));
    }

    qml::LayoutModel model {};
    model.classBegin();
    model.setRepository(&repositoryObject);
    model.componentComplete();

    EXPECT_EQ(model.count(), 3);
    EXPECT_EQ(getObject(model, 0)->name(), QString::number(1));
    EXPECT_EQ(getObject(model, 1)->name(), QString::number(2));
    EXPECT_EQ(getObject(model, 2)->name(), QString::number(3));

    {
        TweetListQuery query {TweetListQuery::Home, std::move(Query::Parameters())};
        repository.append(Layout(QString::number(4), QString(), std::move(query)));
    }

    EXPECT_EQ(model.count(), 4);
    EXPECT_EQ(getObject(model, 0)->name(), QString::number(1));
    EXPECT_EQ(getObject(model, 1)->name(), QString::number(2));
    EXPECT_EQ(getObject(model, 2)->name(), QString::number(3));
    EXPECT_EQ(getObject(model, 3)->name(), QString::number(4));

    repository.remove(1);
    EXPECT_EQ(model.count(), 3);
    EXPECT_EQ(getObject(model, 0)->name(), QString::number(1));
    EXPECT_EQ(getObject(model, 1)->name(), QString::number(3));
    EXPECT_EQ(getObject(model, 2)->name(), QString::number(4));
}

