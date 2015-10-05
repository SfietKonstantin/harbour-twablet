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
#include <qml/datarepositoryobject.h>
#include <qml/layoutmodel.h>
#include <QtCore/QDebug>

namespace tst
{

class TstLayout: public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testLayout()
    {
        Query::Parameters parameters {
            {QLatin1String("test1"), QLatin1String("value1")},
            {QLatin1String("test2"), QLatin1String("value2")}
        };
        Query query {Query::Mentions, std::move(Query::Parameters(parameters))}; // Copy parameters
        Layout layout {
            QLatin1String("Layout name"),
            QLatin1String("userId"),
            std::move(query)
        };

        QVERIFY(layout.query().isValid());
        QCOMPARE(layout.query().type(), Query::Mentions);
        QCOMPARE(layout.query().parameters(), parameters);
        QCOMPARE(layout.unread(), 0);

        Layout movedLayout {std::move(layout)};
        QVERIFY(!layout.isValid());
        QVERIFY(movedLayout.isValid());
        QVERIFY(movedLayout.query().isValid());
        QCOMPARE(movedLayout.name(), QLatin1String("Layout name"));
        QCOMPARE(movedLayout.userId(), QLatin1String("userId"));
        QCOMPARE(movedLayout.query().type(), Query::Mentions);
        QCOMPARE(movedLayout.query().parameters(), parameters);
        QCOMPARE(movedLayout.unread(), 0);

        movedLayout.setName(QLatin1String("New layout"));
        QCOMPARE(movedLayout.name(), QLatin1String("New layout"));
        movedLayout.setUserId(QLatin1String("1"));
        QCOMPARE(movedLayout.userId(), QLatin1String("1"));

        Query::Parameters newParameters {
            {QLatin1String("newTest1"), QLatin1String("newValue1")},
            {QLatin1String("newTest2"), QLatin1String("newValue2")}
        };
        Query newQuery {Query::Friends, std::move(Query::Parameters(newParameters))}; // Copy parameters
        movedLayout.setQuery(std::move(newQuery));
        QCOMPARE(movedLayout.query().type(), Query::Friends);
        QCOMPARE(movedLayout.query().parameters(), newParameters);
        movedLayout.setUnread(123);
        QCOMPARE(movedLayout.unread(), 123);

    }

    void testRepo()
    {
        LayoutRepository repository {};
        for (int i = 0; i < 4; ++i) {
            Query query {Query::Home, std::move(Query::Parameters())};
            repository.append(Layout(QString::number(i + 1), QString(), std::move(query)));
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
        qml::DataRepositoryObject repositoryObject {};
        LayoutRepository &repository (repositoryObject.layouts());

        for (int i = 0; i < 3; ++i) {
            Query query {Query::Home, std::move(Query::Parameters())};
            repository.append(Layout(QString::number(i + 1), QString(), std::move(query)));
        }

        qml::LayoutModel model {};
        model.classBegin();
        model.setRepository(&repositoryObject);
        model.componentComplete();

        QCOMPARE(model.count(), 3);
        QCOMPARE(getObject(model, 0)->name(), QString::number(1));
        QCOMPARE(getObject(model, 1)->name(), QString::number(2));
        QCOMPARE(getObject(model, 2)->name(), QString::number(3));

        {
            Query query {Query::Home, std::move(Query::Parameters())};
            repository.append(Layout(QString::number(4), QString(), std::move(query)));
        }

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
    static qml::LayoutObject * getObject(qml::LayoutModel &model, int index)
    {
        return model.data(model.index(index), qml::LayoutModel::LayoutRole).value<qml::LayoutObject *>();
    }
};

}

QTEST_MAIN(tst::TstLayout)

#include "tst_layout.moc"

