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

#ifndef REPOSITORYPROCESSCALLBACK_H
#define REPOSITORYPROCESSCALLBACK_H

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QLoggingCategory>
#include <QtNetwork/QNetworkReply>
#include "repository.h"
#include "ilistqueryhandler.h"

namespace private_util {

template<class T>
class RepositoryProcessCallback
{
public:
    explicit RepositoryProcessCallback(typename IListQueryHandler<T>::RequestType requestType,
                                       IListQueryHandler<T> &handler, Repository<T> &repository,
                                       std::vector<T> &items, bool &loading)
        : m_requestType(requestType), m_handler(handler), m_repository(repository)
        , m_items(items), m_loading(loading)
    {
    }
    void operator()(QIODevice &reply, QNetworkReply::NetworkError error, const QString &errorMessage)
    {
        class LoadingLock
        {
        public:
            explicit LoadingLock(bool &loading)
                : m_loading(loading)
            {
            }
            ~LoadingLock()
            {
                m_loading = false;
            }
        private:
            bool &m_loading;
        };

        LoadingLock loadingLock {m_loading};

        if (error != QNetworkReply::NoError) {
            qCWarning(QLoggingCategory("repository-process-callback")) << "Error happened";
            qCWarning(QLoggingCategory("repository-process-callback")) << "Error code:" << error;
            qCWarning(QLoggingCategory("repository-process-callback")) << "Error message (Qt):" << errorMessage;
            const QByteArray &data {reply.readAll()};
            qCWarning(QLoggingCategory("repository-process-callback")) << "Error message (Twitter):" << data;

            // Check if Twitter sent us an issue
            QJsonDocument document {QJsonDocument::fromJson(data)};
            if (document.isObject()) {
                const QJsonObject &object {document.object()};
                const QJsonArray &array {object.value(QLatin1String("errors")).toArray()};
                if (array.count() == 1) {
                    const QJsonObject &firstError {array.first().toObject()};
                    if (firstError.value(QLatin1String("code")).toInt() == 88) {
                        m_repository.error(QObject::tr("Twitter rate limit exceeded. Please try again later."));
                        return;
                    }
                }
            }

            m_repository.error(QObject::tr("Network error. Please try again later."));
            return;
        }

        QString newErrorMessage {};
        typename IListQueryHandler<T>::Placement placement {IListQueryHandler<T>::Discard};
        bool returned = m_handler.treatReply(m_requestType, reply.readAll(), m_items, newErrorMessage, placement);
        if (!returned) {
            qCWarning(QLoggingCategory("repository-process-callback")) << "Error happened";
            qCWarning(QLoggingCategory("repository-process-callback")) << "Parsing error: " << newErrorMessage;
            m_repository.error(QObject::tr("Internal error"));
            return;
        } else {
            qCDebug(QLoggingCategory("repository-process-callback")) << "New data available. Count:" << m_items.size();
            switch (placement) {
            case IListQueryHandler<T>::Append:
                m_repository.append(m_items);
                break;
            case IListQueryHandler<T>::Prepend:
                m_repository.prepend(m_items);
                break;
            case IListQueryHandler<T>::Discard:
                break;
            }
            m_repository.finish();
        }
    }
private:
    typename IListQueryHandler<T>::RequestType m_requestType;
    IListQueryHandler<T> &m_handler;
    Repository<T> &m_repository;
    std::vector<T> &m_items;
    bool &m_loading;
};

}

#endif // REPOSITORYPROCESSCALLBACK_H

