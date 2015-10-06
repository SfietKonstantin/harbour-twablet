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

#include "abstractqueryitem.h"
#include <QtCore/QLoggingCategory>

namespace qml
{

AbstractQueryItem::AbstractQueryItem(QObject *parent)
    : QObject(parent), m_network{new QNetworkAccessManager()}
{
}

AccountObject * AbstractQueryItem::account() const
{
    return m_account;
}

void AbstractQueryItem::setAccount(AccountObject *account)
{
    if (m_account != account) {
        m_account = account;
        emit accountChanged();
    }
}

AbstractQueryItem::Status AbstractQueryItem::status() const
{
    return m_status;
}

QString AbstractQueryItem::errorMessage() const
{
    return m_errorMessage;
}

bool AbstractQueryItem::load()
{
    if (m_account == nullptr || !isQueryValid() || m_status == Loading) {
        return false;
    }

    QNetworkReply *reply = createQuery();
    setStatusAndErrorMessage(Loading, QString());

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        QObjectPtr<QNetworkReply> replyPtr {reply};
        const QByteArray &data {replyPtr->readAll()};
        handleReply(data, replyPtr->error(), replyPtr->errorString());
        if (m_status == Loading) {
            if (replyPtr->error() != QNetworkReply::NoError) {
                qCWarning(QLoggingCategory("query-item")) << "Error happened during query";
                qCWarning(QLoggingCategory("query-item")) << "Error code:" << replyPtr->error();
                qCWarning(QLoggingCategory("query-item")) << "Error message (Qt):" << replyPtr->errorString();
                qCWarning(QLoggingCategory("query-item")) << "Error message (Twitter):" << data;
                setStatusAndErrorMessage(Error, tr("Network error. Please try again later."));
            } else {
                setStatusAndErrorMessage(Idle, QString());
            }
        }
    });

    return true;
}

QNetworkAccessManager & AbstractQueryItem::network() const
{
    return *m_network;
}

void AbstractQueryItem::setStatusAndErrorMessage(AbstractQueryItem::Status status,
                                                 const QString &errorMessage)
{
    qCDebug(QLoggingCategory("query-item")) << "Current status" << status << errorMessage;
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

}
