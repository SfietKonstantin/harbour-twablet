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

#ifndef USERCENTRALREPOSITORY_H
#define USERCENTRALREPOSITORY_H

#include <map>
#include <QtNetwork/QNetworkAccessManager>
#include "globals.h"
#include "qobjectutils.h"
#include "account.h"
#include "query.h"
#include "userrepository.h"
#include "ilistqueryhandler.h"
#include "iqueryexecutor.h"

class UserCentralRepository
{
public:
    explicit UserCentralRepository(IQueryExecutor::Ptr queryExecutor);
    DISABLE_COPY_DEFAULT_MOVE(UserCentralRepository);
    bool isValid(int index) const;
    UserRepository * repository(int index);
    void refresh(int index);
    void loadMore(int index);
    int addRepository(const Account &account, const Query &query);
    void removeRepository(int index);
private:
    struct MappingData
    {
        explicit MappingData(const Account &inputAccount, const Query &inputQuery,
                             IListQueryHandler<User>::Ptr &&inputHandler);
        bool loading {false};
        Account account {};
        Query query {};
        UserRepository repository {};
        std::unique_ptr<IListQueryHandler<User>> handler {};
    };
    void load(MappingData &mappingData, IListQueryHandler<User>::RequestType requestType);
    MappingData * getMappingData(int index, const Account &account, const Query &query);
    IQueryExecutor::Ptr m_queryExecutor {nullptr};
    std::map<int, MappingData> m_mapping {};
    int m_index {0};
};

#endif // USERCENTRALREPOSITORY_H
