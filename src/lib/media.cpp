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

#include "media.h"
#include <QtCore/QLoggingCategory>

Media::Media(const QJsonObject &json)
{
    m_id = json.value(QLatin1String("id_str")).toString();
    m_url = json.value(QLatin1String("media_url_https")).toString();
    QString type {json.value(QLatin1String("type")).toString()};
    if (type == QLatin1String("photo")) {
        m_type = Photo;
    } else if (type == QLatin1String("video")) {
        m_type = Video;
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

QString Media::id() const
{
    return m_id;
}

QString Media::url() const
{
    return m_url;
}

Media::Type Media::type() const
{
    return m_type;
}

int Media::width() const
{
    return m_width;
}

int Media::height() const
{
    return m_height;
}

int Media::duration() const
{
    return m_duration;
}

