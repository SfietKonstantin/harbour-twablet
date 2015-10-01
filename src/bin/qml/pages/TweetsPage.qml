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

import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.twablet 1.0
import "LinkHandler.js" as LH

Page {
    id: container
    property string title
    property int queryType
    property var args
    property QtObject account
    property RightPanel panel

    Component.onCompleted: {
        var layoutIndex = Repository.addTemporaryLayout(container.account, container.queryType,
                                                        container.args)
        if (layoutIndex >= 0) {
            layout.layoutIndex = layoutIndex
        }
    }

    Component.onDestruction: {
        Repository.removeTemporaryLayout(layout.layoutIndex)
    }

    ColumnLayout {
        id: layout
        anchors.fill: parent
        temporary: true
        title: container.title
        onHandleLink: {
            LH.handleLink(url, container.panel, container.account, false)
        }
        onOpenTweet: {
            container.panel.openTweet(tweetId, retweetId, container.account, false)
        }

        PullDownMenu {
            MenuItem {
                text: qsTr("Add as column")
                onClicked: Repository.addLayout(container.title, container.account, container.queryType,
                                                container.args)
            }

            MenuItem {
                text: qsTr("Refresh")
                onClicked: Repository.refreshTemporary(container.layoutIndex)
            }
        }
    }
}

