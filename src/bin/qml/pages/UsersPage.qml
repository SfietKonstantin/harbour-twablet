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
    property string title
    property int queryType
    property var args
    property QtObject account
    property RightPanel panel

    Component.onCompleted: {
        var layoutIndex = Repository.addUser(container.account, container.queryType, container.args)
        if (layoutIndex >= 0) {
            userModel.layoutIndex = layoutIndex
        }
    }

    Component.onDestruction: {
        Repository.removeUser(userModel.layoutIndex)
    }

    SilicaListView {
        anchors.fill: parent
        model: UserModel {
            id: userModel
            repository: Repository
        }

        header: PageHeader {
            title: container.title
        }

        delegate: BackgroundItem {
            id: delegate
            height: Theme.itemSizeMedium
            anchors.left: parent.left; anchors.right: parent.right
            onClicked: container.panel.openUser(model.id, container.account, false)

            TwitterImage {
                id: profilePicture
                anchors.verticalCenter: parent.verticalCenter
                width: Theme.itemSizeSmall
                height: Theme.itemSizeSmall
                source: model.item.imageUrl
            }

            Column {
                id: column
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: profilePicture.right; anchors.leftMargin: Theme.paddingMedium
                anchors.right: parent.right; anchors.rightMargin: Theme.paddingMedium
                spacing: Theme.paddingSmall

                Label {
                    anchors.left: parent.left; anchors.right: parent.right
                    truncationMode: TruncationMode.Elide
                    text: model.item.name
                    color: delegate.pressed ? Theme.highlightColor : Theme.primaryColor
                }

                Label {
                    anchors.left: parent.left; anchors.right: parent.right
                    truncationMode: TruncationMode.Elide
                    text: "@" + model.item.screenName
                    font.pixelSize: Theme.fontSizeSmall
                    color: delegate.pressed ? Theme.secondaryHighlightColor : Theme.secondaryColor
                }
            }
        }

        footer: LoadMoreButton {
            model: userModel
            onClicked: Repository.userLoadMore(userModel.layoutIndex)
        }

        StatusPlaceholder {
            model: userModel
            errorMessage: (model.status === Model.Idle && model.count === 0) ? qsTr("No friends") : ""
        }

        VerticalScrollDecorator {}
    }
}

