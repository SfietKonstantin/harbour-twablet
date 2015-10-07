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

#ifndef MEDIAENTITY_H
#define MEDIAENTITY_H

#include "entity.h"
#include "globals.h"

class QJsonObject;
class MediaEntity: public Entity
{
public:
    enum MediaType
    {
        InvalidMedia,
        Photo,
        Video,
        Gif
    };
    explicit MediaEntity() = default;
    explicit MediaEntity(const QJsonObject &json);
    DEFAULT_COPY_DEFAULT_MOVE(MediaEntity);
    Type type() const override;
    bool isValid() const override;
    QString id() const;
    QString text() const;
    QString displayUrl() const;
    QString expandedUrl() const;
    QString mediaUrl() const;
    QString mediaUrlLarge() const;
    MediaType mediaType() const;
    int width() const;
    int height() const;
    int duration() const;
private:
    QString m_id {};
    QString m_text {};
    QString m_displayUrl {};
    QString m_expandedUrl {};
    QString m_mediaUrl {};
    MediaType m_mediaType {InvalidMedia};
    int m_width {-1};
    int m_height {-1};
    int m_duration {-1};
};

#endif // MEDIAENTITY_H
