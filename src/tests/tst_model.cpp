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
#include "testmodel.h"

TEST(repository, Move)
{
    TestRepositoryContainer container {};
    container.repository().append(TestData(0));
    container.repository().append(TestData(1));
    container.repository().append(TestData(2));
    container.repository().append(TestData(3));

    EXPECT_EQ((std::begin(container.repository()) + 0)->value(), 0);
    EXPECT_EQ((std::begin(container.repository()) + 1)->value(), 1);
    EXPECT_EQ((std::begin(container.repository()) + 2)->value(), 2);
    EXPECT_EQ((std::begin(container.repository()) + 3)->value(), 3);

    container.repository().move(1, 0);
    EXPECT_EQ((std::begin(container.repository()) + 0)->value(), 1);
    EXPECT_EQ((std::begin(container.repository()) + 1)->value(), 0);
    EXPECT_EQ((std::begin(container.repository()) + 2)->value(), 2);
    EXPECT_EQ((std::begin(container.repository()) + 3)->value(), 3);

    container.repository().move(0, 2);
    EXPECT_EQ((std::begin(container.repository()) + 0)->value(), 0);
    EXPECT_EQ((std::begin(container.repository()) + 1)->value(), 1);
    EXPECT_EQ((std::begin(container.repository()) + 2)->value(), 2);
    EXPECT_EQ((std::begin(container.repository()) + 3)->value(), 3);

    container.repository().move(0, 3);
    EXPECT_EQ((std::begin(container.repository()) + 0)->value(), 1);
    EXPECT_EQ((std::begin(container.repository()) + 1)->value(), 2);
    EXPECT_EQ((std::begin(container.repository()) + 2)->value(), 0);
    EXPECT_EQ((std::begin(container.repository()) + 3)->value(), 3);

    container.repository().move(2, 4);
    EXPECT_EQ((std::begin(container.repository()) + 0)->value(), 1);
    EXPECT_EQ((std::begin(container.repository()) + 1)->value(), 2);
    EXPECT_EQ((std::begin(container.repository()) + 2)->value(), 3);
    EXPECT_EQ((std::begin(container.repository()) + 3)->value(), 0);

    container.repository().move(1, 3);
    EXPECT_EQ((std::begin(container.repository()) + 0)->value(), 1);
    EXPECT_EQ((std::begin(container.repository()) + 1)->value(), 3);
    EXPECT_EQ((std::begin(container.repository()) + 2)->value(), 2);
    EXPECT_EQ((std::begin(container.repository()) + 3)->value(), 0);

    // Invalid move
    container.repository().move(0, 5);
    EXPECT_EQ((std::begin(container.repository()) + 0)->value(), 1);
    EXPECT_EQ((std::begin(container.repository()) + 1)->value(), 3);
    EXPECT_EQ((std::begin(container.repository()) + 2)->value(), 2);
    EXPECT_EQ((std::begin(container.repository()) + 3)->value(), 0);

    container.repository().move(4, 0);
    EXPECT_EQ((std::begin(container.repository()) + 0)->value(), 1);
    EXPECT_EQ((std::begin(container.repository()) + 1)->value(), 3);
    EXPECT_EQ((std::begin(container.repository()) + 2)->value(), 2);
    EXPECT_EQ((std::begin(container.repository()) + 3)->value(), 0);

    container.repository().move(0, 1);
    EXPECT_EQ((std::begin(container.repository()) + 0)->value(), 1);
    EXPECT_EQ((std::begin(container.repository()) + 1)->value(), 3);
    EXPECT_EQ((std::begin(container.repository()) + 2)->value(), 2);
    EXPECT_EQ((std::begin(container.repository()) + 3)->value(), 0);
}

static int getValue(TestModel &model, int index)
{
    return model.data(model.index(index), TestModel::ValueRole).toInt();
}

TEST(model, LocalMove)
{
    TestRepositoryContainer container {};
    container.repository().append(TestData(0));
    container.repository().append(TestData(1));
    container.repository().append(TestData(2));
    container.repository().append(TestData(3));

    TestModel model {};
    model.classBegin();
    model.setRepository(&container);
    model.componentComplete();

    EXPECT_EQ(model.count(), 4);
    EXPECT_EQ(getValue(model, 0), 0);
    EXPECT_EQ(getValue(model, 1), 1);
    EXPECT_EQ(getValue(model, 2), 2);
    EXPECT_EQ(getValue(model, 3), 3);

    model.startMove();
    model.move(1, 0);
    EXPECT_EQ(getValue(model, 0), 1);
    EXPECT_EQ(getValue(model, 1), 0);
    EXPECT_EQ(getValue(model, 2), 2);
    EXPECT_EQ(getValue(model, 3), 3);

    model.move(0, 2);
    EXPECT_EQ(getValue(model, 0), 0);
    EXPECT_EQ(getValue(model, 1), 1);
    EXPECT_EQ(getValue(model, 2), 2);
    EXPECT_EQ(getValue(model, 3), 3);

    model.move(0, 3);
    EXPECT_EQ(getValue(model, 0), 1);
    EXPECT_EQ(getValue(model, 1), 2);
    EXPECT_EQ(getValue(model, 2), 0);
    EXPECT_EQ(getValue(model, 3), 3);

    model.move(2, 4);
    EXPECT_EQ(getValue(model, 0), 1);
    EXPECT_EQ(getValue(model, 1), 2);
    EXPECT_EQ(getValue(model, 2), 3);
    EXPECT_EQ(getValue(model, 3), 0);

    model.move(1, 3);
    EXPECT_EQ(getValue(model, 0), 1);
    EXPECT_EQ(getValue(model, 1), 3);
    EXPECT_EQ(getValue(model, 2), 2);
    EXPECT_EQ(getValue(model, 3), 0);

    // Invalid move
    model.move(0, 5);
    EXPECT_EQ(getValue(model, 0), 1);
    EXPECT_EQ(getValue(model, 1), 3);
    EXPECT_EQ(getValue(model, 2), 2);
    EXPECT_EQ(getValue(model, 3), 0);

    model.move(4, 0);
    EXPECT_EQ(getValue(model, 0), 1);
    EXPECT_EQ(getValue(model, 1), 3);
    EXPECT_EQ(getValue(model, 2), 2);
    EXPECT_EQ(getValue(model, 3), 0);

    model.move(0, 1);
    EXPECT_EQ(getValue(model, 0), 1);
    EXPECT_EQ(getValue(model, 1), 3);
    EXPECT_EQ(getValue(model, 2), 2);
    EXPECT_EQ(getValue(model, 3), 0);
    model.endMove();
}
