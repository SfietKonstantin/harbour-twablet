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

Dialog {
    id: container
    property SettingsPage parentPage: null
    property int index: -1
    property string userId
    property string screenName
    property string token
    property string tokenSecret
    property string initialName
    readonly property bool editing: index != -1
    canAccept: nameField.text.length > 0
    allowedOrientations: app.defaultAllowedOrientations

    onAccepted: {
        if (!container.editing) {
            var index = Repository.addAccount(nameField.text, container.userId, container.screenName,
                                              container.token, container.tokenSecret)
            Repository.addDefaultLayouts(index, qsTr("Home"), homeTl.checked,
                                         qsTr("Mentions"), mentionsTl.checked)
        } else {
            Repository.updateAccountName(container.index, nameField.text)
        }
    }

    SilicaFlickable {
        id: flickable
        anchors.fill: parent

        Column {
            anchors.left: parent.left; anchors.right: parent.right
            DialogHeader {
                id: header
                acceptText: container.editing ? qsTr("Edit account") : qsTr("Add account")
            }

            Item {
                anchors.left: parent.left; anchors.leftMargin: Theme.horizontalPageMargin
                anchors.right: parent.right; anchors.rightMargin: Theme.horizontalPageMargin
                height: Theme.itemSizeLarge

                Image {
                    id: icon
                    width: Theme.itemSizeMedium
                    height: Theme.itemSizeMedium
                    source: "image://theme/icon-m-service-twitter"
                }

                Label {
                    anchors.left: icon.right; anchors.leftMargin: Theme.paddingMedium
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    text: container.screenName
                    font.pixelSize: Theme.fontSizeLarge
                    color: Theme.highlightColor
                }
            }
            TextField {
                id: nameField
                anchors.left: parent.left; anchors.right: parent.right
                placeholderText: qsTr("Name of the account")
                text: container.initialName
            }
            TextSwitch {
                id: homeTl
                visible: !container.editing
                text: qsTr("Add home timeline")
                checked: true
            }
            TextSwitch {
                id: mentionsTl
                visible: !container.editing
                text: qsTr("Add mentions timeline")
                checked: true
            }
        }

        PullDownMenu {
            visible: container.editing
            MenuItem {
                text: qsTr("Remove")
                onClicked: {
                    container.parentPage.removeAccount(container.index)
                    pageStack.pop()
                }
            }
        }
    }
}
