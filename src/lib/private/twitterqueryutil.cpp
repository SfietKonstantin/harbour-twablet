/*
 * Copyright (C) 2015 Lucien XU <sfietkonstantin@free.fr>
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

#include "twitterqueryutil.h"
#include "twitterdatautil.h"
#include "twitter-secrets.h"
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>

QNetworkRequest TwitterQueryUtil::createGetRequest(const QString &path,
                                                   const std::map<QString, QString> &parameters,
                                                   const TwitterUser &user)
{
    QString url {QLatin1String("https://api.twitter.com/1.1/") + path};
    std::vector<std::pair<QString, QString>> parametersVector (std::begin(parameters),
                                                               std::end(parameters));

    QByteArray header {TwitterDataUtil::authorizationHeader(TWITTER_CONSUMER_KEY, TWITTER_CONSUMER_SECRET, "GET", url.toLocal8Bit(),
                                                            parametersVector, user.token(), user.tokenSecret())};
    QUrl urlObject {url};
    QUrlQuery query {};
    QList<QPair<QString, QString>> parametersQt {};
    for (const std::pair<QString, QString> &parameter : parameters) {
        parametersQt.append(qMakePair(parameter.first, parameter.second));
    }
    query.setQueryItems(parametersQt);
    urlObject.setQuery(query);

    QNetworkRequest request {urlObject};
    request.setRawHeader("Authorization", header);
    return request;
}
