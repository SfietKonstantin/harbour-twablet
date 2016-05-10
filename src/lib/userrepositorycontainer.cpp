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

#include "userrepositorycontainer.h"
#include "private/accountutil.h"
#include "private/debughelper.h"
#include "private/repositoryquerycallback.h"
#include "private/twitterqueryutil.h"
#include "repositoryqueryhandlerfactory.h"
#include <QtCore/QLoggingCategory>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtNetwork/QNetworkReply>

UserRepositoryContainer::UserRepositoryContainer(IQueryExecutor::ConstPtr &&queryExecutor)
    : m_queryExecutor(std::move(queryExecutor))
{
    Q_ASSERT_X(m_queryExecutor, "UserCentralRepository", "NULL query executor");
}

UserRepository * UserRepositoryContainer::repository(const Account &account, const Query &query)
{
    auto it = m_mapping.find(ContainerKey{Account{account}, Query{query}});
    if (it == std::end(m_mapping)) {
        return nullptr;
    }
    return &(it->second.repository);
}

void UserRepositoryContainer::referenceQuery(const Account &account, const Query &query)
{
    Data *data {getMappingData(ContainerKey{Account{account}, Query{query}})};
    if (data == nullptr) {
        return;
    }
    ++data->refcount;
}

void UserRepositoryContainer::dereferenceQuery(const Account &account, const Query &query)
{
    Data *data {getMappingData(ContainerKey{Account{account}, Query{query}})};
    if (data == nullptr) {
        return;
    }
    --data->refcount;
    if (data->refcount == 0) {
        m_mapping.erase(ContainerKey{Account{account}, Query{query}});
    }
}

void UserRepositoryContainer::refresh(const Account &account, const Query &query)
{
    auto it = m_mapping.find(ContainerKey{Account{account}, Query{query}});
    if (it != std::end(m_mapping)) {
        load(it->first, it->second, IRepositoryQueryHandler<User>::Refresh);
    }
}

void UserRepositoryContainer::loadMore(const Account &account, const Query &query)
{
    auto it = m_mapping.find(ContainerKey{Account{account}, Query{query}});
    if (it != std::end(m_mapping)) {
        load(it->first, it->second, IRepositoryQueryHandler<User>::LoadMore);
    }
}

void UserRepositoryContainer::load(const ContainerKey &key, Data &mappingData,
                                   IRepositoryQueryHandler<User>::RequestType requestType)
{
    if (mappingData.loading) {
        return;
    }

    mappingData.loading = true;

    QByteArray path {key.query().path()};
    Query::Parameters parameters (key.query().parameters());
    const Query::Parameters &additionalParameters (mappingData.handler->additionalParameters(requestType));
    parameters.insert(std::begin(additionalParameters), std::end(additionalParameters));

    mappingData.repository.start();

    m_queryExecutor->execute(key.query().requestType(), path, parameters, key.account(), [this, &mappingData, requestType](QIODevice &reply, QNetworkReply::NetworkError error, const QString &errorMessage) {
        std::vector<User> items {};
        private_util::RepositoryQueryCallback<User> callback {
            requestType,
            *mappingData.handler,
            mappingData.repository,
            items,
            mappingData.loading
        };
        callback(reply, error, errorMessage);
    });
}

UserRepositoryContainer::Data * UserRepositoryContainer::getMappingData(const ContainerKey &key)
{
    auto it = m_mapping.find(key);
    if (it != std::end(m_mapping)) {
        return &(it->second);
    }

    if (!private_util::isAccountValid(key.account())) {
        return nullptr;
    }

    IRepositoryQueryHandler<User>::Ptr handler {RepositoryQueryHandlerFactory::createUser(key.query())};
    if (!handler) {
        return nullptr;
    }
    return &(m_mapping.emplace(key, Data{std::move(handler)}).first->second);
}


UserRepositoryContainer::Data::Data(IRepositoryQueryHandler<User>::Ptr &&inputHandler)
    : handler{std::move(inputHandler)}
{
}
