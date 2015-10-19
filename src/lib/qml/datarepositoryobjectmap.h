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

#ifndef DATAREPOSITORYOBJECTMAP_H
#define DATAREPOSITORYOBJECTMAP_H

#include <QtCore/QtGlobal>

#include "tweetrepository.h"
#include "userrepository.h"
#include "datarepositoryobject.h"
#include "itweetrepositorycontainerobject.h"
#include "ilayoutcontainerobject.h"

class Account;
class Layout;
class Tweet;
class AccountRepository;
class LayoutRepository;

namespace qml
{

class DataRepositoryObject;
template<class T> class DataRepositoryObjectMap;
template<> class DataRepositoryObjectMap<Account>
{
public:
    static AccountRepository * get(DataRepositoryObject &repository, int layoutIndex, bool temporary)
    {
        Q_UNUSED(layoutIndex)
        Q_UNUSED(temporary)
        return &(repository.accounts());
    }
};
template<> class DataRepositoryObjectMap<Layout>
{
public:
    static LayoutRepository * get(QObject &object, int layoutIndex, bool temporary)
    {
        Q_UNUSED(layoutIndex)
        Q_UNUSED(temporary)
        ILayoutContainerObject *layoutContainer = qobject_cast<ILayoutContainerObject *>(&object);
        return layoutContainer != nullptr ? &(layoutContainer->layouts()) : nullptr;
    }
};

template<class T> class DataRepositoryObjectMap;
template<> class DataRepositoryObjectMap<Tweet>
{
public:
    static TweetRepository * get(QObject &object, int layoutIndex, bool temporary)
    {
        ILayoutContainerObject *layoutContainer = qobject_cast<ILayoutContainerObject *>(&object);
        ITweetRepositoryContainerObject *tweetContainer = qobject_cast<ITweetRepositoryContainerObject *>(&object);
        if (layoutContainer == nullptr || tweetContainer == nullptr) {
            return nullptr;
        }

        const Layout *layout {nullptr};
        if (temporary) {
            layout = tweetContainer->temporaryLayout(layoutIndex);
        } else {
            const LayoutRepository &layoutRepository (layoutContainer->layouts());
            if (layoutIndex >= 0 && layoutIndex < layoutRepository.size()) {
                layout = &(*(std::begin(layoutRepository) + layoutIndex));
            }
        }

        return layout != nullptr ? tweetContainer->tweets(*layout) : nullptr;
    }
};

template<> class DataRepositoryObjectMap<User>
{
public:
    static UserRepository * get(DataRepositoryObject &repository, int layoutIndex, bool temporary)
    {
        Q_UNUSED(temporary)
        return repository.user(layoutIndex);
    }
};

}

#endif // DATAREPOSITORYOBJECTMAP_H

