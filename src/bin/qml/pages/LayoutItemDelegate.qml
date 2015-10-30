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

SimpleListItem {
    id: container
    property Page page
    property int index
    property alias name: name.text
    signal startMove(int index)
    signal endMove()
    signal dragEntered(int dragIndex, int dropIndex)
    highlighted: down && !content.held
    onPressAndHold: content.held = true
    onReleased: content.held = false
    drag.target: content.held ? content : undefined
    drag.axis: Drag.YAxis
    anchors.left: parent.left; anchors.right: parent.right
    height: content.height

    Item {
        id: content
        property bool held: false
        width: container.width
        height: Theme.itemSizeMedium
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        Drag.active: content.held
        Drag.source: container
        Drag.hotSpot.x: width / 2
        Drag.hotSpot.y: height / 2

        Rectangle {
            anchors.fill: parent
            color: Theme.secondaryHighlightColor
            visible: content.held
        }

        Label {
            id: name
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left; anchors.leftMargin: Theme.horizontalPageMargin
            anchors.right: parent.right; anchors.rightMargin: Theme.horizontalPageMargin
            color: container.pressed ? Theme.highlightColor : Theme.primaryColor
        }

        states: State {
            name: "held"
            when: content.held
            ParentChange { target: content; parent: container.page }
            AnchorChanges {
                target: content
                anchors { horizontalCenter: undefined; verticalCenter: undefined }
            }
        }

        transitions: [
            Transition {
                from: ""
                to: "held"
                ScriptAction {
                    script: {
                        container.startMove(container.index)
                    }
                }
            },
            Transition {
                from: "held"
                to: ""
                ScriptAction {
                    script: {
                        container.endMove()
                    }
                }
            }
        ]
    }

    DropArea {
        anchors.fill: parent
        onEntered: container.dragEntered(drag.source.index, container.index)
    }
}
