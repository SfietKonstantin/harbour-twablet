/*
 * Copyright (C) 2013 Jolla Ltd. <chris.adams@jollamobile.com>
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
 *   * Neither the name of Nemo Mobile nor the names of its contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
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

#include "twitterdatautil.h"

#include <QtCore/QVariantMap>
#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QByteArray>
#include <QtCore/QUuid>
#include <QtCore/qmath.h>
#include <QtCore/QDateTime>
#include <QtCore/QUrl>
#include <QtCore/QStringList>
#include <QtCore/QMessageAuthenticationCode>
#include <QtCore/QDebug>

static const char *OAUTH_SIGNATURE_METHOD = "HMAC-SHA1";
static const char *OAUTH_VERSION = "1.0";

QByteArray TwitterDataUtil::authorizationHeader(const QByteArray &oauthConsumerKey,
                                                const QByteArray &oauthConsumerSecret,
                                                const QByteArray &requestMethod,
                                                const QByteArray &requestUrl,
                                                const std::vector<std::pair<QString, QString>> &parameters,
                                                const QByteArray &oauthToken,
                                                const QByteArray &oauthTokenSecret,
                                                const QByteArray &oauthNonce,
                                                const QByteArray &oauthTimestamp)
{
    // Twitter requires all requests to be signed with an authorization header.
    QByteArray nonce {oauthNonce};
    if (nonce.isEmpty()) {
        nonce = QUuid::createUuid().toByteArray().toBase64();
    }
    QByteArray timestamp {oauthTimestamp};
    if (timestamp.isEmpty()) {
        timestamp = QByteArray::number(qFloor(QDateTime::currentMSecsSinceEpoch() / 1000.0));
    }

    // now build up the encoded parameters map.  We use a map to perform alphabetical sorting.
    QMap<QByteArray, QByteArray> encodedParams {};
    encodedParams.insert(QUrl::toPercentEncoding(QLatin1String("oauth_consumer_key")), QUrl::toPercentEncoding(QLatin1String(oauthConsumerKey)));
    encodedParams.insert(QUrl::toPercentEncoding(QLatin1String("oauth_nonce")), QUrl::toPercentEncoding(QLatin1String(nonce)));
    encodedParams.insert(QUrl::toPercentEncoding(QLatin1String("oauth_signature_method")), QUrl::toPercentEncoding(QLatin1String(OAUTH_SIGNATURE_METHOD)));
    encodedParams.insert(QUrl::toPercentEncoding(QLatin1String("oauth_timestamp")), QUrl::toPercentEncoding(QLatin1String(timestamp)));
    encodedParams.insert(QUrl::toPercentEncoding(QLatin1String("oauth_version")), QUrl::toPercentEncoding(QLatin1String(OAUTH_VERSION)));
    if (!oauthToken.isEmpty()) {
        encodedParams.insert(QUrl::toPercentEncoding(QLatin1String("oauth_token")), QUrl::toPercentEncoding(QLatin1String(oauthToken)));
    }
    for (const std::pair<QString, QString> &parameter : parameters) {
        encodedParams.insert(QUrl::toPercentEncoding(parameter.first), QUrl::toPercentEncoding(parameter.second));
    }

    QByteArray parametersByteArray {};
    QList<QByteArray> keys = encodedParams.keys();
    for (const QByteArray &key : keys) {
        parametersByteArray += key + QByteArray("=") + encodedParams.value(key) + QByteArray("&");
    }
    parametersByteArray.chop(1);

    QByteArray signatureBaseString {requestMethod.toUpper() + QByteArray("&") + QUrl::toPercentEncoding(QLatin1String(requestUrl)) + QByteArray("&") + QUrl::toPercentEncoding(QLatin1String(parametersByteArray))};
    QByteArray signingKey {QUrl::toPercentEncoding(QLatin1String(oauthConsumerSecret)) + QByteArray("&") + QUrl::toPercentEncoding(QLatin1String(oauthTokenSecret))};

    QByteArray oauthSignature {QMessageAuthenticationCode::hash(signatureBaseString, signingKey, QCryptographicHash::Sha1).toBase64()};
    encodedParams.insert(QUrl::toPercentEncoding(QLatin1String("oauth_signature")), QUrl::toPercentEncoding(QLatin1String(oauthSignature)));

    // now generate the Authorization header from the encoded parameters map.
    // we need to remove the query items from the encoded parameters map first.
    QByteArray authHeader = QByteArray("OAuth ");

    for (const std::pair<QString, QString> &parameter : parameters) {
        encodedParams.remove(QUrl::toPercentEncoding(parameter.first));
    }
    keys = encodedParams.keys();
    foreach (const QByteArray &key, keys) {
        authHeader += key + "=\"" + encodedParams.value(key) + "\", ";
    } 
    authHeader.chop(2);
    return authHeader;
}

