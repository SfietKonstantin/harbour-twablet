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

#ifndef IQUERYITEM_H
#define IQUERYITEM_H

#include <QtCore/QObject>
#include <QtQml/QQmlParserStatus>
#include "globals.h"

namespace qml
{

class AccountObject;
class IQueryItem : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
    Q_PROPERTY(QObject * repository READ repository WRITE setRepository NOTIFY repositoryChanged)
    Q_PROPERTY(QObject * query READ query WRITE setQuery NOTIFY queryChanged)
    Q_ENUMS(Status)
public:
    enum Status
    {
        Idle,
        Loading,
        Error
    };
    DISABLE_COPY_DISABLE_MOVE(IQueryItem);
    virtual Status status() const = 0;
    virtual QString errorMessage() const = 0;
    virtual QObject * repository() const = 0;
    virtual void setRepository(QObject *repository) = 0;
    virtual QObject * query() const = 0;
    virtual void setQuery(QObject *query) = 0;
public slots:
    virtual bool load() = 0;
signals:
    void statusChanged();
    void errorMessageChanged();
    void repositoryChanged();
    void queryChanged();
    void finished();
    void error();
protected:
    explicit IQueryItem(QObject *parent = 0) : QObject(parent) {}
};

}

#endif // IQUERYITEM_H

