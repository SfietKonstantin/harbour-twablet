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

#ifndef QUOTEDTWEET_H
#define QUOTEDTWEET_H

#include "globals.h"
#include "user.h"

/**
 * @brief A quoted tweet
 *
 * This class represents an quoted tweet.
 * A quoted tweet is a tweet, however, since
 * most of the properties are not displayed,
 * we only save some of the properties we are
 * interested in.
 */
class QuotedTweet
{
public:
    explicit QuotedTweet() = default;
    /**
     * @brief Constructs a quoted tweet from a JSON object
     *
     * This constructor parses the input JSON object
     * that is retrieved from Twitter to create a quoted tweet.
     *
     * @param json JSON object to parse.
     */
    explicit QuotedTweet(const QJsonObject &json);
    DEFAULT_COPY_DEFAULT_MOVE(QuotedTweet);
    /**
     * @brief If the quoted tweet instance is valid
     *
     * An instance of a quoted tweet is valid if it contains an id.
     *
     * @return if the quoted tweet instance is valid.
     */
    bool isValid() const;
    /**
     * @brief Id of the quoted tweet
     * @return id of the quoted tweet.
     */
    QString id() const;
    /**
     * @brief Text of the quoted tweet
     * @return text of the quoted tweet.
     */
    QString text() const;
    /**
     * @brief User who sent the quoted tweet
     * @return user who sent the quoted tweet.
     */
    User user() const;
    /**
     * @brief Entities contained in this tweet
     * @return entities contained in this tweet.
     */
    Entity::List entities() const;
private:
    QString m_id {};
    QString m_text {};
    User m_user {};
    Entity::List m_entities;
};

#endif // QUOTEDTWEET_H

