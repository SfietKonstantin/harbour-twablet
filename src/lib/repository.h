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

#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <vector>
#include <set>
#include <QtCore/QString>
#include "globals.h"
#include "ilistener.h"

template<class T>
class Repository
{
public:
    using List = std::vector<T>;
    explicit Repository() {}
    DISABLE_COPY_DEFAULT_MOVE(Repository);
    typename List::const_iterator begin() const
    {
        return std::begin(m_data);
    }
    typename List::const_iterator end() const
    {
        return std::end(m_data);
    }
    void add(T &&data)
    {
        m_data.emplace_back(data);
        for (IListener<T> *listener : m_listeners) {
            listener->doAdd(*(std::end(m_data) - 1));
        }
    }
    void add(const std::vector<T> &data)
    {
        for (const T &entry : data) {
            m_data.emplace_back(entry);
        }
        for (IListener<T> *listener : m_listeners) {
            listener->doAdd(data);
        }
    }
    void update(int index, T &&data)
    {
        if (index < 0 || static_cast<typename List::size_type>(index) >= m_data.size()) {
            return;
        }
        m_data[index] = std::move(data);
        for (IListener<T> *listener : m_listeners) {
            listener->doUpdate(index, *(m_data.begin() + index));
        }
    }
    void remove(int index)
    {
        if (index < 0 || static_cast<typename List::size_type>(index) >= m_data.size()) {
            return;
        }
        m_data.erase(std::begin(m_data) + index);
        for (IListener<T> *listener : m_listeners) {
            listener->doRemove(index);
        }
    }
    void start()
    {
        for (IListener<T> *listener : m_listeners) {
            listener->doStart();
        }
    }
    void error(const QString &error)
    {
        for (IListener<T> *listener : m_listeners) {
            listener->doError(error);
        }
    }
    void finish()
    {
        for (IListener<T> *listener : m_listeners) {
            listener->doFinish();
        }
    }
    void addListener(IListener<T> &listener)
    {
        m_listeners.insert(&listener);
    }
    void removeListener(IListener<T> &listener)
    {
        m_listeners.erase(&listener);
    }
    std::vector<T> m_data {};
private:
    std::set<IListener<T> *> m_listeners {};
    QString m_errorString {};
};

#endif // REPOSITORY_H
