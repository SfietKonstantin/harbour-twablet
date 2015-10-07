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

#include "mediamodel.h"

namespace qml
{

MediaModel::MediaModel(const Entity::List &entities, QObject *parent)
    : QAbstractListModel(parent)
{
    std::vector<MediaEntity> media {};
    for (const Entity::Ptr &entity : entities) {
        if (entity->type() == Entity::Media) {
            MediaEntity *mediaEntity {dynamic_cast<MediaEntity *>(entity.get())};
            media.emplace_back(*mediaEntity);
        }
    }


    beginInsertRows(QModelIndex(), 0, media.size() - 1);
    for (const MediaEntity &medium : media) {
        m_data.emplace_back(MediaObject::create(medium, this));
    }
    emit countChanged();
    endInsertRows();
}

MediaModel * MediaModel::create(const Entity::List &entities, QObject *parent)
{
    return new MediaModel(entities, parent);
}

int MediaModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_data.size();
}

QVariant MediaModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if (row < 0 || row >= rowCount()) {
        return QVariant();
    }
    const QObjectPtr<MediaObject> &media = m_data[row];
    switch (role) {
    case MediaRole:
        return QVariant::fromValue(media.get());
        break;
    default:
        return QVariant();
        break;
    }
}

int MediaModel::count() const
{
    return rowCount();
}

QHash<int, QByteArray> MediaModel::roleNames() const
{
    return {{MediaRole, "media"}};
}

}
