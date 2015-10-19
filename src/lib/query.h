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

#include "globals.h"
#include <map>
#include <QtCore/QByteArray>

/**
 * @brief A query
 *
 * This class describes a query that can be sent to Twitter API.
 * A query is described by
 * - a request type (either GET or POST)
 * - a path
 * - a list of parameters
 *
 * Those properties can be accessed via requestType(), path() and parameters().
 *
 * For a query to be valid, a path must be set. isValid() can be used to check
 * the validity of the query.
 *
 * Query contains the raw information about the query and perform no check about
 * the validity of the path or the parameters. Specialized subclasses can be used
 * to create queries for tweets, users or other requests.
 *
 * @see TweetListQuery
 * @see UserListQuery
 */
class Query
{
public:
    using Parameters = std::map<QByteArray, QByteArray>;
    /**
     * @brief The request type
     */
    enum RequestType
    {
        /**
         * @brief a GET request
         */
        Get,
        /**
         * @brief a POST request
         */
        Post
    };
    explicit Query() = default;
    /**
     * @brief Constructor
     * @param requestType the request type.
     * @param path the path.
     * @param parameters the parameters.
     */
    explicit Query(RequestType requestType, QByteArray &&path, Parameters &&parameters);
    virtual ~Query();
    DEFAULT_COPY_DEFAULT_MOVE(Query);
    bool operator==(const Query &other) const;
    bool operator!=(const Query &other) const;
    bool operator<(const Query &other) const;
    /**
     * @brief If the query is valid
     *
     * A query is valid if it contains a non-empty path.
     *
     * @return if the query is valid.
     */
    bool isValid() const;
    /**
     * @brief The request type
     * @return the request type.
     */
    RequestType requestType() const;
    /**
     * @brief The path
     * @return the path.
     */
    QByteArray path() const;
    /**
     * @brief The list of parameters
     * @return the list of parameters.
     */
    Parameters parameters() const;
private:
    RequestType m_type {Get};
    QByteArray m_path {};
    Parameters m_parameters {};
};

/**
 * @brief A specialized Query to request a list of tweets
 *
 * This class describes a query that is used to request a list
 * of tweets. It performs checks, and uses a list of predefined
 * types to determine the list of tweets to query.
 */
class TweetListQuery: public Query
{
public:
    /**
     * @brief The type of query
     */
    enum Type
    {
        /**
         * @brief An invalid query
         */
        Invalid = 0,
        /**
         * @brief The home timeline
         */
        Home = 1,
        /**
         * @brief The mentions
         */
        Mentions = 2,
        /**
         * @brief Search for a list of tweets
         */
        Search = 3,
        /**
         * @brief The favorites of a given user
         */
        Favorites = 4,
        /**
         * @brief The timeline of a given user
         */
        UserTimeline = 5
    };
    explicit TweetListQuery() = default;
    /**
     * @brief Constructor
     *
     * This constructors uses Type to create the path
     * and parameters to be used in the query.
     *
     * - For Search, "q" is a mandatory additional parameter
     * - For Favorites and UserTimeline, "user_id" is a mandatory additional parameter
     *
     * @param type type of query to create.
     * @param additionalParameters additional parameters.
     */
    explicit TweetListQuery(Type type, Parameters &&additionalParameters);
    DEFAULT_COPY_DEFAULT_MOVE(TweetListQuery);
    /**
     * @brief Path from type
     * @param type type of the query.
     * @return path of the query.
     */
    static QByteArray pathFromType(Type type);
    /**
     * @brief Type of the query
     * @return type of the query.
     */
    Type type() const;
private:
    static QByteArray pathFromTypeWithCheck(Type type, const Parameters &additionalParameters);
    static Parameters makeParameters(Type type, const Parameters &additionalParameters);
    Type m_type {Invalid};
};

class UserListQuery: public Query
{
public:
    enum Type
    {
        Invalid = 0,
        Friends,
        Followers
    };
    explicit UserListQuery() = default;
    explicit UserListQuery(Type type, Parameters &&additionalParameters);
    DEFAULT_COPY_DEFAULT_MOVE(UserListQuery);
    static QByteArray pathFromType(Type type);
private:
    static QByteArray pathFromTypeWithCheck(Type type, const Parameters &additionalParameters);
    static Parameters makeParameters(const Parameters &additionalParameters);
};

#endif // QUERY_H
