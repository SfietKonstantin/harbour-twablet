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

#include "query.h"
#include <QtCore/QLoggingCategory>
#include "private/maputil.h"
#include "private/debughelper.h"

static const QLoggingCategory logger {"query"};

Query::Query(Query::RequestType type, QByteArray &&path, Parameters &&parameters)
    : m_type(type), m_path(std::move(path)), m_parameters(std::move(parameters))
{
}

Query::~Query()
{
}

bool Query::operator==(const Query &other) const
{
    return (m_type == other.m_type) && (m_path == other.m_path)
            && (m_parameters == other.m_parameters);
}

bool Query::operator!=(const Query &other) const
{
    return !(*this == other);
}

bool Query::operator<(const Query &other) const
{
    if (m_type == other.m_type) {
        if (m_path == other.m_path) {
            return m_parameters < other.m_parameters;
        }
        return m_path < other.m_path;
    }
    return m_type < other.m_type;
}

bool Query::isValid() const
{
    return !m_path.isEmpty();
}

Query::RequestType Query::requestType() const
{
    return m_type;
}

QByteArray Query::path() const
{
    return m_path;
}

Query::Parameters Query::parameters() const
{
    return m_parameters;
}

TweetRepositoryQuery::TweetRepositoryQuery(Type type, Parameters &&additionalParameters)
    : Query(Get, pathFromTypeWithCheck(type, additionalParameters)
            , makeParameters(type, additionalParameters))
    , m_type(type)
{
}

QByteArray TweetRepositoryQuery::pathFromType(Type type)
{
    switch (type)
    {
    case Home:
        return QByteArray{"statuses/home_timeline.json"};
        break;
    case Mentions:
        return QByteArray{"statuses/mentions_timeline.json"};
        break;
    case Search:
        return QByteArray{"search/tweets.json"};
        break;
    case Favorites:
        return QByteArray{"favorites/list.json"};
        break;
    case UserTimeline:
        return QByteArray{"statuses/user_timeline.json"};
        break;
    default:
        qCDebug(logger) << "Type" << type << "is not compatible with TweetRepositoryQuery";
        return QByteArray{};
        break;
    }
}

TweetRepositoryQuery::Type TweetRepositoryQuery::type() const
{
    return m_type;
}

QByteArray TweetRepositoryQuery::pathFromTypeWithCheck(Type type, const Query::Parameters &additionalParameters)
{
    bool ok {false};
    switch (type) {
    case Home:
        ok = true;
        break;
    case Mentions:
        ok = true;
        break;
    case Search:
        if (private_util::hasValue(additionalParameters, QByteArray{"q"})) {
            ok = true;
        } else {
            qCDebug(logger) << "Search query must contain parameter q";
        }
        break;
    case Favorites:
        if (private_util::hasValue(additionalParameters, QByteArray{"user_id"})) {
            ok = true;
        } else {
            qCDebug(logger) << "Favorites query must contain parameter user_id";
        }
        break;
    case UserTimeline:
        if (private_util::hasValue(additionalParameters, QByteArray{"user_id"})) {
            ok = true;
        } else {
            qCDebug(logger) << "UserTimeline query must contain parameter user_id";
        }
        break;
    default:
        qCDebug(logger) << "Type" << type << "is not compatible with TweetRepositoryQuery";
        break;
    }

    return ok ? pathFromType(type) : QByteArray{};
}

Query::Parameters TweetRepositoryQuery::makeParameters(Type type, const Parameters &additionalParameters)
{
    Parameters returned {};
    switch (type)
    {
    case Home:
    {
        returned = Parameters{
            {"count", QByteArray::number(200)},
            {"trim_user", "false"},
            {"include_entities", "true"}
        };
        break;
    }
    case Mentions:
    {
        returned = Parameters{
            {"count", QByteArray::number(200)},
            {"trim_user", "false"},
            {"include_entities", "true"}
        };
        break;
    }
    case Search:
    {
        QByteArray q = private_util::getValue(additionalParameters, QByteArray{"q"});
        if (q.isEmpty()) {
            qCDebug(logger) << "Search query must contain parameter q";
            break;
        }
        QByteArray resultType = private_util::getValue(additionalParameters, QByteArray{"result_type"});
        returned = Parameters{
            {"count", QByteArray::number(100)},
            {"include_entities", "true"},
            {"q", q.toLower()}
        };
        if (resultType == "mixed" || resultType == "recent" || resultType == "popular") {
            returned.emplace("result_type", resultType);
        }
        break;
    }
    case Favorites:
    {
        QByteArray userId = private_util::getValue(additionalParameters, QByteArray{"user_id"});
        if (userId.isEmpty()) {
            qCDebug(logger) << "Favorites query must contain parameter user_id";
            break;
        }
        returned = Parameters{
            {"count", QByteArray::number(200)},
            {"include_entities", "true"},
            {"user_id", userId}
        };
        break;
    }
    case UserTimeline:
    {
        QByteArray userId = private_util::getValue(additionalParameters, QByteArray{"user_id"});
        if (userId.isEmpty()) {
            qCDebug(logger) << "UserTimeline query must contain parameter user_id";
            break;
        }
        QByteArray excludeReplies = private_util::getValue(additionalParameters, QByteArray{"exclude_replies"}, QByteArray{"false"});
        QByteArray includeRts = private_util::getValue(additionalParameters, QByteArray{"include_rts"}, QByteArray{"true"});
        returned = Parameters{
            {"count", QByteArray::number(200)},
            {"trim_user", "false"},
            {"include_entities", "true"},
            {"user_id", userId},
            {"exclude_replies", excludeReplies},
            {"include_rts", includeRts}
        };
        break;
    }
    default:
        break;
    }
    return returned;
}

