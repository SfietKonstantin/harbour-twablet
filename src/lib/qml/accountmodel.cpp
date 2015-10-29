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

#include "accountmodel.h"

namespace qml
{

AccountModel::AccountModel(QObject *parent) :
    Model<Account, AccountObject>(parent)
{
}

QVariant AccountModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if (row < 0 || row >= rowCount()) {
        return QVariant();
    }
    const QObjectPtr<AccountObject> &account = m_items[row];
    switch (role) {
    case NameRole:
        return account->name();
        break;
    case ScreenNameRole:
        return account->screenName();
        break;
    case AccountRole:
        return QVariant::fromValue(account.get());
        break;
    default:
        return QVariant();
        break;
    }
}

AccountObject * AccountModel::get(const QString &userId) const
{
    auto it = std::find_if(std::begin(m_items), std::end(m_items), [userId](const QObjectPtr<AccountObject> &object) {
        return object->userId() == userId;
    });
    return it == std::end(m_items) ? nullptr : it->get();
}

int AccountModel::getIndex(const QString &userId) const
{
    for (size_t i = 0; i < m_items.size(); ++i) {
        if (m_items[i]->userId() == userId) {
            return i;
        }
    }
    return -1;
}

QHash<int, QByteArray> AccountModel::roleNames() const
{
    return {{NameRole, "name"}, {ScreenNameRole, "screenName"}, {AccountRole, "account"}};
}

}
