/*
 * Copyright (C) 2013 Lucien XU <sfietkonstantin@free.fr>
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

Page {
    id: container
    property RightPanel rightPanel
    allowedOrientations: app.defaultAllowedOrientations

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: mainColumn.height + Theme.paddingLarge + buttonsColumn.height
        Column {
            id: mainColumn
            spacing: Theme.paddingMedium
            anchors.left: parent.left; anchors.right: parent.right
            PageHeader {
                title: qsTr("About")
            }

            Image {
                anchors.horizontalCenter: parent.horizontalCenter
                source: "../../data/logo.png"
            }

            Column {
                anchors.left: parent.left; anchors.leftMargin: Theme.horizontalPageMargin
                anchors.right: parent.right; anchors.rightMargin: Theme.horizontalPageMargin
                spacing: Theme.paddingSmall

                Label {
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: Theme.highlightColor
                    font.pixelSize: Theme.fontSizeHuge
                    text: qsTr("Twablet")
                }

                Label {
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: Theme.highlightColor
                    wrapMode: Text.WordWrap
                    text: qsTr("By Lucien Xu")
                }

                Label {
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: Theme.secondaryHighlightColor
                    wrapMode: Text.WordWrap
                    text: qsTr("Version %1").arg(INFO.version)
                }
            }

            Label {
                wrapMode: Text.WordWrap
                anchors.left: parent.left; anchors.leftMargin: Theme.horizontalPageMargin
                anchors.right: parent.right; anchors.rightMargin: Theme.horizontalPageMargin
                font.pixelSize: Theme.fontSizeSmall
                text: qsTr("Twablet is a Twitter client designed for bigger screens. It is an Open Source software, meaning that it can be modified and enhanced by anybody. If you like Twablet, please consider a donation. It will help improving the software, while keeping it free and Open Source.")
            }
        }

        Column {
            id: buttonsColumn
            anchors.top: mainColumn.bottom; anchors.topMargin: Theme.paddingLarge
            anchors.left: parent.left; anchors.right: parent.right
            MiniButton {
                anchors.left: parent.left; anchors.right: parent.right
                text: qsTr("Donate via Paypal")
                onClicked: Qt.openUrlExternally(INFO.paypal)
            }
            MiniButton {
                anchors.left: parent.left; anchors.right: parent.right
                text: qsTr("Flattr me")
                onClicked: Qt.openUrlExternally(INFO.flattr)
            }
            MiniButton {
                AccountSelectionModel {
                    id: accountSelectionModel
                    repository: Repository
                    Component.onCompleted: {
                        accountSelectionModel.index = 0
                    }
                }

                anchors.left: parent.left; anchors.right: parent.right
                text: qsTr("Follow me on Twitter")
                onClicked: {
                    container.rightPanel.openUser("361931860", accountSelectionModel.selection, Info.Replace)
                }
            }
        }

        VerticalScrollDecorator {}
    }
}
