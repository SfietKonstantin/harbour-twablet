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

MouseArea {
    id: container
    height: background.height
    property QtObject tweet
    property real itemSize: Theme.itemSizeSmall
    property real fontSize: Theme.fontSizeSmall
    property real fontSizeSmall: Theme.fontSizeExtraSmall
    signal handleLink(string url)
    signal handleOpenImageBrowser(QtObject tweet)

    Rectangle {
        id: background
        visible: container.enabled
        anchors.left: parent.left; anchors.leftMargin: Theme.paddingMedium
        anchors.right: parent.right; anchors.rightMargin: Theme.paddingMedium
        color: Theme.secondaryHighlightColor
        height: column.height + Theme.paddingSmall
        opacity: container.pressed ? 0.8 : 0.4
    }

    Column {
        id: column
        spacing: Theme.paddingMedium
        anchors.left: background.left
        anchors.right: background.right

        BackgroundItem {
            id: retweet
            property bool isRetweet: tweet ? tweet.retweetingUser !== null : false
            anchors.left: parent.left
            anchors.right: parent.right
            visible: isRetweet
            height: retweetLabel.height + 2 * Theme.paddingSmall
            onClicked: {
                if (isRetweet) {
                    container.handleLink("user://" + tweet.retweetingUser.id)
                }
            }

            Image {
                id: retweetIcon
                anchors.left: parent.left; anchors.leftMargin: Theme.paddingMedium
                anchors.verticalCenter: parent.verticalCenter
                source: "image://theme/icon-s-retweet" + (retweet.pressed ? ("?"  + Theme.highlightColor) : "")
            }

            Label {
                id: retweetLabel
                anchors.left: retweetIcon.right; anchors.leftMargin: Theme.paddingMedium
                anchors.right: parent.right; anchors.rightMargin: Theme.paddingMedium
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: container.fontSize
                color: retweet.pressed ? Theme.secondaryHighlightColor : Theme.secondaryColor
                text: retweet.isRetweet ? qsTr("Retweeted by %1").arg(tweet.retweetingUser.name) : ""
                wrapMode: Text.Wrap
            }
        }

        TweetHeader {
            id: tweetHeader
            tweet: container.tweet
            padding: container.enabled ? 0 : Theme.paddingMedium
            height: container.itemSize
            fontSize: container.fontSize
            fontSizeSmall: container.fontSizeSmall
            onHandleLink: container.handleLink(url)
        }

        TweetText {
            tweet: container.tweet
            anchors.left: parent.left; anchors.leftMargin: Theme.paddingSmall
            anchors.right: parent.right; anchors.rightMargin: Theme.paddingSmall
            font.pixelSize: container.fontSize
            onHandleLink: container.handleLink(url)
        }

        Item {
            anchors.left: parent.left; anchors.right: parent.right
            height: mediaGrid.height

            Connections {
                target: container
                onTweetChanged: mediaGrid.computeSizes()
            }

            MouseArea {
                anchors.fill: mediaGrid
                enabled: !container.enabled
                onClicked: container.handleOpenImageBrowser(container.tweet, container.account)
            }

            Grid {
                id: mediaGrid
                visible: container.tweet ? container.tweet.media.count > 0 : false
                property real imageWidth: mediaGrid.width
                property real imageHeight: mediaGrid.width * 2 / 3
                property bool isSingle: container.tweet.media.count === 1
                function computeSizes() {
                    if (container.tweet == null) {
                        return
                    }

                    switch (container.tweet.media.count) {
                    case 2:
                        mediaGrid.columns = 2
                        mediaGrid.rows = 1
                        mediaGrid.imageWidth = mediaGrid.width / 2
                        mediaGrid.imageHeight = mediaGrid.width / 2
                        break
                    case 3:
                        mediaGrid.columns = 3
                        mediaGrid.rows = 1
                        mediaGrid.imageWidth = mediaGrid.width / 3
                        mediaGrid.imageHeight = mediaGrid.width / 3
                        break
                    case 4:
                        mediaGrid.columns = 2
                        mediaGrid.rows = 2
                        mediaGrid.imageWidth = mediaGrid.width / 2
                        mediaGrid.imageHeight = mediaGrid.width / 2
                        break
                    default:
                        mediaGrid.columns = 1
                        mediaGrid.rows = 1
                        mediaGrid.imageWidth = mediaGrid.width
                        mediaGrid.imageHeight = mediaGrid.width * 2 / 3
                        break
                    }
                }
                anchors.left: parent.left; anchors.leftMargin: Theme.paddingSmall
                anchors.right: parent.right; anchors.rightMargin: Theme.paddingSmall
                columns: 1
                rows: 1
                clip: true

                Component.onCompleted: computeSizes()
                onWidthChanged: computeSizes()

                Repeater {
                    model: container.tweet ? container.tweet.media : null
                    delegate: TwitterImage {
                        property real ratio: media.size.height / media.size.width
                        property real mediaHeight: container.enabled ? mediaGrid.imageHeight : width * ratio
                        width: mediaGrid.imageWidth
                        height: mediaGrid.isSingle ? mediaHeight : mediaGrid.imageHeight
                        source: media.url
                    }
                }
            }
        }

        TweetFooter {
            tweet: container.tweet
            anchors.left: parent.left; anchors.leftMargin: Theme.paddingSmall
            anchors.right: parent.right; anchors.rightMargin: Theme.paddingSmall
            font.pixelSize: container.fontSize
        }
    }
}
