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

#include "twitterstatus.h"
#include "qobjectutils.h"
#include "accountobject.h"
#include "private/twitterqueryutil.h"
#include <QtCore/QLoggingCategory>
#include <QtCore/QUrlQuery>
#include <QtNetwork/QNetworkReply>

TwitterStatus::TwitterStatus(QObject *parent)
    : QObject(parent), m_network{new QNetworkAccessManager()}
{
}

TwitterStatus::Status TwitterStatus::status() const
{
    return m_status;
}

QString TwitterStatus::errorMessage() const
{
    return m_errorMessage;
}

AccountObject * TwitterStatus::account() const
{
    return m_account;
}

void TwitterStatus::setAccount(AccountObject *account)
{
    if (m_account != account) {
        m_account = account;
        emit accountChanged();
    }
}

QString TwitterStatus::text() const
{
    return m_text;
}

void TwitterStatus::setText(const QString &statusUpdate)
{
    if (m_text != statusUpdate) {
        m_text = statusUpdate;
        emit textChanged();
    }
}

QString TwitterStatus::inReplyTo() const
{
    return m_inReplyTo;
}

void TwitterStatus::setInReplyTo(const QString &inReplyTo)
{
    if (m_inReplyTo != inReplyTo) {
        m_inReplyTo = inReplyTo;
        emit inReplyToChanged();
    }
}

bool TwitterStatus::post()
{
    if (m_account == nullptr || m_text.isEmpty()) {
        return false;
    }

    QString path {QLatin1String("statuses/update.json")};
    std::map<QString, QString> parameters {{QLatin1String("status"), m_text}};
    if (!m_inReplyTo.isEmpty()) {
        parameters.insert({QLatin1String("in_reply_to_status_id"), m_inReplyTo});
    }

    QNetworkReply *reply = TwitterQueryUtil::post(m_network.get(), path, {}, parameters, m_account->account());
    setStatusAndErrorMessage(Loading, QString());

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        QObjectPtr<QNetworkReply> replyPtr {reply};

        if (replyPtr->error() != QNetworkReply::NoError) {
            qCWarning(QLoggingCategory("twitter-status")) << "Error happened during status update";
            qCWarning(QLoggingCategory("twitter-status")) << "Error code:" << reply->error();
            qCWarning(QLoggingCategory("twitter-status")) << "Error message (Qt):" << reply->errorString();
            qCWarning(QLoggingCategory("twitter-status")) << "Error message (Twitter):" << reply->readAll();

            if (reply->error() == QNetworkReply::ContentOperationNotPermittedError) {
                setStatusAndErrorMessage(Error, tr("Twitter do not allow to send the same tweet twice."));
            } else {
                setStatusAndErrorMessage(Error, tr("Network error. Please try again later."));
            }
            return;
        }

        setStatusAndErrorMessage(Idle, QString());
    });

    return true;
}

void TwitterStatus::setStatusAndErrorMessage(Status status, const QString &errorMessage)
{
    if (m_status != status) {
        m_status = status;
        emit statusChanged();
    }

    if (m_errorMessage != errorMessage) {
        m_errorMessage = errorMessage;
        emit errorMessageChanged();
    }
}

