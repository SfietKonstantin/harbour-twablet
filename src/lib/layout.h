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

#ifndef LAYOUT_H
#define LAYOUT_H

#include <QtCore/QString>
#include "query.h"

/**
 * @brief A layout
 *
 * A layout describes the content to display in a
 * column. It consists of a name, the user id
 * of the account to use and the Query to use.
 * These properties can be accessed via name(),
 * userId() and query().
 *
 * This class also manage the number of unread entries in
 * the column. This can be accessed via unread() and set
 * via setUnread().
 */
class Layout
{
public:
    explicit Layout() = default;
    /**
     * @brief Constructor
     * @param name name of the layout.
     * @param userId user id of the account.
     * @param query query.
     */
    explicit Layout(const QString &name, const QString &accountUserId, TweetListQuery &&query);
    DEFAULT_COPY_DEFAULT_MOVE(Layout);
    /**
     * @brief If the Layout instance is valid
     *
     * An instance of a Layout is valid if it has a name,
     * an user id and a valid Query.
     *
     * @return if the Layout instance is valid.
     */
    bool isValid() const;
    /**
     * @brief Name of the layout
     * @return name of the layout.
     */
    QString name() const;
    /**
     * @brief Set the name of the layout
     * @param name name of the layout.
     */
    void setName(const QString &name);
    /**
     * @brief User id of the account to use
     * @return user id of the account to use.
     */
    QString accountUserId() const;
    /**
     * @brief Set the user id of the account to use
     * @param userId user id of the account to use.
     */
    void setAccountUserId(const QString &accountUserId);
    /**
     * @brief Query to use
     * @return query to use.
     */
    TweetListQuery query() const;
    /**
     * @brief Set the query to use
     * @param query query to use.
     */
    void setQuery(TweetListQuery &&query);
    /**
     * @brief The number of unread entries
     * @return number of unread entries.
     */
    int unread() const;
    /**
     * @brief Set the number of unread entries
     * @param unread number of unread entries.
     */
    void setUnread(int unread);
private:
    QString m_name {};
    QString m_accountUserId {};
    TweetListQuery m_query {};
    int m_unread {0};
};

#endif // LAYOUT_H
