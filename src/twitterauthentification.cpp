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

#include "twitterauthentification.h"
#include "twitter-secrets.h"
#include "private/twitterdatautil.h"
#include "private/qnetworkreplyutils.h"
#include <QLoggingCategory>

static const char *TWITTER_API_REQUEST_TOKEN = "https://api.twitter.com/oauth/request_token";
static const char *TWITTER_API_REQUEST_TOKEN_PARAM_KEY = "oauth_callback";
static const char *TWITTER_API_REQUEST_TOKEN_PARAM_VALUE = "oob";
static const char *TWITTER_API_AUTHORIZE = "https://api.twitter.com/oauth/authorize";
static const char *TWITTER_API_ACCESS_TOKEN = "https://api.twitter.com/oauth/access_token";
static const char *TWITTER_API_ACCESS_TOKEN_PARAM_KEY = "oauth_verifier";
static const char *TWITTER_API_OAUTH_TOKEN_KEY = "oauth_token";
static const char *TWITTER_API_OAUTH_TOKEN_SECRET_KEY = "oauth_token_secret";
static const char *TWITTER_API_OAUTH_CALLBACK_CONFIRMED_KEY = "oauth_callback_confirmed";
static const char *TWITTER_API_OAUTH_CALLBACK_CONFIRMED_VALUE = "true";
static const char *TWITTER_API_OAUTH_USER_ID_KEY = "user_id";
static const char *TWITTER_API_OAUTH_SCREEN_NAME_KEY = "screen_name";

TwitterAuthentification::TwitterAuthentification(QObject *parent)
    : QObject(parent), m_networkAccessManager{new QNetworkAccessManager(this)}
{
}

QString TwitterAuthentification::pin() const
{
    return m_pin;
}

void TwitterAuthentification::setPin(const QString &pin)
{
    if (m_pin != pin) {
        m_pin = pin;
        emit pinChanged();
    }
}

QString TwitterAuthentification::token() const
{
    return m_token;
}

QString TwitterAuthentification::tokenSecret() const
{
    return m_tokenSecret;
}

QString TwitterAuthentification::userId() const
{
    return m_userId;
}

QString TwitterAuthentification::screenName() const
{
    return m_screenName;
}

void TwitterAuthentification::startRequest()
{
    std::vector<std::pair<QString, QString>> args {{QLatin1String(TWITTER_API_REQUEST_TOKEN_PARAM_KEY), QLatin1String(TWITTER_API_REQUEST_TOKEN_PARAM_VALUE)}};
    QByteArray header {TwitterDataUtil::authorizationHeader(TWITTER_CONSUMER_KEY, TWITTER_CONSUMER_SECRET, "POST", TWITTER_API_REQUEST_TOKEN, args)};
    qCDebug(QLoggingCategory("twitter-auth")) << "The authentification header for the start request is:" << header;

    QByteArray url {QByteArray(TWITTER_API_REQUEST_TOKEN) + "?" + TWITTER_API_REQUEST_TOKEN_PARAM_KEY + "=" + TWITTER_API_REQUEST_TOKEN_PARAM_VALUE};
    QNetworkRequest request {QUrl(QLatin1String(url))};
    request.setRawHeader("Authorization", header);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QByteArray("application/x-www-form-urlencoded"));

    QNetworkReply *reply {m_networkAccessManager->post(request, QByteArray())};
    connect(reply, &QNetworkReply::finished, [reply, this]() {
        QNetworkReplyPtr replyPtr {reply};
        if (replyPtr->error() != QNetworkReply::NoError) {
            qCDebug(QLoggingCategory("twitter-auth")) << "Error happened. Code:" << reply->error();
            qCDebug(QLoggingCategory("twitter-auth")) << "Error message (Qt):" << reply->errorString();
            qCDebug(QLoggingCategory("twitter-auth")) << "Error message (Twitter):" << reply->readAll();
            emit error(tr("An error happened when connecting to Twitter. Please check your internet connection."));
            return;
        }

        QList<QByteArray> data {replyPtr->readAll().split('&')};
        QMap<QByteArray, QByteArray> dataMap;
        for (const QByteArray &dataEntry : data) {
            QList<QByteArray> pair = dataEntry.split('=');
            dataMap.insert(pair.first(), pair.last());
        }

        if (dataMap.value(TWITTER_API_OAUTH_CALLBACK_CONFIRMED_KEY) != QByteArray(TWITTER_API_OAUTH_CALLBACK_CONFIRMED_VALUE)) {
            qCWarning(QLoggingCategory("twitter-auth")) << "The callback is not confirmed";
            return;
        }

        m_tempToken = std::move(dataMap.value(TWITTER_API_OAUTH_TOKEN_KEY));
        m_tempTokenSecret = std::move(dataMap.value(TWITTER_API_OAUTH_TOKEN_SECRET_KEY));

        // Now we need to continue the PIN based authentification
        QByteArray url {QByteArray(TWITTER_API_AUTHORIZE) + "?" + TWITTER_API_OAUTH_TOKEN_KEY + "=" + m_tempToken};
        qCDebug(QLoggingCategory("twitter-auth")) << "Sending url" << url;
        emit sendAuthorize(QUrl(QLatin1String(url)));
    });
}

