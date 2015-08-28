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

#include <sailfishapp.h>
#include "twitterauthentification.h"
#include "twitteruser.h"
#include "twitterusermodel.h"
#include "twittermodel.h"
#include "twittermodelitem.h"
#include "twittermodelquery.h"
#include "twitterstatus.h"
#include "twittertweet.h"
#include "twittertimelinequery.h"
#include "twittermodelquerymodel.h"
#include "layout.h"
#include "layoutmodel.h"

int main(int argc, char *argv[])
{
    qmlRegisterUncreatableType<TwitterUser>("harbour.twablet", 1, 0, "TwitterUser", QLatin1String("Uncreatable"));
    qmlRegisterUncreatableType<TwitterModelItem>("harbour.twablet", 1, 0, "TwitterModelItem", QLatin1String("Uncreatable"));
    qmlRegisterUncreatableType<TwitterModelQuery>("harbour.twablet", 1, 0, "TwitterModelQuery", QLatin1String("Uncreatable"));
    qmlRegisterUncreatableType<TwitterStatus>("harbour.twablet", 1, 0, "TwitterStatus", QLatin1String("Uncreatable"));
    qmlRegisterUncreatableType<TwitterTweet>("harbour.twablet", 1, 0, "TwitterTweet", QLatin1String("Uncreatable"));
    qmlRegisterUncreatableType<TwitterTimelineQuery>("harbour.twablet", 1, 0, "TwitterTimelineQuery", QLatin1String("Uncreatable"));
    qmlRegisterUncreatableType<Layout>("harbour.twablet", 1, 0, "Layout", QLatin1String("Uncreatable"));
    qmlRegisterType<TwitterUserModel>("harbour.twablet", 1, 0, "TwitterUserModel");
    qmlRegisterType<TwitterAuthentification>("harbour.twablet", 1, 0, "TwitterAuthentification");
    qmlRegisterType<TwitterModel>("harbour.twablet", 1, 0, "TwitterModel");
    qmlRegisterType<TwitterModelQueryModel>("harbour.twablet", 1, 0, "TwitterModelQueryModel");
    qmlRegisterType<LayoutModel>("harbour.twablet", 1, 0, "LayoutModel");
    return SailfishApp::main(argc, argv);
}
