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
#include "friendsqueryhandler.h"
#include "followersqueryhandler.h"
#include "private/twitterqueryutil.h"
#include "private/repositoryprocesscallback.h"
#include <QtCore/QLoggingCategory>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtNetwork/QNetworkReply>

UserCentralRepository::UserCentralRepository(IQueryExecutor::Ptr queryExecutor)
    : m_queryExecutor(std::move(queryExecutor))
{
    Q_ASSERT_X(m_queryExecutor, "UserCentralRepository", "NULL query executor");
}

bool UserCentralRepository::isValid(int index) const
{
    return m_mapping.find(index) != std::end(m_mapping);
}

UserRepository * UserCentralRepository::repository(int index)
{
    auto it = m_mapping.find(index);
    if (it == std::end(m_mapping)) {
        return nullptr;
    }
    return &(it->second.repository);
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
    if (mappingData.loading) {
        return;
    }

    mappingData.loading = true;

    QString path {};
    std::map<QByteArray, QByteArray> parameters {};
    mappingData.handler->createRequest(requestType, path, parameters);
    qCDebug(QLoggingCategory("user-central-repository")) << "Request:" << path << parameters;
    mappingData.repository.start();

    m_queryExecutor->execute(path, parameters, mappingData.account, [this, &mappingData, requestType](QIODevice &reply, QNetworkReply::NetworkError error, const QString &errorMessage) {
        std::vector<User> items {};
        private_util::RepositoryProcessCallback<User> callback {
            requestType,
            *mappingData.handler,
            mappingData.repository,
            items,
            mappingData.loading
        };
        callback(reply, error, errorMessage);
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
        std::unique_ptr<IQueryHandler<User>> handler {new FriendsQueryHandler(query.parameters())};
        return &(m_mapping.emplace(index, MappingData{account, query, std::move(handler)}).first->second);
        break;
    }
    case Query::Followers:
    {
        std::unique_ptr<IQueryHandler<User>> handler {new FollowersQueryHandler(query.parameters())};
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