UserRepositoryQuery::UserRepositoryQuery(Type type, Query::Parameters &&additionalParameters)
    : Query(Get, std::move(pathFromType(type)), std::move(makeParameters(std::move(additionalParameters))))
{
}

QByteArray UserRepositoryQuery::pathFromType(UserRepositoryQuery::Type type)
{
    switch (type)
    {
    case Friends:
        return QByteArray{"friends/list.json"};
        break;
    case Followers:
        return QByteArray{"followers/list.json"};
        break;
    default:
        qCDebug(logger) << "Type" << type << "is not compatible with UserRepositoryQuery";
        return QByteArray{};
        break;
    }
}

QByteArray UserRepositoryQuery::pathFromTypeWithCheck(Type type, const Query::Parameters &additionalParameters)
{
    if (!private_util::hasValue(additionalParameters, QByteArray{"user_id"})) {
        qCDebug(logger) << "UserRepositoryQuery based query must contain parameter user_id";
        return QByteArray{};
    }

    return  pathFromType(type);
}

Query::Parameters UserRepositoryQuery::makeParameters(const Query::Parameters &additionalParameters)
{
    QByteArray userId = private_util::getValue(additionalParameters, QByteArray{"user_id"});
    if (userId.isEmpty()) {
        qCDebug(logger) << "UserRepositoryQuery based query must contain parameter user_id";
        return Parameters{};
    }

    return Parameters{
        {"count", QByteArray::number(200)},
        {"user_id", userId},
        {"skip_status", "true"},
        {"include_user_entities", "true"}
    };
}

TweetItemQuery::TweetItemQuery(Type type, Parameters &&additionalParameters)
    : Query(requestTypeFromType(type), std::move(pathFromTypeWithCheck(type, additionalParameters)),
            makeParameters(type, additionalParameters))
{
}

QByteArray TweetItemQuery::pathFromType(Type type)
{
    switch (type) {
    case Show:
        return QByteArray{"statuses/show.json"};
        break;
    case StatusUpdate:
        return QByteArray{"statuses/update.json"};
        break;
    case Favorite:
        return QByteArray{"favorites/create.json"};
        break;
    case Unfavorite:
        return QByteArray{"favorites/destroy.json"};
        break;
    case Retweet:
        return QByteArray{"statuses/retweet.json"};
        break;
    default:
        qCDebug(logger) << "Type" << type << "is not compatible with TweetItemQuery";
        return QByteArray{};
        break;
    }
}

Query::RequestType TweetItemQuery::requestTypeFromType(TweetItemQuery::Type type)
{
    return type == Invalid || type == Show ? Get : Post;
}

QByteArray TweetItemQuery::pathFromTypeWithCheck(Type type, const Query::Parameters &additionalParameters)
{
    bool ok {false};
    switch (type) {
    case StatusUpdate:
        if (private_util::hasValue(additionalParameters, QByteArray{"status"})) {
            ok = true;
        } else {
            qCDebug(logger) << "StatusUpdate query must contain parameter status";
        }
        break;
    case Show:
    case Favorite:
    case Unfavorite:
    case Retweet:
        if (private_util::hasValue(additionalParameters, QByteArray{"id"})) {
            ok = true;
        } else {
            qCDebug(logger) << "Show, Favorite, Unfavorite or Retweet query must contain parameter id";
        }
        break;
    default:
        qCDebug(logger) << "Type" << type << "is not compatible with TweetItemQuery";
        break;
    }

    return ok ? pathFromType(type) : QByteArray{};
}

Query::Parameters TweetItemQuery::makeParameters(Type type, const Query::Parameters &additionalParameters)
{
    Parameters returned {};
    switch (type)
    {
    case StatusUpdate:
    {
        QByteArray status = private_util::getValue(additionalParameters, QByteArray{"status"});
        if (status.isEmpty()) {
            qCDebug(logger) << "StatusUpdate query must contain parameter status";
            break;
        }
        returned = Parameters{
            {"status", status}
        };
        QByteArray inReplyTo = private_util::getValue(additionalParameters, QByteArray{"in_reply_to_status_id"});
        if (!inReplyTo.isEmpty()) {
            returned.emplace("in_reply_to_status_id", inReplyTo);
        }
        break;
    }
    case Show:
    {
        QByteArray id = private_util::getValue(additionalParameters, QByteArray{"id"});
        if (id.isEmpty()) {
            qCDebug(logger) << "Show query must contain parameter id";
            break;
        }
        returned = Parameters{
            {"id", id},
            {"trim_user", "false"},
            {"include_my_retweet", "false"},
            {"include_entities", "true"}
        };
        break;
    }
    case Favorite:
    case Unfavorite:
    case Retweet:
    {
        QByteArray id = private_util::getValue(additionalParameters, QByteArray{"id"});
        if (id.isEmpty()) {
            qCDebug(logger) << "Favorite, Unfavorite or Retweet query must contain parameter id";
            break;
        }
        returned = Parameters{
            {"id", id}
        };
        break;
    }
    default:
        qCDebug(logger) << "Type" << type << "is not compatible with TweetItemQuery";
        break;
    }
    return returned;
}

