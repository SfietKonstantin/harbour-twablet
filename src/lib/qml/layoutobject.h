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

#ifndef LAYOUTOBJECT_H
#define LAYOUTOBJECT_H

#include <QtCore/QObject>
#include "layout.h"
#include "model.h"
#include "qobjectutils.h"
#include "querytypeobject.h"
#include "tweetmodelquerywrapperobject.h"

namespace qml
{

class LayoutObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString accountUserId READ accountUserId NOTIFY accountUserIdChanged)
    Q_PROPERTY(qml::QueryTypeObject::TweetModelType queryType READ queryType NOTIFY queryTypeChanged)
    Q_PROPERTY(QObject * query READ query NOTIFY queryChanged)
    Q_PROPERTY(QVariantMap parameters READ parameters NOTIFY parametersChanged)
public:
    DISABLE_COPY_DISABLE_MOVE(LayoutObject);
    static LayoutObject * create(const Layout &data, QObject *parent = 0);
    QString name() const;
    QString accountUserId() const;
    int unread() const;
    QueryTypeObject::TweetModelType queryType() const;
    QObject * query() const;
    QVariantMap parameters() const;
    void update(const Layout &other);
signals:
    void nameChanged();
    void accountUserIdChanged();
    void queryTypeChanged();
    void unreadChanged();
    void queryChanged();
    void parametersChanged();
private:
    explicit LayoutObject(const Layout &data, QObject *parent = 0);
    Layout m_data {};
    QObjectPtr<TweetModelQueryWrapperObject> m_query {};
    QVariantMap m_parameters {};
};

}

#endif // LAYOUTOBJECT_H
