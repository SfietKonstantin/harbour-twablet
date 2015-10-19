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

#include "mediaentity.h"
#include <QtCore/QLoggingCategory>
#include "entityvisitor.h"

MediaEntity::MediaEntity(const QJsonObject &json)
{
    m_id = std::move(json.value(QLatin1String("id_str")).toString());

    m_text = std::move(json.value(QLatin1String("url")).toString());
    m_displayUrl = std::move(json.value(QLatin1String("display_url")).toString());
    m_expandedUrl = std::move(json.value(QLatin1String("expanded_url")).toString());

    m_mediaUrl = std::move(json.value(QLatin1String("media_url_https")).toString());
    QString type {json.value(QLatin1String("type")).toString()};
    if (type == QLatin1String("photo")) {
        m_mediaType = Photo;
    } else if (type == QLatin1String("video")) {
        m_mediaType = Video;
    } else if (type == QLatin1String("animated_gif")) {
        m_mediaType = Gif;
    } else {
        qCDebug(QLoggingCategory("media")) << "Unknown type" << type;
    }

    // Use "large" for size
    QJsonObject sizes {json.value(QLatin1String("sizes")).toObject()};
    QJsonObject sizeLarge {sizes.value(QLatin1String("large")).toObject()};
    m_width = sizeLarge.value(QLatin1String("w")).toInt();
    m_height = sizeLarge.value(QLatin1String("h")).toInt();

    if (json.contains(QLatin1String("duration_millis"))) {
        m_duration = json.value(QLatin1String("duration_millis")).toInt();
    }
}

bool MediaEntity::isValid() const
{
    return !m_id.isEmpty() && !m_text.isEmpty() && !m_displayUrl.isEmpty() && !m_expandedUrl.isEmpty() && !m_mediaUrl.isEmpty();
}

QString MediaEntity::id() const
{
    return m_id;
}

QString MediaEntity::text() const
{
    return m_text;
}

QString MediaEntity::displayUrl() const
{
    return m_displayUrl;
}

QString MediaEntity::expandedUrl() const
{
    return m_expandedUrl;
}

QString MediaEntity::mediaUrl() const
{
    return m_mediaUrl;
}

QString MediaEntity::mediaUrlLarge() const
{
    if (m_mediaUrl.isEmpty()) {
        return QString();
    }
    return m_mediaUrl + QLatin1String(":large");
}

MediaEntity::Type MediaEntity::mediaType() const
{
    return m_mediaType;
}

int MediaEntity::width() const
{
    return m_width;
}

int MediaEntity::height() const
{
    return m_height;
}

int MediaEntity::duration() const
{
    return m_duration;
}

void MediaEntity::accept(EntityVisitor &visitor) const
{
    visitor.visitMedia(*this);
}

