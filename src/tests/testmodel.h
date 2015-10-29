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

#ifndef TESTMODEL_H
#define TESTMODEL_H

#include <repository.h>
#include <qml/model.h>

class TestData
{
public:
    explicit TestData() = default;
    DEFAULT_COPY_DEFAULT_MOVE(TestData);
    explicit TestData(int value);
    int value() const;
private:
    int m_value {-1};
};

class TestDataObject: public QObject
{
    Q_OBJECT
public:
    static TestDataObject * create(const TestData &data, QObject *parent = 0);
    int value() const;
    void update(const TestData &data);
private:
    explicit TestDataObject(const TestData &data, QObject *parent = 0);
    TestData m_data {};
};

Q_DECLARE_METATYPE(TestDataObject *)

using TestRepository = Repository<TestData>;

class TestRepositoryContainer: public QObject
{
    Q_OBJECT
public:
    explicit TestRepositoryContainer(QObject *parent = 0);
    TestRepository & repository();
private:
    TestRepository m_repository {};
};

namespace qml
{

template<> class DataRepositoryObjectMap<TestData>
{
public:
    static void getQueryInfo(QObject *queryWrapper, QString &accountUserId, Query &query)
    {
        Q_UNUSED(queryWrapper)
        Q_UNUSED(accountUserId)
        Q_UNUSED(query)
    }
    static Repository<TestData> * get(QObject &object, const QString &accountUserId, const Query &query)
    {
        Q_UNUSED(accountUserId)
        Q_UNUSED(query)
        TestRepositoryContainer *container = qobject_cast<TestRepositoryContainer *>(&object);
        return container != nullptr ? &(container->repository()) : nullptr;
    }
    static void addListener(Repository<TestData> &repository, IRepositoryListener<TestData> &listener,
                            QObject *object, const QString &accountUserId, const Query &query)
    {
        Q_UNUSED(object)
        Q_UNUSED(accountUserId)
        Q_UNUSED(query)
        repository.addListener(listener);
    }
    static void removeListener(Repository<TestData> &repository, IRepositoryListener<TestData> &listener,
                               QObject *object, const QString &accountUserId, const Query &query)
    {
        Q_UNUSED(object)
        Q_UNUSED(accountUserId)
        Q_UNUSED(query)
        repository.removeListener(listener);
    }
};

}

class TestModel: public qml::Model<TestData, TestDataObject>
{
    Q_OBJECT
public:
    enum Roles {
        ValueRole = Qt::UserRole + 1,
        ItemRole
    };
    explicit TestModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role) const override final;
private:
    QHash<int, QByteArray> roleNames() const override final;

};

#endif // TESTMODEL_H
