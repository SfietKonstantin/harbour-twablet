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

#ifndef TESTREPOSITORYLISTENER_H
#define TESTREPOSITORYLISTENER_H

#include <irepositorylistener.h>
#include <QtCore/QString>

template<class T>
class TestRepositoryListener: public IRepositoryListener<T>
{
public:
    class Data
    {
    public:
        enum Status
        {
            InvalidStatus,
            NoChangeStatus,
            Idle,
            Loading,
            Error,
            Invalidated
        };
        enum InsertionType
        {
            InvalidInsertionType,
            NoChangeInsertionType,
            Prepend,
            Append,
            Update,
            Remove,
            Move
        };
        explicit Data() = default;
        explicit Data(Status status, InsertionType insertionType,
                      const std::vector<QString> &insertedIds, const QString &updatedId,
                      int index1, int index2)
            : status{status}, insertionType{insertionType}, insertedIds{insertedIds}
            , updatedId{updatedId}, index1{index1}, index2{index2}
        {
        }
        static Data createAppend(const std::vector<QString> &itemsIds)
        {
            return Data(NoChangeStatus, Append, itemsIds, QString{}, -1, -1);
        }
        static Data createPrepend(const std::vector<QString> &itemsIds)
        {
            return Data(NoChangeStatus, Prepend, itemsIds, QString{}, -1, -1);
        }
        static Data createUpdate(int index, const QString &itemId)
        {
            return Data(NoChangeStatus, Update, std::vector<QString>{}, itemId, index, -1);
        }
        static Data createRemove(int index)
        {
            return Data(NoChangeStatus, Prepend, std::vector<QString>{}, QString{}, index, -1);
        }
        static Data createMove(int start, int end)
        {
            return Data(NoChangeStatus, Move, std::vector<QString>{}, QString{}, start, end);
        }
        static Data createInvalidate()
        {
            return Data(Invalidated, NoChangeInsertionType, std::vector<QString>{}, QString{}, -1, -1);
        }
        static Data createIdle()
        {
            return Data(Idle, NoChangeInsertionType, std::vector<QString>{}, QString{}, -1, -1);
        }
        static Data createLoading()
        {
            return Data(Loading, NoChangeInsertionType, std::vector<QString>{}, QString{}, -1, -1);
        }
        static Data createError()
        {
            return Data(Error, NoChangeInsertionType, std::vector<QString>{}, QString{}, -1, -1);
        }
        bool operator==(const Data &other) const
        {
            return status == other.status && insertionType == other.insertionType
                   && insertedIds == other.insertedIds && updatedId == other.updatedId
                   && index1 == other.index1 && index2 == other.index2;
        }
        Status status {InvalidStatus};
        InsertionType insertionType {InvalidInsertionType};
        std::vector<QString> insertedIds{};
        QString updatedId {};
        int index1 {-1};
        int index2 {-1};
    };
    std::vector<Data> data {};
private:
    void doAppend(const T &item) override
    {
        data.emplace_back(Data::createAppend(std::vector<QString>{item.id()}));
    }
    void doAppend(const std::vector<T> &items) override
    {
        std::vector<QString> ids {};
        for (const T &item : items) {
            ids.push_back(item.id());
        }
        data.emplace_back(Data::createAppend(ids));
    }
    void doPrepend(const std::vector<T> &items) override
    {
        std::vector<QString> ids {};
        for (const T &item : items) {
            ids.push_back(item.id());
        }
        data.emplace_back(Data::createPrepend(ids));
    }
    void doUpdate(int index, const T &item) override
    {
        data.emplace_back(Data::createUpdate(index, item.id()));
    }
    void doRemove(int index) override
    {
        data.emplace_back(Data::createRemove(index));
    }
    void doMove(int start, int end) override
    {
        data.emplace_back(Data::createMove(start, end));
    }
    void doInvalidate() override
    {
        data.emplace_back(Data::createInvalidate());
    }
    void doStart() override
    {
        data.emplace_back(Data::createLoading());
    }
    void doError(const QString &error) override
    {
        Q_UNUSED(error);
        data.emplace_back(Data::createError());
    }
    void doFinish() override
    {
        data.emplace_back(Data::createIdle());
    }
};

#endif // TESTREPOSITORYLISTENER_H

