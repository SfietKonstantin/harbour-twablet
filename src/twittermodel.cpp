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

#include "twittermodel.h"
#include "twittermodelquery.h"
#include "private/twitterqueryutil.h"
#include "private/qnetworkreplyutils.h"
#include <QtCore/QLoggingCategory>

TwitterModel::TwitterModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_network(new QNetworkAccessManager(this))
{
}

void TwitterModel::classBegin()
{
}

void TwitterModel::componentComplete()
{
}

int TwitterModel::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return m_data.count();
}

QVariant TwitterModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if (row < 0 || row >= rowCount()) {
        return QVariant();
    }

    switch (role) {
    case ItemRole:
        return QVariant::fromValue(m_data[row]);
        break;
    default:
        return QVariant();
        break;
    }
}

int TwitterModel::count() const
{
    return rowCount();
}

TwitterStatus::Status TwitterModel::status() const
{
    return m_status;
}

QString TwitterModel::errorMessage() const
{
    return m_errorMessage;
}

TwitterUser * TwitterModel::user() const
{
    return m_user;
}

void TwitterModel::setUser(TwitterUser *user)
{
    if (m_user != user) {
        m_user = user;
        emit userChanged();
    }
}

TwitterModelItem::Type TwitterModel::type() const
{
    return m_query != nullptr ? m_query->type() : TwitterModelItem::Invalid;
}

TwitterModelQuery * TwitterModel::query() const
{
    return m_query;
}

void TwitterModel::setQuery(TwitterModelQuery *query)
{
    if (m_query != query) {
        TwitterModelItem::Type oldType {type()};

        m_query = query;
        emit queryChanged();

        if (type() != oldType) {
            emit typeChanged();
        }
    }
}

void TwitterModel::load()
{
    if (m_query == nullptr || m_user == nullptr) {
        return;
    }

    QString path {};
    std::map<QString, QString> parameters {};
    m_query->createRequest(path, parameters);
    QNetworkReply *reply = m_network->get(TwitterQueryUtil::createGetRequest(path, parameters, *m_user));
    setStatusAndError(TwitterStatus::Loading, QString());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        QNetworkReplyPtr replyPtr {reply};
        if (replyPtr->error() != QNetworkReply::NoError) {
            qCDebug(QLoggingCategory("twitter-model")) << "Error happened during request. Code:" << reply->error();
            qCDebug(QLoggingCategory("twitter-model")) << "Error message (Qt):" << reply->errorString();
            qCDebug(QLoggingCategory("twitter-model")) << "Error message (Twitter):" << reply->readAll();
            setStatusAndError(TwitterStatus::Error, tr("Error while sending query to Twitter, check your internet connection."));
            return;
        }

        if (m_query != nullptr) {
            QList<TwitterModelItem *> items {};
            QString errorMessage {};
            if (!m_query->treatReply(reply->readAll(), items, errorMessage, this)) {
                qCDebug(QLoggingCategory("twitter-model")) << "Error happened during parsing: " << errorMessage;
                setStatusAndError(TwitterStatus::Error, tr("Error while reading data from Twitter."));
                return;
            } else {
                append(std::move(items));
            }
        }
    });
}

QHash<int, QByteArray> TwitterModel::roleNames() const
{
    return {{ItemRole, "item"}};
}

void TwitterModel::setStatusAndError(TwitterStatus::Status status, const QString &errorMessage)
{
    if (m_errorMessage != errorMessage) {
        m_errorMessage = errorMessage;
        emit errorMessageChanged();
    }

    if (m_status != status) {
        m_status = status;
        emit statusChanged();
    }
}

void TwitterModel::prepend(QList<TwitterModelItem *> &&items)
{
    if (items.isEmpty()) {
        return;
    }

    beginInsertRows(QModelIndex(), 0, items.count() - 1);
    QList<TwitterModelItem *> oldData;
    m_data = std::move(items);
    m_data.append(std::move(oldData));
    emit countChanged();
    endInsertRows();
}

void TwitterModel::append(QList<TwitterModelItem *> &&items)
{
    if (items.isEmpty()) {
        return;
    }

    beginInsertRows(QModelIndex(), rowCount(), rowCount() + items.count() - 1);
    m_data.append(std::move(items));
    emit countChanged();
    endInsertRows();
}



