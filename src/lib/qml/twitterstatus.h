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

#ifndef TWITTERSTATUS_H
#define TWITTERSTATUS_H

#include <QtCore/QObject>
#include <QtNetwork/QNetworkAccessManager>
#include "globals.h"
#include "qobjectutils.h"

class AccountObject;
class TwitterStatus : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
    Q_PROPERTY(AccountObject * account READ account WRITE setAccount NOTIFY accountChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString inReplyTo READ inReplyTo WRITE setInReplyTo NOTIFY inReplyToChanged)
    Q_ENUMS(Status)
public:
    enum Status
    {
        Idle,
        Loading,
        Error
    };
    explicit TwitterStatus(QObject *parent = 0);
    DISABLE_COPY_DISABLE_MOVE(TwitterStatus);
    Status status() const;
    QString errorMessage() const;
    AccountObject * account() const;
    void setAccount(AccountObject *account);
    QString text() const;
    void setText(const QString &text);
    QString inReplyTo() const;
    void setInReplyTo(const QString &inReplyTo);
public slots:
    bool post();
signals:
    void statusChanged();
    void errorMessageChanged();
    void accountChanged();
    void textChanged();
    void inReplyToChanged();
private:
    void setStatusAndErrorMessage(Status status, const QString &errorMessage);
    QObjectPtr<QNetworkAccessManager> m_network {nullptr};
    Status m_status {Idle};
    QString m_errorMessage {};
    AccountObject *m_account {nullptr};
    QString m_text {};
    QString m_inReplyTo {};
};

#endif // TWITTERSTATUS_H