UserItemQuery::UserItemQuery(UserItemQuery::Type type, Query::Parameters &&additionalParameters)
    : Query(requestTypeFromType(type), std::move(pathFromTypeWithCheck(type, additionalParameters)),
            makeParameters(type, additionalParameters))
{
}

QByteArray UserItemQuery::pathFromType(Type type)
{
    switch (type) {
    case Show:
        return QByteArray{"users/show.json"};
        break;
    case Follow:
        return QByteArray{"friendships/create.json"};
        break;
    case Unfollow:
        return QByteArray{"friendships/destroy.json"};
        break;
    default:
        qCDebug(logger) << "Type" << type << "is not compatible with UserItemQuery";
        return QByteArray{};
        break;
    }
}

Query::RequestType UserItemQuery::requestTypeFromType(UserItemQuery::Type type)
{
    return type == Show || type == Invalid ? Get : Post;
}

QByteArray UserItemQuery::pathFromTypeWithCheck(Type type, const Query::Parameters &additionalParameters)
{
    bool ok {false};
    switch (type) {
    case Show:
    case Follow:
    case Unfollow:
        if (private_util::hasValue(additionalParameters, QByteArray{"user_id"})) {
            ok = true;
        } else {
            qCDebug(logger) << "Show, Follow or Unfollow query must contain parameter user_id";
        }
        break;
    default:
        qCDebug(logger) << "Type" << type << "is not compatible with UserItemQuery";
        break;
    }

    return ok ? pathFromType(type) : QByteArray{};
}

Query::Parameters UserItemQuery::makeParameters(UserItemQuery::Type type,
                                                const Query::Parameters &additionalParameters)
{
    Parameters returned {};
    switch (type)
    {
    case Show:
    {
        QByteArray userId = private_util::getValue(additionalParameters, QByteArray{"user_id"});
        if (userId.isEmpty()) {
            qCDebug(logger) << "Show query must contain parameter user_id";
            break;
        }
        returned = Parameters{
            {"user_id", userId},
            {"include_entities", "true"}
        };
        break;
    }
    case Follow:
    {
        QByteArray userId = private_util::getValue(additionalParameters, QByteArray{"user_id"});
        if (userId.isEmpty()) {
            qCDebug(logger) << "Follow query must contain parameter user_id";
            break;
        }
        returned = Parameters{
            {"user_id", userId},
            {"follow", "true"}
        };
        break;
    }
    case Unfollow:
    {
        QByteArray userId = private_util::getValue(additionalParameters, QByteArray{"user_id"});
        if (userId.isEmpty()) {
            qCDebug(logger) << "Unfollow query must contain parameter user_id";
            break;
        }
        returned = Parameters{
            {"user_id", userId}
        };
        break;
    }
    default:
        qCDebug(logger) << "Type" << type << "is not compatible with UserItemQuery";
        break;
    }
    return returned;
}

ListRepositoryQuery::ListRepositoryQuery(Type type, Query::Parameters &&additionalParameters)
    : Query(Get, std::move(pathFromTypeWithCheck(type, additionalParameters)),
            makeParameters(additionalParameters))
{
}

QByteArray ListRepositoryQuery::pathFromType(Type type)
{
    switch (type) {
    case Subscriptions:
        return QByteArray{"lists/subscriptions.json"};
        break;
    case Ownerships:
        return QByteArray{"lists/ownerships.json"};
        break;
    case Memberships:
        return QByteArray{"lists/memberships.json"};
        break;
    default:
        qCDebug(logger) << "Type" << type << "is not compatible with ListRepositoryQuery";
        return QByteArray{};
        break;
    }
}

QByteArray ListRepositoryQuery::pathFromTypeWithCheck(Type type, const Query::Parameters &additionalParameters)
{
    if (!private_util::hasValue(additionalParameters, QByteArray{"user_id"})) {
        qCDebug(logger) << "ListRepositoryQuery based query must contain parameter user_id";
        return QByteArray{};
    }

    return  pathFromType(type);
}

Query::Parameters ListRepositoryQuery::makeParameters(const Query::Parameters &additionalParameters)
{
    QByteArray userId = private_util::getValue(additionalParameters, QByteArray{"user_id"});
    if (userId.isEmpty()) {
        qCDebug(logger) << "ListRepositoryQuery based query must contain parameter user_id";
        return Parameters{};
    }

    return Parameters{
        {"count", QByteArray::number(200)},
        {"user_id", userId}
    };
}
