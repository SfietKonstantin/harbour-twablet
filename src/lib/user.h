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

#ifndef USER_H
#define USER_H

#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QJsonObject>
#include "globals.h"
#include "entity.h"

class User
{
public:
    explicit User() = default;
    explicit User(const QJsonObject &json);
    DEFAULT_COPY_DEFAULT_MOVE(User);
    bool isValid() const;
    QString id() const;
    QString name() const;
    QString screenName() const;
    QString description() const;
    std::vector<Entity::Ptr> descriptionEntities() const;
    QString location() const;
    QString url() const;
    std::vector<Entity::Ptr> urlEntities() const;
    bool isProtected() const;
    bool isFollowing() const;
    int statusesCount() const;
    int followersCount() const;
    int friendsCount() const;
    int listedCount() const;
    int favouritesCount() const;
    QString imageUrl() const;
    QString bannerUrl() const;
    QDateTime createdAt() const;
private:
    QString m_id {};
    QString m_name {};
    QString m_screenName {};
    QString m_description {};
    std::vector<Entity::Ptr> m_descriptionEntities {};
    QString m_location {};
    QString m_url {};
    std::vector<Entity::Ptr> m_urlEntities {};
    bool m_protected {false};
    bool m_following {false};
    int m_statusesCount {0};
    int m_followersCount {0};
    int m_friendsCount {0};
    int m_listedCount {0};
    int m_favouritesCount {0};
    QString m_imageUrl {};
    QString m_bannerUrl {};
    QDateTime m_createdAt {};
};

#endif // USER_H
