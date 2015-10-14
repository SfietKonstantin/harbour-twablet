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

#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#ifndef DESKTOP
#include <sailfishapp.h>
#else
#include <QtGui/QGuiApplication>
#include <QtQuick/QQuickView>
#endif
#include <QtQml/qqml.h>
#include <QtQml/QQmlEngine>
#include "qml/datarepositoryobject.h"
#include "qml/twitterauthentification.h"
#include "qml/accountobject.h"
#include "qml/accountselectionmodel.h"
#include "qml/layoutmodel.h"
#include "qml/queryobject.h"
#include "qml/querylistmodel.h"
#include "qml/tweetmodel.h"
#include "qml/statusupdatequeryitem.h"
#include "qml/userqueryitem.h"
#include "qml/descriptionformatter.h"
#include "qml/tweetformatter.h"
#include "qml/quotedtweetformatter.h"
#include "qml/usermodel.h"
#include "qml/tweetqueryitem.h"
#include "qml/retweetqueryitem.h"
#include "qml/favoritequeryitem.h"
#include "version.h"
#include "networkmonitor.h"

static const char *PAYPAL_DONATE = "https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&"
                                   "hosted_button_id=R6AJV4U2G33XG";
static const char *FLATTR_DONATE = "https://flattr.com/profile/sfiet_konstantin";

class Info: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString version READ version CONSTANT)
    Q_PROPERTY(QString paypal READ paypal CONSTANT)
    Q_PROPERTY(QString flattr READ flattr CONSTANT)
    Q_ENUMS(PushType)
public:
    enum PushType
    {
        Push = 0,
        Clear = 1,
        Replace = 2
    };
    explicit Info(QObject *parent = 0)
        : QObject(parent)
    {
    }
    QString version() const
    {
        return QLatin1String(Version);
    }
    QString paypal() const
    {
        return QLatin1String(PAYPAL_DONATE);
    }
    QString flattr() const
    {
        return QLatin1String(FLATTR_DONATE);
    }
};

int main(int argc, char *argv[])
{
    QLoggingCategory::setFilterRules(QLatin1String("tweet-central-repository.debug=false"));

    qmlRegisterUncreatableType<qml::DataRepositoryObject>("harbour.twablet", 1, 0, "DataRepository", QLatin1String("Uncreatable"));
    qmlRegisterUncreatableType<qml::IModel>("harbour.twablet", 1, 0, "Model", QLatin1String("Uncreatable"));
    qmlRegisterUncreatableType<qml::AccountObject>("harbour.twablet", 1, 0, "Account", QLatin1String("Uncreatable"));
    qmlRegisterUncreatableType<qml::LayoutObject>("harbour.twablet", 1, 0, "Layout", QLatin1String("Uncreatable"));
    qmlRegisterUncreatableType<qml::QueryObject>("harbour.twablet", 1, 0, "Query", QLatin1String("Uncreatable"));
    qmlRegisterUncreatableType<qml::TweetObject>("harbour.twablet", 1, 0, "Tweet", QLatin1String("Uncreatable"));
    qmlRegisterUncreatableType<qml::QuotedTweetObject>("harbour.twablet", 1, 0, "QuotedTweet", QLatin1String("Uncreatable"));
    qmlRegisterUncreatableType<qml::UserObject>("harbour.twablet", 1, 0, "User", QLatin1String("Uncreatable"));
    qmlRegisterUncreatableType<qml::MediaObject>("harbour.twablet", 1, 0, "Media", QLatin1String("Uncreatable"));
    qmlRegisterUncreatableType<qml::MediaModel>("harbour.twablet", 1, 0, "MediaModel", QLatin1String("Uncreatable"));
    qmlRegisterUncreatableType<qml::AbstractQueryItem>("harbour.twablet", 1, 0, "QueryItem", QLatin1String("Uncreatable"));
    qmlRegisterUncreatableType<Info>("harbour.twablet", 1, 0, "Info", QLatin1String("Uncreatable"));
    qmlRegisterType<qml::TwitterAuthentification>("harbour.twablet", 1, 0, "TwitterAuthentification");
    qmlRegisterType<qml::AccountModel>("harbour.twablet", 1, 0, "AccountModel");
    qmlRegisterType<qml::AccountSelectionModel>("harbour.twablet", 1, 0, "AccountSelectionModel");
    qmlRegisterType<qml::LayoutModel>("harbour.twablet", 1, 0, "LayoutModel");
    qmlRegisterType<qml::TweetModel>("harbour.twablet", 1, 0, "TweetModel");
    qmlRegisterType<qml::QueryListModel>("harbour.twablet", 1, 0, "QueryListModel");
    qmlRegisterType<qml::StatusUpdateQueryItem>("harbour.twablet", 1, 0, "StatusUpdateQueryItem");
    qmlRegisterType<qml::UserQueryItem>("harbour.twablet", 1, 0, "UserQueryItem");
    qmlRegisterType<qml::DescriptionFormatter>("harbour.twablet", 1, 0, "DescriptionFormatter");
    qmlRegisterType<qml::TweetFormatter>("harbour.twablet", 1, 0, "TweetFormatter");
    qmlRegisterType<qml::QuotedTweetFormatter>("harbour.twablet", 1, 0, "QuotedTweetFormatter");
    qmlRegisterType<qml::UserModel>("harbour.twablet", 1, 0, "UserModel");
    qmlRegisterType<qml::TweetQueryItem>("harbour.twablet", 1, 0, "TweetQueryItem");
    qmlRegisterType<qml::RetweetQueryItem>("harbour.twablet", 1, 0, "RetweetQueryItem");
    qmlRegisterType<qml::FavoriteQueryItem>("harbour.twablet", 1, 0, "FavoriteQueryItem");
    qmlRegisterSingletonType<qml::DataRepositoryObject>("harbour.twablet", 1, 0, "Repository",
                                                          [](QQmlEngine *e, QJSEngine *) -> QObject * {
        return new qml::DataRepositoryObject(e);
    });
    qmlRegisterSingletonType<Info>("harbour.twablet", 1, 0, "INFO",
                                   [](QQmlEngine *e, QJSEngine *) -> QObject * {
        return new Info(e);
    });
    qmlRegisterSingletonType<NetworkMonitor>("harbour.twablet", 1, 0, "NetworkMonitor",
                                             [](QQmlEngine *e, QJSEngine *) -> QObject * {
        return new NetworkMonitor(e);
    });

#ifndef DESKTOP
    return SailfishApp::main(argc, argv);
#else
    QGuiApplication app {argc, argv};
    QQuickView view {};
    view.setSource(QUrl(QLatin1String("qrc:/qml/harbour-twablet.qml")));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.show();
    return app.exec();
#endif
}

#include "main.moc"
