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

Item {
    id: container
    property string title
    property SilicaListView flickable: view
    property alias query: twitterModel.query
    signal handleLink(string url)
    signal openTweet(string tweetId, string retweetId)
    signal unreadChanged(int unread)
    signal removed()
    function setUnread(index) {
        if (internal.unread > index && index !== -1) {
            internal.unread = index
        }
    }
    function scrollToTop() {
        view.scrollToTop()
    }

    clip: true

    QtObject {
        id: internal
        property int unread: 0
        property double refreshDelta
        property bool isInit
        property double opacity: isLoading ? 0. : 1
        property bool isLoading: twitterModel.count === 0 && twitterModel.status === Model.Loading
        onUnreadChanged: {
            container.unreadChanged(unread)
        }

        Behavior on opacity {
            NumberAnimation { duration: 200 }
        }
    }

    // Thanks tweetian :)
    Timer {
        id: refreshUnreadCountTimer
        interval: 250
        repeat: false
        onTriggered: container.setUnread(view.indexAt(0, container.contentY + 5))
    }

    SilicaListView {
        id: view
        anchors.fill: parent
        spacing: Theme.paddingMedium

        model: TweetModel {
            id: twitterModel
            repository: Repository
        }

        onContentYChanged: refreshUnreadCountTimer.running = true

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
                    internal.unread += insertedCount
                }
            }
        }

        header: Column {
            width: container.width

            PageHeader {
                id: header
                anchors.left: parent.left; anchors.right: parent.right
                title: container.title
                height: Theme.itemSizeLarge

                BusyIndicator {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left; anchors.leftMargin: Theme.paddingMedium
                    running: twitterModel.status === Model.Loading
                    size: BusyIndicatorSize.Small
                }
            }

            StatusHeader {
                model: twitterModel
            }
        }

        delegate: TweetDelegate {
            id: delegate
            onOpenTweet: {
                container.openTweet(originalId, id)
            }
            width: container.width
            tweet: model.item
            onHandleLink: container.handleLink(url)
            opacity: internal.opacity
            itemSize: Theme.itemSizeExtraSmall
            fontSize: Theme.fontSizeExtraSmall
            fontSizeSmall: Theme.fontSizeTiny
        }

        footer: LoadMoreButton {
            model: twitterModel
            onClicked: Repository.loadMore(twitterModel.query)
        }

        StatusPlaceholder {
            model: twitterModel
            errorMessage: (model.status === Model.Idle && model.count === 0) ? qsTr("No tweets") : ""
        }

        VerticalScrollDecorator {}
    }
}
