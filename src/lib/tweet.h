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

#ifndef TWEET_H
#define TWEET_H

#include <QtCore/QString>
#include <QtCore/QDateTime>
#include "globals.h"
#include "user.h"

/**
 * @brief A tweet
 *
 * This class represents an tweet from Twitter API.
 *
 * See https://dev.twitter.com/overview/api/tweets.
 */
class Tweet
{
public:
    explicit Tweet() = default;
    /**
     * @brief Constructs a Tweet from a JSON object
     *
     * This constructor parses the input JSON object
     * that is retrieved from Twitter to create a Tweet.
     *
     * It also performs some adaptations to make the
     * data more display friendly. All fields except id()
     * and source() will be parsed from the retweeted tweet
     * (if there is one). Id of the retweeted tweet can be
     * accessed with originalId(). The user who sent the
     * retweet can be accessed with retweetingUser().
     *
     * @param json JSON object to parse.
     */
    explicit Tweet(const QJsonObject &json);
    DEFAULT_COPY_DEFAULT_MOVE(Tweet);
    /**
     * @brief If the Tweet instance is valid
     *
     * An instance of a Tweet is valid if it contains an id.
     *
     * @return if the Tweet instance is valid.
     */
    bool isValid() const;
    /**
     * @brief Id of the tweet
     * @return id of the tweet.
     */
    QString id() const;
    /**
     * @brief Id of the retweet
     *
     * This method will return the id of the
     * retweet that is contained in the tweet,
     * if there is any retweet.
     *
     * If there is no retweet, this method will
     * return a null QString().
     *
     * @return id of the retweet.
     */
    QString originalId() const;
    /**
     * @brief Text of the tweet
     * @return text of the tweet.
     */
    QString text() const;
    /**
     * @brief User who sent the tweet
     *
     * If this tweet is a retweet, this
     * method will return the user who
     * sent the original tweet.
     *
     * To get the user who sent the retweet,
     * use retweetingUser().
     *
     * @return user who sent the tweet.
     */
    User user() const;
    /**
     * @brief User who sent the tweet, or retweet
     *
     * If this tweet is a retweet, this
     * method will return the user who
     * sent the retweet.
     *
     * To get the user who sent the original tweet,
     * use user().
     *
     * @return user who sent the tweet or retweet.
     */
    User retweetingUser() const;
    /**
     * @brief When the tweet has been sent
     * @return when the tweet has been sent.
     */
    QDateTime timestamp() const;
    /**
     * @brief The number of times this tweet has been favorited
     * @return the number of times this tweet has been favorited.
     */
    int favoriteCount() const;
    /**
     * @brief If the current account favorited this tweet
     * @return if the current account favorited this tweet.
     */
    bool isFavorited() const;
    /**
     * @brief Set if the current account favorited this tweet
     * @param favorited if the current account favorited this tweet.
     */
    void setFavorited(bool favorited);
    /**
     * @brief The number of times this tweet has been retweeted
     * @return the number of times this tweet has been retweeted.
     */
    int retweetCount() const;
    /**
     * @brief If the current account retweeted this tweet
     * @return if the current account retweeted this tweet.
     */
    bool isRetweeted() const;
    /**
     * @brief Set if the current account retweeted this tweet
     * @param favorited if the current account retweeted this tweet.
     */
    void setRetweeted(bool retweeted);
    /**
     * @brief Id of the tweet that this tweet replies to
     * @return id of the tweet that this tweet replies to.
     */
    QString inReplyTo() const;
    /**
     * @brief What that was used to post this tweet
     * @return what that was used to post this tweet.
     */
    QString source() const;
    /**
     * @brief Entities contained in this tweet
     * @return entities contained in this tweet.
     */
    Entity::List entities() const;
private:
    QString m_id {};
    QString m_originalId {};
    QString m_text {};
    User m_user {};
    User m_retweetingUser {};
    QDateTime m_timestamp {};
    int m_favoriteCount {};
    bool m_favorited {};
    int m_retweetCount {};
    bool m_retweeted {};
    QString m_inReplyTo {};
    QString m_source {};
    Entity::List m_entities;
};

#endif // TWEET_H
