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

#include "usermentionentity.h"
#include "entityvisitor.h"

UserMentionEntity::UserMentionEntity(const QJsonObject &json)
{
    m_screenName = std::move(json.value(QLatin1String("screen_name")).toString());
    m_text = QString(QLatin1String("@%1")).arg(m_screenName);
    m_id = std::move(json.value(QLatin1String("id_str")).toString());
    m_name = std::move(json.value(QLatin1String("name")).toString());
}

bool UserMentionEntity::isValid() const
{
    return !m_id.isEmpty() && !m_screenName.isEmpty() && !m_name.isEmpty();
}

QString UserMentionEntity::text() const
{
    return m_text;
}

QString UserMentionEntity::id() const
{
    return m_id;
}

QString UserMentionEntity::screenName() const
{
    return m_screenName;
}

QString UserMentionEntity::name() const
{
    return m_name;
}

void UserMentionEntity::accept(EntityVisitor &visitor) const
{
    visitor.visitUserMention(*this);
}

