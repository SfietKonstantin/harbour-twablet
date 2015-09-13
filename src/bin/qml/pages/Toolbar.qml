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
    property int lastTappedIndex: -1
    property alias currentIndex: view.currentIndex
    property int columnCount
    property ListView mainView
    anchors.left: parent.left; anchors.right: parent.right
    anchors.bottom: parent.bottom
    height: layoutModel.count > 0 ? Theme.itemSizeSmall : 0
    gradient: Gradient {
        GradientStop { position: 0; color: Theme.rgba(Theme.secondaryHighlightColor, 0.2) }
        GradientStop { position: 1; color: "transparent" }
    }


    SilicaListView {
        id: view
        property int capacity: Math.floor(view.width / Theme.itemSizeMedium)
        property double cellWidth: view.width / capacity === Infinity ? 0 : view.width / capacity
        anchors.fill: parent
        model: LayoutModel { id: layoutModel; repository: Repository }
        orientation: Qt.Horizontal

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
            width: view.cellWidth * container.columnCount
            height: Theme.paddingMedium / 2
            x: (mainView.contentX - mainView.originX) / view.capacity * container.columnCount - (view.contentX - view.originX)
        }
    }

}

