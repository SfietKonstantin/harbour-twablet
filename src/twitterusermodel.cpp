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

#include "twitterusermodel.h"
#include "twitteruser.h"
#include "private/configutils.h"
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

TwitterUserModel::TwitterUserModel(QObject *parent) :
    QAbstractListModel(parent)
{
    load();
}

TwitterUserModel::~TwitterUserModel()
{

}

void TwitterUserModel::classBegin()
{
}

void TwitterUserModel::componentComplete()
{
}

int TwitterUserModel::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return m_data.count();
}

QVariant TwitterUserModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if (row < 0 || row >= rowCount()) {
        return QVariant();
    }
    TwitterUser *user = m_data[row];
    switch (role) {
    case NameRole:
        return user->name();
        break;
    case ScreenNameRole:
        return user->screenName();
        break;
    case UserRole:
        return QVariant::fromValue(user);
        break;
    default:
        return QVariant();
        break;
    }
}

int TwitterUserModel::count() const
{
    return rowCount();
}

TwitterUser * TwitterUserModel::get(int index) const
{
    if (index < 0 || index >= rowCount()) {
        return nullptr;
    }
    return m_data[index];
}

void TwitterUserModel::addUser(const QString &name, const QString &userId, const QString &screenName,
                               const QString &token, const QString &tokenSecret)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_data.append(TwitterUser::create(name, userId, screenName, token, tokenSecret, this));
    emit countChanged();
    endInsertRows();
    save();
}

void TwitterUserModel::updateUser(int row, const QString &name)
{
    if (row < 0 || row >= rowCount()) {
        return;
    }

    m_data[row]->setName(name);
    emit dataChanged(index(row), index(row), {NameRole});
    save();
}

void TwitterUserModel::removeUser(int row)
{
    if (row < 0 || row >= rowCount()) {
        return;
    }

    beginRemoveRows(QModelIndex(), row, row);
    m_data.takeAt(row)->deleteLater();
    emit countChanged();
    endRemoveRows();
    save();
}

QHash<int, QByteArray> TwitterUserModel::roleNames() const
{
    return {{NameRole, "name"}, {ScreenNameRole, "screenName"}, {UserRole, "user"}};
}

void TwitterUserModel::load()
{
    const QJsonObject &config {ConfigUtils::configuration()};
    const QJsonArray &users {config.value(QLatin1String("users")).toArray()};

    QList<TwitterUser *> data;
    for (const QJsonValue &userValue : users) {
        const QJsonObject &user {userValue.toObject()};
        const QString &name {user.value(QLatin1String("name")).toString()};
        const QString &userId {user.value(QLatin1String("userId")).toString()};
        const QString &screenName {user.value(QLatin1String("screenName")).toString()};
        const QString &token {user.value(QLatin1String("token")).toString()};
        const QString &tokenSecret {user.value(QLatin1String("tokenSecret")).toString()};
        if (!name.isEmpty() && !userId.isEmpty()) {
            data.append(TwitterUser::create(name, userId, screenName, token, tokenSecret));
        }
    }

    beginInsertRows(QModelIndex(), 0, data.count() - 1);
    m_data = std::move(data);
    emit countChanged();
    endInsertRows();
}

void TwitterUserModel::save()
{
    ConfigUtils::writeConfiguration([this](QJsonObject &config) {
        QJsonArray users {};
        for (const TwitterUser *user : m_data) {
            QJsonObject userObject {};
            userObject.insert(QLatin1String("name"), user->name());
            userObject.insert(QLatin1String("userId"), user->userId());
            userObject.insert(QLatin1String("screenName"), user->screenName());
            userObject.insert(QLatin1String("token"), user->token());
            userObject.insert(QLatin1String("tokenSecret"), user->tokenSecret());
            users.append(userObject);
        }
        config.insert(QLatin1String("users"), users);
    });
}
