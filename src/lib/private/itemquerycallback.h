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

#ifndef ITEMQUERYCALLBACK_H
#define ITEMQUERYCALLBACK_H

#include <set>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QLoggingCategory>
#include <QtNetwork/QNetworkReply>
#include "debughelper.h"
#include "iitemqueryhandler.h"
#include "iitemlistener.h"

static const QLoggingCategory iqcLogger {"item-query-callback"};

namespace private_util {

template<class T>
class ItemQueryCallback
{
public:
    explicit ItemQueryCallback(IItemQueryHandler<T> &handler,
                               const std::set<IItemListener<T> *> &listeners)
        : m_handler(handler), m_listeners(listeners)
    {
    }
    void operator()(QIODevice &reply, QNetworkReply::NetworkError error, const QString &errorMessage)
    {
        if (error != QNetworkReply::NoError) {
            qCWarning(iqcLogger) << "Network error";
            qCWarning(iqcLogger) << "  Error code:" << error;
            qCWarning(iqcLogger) << "  Error message (Qt):" << errorMessage;
            const QByteArray &data {reply.readAll()};
            qCWarning(iqcLogger) << "  Error message (Twitter):" << data;

            QString newErrorMessage {};
            if (m_handler.treatError(data, error, newErrorMessage)) {
                doError(newErrorMessage);
            }

            doError(QObject::tr("Network error. Please try again later."));
            return;
        }

        T item {};
        QString newErrorMessage {};
        bool returned = m_handler.treatReply(reply.readAll(), item, newErrorMessage);
        if (!returned) {
            qCWarning(iqcLogger) << "Parsing error: " << newErrorMessage;
            doError(QObject::tr("Internal error"));
            return;
        } else {
            qCDebug(iqcLogger) << "Finished";
            doFinish(item);
        }
    }
private:
    IItemQueryHandler<T> &m_handler;
    std::set<IItemListener<T> *> m_listeners;
    void doError(const QString &error)
    {
        for (IItemListener<T> *listener : m_listeners) {
            listener->onError(error);
        }
    }
    void doFinish(const T &item)
    {
        for (IItemListener<T> *listener : m_listeners) {
            listener->onFinish(std::move(T(item)));
        }
    }
};

}

#endif // ITEMQUERYCALLBACK_H

