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
    signal openUser(string id)

    Rectangle {
        id: background
        anchors.left: parent.left; anchors.leftMargin: Theme.paddingMedium
        anchors.right: parent.right; anchors.rightMargin: Theme.paddingMedium
        color: Theme.secondaryHighlightColor
        height: column.height + Theme.paddingSmall
        opacity: 0.4
    }

    Column {
        id: column
        spacing: Theme.paddingMedium
        anchors.left: background.left
        anchors.right: background.right

        BackgroundItem {
            id: retweet
            property bool isRetweet: tweet.retweetingUser !== null
            anchors.left: parent.left
            anchors.right: parent.right
            visible: isRetweet
            height: retweetLabel.height + 2 * Theme.paddingSmall
            onClicked: {
                if (isRetweet) {
                    container.openUser(tweet.retweetingUser.id)
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
                font.pixelSize: Theme.fontSizeSmall
                color: retweet.pressed ? Theme.secondaryHighlightColor : Theme.secondaryColor
                text: retweet.isRetweet ? qsTr("Retweeted by %1").arg(tweet.retweetingUser.name) : ""
                wrapMode: Text.Wrap
            }
        }

        BackgroundItem {
            id: header
            anchors.left: parent.left
            anchors.right: parent.right
            height: Theme.itemSizeSmall
            onClicked: container.openUser(container.tweet.user.id)

            TwitterImage {
                id: profilePicture
                anchors.top: parent.top; anchors.left: parent.left
                width: Theme.itemSizeSmall
                height: Theme.itemSizeSmall
                source: container.tweet.user.imageUrl
            }

            Column {
                id: headerColumn
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: profilePicture.right; anchors.leftMargin: Theme.paddingMedium
                anchors.right: parent.right; anchors.rightMargin: Theme.paddingMedium

                Label {
                    color: header.down ? Theme.highlightColor : Theme.primaryColor
                    anchors.left: parent.left; anchors.right: parent.right
                    text: container.tweet.user.name
                }

                Label {
                    id: screenName
                    color: header.down ? Theme.secondaryHighlightColor : Theme.secondaryColor
                    font.pixelSize: Theme.fontSizeSmall
                    anchors.left: parent.left; anchors.right: parent.right
                    text: "@" + container.tweet.user.screenName
                }
            }
        }

        Label {
            id: text
            anchors.left: parent.left; anchors.leftMargin: Theme.paddingSmall
            anchors.right: parent.right; anchors.rightMargin: Theme.paddingSmall
            font.pixelSize: Theme.fontSizeSmall
            wrapMode: Text.Wrap
            color: Theme.highlightColor
            text: container.tweet.text
        }

        Item {
            anchors.left: parent.left; anchors.right: parent.right
            height: mediaGrid.height

            Rectangle {
                anchors.fill: mediaGrid
                color: Theme.secondaryHighlightColor
            }

            Grid {
                id: mediaGrid
                visible: container.tweet.media.count > 0
                property real smallSize: mediaGrid.width * 2 / 3
                property real largeSize: mediaGrid.width * 2 / 3
                anchors.left: parent.left; anchors.leftMargin: Theme.paddingSmall
                anchors.right: parent.right; anchors.rightMargin: Theme.paddingSmall
                columns: 3
                rows: 2

                Repeater {
                    model: container.tweet.media
                    delegate: TwitterImage {
                        property bool isFirst: index === 0
                        property bool isSingle: index === 0 && container.tweet.media.count === 1
                        property real mediaHeight: width / 3 * 2
                        width: isSingle ? mediaGrid.width : (isFirst ? mediaGrid.largeSize : mediaGrid.smallSize)
                        height: isSingle ? mediaHeight : (isFirst ? mediaGrid.largeSize : mediaGrid.smallSize)
                        source: media.url
                    }
                }
            }
        }



        Label {
            anchors.left: parent.left; anchors.leftMargin: Theme.paddingSmall
            anchors.right: parent.right; anchors.rightMargin: Theme.paddingSmall
            color: Theme.secondaryHighlightColor
            font.pixelSize: Theme.fontSizeSmall
            wrapMode: Text.Wrap
            text: container.tweet.sourceName + " | " + Format.formatDate(container.tweet.timestamp, Formatter.DurationElapsed)
        }
    }
}
