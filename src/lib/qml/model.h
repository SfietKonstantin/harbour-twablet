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

#ifndef MODEL_H
#define MODEL_H

#include "imodel.h"
#include "irepositorylistener.h"
#include "qobjectutils.h"
#include "datarepositoryobjectmap.h"
#include <QtCore/QLoggingCategory>

static QLoggingCategory mLogging {"model"};

namespace qml
{

template<class T, class O>
class Model: public IModel, public IRepositoryListener<T>
{
public:
    ~Model()
    {
        if (m_internalRepository != nullptr) {
            DataRepositoryObjectMap<T>::removeListener(*m_internalRepository, *this,
                                                       m_repository, m_internalAccountUserId,
                                                       m_internalQuery);
        }
    }
    void classBegin() override
    {
    }
    void componentComplete() override
    {
        m_complete = true;
        refreshData();
    }
    int rowCount(const QModelIndex &parent = QModelIndex()) const override final
    {
        Q_UNUSED(parent)
        return m_items.size();
    }
    int count() const override
    {
        return rowCount();
    }
    Status status() const override
    {
        return m_status;
    }
    QString errorMessage() const override
    {
        return m_errorMessage;
    }
    QObject * repository() const override
    {
        return m_repository;
    }
    void setRepository(QObject *repository) override
    {
        if (m_repository != repository) {
            m_repository = repository;
            updateInternalRepository();
            emit repositoryChanged();
        }
    }
    QObject * query() const override
    {
        return m_query;
    }
    void setQuery(QObject *query) override
    {
        if (m_query != query) {
            m_query = query;
            updateInternalRepository();
            emit queryChanged();
        }
    }
public slots:
    void startMove() override
    {
        m_localMove = true;
    }
    void move(int from, int to) override
    {
        if (m_localMove) {
            performMove(from, to);
        }
    }
    void endMove()
    {
        m_localMove = false;
    }
protected:
    explicit Model(QObject *parent = 0)
        : IModel(parent) , IRepositoryListener<T>()
    {
    }
    std::deque<QObjectPtr<O>> m_items {};
private:
    void doAppend(const T &item) override
    {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        m_items.emplace_back(O::create(item, this));
        emit countChanged();
        endInsertRows();
    }
    void doAppend(const std::vector<T> &items) override
    {
        beginInsertRows(QModelIndex(), rowCount(), rowCount() + items.size() - 1);
        for (const T &entry : items) {
            m_items.emplace_back(O::create(entry, this));
        }
        emit countChanged();
        endInsertRows();
    }
    void doPrepend(const std::vector<T> &items) override
    {
        emit prependPre();
        beginInsertRows(QModelIndex(), 0, items.size() - 1);
        for (auto it = items.rbegin(); it != items.rend(); ++it) {
            m_items.emplace_front(O::create(*it, this));
        }
        emit countChanged();
        endInsertRows();
        emit prependPost(items.size());
    }
    void doUpdate(int index, const T &item) override
    {
        if (index < 0 || index >= rowCount()) {
            return;
        }
        m_items[index]->update(item);
        emit dataChanged(this->index(index), this->index(index));
    }

    void doRemove(int index) override
    {
        if (index < 0 || index >= rowCount()) {
            return;
        }

        beginRemoveRows(QModelIndex(), index, index);
        m_items.erase(std::begin(m_items) + index);
        emit countChanged();
        endRemoveRows();
    }
    void doMove(int from, int to) override
    {
        if (!m_localMove) {
            performMove(from, to);
        }
    }
    void doInvalidate() override
    {
        m_internalRepository = nullptr;
    }
    void doStart() override
    {
        setStatusAndErrorMessage(Loading, QString());
    }

    void doError(const QString &error) override
    {
        setStatusAndErrorMessage(Error, error);
    }

    void doFinish() override
    {
        setStatusAndErrorMessage(Idle, QString());
    }

    void refreshData()
    {
        if (!m_complete) {
            return;
        }
        std::deque<QObjectPtr<O>> newItems;
        if (m_internalRepository != nullptr) {
            for (const T &item : *m_internalRepository) {
                newItems.emplace_back(O::create(item, this));
            }
        }

        int oldSize = m_items.size();
        int newSize = newItems.size();
        int delta = newSize - oldSize;

        qCDebug(mLogging) << "Refreshing data. Item delta:" << delta;

        if (delta < 0) {
            beginRemoveRows(QModelIndex(), newSize, oldSize - 1);
        } else if (delta > 0) {
            beginInsertRows(QModelIndex(), oldSize, newSize - 1);
        }

        m_items = std::move(newItems);

        emit dataChanged(index(0), index(rowCount() - 1));

        if (delta != 0) {
            emit countChanged();
        }
        if (delta < 0) {
            endRemoveRows();
        } else if (delta > 0) {
            endInsertRows();
        }
    }
    void setStatusAndErrorMessage(Status status, const QString &errorMessage)
    {
        qCDebug(mLogging) << "Current status" << status << errorMessage;
        if (m_status != status) {
            m_status = status;
            emit statusChanged();
            switch (m_status) {
            case Idle:
                emit finished();
                break;
            case Error:
                emit error();
                break;
            default:
                break;
            }
        }

        if (m_errorMessage != errorMessage) {
            m_errorMessage = errorMessage;
            emit errorMessageChanged();
        }
    }
    void updateInternalRepository()
    {
        DataRepositoryObjectMap<T>::getQueryInfo(m_query, m_internalAccountUserId, m_internalQuery);
        Repository<T> *internalRepository = m_repository != nullptr ? DataRepositoryObjectMap<T>::get(*m_repository, m_internalAccountUserId, m_internalQuery) : nullptr;
        if (m_internalRepository != internalRepository) {
            if (m_internalRepository) {
                DataRepositoryObjectMap<T>::removeListener(*m_internalRepository, *this,
                                                           m_repository, m_internalAccountUserId,
                                                           m_internalQuery);
            }
            m_internalRepository = internalRepository;
            if (m_internalRepository) {
                DataRepositoryObjectMap<T>::addListener(*m_internalRepository, *this,
                                                        m_repository, m_internalAccountUserId,
                                                        m_internalQuery);
            }
            refreshData();
        }
    }
    void performMove(int from, int to)
    {
        if (from < 0 || static_cast<std::size_t>(from) >= m_items.size()
            || to < 0 || static_cast<std::size_t>(to) > m_items.size()) {
            return;
        }

        if (!beginMoveRows(QModelIndex(), from, from, QModelIndex(), to)) {
            return;
        }

        int toIndex = (to < from) ? to : to - 1;

        QObjectPtr<O> item = std::move(*(std::begin(m_items) + from));
        m_items.erase(std::begin(m_items) + from);
        m_items.insert(std::begin(m_items) + toIndex, std::move(item));
        endMoveRows();
    }

    bool m_complete {false};
    bool m_localMove {false};
    Status m_status {Idle};
    QString m_errorMessage {};
    QObject *m_repository {nullptr};
    QObject *m_query {nullptr};
    QString m_internalAccountUserId {};
    Query m_internalQuery {};
    Repository<T> *m_internalRepository {nullptr};
};

}

#endif // MODEL_H

