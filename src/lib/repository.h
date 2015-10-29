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

#include <deque>
#include <set>
#include <QtCore/QString>
#include "globals.h"
#include "irepositorylistener.h"

/**
 * @brief A generic container
 */
template<class T>
class Repository
{
public:
    using List = std::deque<T>;
    explicit Repository() {}
    DISABLE_COPY_DEFAULT_MOVE(Repository);
    ~Repository()
    {
        for (IRepositoryListener<T> *listener : m_listeners) {
            listener->doInvalidate();
            removeListener(*listener);
        }
    }
    typename List::const_iterator begin() const
    {
        return std::begin(m_data);
    }
    typename List::const_iterator end() const
    {
        return std::end(m_data);
    }
    bool empty() const
    {
        return m_data.empty();
    }
    int size() const
    {
        return m_data.size();
    }
    T & append(T &&data)
    {
        m_data.emplace_back(data);
        for (IRepositoryListener<T> *listener : m_listeners) {
            listener->doAppend(*(std::end(m_data) - 1));
        }
        return *(std::end(m_data) - 1);
    }
    void append(const std::vector<T> &data)
    {
        for (const T &entry : data) {
            m_data.emplace_back(entry);
        }
        for (IRepositoryListener<T> *listener : m_listeners) {
            listener->doAppend(data);
        }
    }
    void prepend(const std::vector<T> &data)
    {
        for (auto it = data.rbegin(); it != data.rend(); ++it) {
            m_data.emplace_front(*it);
        }
        for (IRepositoryListener<T> *listener : m_listeners) {
            listener->doPrepend(data);
        }
    }
    void update(int index, T &&data)
    {
        if (index < 0 || static_cast<std::size_t>(index) >= m_data.size()) {
            return;
        }
        m_data[index] = std::move(data);
        for (IRepositoryListener<T> *listener : m_listeners) {
            listener->doUpdate(index, *(std::begin(m_data) + index));
        }
    }
    void remove(int index)
    {
        if (index < 0 || static_cast<std::size_t>(index) >= m_data.size()) {
            return;
        }
        m_data.erase(std::begin(m_data) + index);
        for (IRepositoryListener<T> *listener : m_listeners) {
            listener->doRemove(index);
        }
    }
    void move(int from, int to)
    {
        if (from < 0 || static_cast<std::size_t>(from) >= m_data.size()
            || to < 0 || static_cast<std::size_t>(to) > m_data.size()) {
            return;
        }

        if (to == from || to == from + 1) {
            return;
        }

        int toIndex = (to < from) ? to : to - 1;

        T data {*(std::begin(m_data) + from)};
        m_data.erase(std::begin(m_data) + from);
        m_data.insert(std::begin(m_data) + toIndex, data);
        for (IRepositoryListener<T> *listener : m_listeners) {
            listener->doMove(from, to);
        }
    }
    void start()
    {
        m_status = Loading;
        for (IRepositoryListener<T> *listener : m_listeners) {
            listener->doStart();
        }
    }
    void error(const QString &error)
    {
        m_status = Error;
        m_lastError = error;
        for (IRepositoryListener<T> *listener : m_listeners) {
            listener->doError(error);
        }
    }
    void finish()
    {
        m_status = Idle;
        for (IRepositoryListener<T> *listener : m_listeners) {
            listener->doFinish();
        }
    }
    void addListener(IRepositoryListener<T> &listener)
    {
        m_listeners.insert(&listener);
        switch (m_status) {
        case Idle:
            break;
        case Loading:
            listener.doStart();
            break;
        case Error:
            listener.doError(m_lastError);
            break;
        }
    }
    void removeListener(IRepositoryListener<T> &listener)
    {
        m_listeners.erase(&listener);
    }
    const std::set<IRepositoryListener<T> *> listeners() const
    {
        return m_listeners;
    }
protected:
    std::deque<T> m_data {};
private:
    enum Status {
        Idle,
        Loading,
        Error
    };
    std::set<IRepositoryListener<T> *> m_listeners {};
    Status m_status {Idle};
    QString m_lastError {};
};

#endif // REPOSITORY_H
