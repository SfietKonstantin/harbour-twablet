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

#ifndef USEROBJECT_H
#define USEROBJECT_H

#include <QtCore/QObject>
#include "user.h"
#include "model.h"

namespace qml
{

class UserObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool valid READ isValid CONSTANT)
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString screenName READ screenName CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(QString location READ location CONSTANT)
    Q_PROPERTY(QString displayUrl READ displayUrl CONSTANT)
    Q_PROPERTY(QString url READ url CONSTANT)
    Q_PROPERTY(bool protected READ isProtected CONSTANT)
    Q_PROPERTY(bool following READ isFollowing CONSTANT)
    Q_PROPERTY(int statusesCount READ statusesCount CONSTANT)
    Q_PROPERTY(int followersCount READ followersCount CONSTANT)
    Q_PROPERTY(int friendsCount READ friendsCount CONSTANT)
    Q_PROPERTY(int listedCount READ listedCount CONSTANT)
    Q_PROPERTY(int favouritesCount READ favouritesCount CONSTANT)
    Q_PROPERTY(QString imageUrl READ imageUrl CONSTANT)
    Q_PROPERTY(QString bannerUrl READ bannerUrl CONSTANT)
    Q_PROPERTY(int tweetsPerDay READ tweetsPerDay CONSTANT)
public:
    DISABLE_COPY_DISABLE_MOVE(UserObject);
    static UserObject * create(const User &data, QObject *parent = 0);
    bool isValid() const;
    QString id() const;
    QString name() const;
    QString screenName() const;
    QString description() const;
    QString location() const;
    QString displayUrl() const;
    QString url() const;
    bool isProtected() const;
    bool isFollowing() const;
    int statusesCount() const;
    int followersCount() const;
    int friendsCount() const;
    int listedCount() const;
    int favouritesCount() const;
    QString imageUrl() const;
    QString bannerUrl() const;
    int tweetsPerDay() const;
    User data() const;
private:
    explicit UserObject(const User &data, QObject *parent = 0);
    void update(const User &other);
    void initializeUrl();
    User m_data {};
    QString m_displayUrl {};
    QString m_url {};
    friend class Model<User, UserObject>;
};

}

#endif // USEROBJECT_H
