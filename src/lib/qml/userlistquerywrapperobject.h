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

#ifndef USERLISTQUERYWRAPPEROBJECT_H
#define USERLISTQUERYWRAPPEROBJECT_H

#include <QtCore/QVariantMap>
#include "iquerywrapperobject.h"
#include "query.h"
#include "querytypeobject.h"

namespace qml
{

class UserListQueryWrapperObject : public QObject, public IQueryWrapperObject
{
    Q_OBJECT
    Q_INTERFACES(qml::IQueryWrapperObject)
    Q_PROPERTY(QString accountUserId READ accountUserId WRITE setAccountUserId
               NOTIFY accountUserIdChanged)
    Q_PROPERTY(qml::QueryTypeObject::UserListType type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QVariantMap parameters READ parameters WRITE setParameters NOTIFY parametersChanged)
public:
    explicit UserListQueryWrapperObject(QObject *parent = 0);
    QString accountUserId() const override;
    void setAccountUserId(const QString &accountUserId);
    Query query() const override;
    void setQuery(UserListQuery &&query);
    QueryTypeObject::UserListType type() const;
    void setType(QueryTypeObject::UserListType type);
    QVariantMap parameters() const;
    void setParameters(const QVariantMap &parameters);
    void accept(QueryWrapperVisitor &visitor) const override;
signals:
    void accountUserIdChanged();
    void typeChanged();
    void parametersChanged();
private:
    void updateQuery();
    UserListQuery::Type convertedType() const;
    QString m_accountUserId {};
    UserListQuery m_query {};
    QueryTypeObject::UserListType m_type {QueryTypeObject::InvalidUserList};
    QVariantMap m_parameters {};
};

}

#endif // USERLISTQUERYWRAPPEROBJECT_H
