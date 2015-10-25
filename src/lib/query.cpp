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
#include "private/maputil.h"

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

TweetListQuery::TweetListQuery(Type type, Parameters &&additionalParameters)
    : Query(Get, pathFromTypeWithCheck(type, additionalParameters)
            , makeParameters(type, additionalParameters))
    , m_type(type)
{
}

QByteArray TweetListQuery::pathFromType(Type type)
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
        return QByteArray();
        break;
    }
}

TweetListQuery::Type TweetListQuery::type() const
{
    return m_type;
}

QByteArray TweetListQuery::pathFromTypeWithCheck(TweetListQuery::Type type,
                                                 const Query::Parameters &additionalParameters)
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
        ok = private_util::hasValue(additionalParameters, QByteArray{"q"});
        break;
    case Favorites:
        ok = private_util::hasValue(additionalParameters, QByteArray{"user_id"});
        break;
    case UserTimeline:
        ok = private_util::hasValue(additionalParameters, QByteArray{"user_id"});
        break;
    default:
        break;
    }

    return ok ? pathFromType(type) : QByteArray{};
}

Query::Parameters TweetListQuery::makeParameters(Type type, const Parameters &additionalParameters)
{
    Parameters returned {};
    switch (type)
    {
    case Home:
    {
        returned = std::move(Parameters{
            {"count", QByteArray::number(200)},
            {"trim_user", "false"},
            {"include_entities", "true"}
        });
        break;
    }
    case Mentions:
    {
        returned = std::move(Parameters{
            {"count", QByteArray::number(200)},
            {"trim_user", "false"},
            {"include_entities", "true"}
        });
        break;
    }
    case Search:
    {
        QByteArray q = private_util::getValue(additionalParameters, QByteArray{"q"});
        if (q.isEmpty()) {
            break;
        }
        QByteArray resultType = private_util::getValue(additionalParameters, QByteArray{"result_type"});
        Parameters searchParameters {
            {"count", QByteArray::number(100)},
            {"include_entities", "true"},
            {"q", q.toLower()}
        };
        if (resultType == "mixed" || resultType == "recent" || resultType == "popular") {
            searchParameters.emplace("result_type", resultType);
        }
        returned = std::move(searchParameters);
        break;
    }
    case Favorites:
    {
        QByteArray userId = private_util::getValue(additionalParameters, QByteArray{"user_id"});
        if (userId.isEmpty()) {
            break;
        }
        returned = std::move(Parameters{
            {"count", QByteArray::number(200)},
            {"include_entities", "true"},
            {"user_id", userId}
        });
        break;
    }
    case UserTimeline:
    {
        QByteArray userId = private_util::getValue(additionalParameters, QByteArray{"user_id"});
        if (userId.isEmpty()) {
            break;
        }
        QByteArray excludeReplies = private_util::getValue(additionalParameters, QByteArray{"exclude_replies"}, QByteArray{"false"});
        QByteArray includeRts = private_util::getValue(additionalParameters, QByteArray{"include_rts"}, QByteArray{"true"});
        returned = std::move(Parameters{
            {"count", QByteArray::number(200)},
            {"trim_user", "false"},
            {"include_entities", "true"},
            {"user_id", userId},
            {"exclude_replies", excludeReplies},
            {"include_rts", includeRts}
        });
        break;
    }
    default:
        break;
    }
    return returned;
}

UserListQuery::UserListQuery(UserListQuery::Type type, Query::Parameters &&additionalParameters)
    : Query(Get, std::move(pathFromType(type)), std::move(makeParameters(std::move(additionalParameters))))
{
}

QByteArray UserListQuery::pathFromType(UserListQuery::Type type)
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
        return QByteArray();
        break;
    }
}

QByteArray UserListQuery::pathFromTypeWithCheck(UserListQuery::Type type,
                                                const Query::Parameters &additionalParameters)
{
    return private_util::hasValue(additionalParameters, QByteArray{"user_id"}) ? pathFromType(type)
                                                                               : QByteArray{};
}

Query::Parameters UserListQuery::makeParameters(const Query::Parameters &additionalParameters)
{
    QByteArray userId = private_util::getValue(additionalParameters, QByteArray{"user_id"});
    if (userId.isEmpty()) {
        return Parameters{};
    }

    return Parameters{
        {"count", QByteArray::number(200)},
        {"user_id", userId},
        {"skip_status", "true"},
        {"include_user_entities", "true"}
    };
}


