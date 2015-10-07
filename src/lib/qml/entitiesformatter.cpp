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

#include "entitiesformatter.h"
#include "mediaentity.h"
#include "urlentity.h"
#include "usermentionentity.h"
#include "hashtagentity.h"

namespace qml
{

EntitiesFormatter::EntitiesFormatter(QObject *parent)
    : QObject(parent)
{
}

void EntitiesFormatter::classBegin()
{
}

void EntitiesFormatter::componentComplete()
{
    m_complete = true;
    format();
}

QString EntitiesFormatter::text() const
{
    return m_text;
}

void EntitiesFormatter::doFormat(const QString &input, const Entity::List &entities)
{
    if (!m_complete) {
        return;
    }

    QString text {input};
    for (const Entity::Ptr &entity : entities) {
        switch (entity->type()) {
        case Entity::Media:
            doFormatMedia(text, dynamic_cast<MediaEntity *>(entity.get()));
            break;
        case Entity::Url:
            doFormatUrl(text, dynamic_cast<UrlEntity *>(entity.get()));
            break;
        case Entity::UserMention:
            doFormatUserMention(text, dynamic_cast<UserMentionEntity *>(entity.get()));
            break;
        case Entity::Hashtag:
            doFormatHashtag(text, dynamic_cast<HashtagEntity *>(entity.get()));
            break;
        default:
            break;
        }
    }

    if (m_text != text) {
        m_text = text;
        emit textChanged();
    }
}

void EntitiesFormatter::doFormatMedia(QString &text, MediaEntity *entity)
{
    if (!entity || !entity->isValid()) {
        return;
    }

    QString after {QString(QLatin1String("<a href=\"%1\">%2</a>")).arg(entity->expandedUrl(), entity->displayUrl())};
    text.replace(entity->text(), after);
}

void EntitiesFormatter::doFormatUrl(QString &text, UrlEntity *entity)
{
    if (!entity || !entity->isValid()) {
        return;
    }

    QString after {QString(QLatin1String("<a href=\"%1\">%2</a>")).arg(entity->expandedUrl(), entity->displayUrl())};
    text.replace(entity->text(), after);
}

void EntitiesFormatter::doFormatUserMention(QString &text, UserMentionEntity *entity)
{
    if (!entity || !entity->isValid()) {
        return;
    }

    QString after {QString(QLatin1String("<a href=\"user://%1\">@%2</a>")).arg(entity->id(), entity->screenName())};
    text.replace(entity->text(), after, Qt::CaseInsensitive);
}

void EntitiesFormatter::doFormatHashtag(QString &text, HashtagEntity *entity)
{
    if (!entity || !entity->isValid()) {
        return;
    }

    QString before {QString(QLatin1String("#%1")).arg(entity->text())};
    QString after {QString(QLatin1String("<a href=\"hashtag://%1\">#%1</a>")).arg(entity->text())};
    text.replace(before, after, Qt::CaseInsensitive);
}

}
