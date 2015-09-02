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

Page {
    id: container
    allowedOrientations: app.defaultAllowedOrientations

    TwitterUserModel { id: userModel; repository: Repository }
    LayoutModel { id: layoutModel; repository: Repository }

    Component.onCompleted: {
        if (layoutModel.count > 0) {
            Repository.refresh()
        }
    }

    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {
            visible: layoutModel.count === 0
            MenuItem {
                text: qsTr("About")
                enabled: false
            }

            MenuItem {
                text: qsTr("Accounts")
                onClicked: pageStack.push(Qt.resolvedUrl("AccountsPage.qml"))
            }
        }

        PushUpMenu {
            visible: layoutModel.count > 0
            MenuItem {
                text: qsTr("Refresh")
                onClicked: Repository.refresh()
            }

            MenuItem {
                text: qsTr("New tweet")
                enabled: false
            }

            MenuItem {
                text: qsTr("Accounts")
                onClicked: pageStack.push(Qt.resolvedUrl("AccountsPage.qml"))
            }

            MenuItem {
                text: qsTr("About")
                enabled: false
            }
        }

        ViewPlaceholder {
            enabled: userModel.count === 0
            text: qsTr("There is no account defined. Pull down to configure accounts.")
        }

        ViewPlaceholder {
            enabled: userModel.count > 0 && layoutModel.count === 0
            text: qsTr("There is no column defined. Pull down to add a column.")
        }

        Item {
            width: parent.width
            height: parent.height

            SilicaListView {
                id: view
                property real columnWidth: container.isLandscape ? (Screen.sizeCategory === Screen.Large ? view.width / 3 : view.width / 2) : view.width
                anchors.left: parent.left; anchors.right: parent.right
                anchors.top: parent.top; anchors.bottom: toolbar.top

                clip: true
                cacheBuffer: columnWidth * layoutModel.count
                orientation: Qt.Horizontal
                model: layoutModel
                snapMode: container.isLandscape ? ListView.NoSnap : ListView.SnapOneItem
                delegate: ColumnLayout {
                    width: view.columnWidth
                    height: view.height
                    layoutIndex: index
                    title: name
                }
                footer: Item {
                    visible: layoutModel.count > 0
                    width: view.columnWidth
                    height: view.height

                    Button {
                        anchors.centerIn: parent
                        text: qsTr("Add column")
                        onClicked: pageStack.push(Qt.resolvedUrl("AddColumnPage.qml"), {userModel: userModel})
                    }
                }

                HorizontalScrollDecorator {}

            }
            Rectangle {
                id: toolbar
                anchors.left: parent.left; anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: layoutModel.count > 0 ? Theme.itemSizeSmall : 0
                gradient: Gradient {
                    GradientStop { position: 0; color: Theme.rgba(Theme.secondaryHighlightColor, 0.2) }
                    GradientStop { position: 1; color: "transparent" }
                }
            }
        }
    }
}


