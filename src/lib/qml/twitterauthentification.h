/*
 * Copyright (C) 2015 Lucien XU <sfietkonstantin@free.fr>
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

#ifndef TWITTERAUTHENTIFICATION_H
#define TWITTERAUTHENTIFICATION_H

#include <QtCore/QObject>
#include <QtNetwork/QNetworkAccessManager>
#include "globals.h"
#include "qobjectutils.h"

namespace qml
{

class TwitterAuthentification : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString pin READ pin WRITE setPin NOTIFY pinChanged)
    Q_PROPERTY(QString token READ token NOTIFY tokenChanged)
    Q_PROPERTY(QString tokenSecret READ tokenSecret NOTIFY tokenSecretChanged)
    Q_PROPERTY(QString userId READ userId NOTIFY userIdChanged)
    Q_PROPERTY(QString screenName READ screenName NOTIFY nameChanged)
public:
    explicit TwitterAuthentification(QObject *parent = 0);
    DISABLE_COPY_DISABLE_MOVE(TwitterAuthentification);
    QString pin() const;
    void setPin(const QString &pin);
    QString token() const;
    QString tokenSecret() const;
    QString userId() const;
    QString screenName() const;
public slots:
    void startRequest();
    void continueRequest();
signals:
    void error(const QString &errorMessage);
    void pinChanged();
    void tokenChanged();
    void tokenSecretChanged();
    void nameChanged();
    void userIdChanged();
    void sendAuthorize(const QUrl &url);
    void done();
private:
    void setData(QString &&token, QString &&tokenSecret, QString &&userId, QString &&screenName);
    QObjectPtr<QNetworkAccessManager> m_network {nullptr};
    QByteArray m_tempToken {};
    QByteArray m_tempTokenSecret {};
    QString m_pin {};
    QString m_token {};
    QString m_tokenSecret {};
    QString m_userId {};
    QString m_screenName {};
};

}

#endif // TWITTERAUTHENTIFICATION_H
