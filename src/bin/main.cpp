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
#include "qml/usermodel.h"
#include "qml/tweetqueryitem.h"
#include "qml/retweetqueryitem.h"

int main(int argc, char *argv[])
{
    qmlRegisterUncreatableType<DataRepositoryObject>("harbour.twablet", 1, 0, "DataRepository", QLatin1String("Uncreatable"));
    qmlRegisterUncreatableType<IModel>("harbour.twablet", 1, 0, "Model", QLatin1String("Uncreatable"));
    qmlRegisterUncreatableType<AccountObject>("harbour.twablet", 1, 0, "Account", QLatin1String("Uncreatable"));
    qmlRegisterUncreatableType<LayoutObject>("harbour.twablet", 1, 0, "Layout", QLatin1String("Uncreatable"));
    qmlRegisterUncreatableType<QueryObject>("harbour.twablet", 1, 0, "Query", QLatin1String("Uncreatable"));
    qmlRegisterUncreatableType<TweetObject>("harbour.twablet", 1, 0, "Tweet", QLatin1String("Uncreatable"));
    qmlRegisterUncreatableType<UserObject>("harbour.twablet", 1, 0, "User", QLatin1String("Uncreatable"));
    qmlRegisterUncreatableType<MediaObject>("harbour.twablet", 1, 0, "Media", QLatin1String("Uncreatable"));
    qmlRegisterUncreatableType<MediaModel>("harbour.twablet", 1, 0, "MediaModel", QLatin1String("Uncreatable"));
    qmlRegisterUncreatableType<AbstractQueryItem>("harbour.twablet", 1, 0, "QueryItem", QLatin1String("Uncreatable"));
    qmlRegisterType<TwitterAuthentification>("harbour.twablet", 1, 0, "TwitterAuthentification");
    qmlRegisterType<AccountModel>("harbour.twablet", 1, 0, "AccountModel");
    qmlRegisterType<AccountSelectionModel>("harbour.twablet", 1, 0, "AccountSelectionModel");
    qmlRegisterType<LayoutModel>("harbour.twablet", 1, 0, "LayoutModel");
    qmlRegisterType<TweetModel>("harbour.twablet", 1, 0, "TweetModel");
    qmlRegisterType<QueryListModel>("harbour.twablet", 1, 0, "QueryListModel");
    qmlRegisterType<StatusUpdateQueryItem>("harbour.twablet", 1, 0, "StatusUpdateQueryItem");
    qmlRegisterType<UserQueryItem>("harbour.twablet", 1, 0, "UserQueryItem");
    qmlRegisterType<DescriptionFormatter>("harbour.twablet", 1, 0, "DescriptionFormatter");
    qmlRegisterType<TweetFormatter>("harbour.twablet", 1, 0, "TweetFormatter");
    qmlRegisterType<UserModel>("harbour.twablet", 1, 0, "UserModel");
    qmlRegisterType<TweetQueryItem>("harbour.twablet", 1, 0, "TweetQueryItem");
    qmlRegisterType<RetweetQueryItem>("harbour.twablet", 1, 0, "RetweetQueryItem");
    qmlRegisterSingletonType<DataRepositoryObject>("harbour.twablet", 1, 0, "Repository",
                                                          [](QQmlEngine *e, QJSEngine *) -> QObject * {
        return new DataRepositoryObject(e);
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
