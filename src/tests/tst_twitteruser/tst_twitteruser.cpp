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

#include <QtTest/QtTest>
#include <QtTest/QSignalSpy>
#include <loadsavemanager.h>
#include <qml/twitterdatarepositoryobject.h>
#include <qml/twitterusermodel.h>

class TstTwitterUser: public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase()
    {
        QVERIFY(LoadSaveManager().clear());
    }

    void testRepo()
    {
        TwitterUserRepository repository {};
        for (int i = 0; i < 4; ++i) {
            repository.add(TwitterUser(QString::number(i + 1), QString(), QString(),
                                       QByteArray(), QByteArray()));
        }
        QCOMPARE((std::begin(repository) + 0)->name(), QString::number(1));
        QCOMPARE((std::begin(repository) + 1)->name(), QString::number(2));
        QCOMPARE((std::begin(repository) + 2)->name(), QString::number(3));
        QCOMPARE((std::begin(repository) + 3)->name(), QString::number(4));
        QCOMPARE(std::begin(repository) + 4, std::end(repository));
        repository.remove(1);
        QCOMPARE((std::begin(repository) + 0)->name(), QString::number(1));
        QCOMPARE((std::begin(repository) + 1)->name(), QString::number(3));
        QCOMPARE((std::begin(repository) + 2)->name(), QString::number(4));
        QCOMPARE(std::begin(repository) + 3, std::end(repository));
    }
    void testModel()
    {
        TwitterDataRepositoryObject repositoryObject {};
        TwitterUserRepository &repository {repositoryObject.users()};

        for (int i = 0; i < 3; ++i) {
            repository.add(TwitterUser(QString::number(i + 1), QString(), QString(),
                                       QByteArray(), QByteArray()));
        }

        TwitterUserModel model {};
        model.classBegin();
        model.setRepository(&repositoryObject);
        model.componentComplete();

        QCOMPARE(model.count(), 3);
        QCOMPARE(getObject(model, 0)->name(), QString::number(1));
        QCOMPARE(getObject(model, 1)->name(), QString::number(2));
        QCOMPARE(getObject(model, 2)->name(), QString::number(3));

        repository.add(TwitterUser(QString::number(4), QString(), QString(),
                                   QByteArray(), QByteArray()));

        QCOMPARE(model.count(), 4);
        QCOMPARE(getObject(model, 0)->name(), QString::number(1));
        QCOMPARE(getObject(model, 1)->name(), QString::number(2));
        QCOMPARE(getObject(model, 2)->name(), QString::number(3));
        QCOMPARE(getObject(model, 3)->name(), QString::number(4));

        repository.remove(1);
        QCOMPARE(model.count(), 3);
        QCOMPARE(getObject(model, 0)->name(), QString::number(1));
        QCOMPARE(getObject(model, 1)->name(), QString::number(3));
        QCOMPARE(getObject(model, 2)->name(), QString::number(4));
    }
private:
    static TwitterUserObject * getObject(TwitterUserModel &model, int index)
    {
        return model.data(model.index(index), TwitterUserModel::UserRole).value<TwitterUserObject *>();
    }
};


QTEST_MAIN(TstTwitterUser)

#include "tst_twitteruser.moc"

