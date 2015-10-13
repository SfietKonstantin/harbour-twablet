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

BackgroundItem {
    id: container
    property QtObject tweet
    property real fontSize: Theme.fontSizeSmall
    property real fontSizeSmall: Theme.fontSizeExtraSmall
    property real padding
    signal handleLink(string url)
    anchors.left: parent.left; anchors.leftMargin: -padding
    anchors.right: parent.right; anchors.rightMargin: -padding
    height: Theme.itemSizeSmall
    onClicked: container.handleLink("user://" + container.tweet.user.id)

    TwitterImage {
        id: profilePicture
        anchors.top: parent.top;
        anchors.left: parent.left; anchors.leftMargin: container.padding
        width: container.height
        height: container.height
        source: container.tweet ? (Screen.sizeCategory === Screen.Large ? container.tweet.user.imageUrlLarge : container.tweet.user.imageUrl) : ""
    }

    Column {
        id: headerColumn
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: profilePicture.right; anchors.leftMargin: Theme.paddingMedium
        anchors.right: indicators.right
        spacing: Theme.paddingSmall

        Label {
            anchors.left: parent.left; anchors.right: parent.right
            color: container.down ? Theme.highlightColor : Theme.primaryColor
            font.pixelSize: container.fontSize
            text: container.tweet ? container.tweet.user.name : ""
        }

        Label {
            id: screenName
            anchors.left: parent.left; anchors.right: parent.right
            color: container.down ? Theme.secondaryHighlightColor : Theme.secondaryColor
            font.pixelSize: container.fontSizeSmall
            text: "@" + (container.tweet ? container.tweet.user.screenName : "")
        }
    }

    Row {
        id: indicators
        visible: container.tweet ? (container.tweet.favorited || container.tweet.retweeted) : false
        anchors.top: parent.top; anchors.topMargin: Theme.paddingSmall
        anchors.right: parent.right
        anchors.rightMargin: Theme.paddingMedium + container.padding

        Image {
            width: Theme.iconThemeSmall
            height: Theme.iconThemeSmall
            source: "image://theme/icon-s-retweet?" + Theme.highlightColor
            visible: container.tweet ? container.tweet.retweeted : false
        }
        Image {
            width: Theme.iconThemeSmall
            height: Theme.iconThemeSmall
            source: "image://theme/icon-s-favorite?" + Theme.highlightColor
            visible: container.tweet ? container.tweet.favorited : false
        }
    }
}
