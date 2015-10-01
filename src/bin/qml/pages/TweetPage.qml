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
    property alias tweetId: query.identifier
    property alias account: query.account
    property RightPanel panel
    function load() { query.load() }

    TweetQueryItem {
        id: query
    }

    SilicaFlickable {
        anchors.fill: parent
        clip: true
        contentHeight: Math.max(column.height, container.height)

        Column {
            id: column
            opacity: query.status === QueryItem.Idle && query.data ? 1 : 0
            anchors.left: parent.left; anchors.right: parent.right

            PageHeader {
                title: qsTr("Tweet")
            }

            TweetDelegate {
                enabled: false
                anchors.left: parent.left; anchors.right: parent.right
                tweet: query.data
                onHandleLink: LH.handleLink(url, container.panel, container.account, false)
            }

            Row {
               IconButton {
                   icon.source: "image://theme/icon-s-chat"
                   onClicked: replyStatusUpdater.focus()
               }
               IconButton {
                   icon.source: "image://theme/icon-s-retweet"
                   enabled: false
               }
               IconButton {
                   icon.source: "image://theme/icon-s-favorite"
                   enabled: false
               }
            }

            StatusUpdater {
                id: replyStatusUpdater
                replyOnly: true
                inReplyTo: query.data ? query.data.id : ""
                inReplyScreenName: query.data ? "@" + query.data.user.screenName : ""
                anchors.left: parent.left; anchors.right: parent.right
            }

            Behavior on opacity {
                NumberAnimation { duration: 200 }
            }
        }

        StatusPlaceholder {
            query: query
        }

        VerticalScrollDecorator {}
    }
}

