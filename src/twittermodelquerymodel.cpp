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

#include "twittermodelquerymodel.h"
#include "twittermodelquery.h"
#include "twittertimelinequery.h"
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

TwitterModelQueryModel::TwitterModelQueryModel(QObject *parent) :
    QAbstractListModel(parent)
{
    m_data.append(new TwitterTimelineQuery(this));
}

void TwitterModelQueryModel::classBegin()
{
}

void TwitterModelQueryModel::componentComplete()
{
}

int TwitterModelQueryModel::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return m_data.count();
}

QVariant TwitterModelQueryModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if (row < 0 || row >= rowCount()) {
        return QVariant();
    }
    TwitterModelQuery *query = m_data[row];
    switch (role) {
    case NameRole:
        return query->name();
        break;
    case QueryRole:
        return QVariant::fromValue(query);
        break;
    default:
        return QVariant();
        break;
    }
}

int TwitterModelQueryModel::count() const
{
    return rowCount();
}

TwitterModelQuery * TwitterModelQueryModel::get(int index) const
{
    if (index < 0 || index >= rowCount()) {
        return nullptr;
    }
    return m_data[index];
}

QHash<int, QByteArray> TwitterModelQueryModel::roleNames() const
{
    return {{NameRole, "name"}, {QueryRole, "query"}};
}
