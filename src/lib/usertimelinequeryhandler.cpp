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

#include "usertimelinequeryhandler.h"
#include <QtCore/QUrl>

UserTimelineQueryHandler::UserTimelineQueryHandler(const Query::Parameters &parameters)
    : AbstractTweetListQueryHandler()
{
    auto userIdIt = parameters.find(QLatin1String("user_id"));
    if (userIdIt != std::end(parameters)) {
        m_userId = userIdIt->second;
    }

    auto excludeRepliesIt = parameters.find(QLatin1String("exclude_replies"));
    if (excludeRepliesIt != std::end(parameters)) {
        m_excludeReplies = excludeRepliesIt->second;
    }

    auto includeRtsIt = parameters.find(QLatin1String("include_rts"));
    if (includeRtsIt != std::end(parameters)) {
        m_includeRts = includeRtsIt->second;
    }
}

QString UserTimelineQueryHandler::path() const
{
    return QLatin1String("statuses/user_timeline.json");
}

AbstractTweetListQueryHandler::Parameters UserTimelineQueryHandler::commonParameters() const
{
    return Parameters{
        {"count", QByteArray::number(200)},
        {"trim_user", "false"},
        {"include_entities", "true"},
        {"user_id", QUrl::toPercentEncoding(m_userId)},
        {"exclude_replies", QUrl::toPercentEncoding(m_excludeReplies)},
        {"include_rts", QUrl::toPercentEncoding(m_includeRts)}
    };
}

