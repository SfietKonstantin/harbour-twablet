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

#ifndef QUERYITEM_H
#define QUERYITEM_H

#include <QtCore/QLoggingCategory>
#include "iaccountrepositorycontainerobject.h"
#include "iitemlistener.h"
#include "iitemquerycontainerobject.h"
#include "iqueryitem.h"
#include "iquerywrapperobject.h"
#include "queryitemfactory.h"

static const QLoggingCategory qiLogger {"query-item"};

namespace qml
{

template<class T, class O>
class QueryItem: public IQueryItem, public IItemListener<T>
{
public:
    ~QueryItem()
    {
    }
    void classBegin() override
    {
    }
    void componentComplete() override
    {
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
            emit queryChanged();
        }
    }
    O item() const
    {
        return m_item;
    }
public slots:
    bool load() override
    {
        if (m_repository == nullptr) {
            qWarning(qiLogger) << "Cannot load without repository";
            return false;
        }
        if (m_query == nullptr) {
            qWarning(qiLogger) << "Cannot load without query";
            return false;
        }
        IAccountRepositoryContainerObject *accountContainer = qobject_cast<IAccountRepositoryContainerObject *>(m_repository);
        IItemQueryContainerObject *queryContainer = qobject_cast<IItemQueryContainerObject *>(m_repository);
        IQueryWrapperObject *queryWrapper = qobject_cast<IQueryWrapperObject *>(m_query);
        if (accountContainer == nullptr || queryContainer == nullptr || queryWrapper == nullptr) {
            qWarning(qiLogger) << "repository or query do not implement the correct interfaces";
            return false;
        }
        const Account &account {accountContainer->account(queryWrapper->accountUserId())};
        queryContainer->itemQueryContainer()->executeQuery(account, queryWrapper->query(), *this);
        return true;
    }
protected:
    explicit QueryItem(QObject *parent = 0)
        : IQueryItem(parent), IItemListener<T>()
    {
    }
    void setItem(T &&item)
    {
        m_item = QueryItemFactory<T, O>::create(item, this);
        doItemChanged();
    }
    virtual void doItemChanged() = 0;
private:
    void onStart() override
    {
        setStatusAndErrorMessage(Loading, QString());
    }
    void onError(const QString &error) override
    {
        setStatusAndErrorMessage(Error, error);
    }
    void onFinish(T &&item) override
    {
        m_item = QueryItemFactory<T, O>::create(item, this);
        doItemChanged();
        setStatusAndErrorMessage(Idle, QString());
    }
    void setStatusAndErrorMessage(Status status, const QString &errorMessage)
    {
        qCDebug(qiLogger) << "Current status" << status << errorMessage;
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
    Status m_status {Idle};
    QString m_errorMessage {};
    QObject *m_repository {nullptr};
    QObject *m_query{nullptr};
    O m_item {};
};

}

#endif // QUERYITEM_H
