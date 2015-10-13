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
    property RightPanel panel
    property QtObject tweet
    property QtObject account
    allowedOrientations: app.defaultAllowedOrientations

    Drawer {
        id: drawer
        anchors.fill: parent
        dock: container.isPortrait ? Dock.Top: Dock.Left

        background: SilicaFlickable {
            anchors.fill: parent
            contentHeight: tweetColumn.height

            Column {
                id: tweetColumn
                anchors.left: parent.left; anchors.leftMargin: Theme.paddingMedium
                anchors.right: parent.right; anchors.rightMargin: Theme.paddingMedium
                spacing: Theme.paddingMedium

                PageHeader {
                    title: qsTr("Tweet")
                }

                TweetHeader {
                    tweet: container.tweet
                    padding: Theme.paddingMedium
                    anchors.left: parent.left; anchors.right: parent.right
                    onHandleLink: LH.handleLink(url, container.panel, container.account, Info.Replace)
                }

                TweetText {
                    tweet: container.tweet
                    anchors.left: parent.left; anchors.leftMargin: Theme.paddingSmall
                    anchors.right: parent.right; anchors.rightMargin: Theme.paddingSmall
                    font.pixelSize: Theme.fontSizeSmall
                    onHandleLink: LH.handleLink(url, container.panel, container.account, Info.Replace)
                }

                TweetFooter {
                    tweet: container.tweet
                    anchors.left: parent.left; anchors.leftMargin: Theme.paddingSmall
                    anchors.right: parent.right; anchors.rightMargin: Theme.paddingSmall
                    font.pixelSize: Theme.fontSizeSmall
                }
            }

            VerticalScrollDecorator {}

        }
        foreground: SilicaListView {
            id: view
            property real imageWidth: Math.max(container.width, container.height)
            anchors.fill: parent
            model: container.tweet.media
            orientation: Qt.Horizontal
            highlightRangeMode: ListView.StrictlyEnforceRange
            cacheBuffer: 4 * view.imageWidth

            delegate: Item {
                width: view.width
                height: view.height

                TwitterImage {
                    anchors.fill: parent
                    image.sourceSize.width: view.imageWidth
                    image.sourceSize.height: view.imageWidth
                    image.fillMode: Image.PreserveAspectFit
                    source: model.media.urlLarge
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: !drawer.open ? drawer.show() : drawer.hide()
                }
            }
        }
    }
}

