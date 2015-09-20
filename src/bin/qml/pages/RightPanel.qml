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

DockedPanel {
    id: container
    function openUser(id, account) {
        _open(Qt.resolvedUrl("UserPage.qml"), {userId: id, account: account})
    }
    function _open(page, args) {
        if (Screen.sizeCategory === Screen.Large) {
            container.open = true
            panelPageStack.clear()
            panelPageStack.push(page, args)
        } else {
            pageStack.push(page, args)
        }
    }

    dock: Dock.Right
    visible: false
    onVisibleChanged: console.debug(visible)

    PageStack {
        id: panelPageStack
        anchors.fill: parent
    }

    states: [
        State {
            name: "opened"; when: container.open
        }
    ]
    transitions: [
        Transition {
            from: ""
            to: "opened"
            PropertyAction {
                target: container
                property: "visible"
                value: true
            }
        },
        Transition {
            from: "opened"
            to: ""
            SequentialAnimation {
                PauseAnimation { duration: 500 }
                PropertyAction {
                    target: container
                    property: "visible"
                    value: false
                }
            }
        }
    ]
}
