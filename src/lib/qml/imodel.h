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

#ifndef IMODEL_H
#define IMODEL_H

#include <QtCore/QAbstractListModel>
#include <QtQml/QQmlParserStatus>
#include "globals.h"
#include "twitterdatarepositoryobject.h"

class IModel : public QAbstractListModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
    Q_PROPERTY(int layoutIndex READ layoutIndex WRITE setLayoutIndex NOTIFY layoutIndexChanged)
    Q_PROPERTY(TwitterDataRepositoryObject * repository READ repository WRITE setRepository
               NOTIFY repositoryChanged)
    Q_ENUMS(Status)
public:
    enum Status
    {
        Idle,
        Loading,
        Error
    };
    DISABLE_COPY_DISABLE_MOVE(IModel);
    virtual ~IModel() {}
    virtual int count() const = 0;
    virtual Status status() const = 0;
    virtual QString errorMessage() const = 0;
    virtual int layoutIndex() const = 0;
    virtual void setLayoutIndex(int layoutIndex) = 0;
    virtual TwitterDataRepositoryObject * repository() const = 0;
    virtual void setRepository(TwitterDataRepositoryObject *repository) = 0;
signals:
    void countChanged();
    void prependPre();
    void prependPost(int insertedCount);
    void statusChanged();
    void errorMessageChanged();
    void layoutIndexChanged();
    void repositoryChanged();
protected:
    explicit IModel(QObject *parent = 0) : QAbstractListModel(parent), QQmlParserStatus() {}
};

#endif // IMODEL_H