void TwitterAuthentification::continueRequest()
{
    if (m_tempToken.isEmpty() || m_pin.isEmpty()) {
        return;
    }

    qCDebug(QLoggingCategory("twitter-auth")) << "Continuing request with pin:" << m_pin;

    std::vector<std::pair<QString, QString>> args {{QLatin1String(TWITTER_API_ACCESS_TOKEN_PARAM_KEY), m_pin}};
    QByteArray header = TwitterDataUtil::authorizationHeader(TWITTER_CONSUMER_KEY, TWITTER_CONSUMER_SECRET, "POST", TWITTER_API_ACCESS_TOKEN, args, m_tempToken, m_tempTokenSecret);
    qCDebug(QLoggingCategory("twitter-auth")) << "The authentification header for the continue request is:" << header;

    QNetworkRequest request (QUrl(QLatin1String(TWITTER_API_ACCESS_TOKEN) + QLatin1String("?") + QLatin1String(TWITTER_API_ACCESS_TOKEN_PARAM_KEY) + QLatin1String("=") + m_pin));
    request.setRawHeader("Authorization", header);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QByteArray("application/x-www-form-urlencoded"));

    QNetworkReply *reply {m_networkAccessManager->post(request, QByteArray())};
    connect(reply, &QNetworkReply::finished, [reply, this]() {
        QNetworkReplyPtr replyPtr {reply};
        if (replyPtr->error() != QNetworkReply::NoError) {
            qCWarning(QLoggingCategory("twitter-auth")) << "Error happened. Code:" << replyPtr->error();
            qCWarning(QLoggingCategory("twitter-auth")) << "Error message (Qt):" << replyPtr->errorString();
            qCWarning(QLoggingCategory("twitter-auth")) << "Error message (Twitter):" << replyPtr->readAll();
            emit error(tr("An error happened when authentification to Twitter please retry."));
            return;
        }

        QList<QByteArray> data = replyPtr->readAll().split('&');
        QMap<QByteArray, QByteArray> dataMap;
        for (const QByteArray &dataEntry : data) {
            QList<QByteArray> pair {dataEntry.split('=')};
            dataMap.insert(pair.first(), pair.last());
        }

        setData(QLatin1String(dataMap.value(TWITTER_API_OAUTH_TOKEN_KEY)),
                QLatin1String(dataMap.value(TWITTER_API_OAUTH_TOKEN_SECRET_KEY)),
                QLatin1String(dataMap.value(TWITTER_API_OAUTH_USER_ID_KEY)),
                QLatin1String(dataMap.value(TWITTER_API_OAUTH_SCREEN_NAME_KEY)));

        qCDebug(QLoggingCategory("twitter-auth")) << "Login successful";
        qCDebug(QLoggingCategory("twitter-auth")) << "Token:" << m_token;
        qCDebug(QLoggingCategory("twitter-auth")) << "Token secret:" << m_tokenSecret;
        qCDebug(QLoggingCategory("twitter-auth")) << "User id:" << m_userId;
        qCDebug(QLoggingCategory("twitter-auth")) << "Screen name:" << m_screenName;
        emit done();
    });
}

void TwitterAuthentification::setData(QString &&token, QString &&tokenSecret,
                                      QString &&userId, QString &&screenName)
{
    if (m_token != token) {
        m_token = std::move(token);
        emit tokenChanged();
    }

    if (m_tokenSecret != tokenSecret) {
        m_tokenSecret = std::move(tokenSecret);
        emit tokenSecretChanged();
    }

    if (m_userId != userId) {
        m_userId = std::move(userId);
        emit userIdChanged();
    }

    if (m_screenName != screenName) {
        m_screenName = std::move(screenName);
        emit nameChanged();
    }
}
