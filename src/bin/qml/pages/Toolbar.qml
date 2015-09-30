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

Rectangle {
    id: container
    signal goToIndex(int index)
    signal goToTop(int index)
    function setReply(inReplyTo, screenName)
    {
        postText.text = screenName + " "
        internal.replyScreenName = screenName
        statusUpdateQueryItem.inReplyTo = inReplyTo
        internal.checkReply()
        postText.cursorPosition = postText.text.length
        postText.focus = true
    }
    property bool isLandscape: false
    property int lastTappedIndex: -1
    property alias currentIndex: view.currentIndex
    property int columnCount
    property ListView mainView
    anchors.left: parent.left; anchors.right: parent.right
    anchors.bottom: parent.bottom
    height: layoutModel.count > 0 ? internal.height : 0
    gradient: Gradient {
        GradientStop { position: 0; color: Theme.rgba(Theme.secondaryHighlightColor, 0.2) }
        GradientStop { position: 1; color: "transparent" }
    }

    QtObject {
        id: internal
        function clearReply() {
            replyScreenName = ""
            statusUpdateQueryItem.inReplyTo = ""
        }
        function checkReply() {
            internal.isReply = (statusUpdateQueryItem.inReplyTo.length > 0
                                && postText.text.indexOf(internal.replyScreenName) === 0)
        }

        property bool isSplit: container.columnCount > 1
        property double viewHeight: (isSplit ? post.height : Theme.itemSizeSmall)
        property double height: (isSplit ? post.height : Theme.itemSizeSmall + post.height)
        property double width: (isSplit ? container.width / 2 : container.width)
        property string replyScreenName
        property bool isReply: false
        onIsReplyChanged: {
            if (!isReply) {
                clearReply()
            }
        }
    }

    Connections {
        target: postText
        onTextChanged: internal.checkReply()
    }

    SilicaListView {
        id: view
        property int capacity: Math.floor(view.width / Theme.itemSizeMedium)
        property double cellWidth: view.width / capacity === Infinity ? 0 : view.width / capacity
        width: internal.width
        height: internal.viewHeight
        model: LayoutModel { id: layoutModel; repository: Repository }
        orientation: Qt.Horizontal
        clip: true

        delegate: ToolbarButton {
            id: delegate
            width: view.cellWidth
            height: view.height
            onClicked: {
                var targetIndex = model.index
                if (container.columnCount >= 3) {
                    targetIndex = Math.max(model.index - 1, 0)
                }

                container.goToIndex(targetIndex)
                if (container.lastTappedIndex == -1) {
                    container.lastTappedIndex = model.index
                } else {
                    if (model.index === container.lastTappedIndex) {
                        container.goToTop(container.lastTappedIndex)
                    } else {
                        container.lastTappedIndex = -1
                    }
                }
            }
            source: {
                switch (model.queryType) {
                case Query.Home:
                    Qt.resolvedUrl("../../data/home.svg")
                    break
                case Query.Mentions:
                    Qt.resolvedUrl("../../data/mail.svg")
                    break
                case Query.Search:
                    Qt.resolvedUrl("../../data/search.svg")
                    break
                case Query.Favorites:
                    "image://theme/icon-l-favorite"
                    break
                case Query.UserTimeline:
                    Qt.resolvedUrl("../../data/me.svg")
                    break
                default:
                    ""
                    break
                }
            }

            Label {
                anchors.top: parent.top; anchors.topMargin: Theme.paddingSmall
                anchors.right: parent.right; anchors.rightMargin: Theme.paddingSmall
                font.pixelSize: Theme.fontSizeSmall
                text: model.unread > 0 ? model.unread : ""
                color: Theme.highlightColor
            }
        }

        footer: ToolbarButton {
            width: view.cellWidth
            height: view.height
            source: "image://theme/icon-m-add"
            onClicked: pageStack.push(Qt.resolvedUrl("AddColumnPage.qml"), {accountModel: accountModel})
        }

        Rectangle {
            property double ratio: (internal.isSplit ? 2 : 1)
            width: view.cellWidth * container.columnCount
            height: Theme.paddingMedium / 2
            x: ((mainView.contentX - mainView.originX) / view.capacity * container.columnCount - (view.contentX - view.originX)) / ratio
        }
    }

    Item {
        id: post
        width: internal.width
        height: postColumn.height + 2 * Theme.paddingSmall
        anchors.right: parent.right; anchors.bottom: parent.bottom

        Column {
            id: postColumn
            anchors.left: parent.left; anchors.leftMargin: Theme.paddingMedium
            anchors.right: parent.right; anchors.rightMargin: Theme.paddingMedium
            anchors.top: parent.top; anchors.topMargin: Theme.paddingSmall
            spacing: Theme.paddingSmall

            Item {
                height: postText.height
                anchors.left: parent.left; anchors.right: parent.right

                StatusUpdateQueryItem {
                    id: statusUpdateQueryItem
                    account: postAccountSelectionModel.selection
                    text: postText.text
                    onStatusChanged: {
                        if (statusUpdateQueryItem.status === QueryItem.Idle) {
                            postText.text = ""
                            Repository.refresh()
                            internal.clearReply()
                        }
                    }
                }

                TextArea {
                    id: postText
                    property int textLeft: 140 - postText.text.length
                    anchors.left: parent.left
                    anchors.right: postSend.left; anchors.rightMargin: Theme.paddingSmall
                    placeholderText: qsTr("Tweet something")
                    label: textLeft >= 0 ? qsTr("%n left", "", textLeft) : qsTr("Tweet too long")
                }

                Label {
                    id: postSend
                    anchors.bottom: parent.bottom; anchors.bottomMargin: Theme.fontSizeSmall + Theme.paddingLarge
                    anchors.right: parent.right; anchors.rightMargin: Theme.paddingSmall
                    text: internal.isReply ? qsTr("Reply") : qsTr("Send")
                    color: (postText.text.length > 0 && statusUpdateQueryItem.status !== QueryItem.Loading) ? Theme.primaryColor : Theme.secondaryColor

                    MouseArea {
                        enabled: postText.text.length > 0 && statusUpdateQueryItem.status !== QueryItem.Loading
                        anchors.fill: parent
                        onClicked: statusUpdateQueryItem.load()
                    }
                }
            }


            Item {
                id: postButtonBar
                visible: false
                anchors.left: parent.left; anchors.right: parent.right
                height: Theme.fontSizeMedium + Theme.paddingSmall

                AccountSelectionModel {
                    id: postAccountSelectionModel
                    repository: Repository
                    Component.onCompleted: {
                        postAccountSelectionModel.index = 0
                    }
                }

                Label {
                    anchors.left: parent.left; anchors.leftMargin: Theme.paddingLarge
                    anchors.bottom: parent.bottom; anchors.bottomMargin: Theme.paddingSmall
                    // text: qsTr("Account: %1").arg(postAccountsModel.currentAccount.name)
                    text: qsTr("Account: %1").arg(postAccountSelectionModel.selection.name)

                    MouseArea {
                        anchors.fill: parent
                    }
                }
            }

            states: [
                State {
                    name: "focused"; when: postText.focus && postAccountsModel.count > 1
                    PropertyChanges {
                        target: postButtonBar
                        visible: true
                    }
                }
            ]
        }
    }
}

