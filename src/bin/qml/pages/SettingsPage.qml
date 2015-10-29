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
    property RightPanel rightPanel
    property bool initial: false
    function removeAccount(index) {
        accounts.remove(index)
    }
    function removeLayout(index) {
        layouts.remove(index)
    }
    allowedOrientations: app.defaultAllowedOrientations

    Connections {
        target: accountModel
        onCountChanged: {
            if (!container.initial && accountModel.count === 0) {
                pageStack.clear()
                pageStack.push(Qt.resolvedUrl("SettingsPage.qml"), {initial: true})
            }
        }
    }

    SilicaFlickable {
        id: view
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column
            anchors.left: parent.left; anchors.right: parent.right

            PageHeader {
                anchors.left: parent.left; anchors.right: parent.right
                title: qsTr("Settings")
            }

            Column {
                id: accounts
                signal remove(int index)
                anchors.left: parent.left; anchors.right: parent.right
                SectionHeader {
                    text: qsTr("Accounts")
                }

                Repeater {
                    model: accountModel
                    delegate: AccountDelegate {
                        id: accountDelegate
                        function remove() {
                            remorseAction(qsTr("Removing account"), function() {
                                Repository.removeAccount(accountDelegate.index)
                            })
                        }
                        index: model.index
                        name: model.name
                        screenName: model.screenName
                        onClicked: {
                            var args = {
                                index: model.index,
                                screenName: model.screenName,
                                initialName: model.name,
                                parentPage: container
                            }
                            pageStack.push(Qt.resolvedUrl("AccountPage.qml"), args)
                        }
                        onRemoved: remove()

                        Connections {
                            target: accounts
                            onRemove: {
                                if (model.index === index) {
                                    accountDelegate.remove()
                                }
                            }
                        }
                    }
                }

                MiniButton {
                    height: Theme.itemSizeMedium
                    text: qsTr("Add account")
                    onClicked: {
                        var args = {
                            initial: container.initial
                        }
                        var page = pageStack.push(Qt.resolvedUrl("AuthentificationPage.qml"), args)
                        page.load()
                    }
                }
            }

            Column {
                id: layouts
                signal remove(int index)
                property int from: -1
                property int to: -1
                anchors.left: parent.left; anchors.right: parent.right
                move: Transition {
                    NumberAnimation {
                        properties: "x,y"
                        duration: 300
                        easing.type: Easing.OutQuad
                    }
                }

                SectionHeader {
                    text: qsTr("Columns")
                    visible: !container.initial || accountModel.count > 0
                }

                Repeater {
                    model: LayoutModel {
                        id: layoutModel
                        repository: Repository
                    }
                    delegate: LayoutItemDelegate {
                        id: layoutItemDelegate
                        function remove() {
                            layoutItemDelegate.remorseAction(qsTr("Removing column"), function() {
                                Repository.removeLayout(model.index)
                            })
                        }
                        page: container
                        index: model.index
                        name: model.name
                        onClicked: {
                            var args = {
                                index: model.index,
                                initialName: model.name,
                                initialAccountUserId: model.layout.accountUserId,
                                initialType: model.queryType,
                                initialParameters: model.layout.parameters,
                                parentPage: container
                            }
                            pageStack.push(Qt.resolvedUrl("AddColumnPage.qml"), args)
                        }
                        onStartMove: {
                            layoutModel.startMove()
                            layouts.from = index
                        }
                        onEndMove: {
                            Repository.moveLayout(layouts.from, layouts.to)
                            layoutModel.endMove()
                            layouts.from = -1
                            layouts.to = -1
                        }
                        onDragEntered: {
                            layoutModel.move(dragIndex, dropIndex)
                            layouts.to = dropIndex
                        }

                        Connections {
                            target: layouts
                            onRemove: {
                                if (model.index === index) {
                                    layoutItemDelegate.remove()
                                }
                            }
                        }
                    }
                }

                MiniButton {
                    height: Theme.itemSizeMedium
                    text: qsTr("Add column")
                    onClicked: pageStack.push(Qt.resolvedUrl("AddColumnPage.qml"))

                    DropArea {
                        anchors.fill: parent
                        onEntered: {
                            layoutModel.move(drag.source.index, layoutModel.count)
                            layouts.to = layoutModel.count
                        }
                    }
                }
            }

            SectionHeader {
                text: qsTr("Credits")
                visible: !container.initial
            }

            MiniButton {
                visible: !container.initial
                anchors.left: parent.left; anchors.right: parent.right
                text: qsTr("About Twablet")
                onClicked: pageStack.push(Qt.resolvedUrl("AboutPage.qml"), {rightPanel: container.rightPanel})
            }
        }

        VerticalScrollDecorator {}
    }

}

