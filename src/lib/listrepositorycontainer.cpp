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

#include "listrepositorycontainer.h"
#include "private/accountutil.h"
#include "private/repositoryquerycallback.h"
#include "repositoryqueryhandlerfactory.h"

ListRepositoryContainer::ListRepositoryContainer(IQueryExecutor::ConstPtr queryExecutor)
    : m_queryExecutor{std::move(queryExecutor)}
{
    Q_ASSERT_X(m_queryExecutor, "ListQueryContainer", "NULL query executor");
}

ListRepository * ListRepositoryContainer::repository(const Account &account, const Query &query)
{
    auto it = m_mapping.find(ContainerKey{Account{account}, Query{query}});
    if (it == std::end(m_mapping)) {
        return nullptr;
    }
    return &(it->second.repository);
}

void ListRepositoryContainer::referenceQuery(const Account &account, const Query &query)
{
    Data *data {getMappingData(ContainerKey{Account{account}, Query{query}})};
    if (data == nullptr) {
        return;
    }
    ++data->refcount;
}

void ListRepositoryContainer::dereferenceQuery(const Account &account, const Query &query)
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

void ListRepositoryContainer::refresh(const Account &account, const Query &query)
{
    auto it = m_mapping.find(ContainerKey{Account{account}, Query{query}});
    if (it != std::end(m_mapping)) {
        load(it->first, it->second, IRepositoryQueryHandler<List>::Refresh);
    }
}

void ListRepositoryContainer::loadMore(const Account &account, const Query &query)
{
    auto it = m_mapping.find(ContainerKey{Account{account}, Query{query}});
    if (it != std::end(m_mapping)) {
        load(it->first, it->second, IRepositoryQueryHandler<List>::LoadMore);
    }
}

void ListRepositoryContainer::load(const ContainerKey &key, ListRepositoryContainer::Data &mappingData,
                                   IRepositoryQueryHandler<List>::RequestType requestType)
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
        std::vector<List> items {};
        private_util::RepositoryQueryCallback<List> callback {
            requestType,
            *mappingData.handler,
            mappingData.repository,
            items,
            mappingData.loading
        };
        callback(reply, error, errorMessage);
    });
}

ListRepositoryContainer::Data * ListRepositoryContainer::getMappingData(const ContainerKey &key)
{
    auto it = m_mapping.find(key);
    if (it != std::end(m_mapping)) {
        return &(it->second);
    }

    if (!private_util::isAccountValid(key.account())) {
        return nullptr;
    }

    IRepositoryQueryHandler<List>::Ptr handler {RepositoryQueryHandlerFactory::createList(key.query())};
    if (!handler) {
        return nullptr;
    }
    return &(m_mapping.emplace(key, Data{std::move(handler)}).first->second);
}

ListRepositoryContainer::Data::Data(IRepositoryQueryHandler<List>::Ptr &&inputHandler)
    : handler{std::move(inputHandler)}
{
}
