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
#include <qml/accountmodel.h>

TEST(account, Account)
{
    Account account {
        QLatin1String("Test Account"),
        QLatin1String("1"),
        QLatin1String("account"),
        QByteArray("token"),
        QByteArray("tokenSecret")
    };

    Account movedAccount {std::move(account)};
    EXPECT_FALSE(account.isValid());
    EXPECT_TRUE(movedAccount.isValid());
    EXPECT_EQ(movedAccount.name(), QLatin1String("Test Account"));
    EXPECT_EQ(movedAccount.userId(), QLatin1String("1"));
    EXPECT_EQ(movedAccount.screenName(), QLatin1String("account"));
    EXPECT_EQ(movedAccount.token(), QByteArray("token"));
    EXPECT_EQ(movedAccount.tokenSecret(), QByteArray("tokenSecret"));

    movedAccount.setName(QLatin1String("New Account"));
    EXPECT_EQ(movedAccount.name(), QLatin1String("New Account"));
}

TEST(account, AccountRepository)
{
    AccountRepository repository {};
    for (int i = 0; i < 4; ++i) {
        repository.append(Account(QString::number(i + 1), QString(), QString(),
                                  QByteArray(), QByteArray()));
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

static qml::AccountObject * getObject(qml::AccountModel &model, int index)
{
    return model.data(model.index(index), qml::AccountModel::AccountRole).value<qml::AccountObject *>();
}

TEST(account, AccountModel)
{
    qml::DataRepositoryObject repositoryObject {};
    AccountRepository &repository (repositoryObject.accounts());

    for (int i = 0; i < 3; ++i) {
        repository.append(Account(QString::number(i + 1), QString(), QString(),
                                         QByteArray(), QByteArray()));
    }

    qml::AccountModel model {};
    model.classBegin();
    model.setRepository(&repositoryObject);
    model.componentComplete();

    EXPECT_EQ(model.count(), 3);
    EXPECT_EQ(getObject(model, 0)->name(), QString::number(1));
    EXPECT_EQ(getObject(model, 1)->name(), QString::number(2));
    EXPECT_EQ(getObject(model, 2)->name(), QString::number(3));

    repository.append(Account(QString::number(4), QString(), QString(),
                              QByteArray(), QByteArray()));

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
