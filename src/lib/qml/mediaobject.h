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

#ifndef MEDIAOBJECT_H
#define MEDIAOBJECT_H

#include <QtCore/QObject>
#include <QtCore/QSize>
#include "mediaentity.h"

namespace qml
{

class MediaObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString url READ url CONSTANT)
    Q_PROPERTY(QString urlLarge READ urlLarge CONSTANT)
    Q_PROPERTY(Type type READ type CONSTANT)
    Q_PROPERTY(int duration READ duration CONSTANT)
    Q_PROPERTY(QSize size READ size CONSTANT)
    Q_ENUMS(Type)
public:
    enum Type
    {
        Invalid = MediaEntity::InvalidMedia,
        Photo = MediaEntity::Photo,
        Video = MediaEntity::Video,
        Gif = MediaEntity::Gif
    };
    DISABLE_COPY_DISABLE_MOVE(MediaObject);
    static MediaObject * create(const MediaEntity &data, QObject *parent = 0);
    QString id() const;
    QString url() const;
    QString urlLarge() const;
    Type type() const;
    QSize size() const;
    int duration() const;
private:
    explicit MediaObject(const MediaEntity &data, QObject *parent = 0);
    MediaEntity m_data {};
    QSize m_size {};
};

}

#endif // MEDIAOBJECT_H
