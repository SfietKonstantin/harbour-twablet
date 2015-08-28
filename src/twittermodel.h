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

#ifndef TWITTERMODEL_H
#define TWITTERMODEL_H

#include <QtCore/QAbstractListModel>
#include <QtQml/QQmlParserStatus>
#include <QtNetwork/QNetworkAccessManager>
#include "twittermodelitem.h"
#include "twitterstatus.h"

class TwitterUser;
class TwitterModelQuery;
class TwitterModel : public QAbstractListModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(TwitterStatus::Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
    Q_PROPERTY(TwitterUser * user READ user WRITE setUser NOTIFY userChanged)
    Q_PROPERTY(TwitterModelQuery * query READ query WRITE setQuery NOTIFY queryChanged)
    Q_PROPERTY(TwitterModelItem::Type type READ type NOTIFY typeChanged)
public:
    enum Roles {
        ItemRole = Qt::UserRole + 1
    };
    explicit TwitterModel(QObject *parent = 0);
    void classBegin() override;
    void componentComplete() override;
    int rowCount(const QModelIndex &index = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    int count() const;
    TwitterStatus::Status status() const;
    QString errorMessage() const;
    TwitterUser * user() const;
    void setUser(TwitterUser *user);
    TwitterModelItem::Type type() const;
    TwitterModelQuery * query() const;
    void setQuery(TwitterModelQuery *query);
public slots:
    void load();
signals:
    void countChanged();
    void statusChanged();
    void errorMessageChanged();
    void userChanged();
    void typeChanged();
    void queryChanged();
protected:
    QHash<int, QByteArray> roleNames() const override;
private:
    void setStatusAndError(TwitterStatus::Status status, const QString &errorMessage);
    void prepend(QList<TwitterModelItem *> &&items);
    void append(QList<TwitterModelItem *> &&items);
    QNetworkAccessManager *m_network {nullptr};
    QList<TwitterModelItem *> m_data {};
    TwitterStatus::Status m_status {TwitterStatus::Idle};
    QString m_errorMessage {};
    TwitterUser * m_user {nullptr};
    TwitterModelQuery *m_query {nullptr};
};

#endif // TWITTERMODEL_H
