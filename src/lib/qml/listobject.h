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

#ifndef LISTOBJECT_H
#define LISTOBJECT_H

#include <QtCore/QObject>
#include "list.h"
#include "userobject.h"
#include "quotedtweetobject.h"

namespace qml
{

class ListObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool valid READ isValid CONSTANT)
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString slug READ slug CONSTANT)
    Q_PROPERTY(QString fullName READ fullName CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(qml::UserObject * user READ user CONSTANT)
    Q_PROPERTY(QString mode READ mode CONSTANT)
    Q_PROPERTY(bool following READ isFollowing CONSTANT)
    Q_PROPERTY(int memberCount READ memberCount CONSTANT)
    Q_PROPERTY(int subscriberCount READ subscriberCount CONSTANT)
    Q_PROPERTY(QString uri READ uri CONSTANT)
public:
    DISABLE_COPY_DISABLE_MOVE(ListObject);
    static ListObject * create(const List &data, QObject *parent = 0);
    bool isValid() const;
    QString id() const;
    QString name() const;
    QString slug() const;
    QString fullName() const;
    QString description() const;
    UserObject * user() const;
    QString mode() const;
    bool isFollowing() const;
    int memberCount() const;
    int subscriberCount() const;
    QString uri() const;
    List data() const;
    void update(const List &other);
private:
    explicit ListObject(const List &data, QObject *parent = 0);
    List m_data {};
    QObjectPtr<UserObject> m_user {nullptr};
};

}

#endif // LISTOBJECT_H
