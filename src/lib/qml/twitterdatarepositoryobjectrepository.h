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

#ifndef TWITTERDATAREPOSITORYOBJECTREPOSITORY_H
#define TWITTERDATAREPOSITORYOBJECTREPOSITORY_H

#include <QtCore/QtGlobal>

#include "twittertweetrepository.h"

class TwitterAccount;
class Layout;
class TwitterTweet;
class TwitterAccountRepository;
class LayoutRepository;
class TwitterDataRepositoryObject;
template<class T> class TwitterDataRepositoryObjectRepository;
template<> class TwitterDataRepositoryObjectRepository<TwitterAccount>
{
public:
    static bool isValid(TwitterDataRepositoryObject *repository, int layoutIndex)
    {
        Q_UNUSED(layoutIndex)
        return repository != nullptr;
    }
    static TwitterAccountRepository & get(TwitterDataRepositoryObject &repository, int layoutIndex)
    {
        Q_UNUSED(layoutIndex)
        return repository.accounts();
    }
};
template<> class TwitterDataRepositoryObjectRepository<Layout>
{
public:
    static bool isValid(TwitterDataRepositoryObject *repository, int layoutIndex)
    {
        Q_UNUSED(layoutIndex)
        return repository != nullptr;
    }
    static LayoutRepository & get(TwitterDataRepositoryObject &repository, int layoutIndex)
    {
        Q_UNUSED(layoutIndex)
        return repository.layouts();
    }
};

template<> class TwitterDataRepositoryObjectRepository<TwitterTweet>
{
public:
    static bool isValid(TwitterDataRepositoryObject *repository, int layoutIndex)
    {
        Q_UNUSED(layoutIndex)
        if (repository == nullptr) {
            return false;
        }
        return (layoutIndex >= 0 && layoutIndex < std::end(repository->layouts()) - std::begin(repository->layouts()));
    }
    static TwitterTweetRepository & get(TwitterDataRepositoryObject &repository, int layoutIndex)
    {
        const Layout &layout {*(std::begin(repository.layouts()) + layoutIndex)};
        return repository.tweets(layout);
    }
};

#endif // TWITTERDATAREPOSITORYOBJECTREPOSITORY_H

