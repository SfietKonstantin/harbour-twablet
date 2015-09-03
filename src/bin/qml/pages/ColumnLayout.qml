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

SilicaListView {
    id: container
    property string title
    property int layoutIndex
    spacing: Theme.paddingMedium

    model: TwitterTweetModel {
        id: twitterModel
        layoutIndex: container.layoutIndex
        repository: Repository
    }

    QtObject {
        id: internal
        property int unread: 0
        property double refreshDelta
        property bool isInit
        onUnreadChanged: {
            Repository.updateLayoutUnread(container.layoutIndex, unread)
        }
    }

    Connections {
        // Workaround to prepend tweets
        // Not showing the first delegate will
        // make the ListView not to scroll
        // After inserting elements we reposition
        // the view
        target: twitterModel
        onPrependPre: {
            internal.isInit = (twitterModel.count === 0)
            if (!internal.isInit) {
                var previousY = container.contentY
                container.positionViewAtIndex(1, ListView.Beginning)
                internal.refreshDelta = container.contentY - previousY
            }
        }
        onPrependPost: {
            if (!internal.isInit) {
                container.positionViewAtIndex(1 + insertedCount, ListView.Beginning)
                container.contentY -= internal.refreshDelta
            }
            // Search for the index of the first item in the view and update the unread info
            var x = container.width / 2
            for (var y = 0; y < container.height; ++y) {
                var index = container.indexAt(x, y)
                if (index !== -1) {
                    internal.unread = index
                    break
                }
            }
        }
    }

    onContentYChanged: {
        var index = container.indexAt(container.width / 2, 0)
        if (index !== -1) {
            internal.unread = Math.min(internal.unread, index)
        }
    }

    header: PageHeader {
        id: pageHeader
        title: container.title
        height: Theme.itemSizeLarge
    }

    delegate: MouseArea {
        id: delegate
        width: container.width
        height: background.height

        Rectangle {
            id: background
            anchors.left: parent.left; anchors.leftMargin: Theme.paddingMedium
            anchors.right: parent.right; anchors.rightMargin: Theme.paddingMedium
            color: Theme.secondaryHighlightColor
            height: Math.max(avatar.height, text.height + Theme.paddingMedium)

            Rectangle {
                id: avatar
                anchors.top: parent.top; anchors.left: parent.left
                width: Theme.itemSizeSmall
                height: Theme.itemSizeSmall
            }

            Label {
                id: text
                anchors.top: parent.top; anchors.topMargin: Theme.paddingSmall
                anchors.left: avatar.right; anchors.leftMargin: Theme.paddingSmall
                anchors.right: parent.right; anchors.rightMargin: Theme.paddingMedium
                text: model.item.text
                font.pixelSize: Theme.fontSizeSmall
                wrapMode: Text.Wrap
                color: Theme.highlightColor
            }
        }
    }

    BusyIndicator {
        anchors.centerIn: parent
        enabled: twitterModel.count === 0 && twitterModel.status === Model.Loading
        size: BusyIndicatorSize.Large
    }

    VerticalScrollDecorator {}
}
