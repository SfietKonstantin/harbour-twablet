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
    property bool initial: false
    canAccept: false
    allowedOrientations: app.defaultAllowedOrientations
    Component.onCompleted: state = "initial"
    function load() {
        authentification.startRequest()
    }

    TwitterAuthentification {
        id: authentification
        pin: pinField.text
        onSendAuthorize: {
            container.state = "startRequestDone"
            webView.url = url
        }
        onError: {
            container.state = "error"
            error.text = errorMessage
        }
        onDone: {
            var args = {
                userId: authentification.userId,
                screenName: authentification.screenName,
                token: authentification.token,
                tokenSecret: authentification.tokenSecret
            }
            if (container.initial) {
                args.acceptDestination = Qt.resolvedUrl("MainPage.qml")
                args.acceptDestinationAction = PageStackAction.Replace
                args.acceptDestinationReplaceTarget = null
            }

            pageStack.replace(Qt.resolvedUrl("AccountPage.qml"), args)
        }
    }

    BusyIndicator {
        id: indicator
        anchors.centerIn: parent
        size: BusyIndicatorSize.Large
    }

    SilicaFlickable {
        id: flickable
        anchors.fill: parent

        Column {
            anchors.left: parent.left; anchors.right: parent.right
            DialogHeader {
                id: header
                acceptText: qsTr("Add account")
            }

            Item {
                id: drawer
                height: flickable.height - header.height
                anchors.left: parent.left; anchors.right: parent.right
                function continueAuthentification() {
                    container.state = "continueRequest"
                    authentification.continueRequest()
                }

                Drawer {
                    anchors.fill: parent
                    dock: (container.orientation & Orientation.PortraitMask) ? Dock.Bottom : Dock.Right
                    open: drawer.visible
                    foreground: SilicaWebView {
                        id: webView
                        anchors.fill: parent
                    }
                    background: Column {
                        anchors.top: parent.top
                        anchors.left: parent.left; anchors.leftMargin: Theme.horizontalPageMargin
                        anchors.right: parent.right; anchors.rightMargin: Theme.horizontalPageMargin
                        anchors.topMargin: Theme.paddingLarge
                        spacing: Theme.paddingLarge

                        Label {
                            anchors.left: parent.left; anchors.right: parent.right
                            text: qsTr("Log into Twitter using the web page and enter the PIN code below")
                            wrapMode: Text.WordWrap
                            color: Theme.highlightColor
                        }

                        TextField {
                            id: pinField
                            anchors.left: parent.left; anchors.right: parent.right
                            placeholderText: qsTr("Enter PIN code here")
                            inputMethodHints: Qt.ImhDigitsOnly
                            EnterKey.enabled: text.length > 0
                            EnterKey.iconSource: "icon-m-enter-accept"
                            EnterKey.onClicked: drawer.continueAuthentification()
                        }

                        Button {
                            text: qsTr("Log in")
                            anchors.horizontalCenter: parent.horizontalCenter
                            onClicked: drawer.continueAuthentification()
                            enabled: pinField.text.length > 0
                        }
                    }
                }
            }
        }
        ViewPlaceholder {
            id: error
        }

        Button {
            visible: error.enabled
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom; anchors.bottomMargin: Theme.paddingMedium
            text: qsTr("Retry")
            onClicked: {
                container.state = "initial"
                authentification.startRequest()
            }
        }
    }

    states: [
        State {
            name: "initial"
            PropertyChanges { target: indicator; running: true }
            PropertyChanges { target: drawer; visible: false }
            PropertyChanges { target: error; enabled: false }
        },State {
            name: "error"
            PropertyChanges { target: indicator; running: false }
            PropertyChanges { target: drawer; visible: false }
            PropertyChanges { target: error; enabled: true }
        }, State {
            name: "startRequestDone"
            PropertyChanges { target: indicator; running: false }
            PropertyChanges { target: drawer; visible: true }
            PropertyChanges { target: error; enabled: false }
        }, State {
            name: "continueRequest"
            PropertyChanges { target: indicator; running: true }
            PropertyChanges { target: drawer; visible: false }
            PropertyChanges { target: error; enabled: false }
        }
    ]
}
