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

#ifndef IREPOSITORYLISTENER_H
#define IREPOSITORYLISTENER_H

#include <vector>

class QString;
/**
 * @brief An interface for a Repository listener
 *
 * This interface is used to implement models-like classes, that listens to a
 * Repository and want to be notified when the Repository changes.
 *
 * This listener can be used to get notifications when some items are inserted,
 * removed, or updated. This is done via doAppend(), doPrepend(), doUpdate() and
 * doRemove().
 *
 * This interface also handle the status of the asynchronous loading operation
 * that takes places in the Repository. This is done via doStart(), doError() and
 * doFinished().
 */
template<class T>
class IRepositoryListener
{
public:
    virtual ~IRepositoryListener() {}
private:
    /**
     * @brief Notify that a new item is appended
     * @param item item to be appended.
     */
    virtual void doAppend(const T &item) = 0;
    /**
     * @brief Notify that new items are appended
     * @param items items to be appended.
     */
    virtual void doAppend(const std::vector<T> &items) = 0;
    /**
     * @brief Notify that new items are prepended
     * @param items items to be prepended.
     */
    virtual void doPrepend(const std::vector<T> &items) = 0;
    /**
     * @brief Notify that an item is updated
     * @param index index of the item that is updated.
     * @param item the new value of the item.
     */
    virtual void doUpdate(int index, const T &item) = 0;
    /**
     * @brief Notify that an item is removed
     * @param index index of the item that is removed.
     */
    virtual void doRemove(int index) = 0;
    /**
     * @brief Notify that an item has moved
     * @param from index of the item to move.
     * @param to the item's new index.
     */
    virtual void doMove(int from, int to) = 0;
    /**
     * @brief Notify that the listened object is now invalid
     *
     * When called on this method, the listener should stop
     * listening.
     */
    virtual void doInvalidate() = 0;
    /**
     * @brief Notify that an asynchronous operation has started
     */
    virtual void doStart() = 0;
    /**
     * @brief Notify that an asynchronous operation has failed
     * @param error error message.
     */
    virtual void doError(const QString &error) = 0;
    /**
     * @brief Notify that an asynchronous operation has finished
     */
    virtual void doFinish() = 0;
    template<class C> friend class Repository;
};

#endif // IREPOSITORYLISTENER_H

