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

#include "querytypemodel.h"
#include "querytypeobject.h"

namespace qml
{

QueryTypeModel::QueryTypeModel(QObject *parent) :
    QAbstractListModel(parent)
{
    m_items.emplace_back(new Data {tr("Home"), QueryTypeObject::Home});
    m_items.emplace_back(new Data {tr("Mentions"), QueryTypeObject::Mentions});
    m_items.emplace_back(new Data {tr("Search"), QueryTypeObject::Search});
}

void QueryTypeModel::classBegin()
{
}

void QueryTypeModel::componentComplete()
{
}

int QueryTypeModel::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return m_items.size();
}

QVariant QueryTypeModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if (row < 0 || row >= rowCount()) {
        return QVariant();
    }
    const std::unique_ptr<Data> &data = m_items[row];
    switch (role) {
    case NameRole:
        return data->name;
        break;
    case QueryTypeRole:
        return data->type;
        break;
    default:
        return QVariant();
        break;
    }
}

int QueryTypeModel::count() const
{
    return rowCount();
}

int QueryTypeModel::getType(int index)
{
    if (index < 0 || index >= rowCount()) {
        return QueryTypeObject::InvalidTweetModel;
    }
    return m_items[index]->type;
}

int QueryTypeModel::getIndex(int type)
{
    for (size_t i = 0; i < m_items.size(); ++i) {
        if (m_items[i]->type == type) {
            return i;
        }
    }
    return -1;
}

QHash<int, QByteArray> QueryTypeModel::roleNames() const
{
    return {{NameRole, "name"}, {QueryTypeRole, "type"}};
}

}
