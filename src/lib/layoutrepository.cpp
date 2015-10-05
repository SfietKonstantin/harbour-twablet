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

#include "layoutrepository.h"
#include <QtCore/QJsonArray>

int LayoutRepository::count() const
{
    return m_data.size();
}

void LayoutRepository::load(const QJsonObject &json)
{
    const QJsonArray &layoutsArray {json.value(QLatin1String("layouts")).toArray()};

    List data;
    for (const QJsonValue &layoutValue : layoutsArray) {
        const QJsonObject &layout {layoutValue.toObject()};
        const QString &name {layout.value(QLatin1String("name")).toString()};
        const QString &userId {layout.value(QLatin1String("userId")).toString()};
        int queryType {layout.value(QLatin1String("queryType")).toInt()};

        Query::Parameters parameters {};
        const QJsonArray &queryParameters {layout.value(QLatin1String("queryArguments")).toArray()};
        for (const QJsonValue &parameterValue : queryParameters) {
            const QJsonObject &parameterObject {parameterValue.toObject()};
            QString key {parameterObject.value(QLatin1String("key")).toString()};
            QString value {parameterObject.value(QLatin1String("value")).toString()};
            parameters.emplace(key, value);
        }

        if (!userId.isEmpty() && queryType != Query::Invalid) {
            Query query {static_cast<Query::Type>(queryType), std::move(parameters)};
            data.emplace_back(name, userId, std::move(query));
        }
    }
    m_data = std::move(data);
}

void LayoutRepository::save(QJsonObject &json) const
{
    QJsonArray layouts {};
    for (const Layout &layout : m_data) {
        QJsonObject layoutObject {};
        layoutObject.insert(QLatin1String("name"), layout.name());
        layoutObject.insert(QLatin1String("userId"), layout.userId());
        layoutObject.insert(QLatin1String("queryType"), layout.query().type());
        QJsonArray parameters {};
        for (const std::pair<QString, QString> &parameter : layout.query().parameters()) {
            QJsonObject parameterObject {};
            parameterObject.insert(QLatin1String("key"), parameter.first);
            parameterObject.insert(QLatin1String("value"), parameter.second);
            parameters.append(parameterObject);
        }
        layoutObject.insert(QLatin1String("queryArguments"), parameters);
        layouts.append(layoutObject);
    }
    json.insert(QLatin1String("layouts"), layouts);
}
