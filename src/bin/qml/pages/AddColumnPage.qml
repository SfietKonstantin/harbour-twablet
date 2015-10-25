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
    property AccountModel accountModel

    canAccept: nameField.text.length > 0 && searchQuery.isSearchOk
    allowedOrientations: app.defaultAllowedOrientations

    onAccepted: {
        var accountIndex = accountCombo.currentIndex
        var queryType = queryListModel.getType(queryCombo.currentIndex)

        var args = new Object
        if (searchQuery.isSearch) {
            args.q = searchQuery.text.trim()
            args.result_type = resultTypeModel.get(resultTypeCombo.currentIndex).value
        }
        Repository.addLayout(nameField.text, accountIndex, queryType, args)
    }

    SilicaFlickable {
        id: flickable
        anchors.fill: parent

        Column {
            anchors.left: parent.left; anchors.right: parent.right
            DialogHeader {
                id: header
                acceptText: qsTr("Add tab")
            }

            TextField {
                id: nameField
                anchors.left: parent.left; anchors.right: parent.right
                placeholderText: qsTr("Name of the column")
                label: qsTr("Name of the column")
            }

            ComboBox {
                id: accountCombo
                label: qsTr("Account")
                menu: ContextMenu {
                    Repeater {
                        model: container.accountModel
                        delegate: MenuItem {
                            text: model.name
                        }
                    }
                }
            }

            ComboBox {
                id: queryCombo
                label: qsTr("Column type")
                menu: ContextMenu {
                    Repeater {
                        model: QueryListModel {id: queryListModel}
                        delegate: MenuItem {
                            text: model.name
                        }
                    }
                }
            }

            TextField {
                id: searchQuery
                property bool isSearch: (queryListModel.getType(queryCombo.currentIndex) === QueryType.Search)
                property bool isSearchOk: (isSearch && searchQuery.text.length > 0 || !isSearch)
                anchors.left: parent.left; anchors.right: parent.right
                visible: isSearch
                placeholderText: qsTr("Search query")
                label: qsTr("Search query")
            }

            ComboBox {
                id: resultTypeCombo
                visible: searchQuery.isSearch
                label: qsTr("Search result type")
                menu: ContextMenu {
                    Repeater {
                        model: ListModel {
                            id: resultTypeModel
                            ListElement {
                                text: QT_TR_NOOP("Recent")
                                value: "recent"
                            }
                            ListElement {
                                text: QT_TR_NOOP("Popular")
                                value: "popular"
                            }
                            ListElement {
                                text: QT_TR_NOOP("Mixed")
                                value: "mixed"
                            }
                        }

                        delegate: MenuItem {
                            text: qsTr(model.text)
                        }
                    }
                }
            }
        }
    }
}
