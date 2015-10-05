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

/**
 * @brief An user
 *
 * This class represents an user from Twitter API.
 *
 * See https://dev.twitter.com/overview/api/users.
 */
class User
{
public:
    explicit User() = default;
    /**
     * @brief Constructs a User from a JSON object
     *
     * This constructor parses the input JSON object
     * that is retrieved from Twitter to create a User.
     *
     * @param json JSON object to parse.
     */
    explicit User(const QJsonObject &json);
    DEFAULT_COPY_DEFAULT_MOVE(User);
    /**
     * @brief If the User instance is valid
     *
     * An instance of a User is valid if it contains an id.
     *
     * @return if the User instance is valid.
     */
    bool isValid() const;
    /**
     * @brief Id of the user
     * @return id of the user.
     */
    QString id() const;
    /**
     * @brief Name of the user
     * @return name of the user.
     */
    QString name() const;
    /**
     * @brief Screen-name of the user
     * @return screen-name of the user.
     */
    QString screenName() const;
    /**
     * @brief Description of the user
     * @return description of the user.
     */
    QString description() const;
    /**
     * @brief Entities present in the description of the user
     * @return entities present in the description of the user.
     */
    std::vector<Entity::Ptr> descriptionEntities() const;
    /**
     * @brief Location of the user
     * @return location of the user.
     */
    QString location() const;
    /**
     * @brief Url of the user
     * @return url of the user.
     */
    QString url() const;
    /**
     * @brief Entities present in the url of the user
     * @return entities present in the url of the user.
     */
    std::vector<Entity::Ptr> urlEntities() const;
    /**
     * @brief If the user account is protected
     * @return if the user account is protected.
     */
    bool isProtected() const;
    /**
     * @brief If the user is following the current account
     * @return if the user is following the current account.
     */
    bool isFollowing() const;
    /**
     * @brief Number of status updates sent by the user
     * @return number of status updates sent by the user.
     */
    int statusesCount() const;
    /**
     * @brief Number of users following the user
     * @return number of users following the user.
     */
    int followersCount() const;
    /**
     * @brief Numbers of users the user is following
     * @return numbers of users the user is following.
     */
    int friendsCount() const;
    /**
     * @brief Number of lists in which the user is
     * @return number of lists in which the user is.
     */
    int listedCount() const;
    /**
     * @brief Number of tweets the user has marked as favourite
     * @return number of tweets the user has marked as favourite.
     */
    int favouritesCount() const;
    /**
     * @brief Url of the user's profile picture
     * @return url of the user's profile picture.
     */
    QString imageUrl() const;
    /**
     * @brief Url of the user's banner
     * @return url of the user's banner.
     */
    QString bannerUrl() const;
    /**
     * @brief When the user created his/her account
     * @return when the user created his/her account.
     */
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
