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
#include <QtNetwork/QNetworkAccessManager>

namespace private_util
{

#ifndef USE_MOCK_SERVER
static const char *TWITTER_API_URL = "https://api.twitter.com/1.1/";
#else
static const char *TWITTER_API_URL = "http://localhost:8000/";
#endif

QNetworkReply * TwitterQueryUtil::get(QNetworkAccessManager &network, const QByteArray &path,
                                      const std::map<QByteArray, QByteArray> &parameters,
                                      const Account &account)
{
    QNetworkRequest request {createGetRequest(path, parameters, account)};
    return network.get(request);
}

QNetworkReply * TwitterQueryUtil::post(QNetworkAccessManager &network,
                                       const QByteArray &path,
                                       const std::map<QByteArray, QByteArray> &parameters,
                                       const std::map<QByteArray, QByteArray> &postData,
                                       const Account &account)
{
    QNetworkRequest request {createPostRequest(path, parameters, postData, account)};
    QUrlQuery postDataQuery {};
    for (const std::pair<QByteArray, QByteArray> &parameter : postData) {
        postDataQuery.addQueryItem(QLatin1String(parameter.first), QLatin1String(parameter.second));
    }
    return network.post(request, postDataQuery.toString(QUrl::FullyEncoded).toLatin1());
}

QNetworkRequest TwitterQueryUtil::createRequest(const QByteArray &type, const QByteArray &path,
                                                const std::map<QByteArray, QByteArray> &parameters,
                                                const std::map<QByteArray, QByteArray> &postData,
                                                const Account &account)
{
    QByteArray url {QByteArray(TWITTER_API_URL) + path};
    std::map<QByteArray, QByteArray> fullParameters (std::begin(parameters), std::end(parameters));
    fullParameters.insert(std::begin(postData), std::end(postData));
    std::vector<std::pair<QByteArray, QByteArray>> parametersVector (std::begin(fullParameters), std::end(fullParameters));

    QByteArray header {TwitterDataUtil::authorizationHeader(TWITTER_CONSUMER_KEY, TWITTER_CONSUMER_SECRET, type, url,
                                                            parametersVector, account.token(), account.tokenSecret())};
    QUrl urlObject {QUrl::fromEncoded(url)};
    QUrlQuery query {};
    for (const std::pair<QByteArray, QByteArray> &parameter : parameters) {
        query.addQueryItem(QLatin1String(parameter.first), QLatin1String(parameter.second));
    }
    urlObject.setQuery(query);

    QNetworkRequest request {urlObject};
    request.setRawHeader("Authorization", header);
    return request;
}

QNetworkRequest TwitterQueryUtil::createGetRequest(const QByteArray &path,
                                                   const std::map<QByteArray, QByteArray> &parameters,
                                                   const Account &account)
{
    return createRequest("GET", path, parameters, {}, account);
}

QNetworkRequest TwitterQueryUtil::createPostRequest(const QByteArray &path,
                                                    const std::map<QByteArray, QByteArray> &parameters,
                                                    const std::map<QByteArray, QByteArray> &postData,
                                                    const Account &account)
{
    QNetworkRequest request {createRequest("POST", path, parameters, postData, account)};
    request.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/x-www-form-urlencoded"));
    return request;
}

}
