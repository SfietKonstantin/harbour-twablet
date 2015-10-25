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
#include "entityvisitor.h"

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

void EntitiesFormatter::doFormat(const QString &input, Entity::List &&entities, bool includeLinks)
{
    class FormatterVisitor: public EntityVisitor
    {
    public:
        explicit FormatterVisitor(const QString &text, bool includeLinks)
            : m_text(text), m_includeLinks(includeLinks)
        {
        }
        QString text() const
        {
            return m_text;
        }
        void visitMedia(const MediaEntity &entity) override
        {
            if (!entity.isValid()) {
                return;
            }

            QString after {};
            if (m_includeLinks) {
                after = QString(QLatin1String("<a href=\"%1\">%2</a>")).arg(entity.expandedUrl(), entity.displayUrl());
            } else {
                after = entity.displayUrl();
            }
            m_text.replace(entity.text(), after);
        }
        void visitUrl(const UrlEntity &entity) override
        {
            if (!entity.isValid()) {
                return;
            }

            QString after {};
            if (m_includeLinks) {
                after = QString(QLatin1String("<a href=\"%1\">%2</a>")).arg(entity.expandedUrl(), entity.displayUrl());
            } else {
                after = entity.displayUrl();
            }
            m_text.replace(entity.text(), after);
        }
        void visitUserMention(const UserMentionEntity &entity) override
        {
            if (!entity.isValid()) {
                return;
            }

            if (m_includeLinks) {
                QString after {QString(QLatin1String("<a href=\"user://%1\">@%2</a>")).arg(entity.id(), entity.screenName())};
                m_text.replace(entity.text(), after, Qt::CaseInsensitive);
            }
        }
        void visitHashtag(const HashtagEntity &entity) override
        {
            if (!entity.isValid()) {
                return;
            }

            if (m_includeLinks) {
                QString before {QString(QLatin1String("#%1")).arg(entity.text())};
                QString after {QString(QLatin1String("<a href=\"hashtag://%1\">#%1</a>")).arg(entity.text())};
                m_text.replace(before, after, Qt::CaseInsensitive);
            }
        }
    private:
        QString m_text {};
        bool m_includeLinks {false};
    };

    if (!m_complete) {
        return;
    }

    // Be sure to render the longer entities first
    std::sort(std::begin(entities), std::end(entities), [](const Entity::Ptr &first, const Entity::Ptr &second) {
        return (first->text().count() > second->text().count());
    });

    // Use HTML escaped to provide correct formatting for styled Label
    FormatterVisitor visitor {input.toHtmlEscaped(), includeLinks};
    for (const Entity::Ptr &entity : entities) {
        if (entity) {
            entity->accept(visitor);
        }
    }

    if (m_text != visitor.text()) {
        m_text = visitor.text();
        emit textChanged();
    }
}

}
