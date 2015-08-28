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

#include "layoutmodel.h"
#include "layout.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QLoggingCategory>
#include <QtCore/QStandardPaths>

LayoutModel::LayoutModel(QObject *parent) :
    QAbstractListModel(parent)
{
    load();
}

void LayoutModel::classBegin()
{
}

void LayoutModel::componentComplete()
{
}

int LayoutModel::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return m_data.count();
}

QVariant LayoutModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if (row < 0 || row >= rowCount()) {
        return QVariant();
    }
    Layout *layout = m_data[row];
    switch (role) {
    case NameRole:
        return layout->name();
        break;
    case LayoutRole:
        return QVariant::fromValue(layout);
        break;
    default:
        return QVariant();
        break;
    }
}

int LayoutModel::count() const
{
    return rowCount();
}

void LayoutModel::addLayout(const QString &name, TwitterUser *user, TwitterModelQuery *query)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_data.append(Layout::create(name, user, query, this));
    emit countChanged();
    endInsertRows();
    save();
}

void LayoutModel::removeLayout(int row)
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

QHash<int, QByteArray> LayoutModel::roleNames() const
{
    return {{NameRole, "name"}, {LayoutRole, "layout"}};
}

void LayoutModel::load()
{
}

void LayoutModel::save()
{
}
