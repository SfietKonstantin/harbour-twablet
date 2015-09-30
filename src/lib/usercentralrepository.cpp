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

#include "usercentralrepository.h"
#include "private/twitterqueryutil.h"
#include "friendsqueryhandler.h"
#include "followersqueryhandler.h"
#include <QtCore/QLoggingCategory>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtNetwork/QNetworkReply>

UserCentralRepository::UserCentralRepository()
    : m_network{new QNetworkAccessManager()}
{
}

bool UserCentralRepository::isValid(int index) const
{
    return m_mapping.find(index) != std::end(m_mapping);
}

UserRepository &UserCentralRepository::repository(int index)
{
    return m_mapping.at(index).repository;
}

void UserCentralRepository::refresh(int index)
{
    auto it = m_mapping.find(index);
    if (it != std::end(m_mapping)) {
        load(it->second, IQueryHandler<User>::Refresh);
    }
}

void UserCentralRepository::loadMore(int index)
{
    auto it = m_mapping.find(index);
    if (it != std::end(m_mapping)) {
        load(it->second, IQueryHandler<User>::LoadMore);
    }
}

int UserCentralRepository::addRepository(const Account &account, const Query &query)
{
    int index = m_index;
    ++m_index;
    MappingData *data {getMappingData(index, account, query)};
    if (!data) {
        return -1;
    }
    loadMore(index);
    return index;
}

void UserCentralRepository::removeRepository(int index)
{
    m_mapping.erase(index);
}

void UserCentralRepository::load(UserCentralRepository::MappingData &mappingData,
                                 IQueryHandler<User>::RequestType requestType)
{
    QString path {};
    std::map<QByteArray, QByteArray> parameters {};
    mappingData.handler->createRequest(requestType, path, parameters);
    qCDebug(QLoggingCategory("user-central-repository")) << "Request:" << path << parameters;

    QNetworkReply *reply {TwitterQueryUtil::get(*m_network, path, parameters, mappingData.account)};
    mappingData.repository.start();

    QObject::connect(reply, &QNetworkReply::finished, [this, &mappingData, reply, requestType]() {
        QObjectPtr<QNetworkReply> replyPtr {reply};

        if (replyPtr->error() != QNetworkReply::NoError) {
            qCWarning(QLoggingCategory("user-central-repository")) << "Error happened during request for users" << mappingData.account.userId() << mappingData.query.type();
            qCWarning(QLoggingCategory("user-central-repository")) << "Error code:" << reply->error();
            qCWarning(QLoggingCategory("user-central-repository")) << "Error message (Qt):" << reply->errorString();
            const QByteArray &data {reply->readAll()};
            qCWarning(QLoggingCategory("user-central-repository")) << "Error message (Twitter):" << data;

            // Check if Twitter sent us an issue
            QJsonDocument document {QJsonDocument::fromJson(data)};
            if (document.isObject()) {
                const QJsonObject &object {document.object()};
                const QJsonArray &array {object.value(QLatin1String("errors")).toArray()};
                if (array.count() == 1) {
                    const QJsonObject &firstError {array.first().toObject()};
                    if (firstError.value(QLatin1String("code")).toInt() == 88) {
                        mappingData.repository.error(QObject::tr("Twitter rate limit exceeded. Please try again later."));
                        return;
                    }
                }
            }

            mappingData.repository.error(QObject::tr("Network error. Please try again later."));
            return;
        }

        std::vector<User> items {};
        QString errorMessage {};
        IQueryHandler<User>::Placement placement {IQueryHandler<User>::Discard};
        bool returned = mappingData.handler->treatReply(requestType, reply->readAll(), items, errorMessage, placement);
        if (!returned) {
            qCWarning(QLoggingCategory("user-central-repository")) << "Error happened during request for users" << mappingData.account.userId() << mappingData.query.type();
            qCWarning(QLoggingCategory("user-central-repository")) << "Parsing error: " << errorMessage;
            mappingData.repository.error(QObject::tr("Internal error"));
            return;
        } else {
            qDebug(QLoggingCategory("user-central-repository")) << "New data available for users" << mappingData.account.userId() << mappingData.query.type() << ". Count:" << items.size();
            switch (placement) {
            case IQueryHandler<User>::Append:
                mappingData.repository.append(items);
                break;
            case IQueryHandler<User>::Prepend:
                mappingData.repository.prepend(items);
                break;
            case IQueryHandler<User>::Discard:
                break;
            }
            mappingData.repository.finish();
        }
    });
}

UserCentralRepository::MappingData * UserCentralRepository::getMappingData(int index, const Account &account,
                                                                           const Query &query)
{
    auto it = m_mapping.find(index);
    if (it != std::end(m_mapping)) {
        return &(it->second);
    }

    switch (query.type()) {
    case Query::Friends:
    {
        std::unique_ptr<IQueryHandler<User>> handler {new FriendsQueryHandler(query.arguments())};
        return &(m_mapping.emplace(index, MappingData{account, query, std::move(handler)}).first->second);
        break;
    }
    case Query::Followers:
    {
        std::unique_ptr<IQueryHandler<User>> handler {new FollowersQueryHandler(query.arguments())};
        return &(m_mapping.emplace(index, MappingData{account, query, std::move(handler)}).first->second);
        break;
    }
    default:
        qCWarning(QLoggingCategory("user-central-repository")) << "Unhandled type" << query.type();
        return nullptr;
        break;
    }
}

UserCentralRepository::MappingData::MappingData(const Account &inputAccount, const Query &inputQuery,
                                                std::unique_ptr<IQueryHandler<User> > &&inputHandler)
    : account(std::move(inputAccount)), query(std::move(inputQuery)), handler(std::move(inputHandler))
{
}
