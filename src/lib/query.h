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

#ifndef QUERY_H
#define QUERY_H

#include <QtCore/QObject>
#include <QtCore/QVariantMap>
#include "globals.h"

/**
 * @brief Information about the query used to load a Layout
 *
 * This class contains information about the query that is
 * used to load a Layout.
 *
 * A query is described by a Type and a list of parameters,
 * that can be stored in Parameters.
 */
class Query
{
public:
    /**
     * @brief Query type
     */
    enum Type
    {
        /**
         * @brief Invalid query
         */
        Invalid = 0,
        /**
         * @brief Query the home timeline
         */
        Home,
        /**
         * @brief Query the mentions timeline
         */
        Mentions,
        /**
         * @brief Search for Tweets
         */
        Search,
        /**
         * @brief Query the list of favourites for a user
         */
        Favorites,
        /**
         * @brief Query the timeline of an user
         */
        UserTimeline,
        /**
         * @brief Query the list of friends of an user (following)
         */
        Friends,
        /**
         * @brief Query the list of followers of an user
         */
        Followers
    };
    /**
     * @brief Additional parameters for the query
     *
     * The additional parameters are passed as a std::map that
     * maps a QString key to a QString value.
     */
    using Parameters = std::map<QString, QString>;
    explicit Query() = default;
    /**
     * @brief Constructor
     * @param type query type.
     * @param parameters query parameters.
     */
    explicit Query(Type type, Parameters &&parameters);
    DEFAULT_COPY_DEFAULT_MOVE(Query);
    /**
     * @brief If the Query instance is valid
     *
     * An instance of a Query is valid if it's type is not Invalid.
     *
     * @return if the Query instance is valid.
     */
    bool isValid() const;
    /**
     * @brief Type of the query
     * @return type of the query.
     */
    Type type() const;
    /**
     * @brief Parameters used for this query
     * @return parameters used for this query.
     */
    Parameters parameters() const;
    bool operator==(const Query &other) const;
    bool operator!=(const Query &other) const;
    bool operator<(const Query &other) const;
private:
    Type m_type {Invalid};
    Parameters m_parameters {};
};

#endif // QUERY_H
