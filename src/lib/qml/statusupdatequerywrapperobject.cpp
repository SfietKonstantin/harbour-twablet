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

#include "statusupdatequerywrapperobject.h"

namespace qml
{

StatusUpdateQueryWrapperObject::StatusUpdateQueryWrapperObject(QObject *parent)
    : TweetQueryWrapperObject{parent}
{
    setType(QueryTypeObject::StatusUpdate);
}

QString StatusUpdateQueryWrapperObject::status() const
{
    return m_status;
}

void StatusUpdateQueryWrapperObject::setStatus(QString status)
{
    if (m_status != status) {
        m_status = status;
        updateParameters();
        emit statusChanged();
    }
}

QString StatusUpdateQueryWrapperObject::inReplyTo() const
{
    return m_inReplyTo;
}

void StatusUpdateQueryWrapperObject::setInReplyTo(QString inReplyTo)
{
    if (m_inReplyTo != inReplyTo) {
        m_inReplyTo = inReplyTo;
        updateParameters();
        emit inReplyToChanged();
    }
}

void StatusUpdateQueryWrapperObject::updateParameters()
{
    QVariantMap parameters = QVariantMap {
        {QLatin1String{"status"}, m_status},
        {QLatin1String{"in_reply_to_status_id"}, m_inReplyTo}
    };
    setParameters(parameters);
}

}

