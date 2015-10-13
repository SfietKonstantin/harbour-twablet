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

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include "globals.h"

/**
 * @brief An account
 *
 * This class contains data about a Twitter account.
 * It stores information used to perform query, that
 * are token() and tokenSecret(), as well as information
 * describing the account, like name(), userId() and screenName().
 */
class Account
{
public:
    explicit Account() = default;
    /**
     * @brief Constructor
     * @param name name of the account.
     * @param userId user id of the account.
     * @param screenName screen-name of the account.
     * @param token token for this account.
     * @param tokenSecret token secret for this account.
     */
    explicit Account(const QString &name, const QString &userId, const QString &screenName,
                     const QByteArray &token, const QByteArray &tokenSecret);
    DEFAULT_COPY_DEFAULT_MOVE(Account);
    /**
     * @brief If the Account instance is valid
     *
     * An instance of a Account is valid if it has a name,
     * an user id, a screen-name, a token and a token secret.
     *
     * @return if the Account instance is valid.
     */
    bool isValid() const;
    /**
     * @brief Name of the account
     * @return name of the account.
     */
    QString name() const;
    /**
     * @brief Set the name of the account
     * @param name name of the account.
     */
    void setName(const QString &name);
    /**
     * @brief Id of the user of the account
     * @return id of the user of the account.
     */
    QString userId() const;
    /**
     * @brief Screen-name of the account
     * @return screen-name of the account.
     */
    QString screenName() const;
    /**
     * @brief Token to use to perform queries for this account
     * @return token to use to perform queries for this account.
     */
    QByteArray token() const;
    /**
     * @brief Token secret to use to perform queries for this account
     * @return token secret to use to perform queries for this account.
     */
    QByteArray tokenSecret() const;
private:
    QString m_name {};
    QString m_userId {};
    QString m_screenName {};
    QByteArray m_token {};
    QByteArray m_tokenSecret {};
};

#endif // ACCOUNT_H
