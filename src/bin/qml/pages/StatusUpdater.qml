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
    height: postColumn.height + 2 * Theme.paddingSmall
    property string inReplyTo
    property string inReplyScreenName
    property bool replyOnly: false
    readonly property bool isReply: inReplyScreenName.length > 0 && (textArea.text.indexOf(inReplyScreenName) === 0 || replyOnly)
    property alias text: textArea.text
    signal statusUpdated()
    function focus() {
        textArea.focus = true
    }

    Column {
        id: postColumn
        anchors.left: parent.left; anchors.right: parent.right
        anchors.top: parent.top; anchors.topMargin: Theme.paddingSmall
        spacing: Theme.paddingSmall

        Item {
            height: textArea.height
            anchors.left: parent.left; anchors.right: parent.right

            StatusUpdateQueryItem {
                id: queryItem
                accountUserId: postAccountSelectionModel.selection.userId
                text: (container.replyOnly ? container.inReplyScreenName + " " : "") + textArea.text
                onFinished: {
                    textArea.text = ""
                    Repository.refresh()
                    container.statusUpdated()
                }
            }

            TextArea {
                id: textArea
                property int textLeft: 140 - textArea.text.length - (container.replyOnly ? container.inReplyScreenName.length + 1 : 0)
                anchors.left: parent.left
                anchors.right: send.left; anchors.rightMargin: Theme.paddingSmall
                placeholderText: container.replyOnly ? qsTr("Reply to %1").arg(container.inReplyScreenName)
                                                     : qsTr("Tweet something")
                label: textLeft >= 0 ? qsTr("%n left", "", textLeft) : qsTr("Tweet too long")
            }

            Label {
                id: send
                property bool sendEnabled: textArea.text.length > 0 && queryItem.status !== QueryItem.Loading
                anchors.bottom: parent.bottom; anchors.bottomMargin: Theme.fontSizeSmall + Theme.paddingLarge
                anchors.right: parent.right; anchors.rightMargin: Theme.paddingMedium
                text: container.isReply ? qsTr("Reply") : qsTr("Send")
                color: sendEnabled ? (sendMouseArea.pressed ? Theme.highlightColor : Theme.primaryColor) : Theme.secondaryColor

                MouseArea {
                    id: sendMouseArea
                    enabled: send.sendEnabled
                    anchors.fill: parent
                    onClicked: queryItem.load(Repository)
                }
            }
        }

        Item {
            id: buttonsBar
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
                text: qsTr("Account: %1").arg(postAccountSelectionModel.selection.name)

                MouseArea {
                    anchors.fill: parent
                }
            }
        }

        states: [
            State {
                name: "focused"; when: textArea.focus && postAccountsModel.count > 1
                PropertyChanges {
                    target: buttonsBar
                    visible: true
                }
            }
        ]
    }
}

