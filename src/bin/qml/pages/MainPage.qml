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
import "LinkHandler.js" as LH

Page {
    id: container
    allowedOrientations: app.defaultAllowedOrientations

    onStatusChanged: {
        if (container.status === PageStatus.Active) {
            if (accountModel.count === 0) {
                pageStack.replace(Qt.resolvedUrl("AccountsPage.qml"), {initial: true})
            }
        }
    }

    onOrientationTransitionRunningChanged: {
        if (orientationTransitionRunning) {
            if (panel.open) {
                internal.panelOpenDuringTransition = true
                panel.hide(true)

            }
        } else {
            if (internal.panelOpenDuringTransition) {
                internal.panelOpenDuringTransition = false
                panel.show(true)
            }
        }
    }

    QtObject {
        id: internal
        property bool panelOpenDuringTransition: false
    }

    LayoutModel { id: layoutModel; repository: Repository }

    Component.onCompleted: {
        if (layoutModel.count > 0) {
            Repository.refresh()
        }
    }

    SilicaFlickable {
        id: main
        anchors.fill: parent
        visible: accountModel.count > 0
        contentWidth: width
        contentHeight: height
        pressDelay: 100
        interactive: !panel.open

        PushUpMenu {
            visible: !panel.open
            MenuItem {
                text: qsTr("Refresh")
                enabled: layoutModel.count > 0
                onClicked: Repository.refresh()
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

        Button {
            anchors.centerIn: parent
            visible: layoutModel.count === 0
            text: qsTr("Add column")
            onClicked: pageStack.push(Qt.resolvedUrl("AddColumnPage.qml"), {accountModel: accountModel})
        }

        Item {
            width: main.width
            height: main.height
            visible: layoutModel.count > 0

            SilicaListView {
                id: view
                function animateToIndex(index) {
                    animation.stop()
                    var currentX = view.contentX
                    view.positionViewAtIndex(index, ListView.Beginning)
                    var destX = view.contentX
                    animation.from = currentX
                    animation.to = destX
                    animation.start()
                }

                property int columnCount: container.isLandscape ? (Screen.sizeCategory === Screen.Large ? 3 : 2) : (Screen.sizeCategory === Screen.Large ? 2 : 1)
                property real columnWidth: container.width / columnCount
                anchors.left: parent.left; anchors.right: panel.visible ? panel.left : parent.right
                anchors.top: parent.top; anchors.bottom: toolbar.top
                onContentXChanged: {
                    toolbar.lastTappedIndex = -1
                    toolbar.currentIndex = indexAt(view.contentX, view.height / 2)
                }

                clip: true
                cacheBuffer: columnWidth * layoutModel.count
                orientation: Qt.Horizontal
                model: layoutModel
                snapMode: view.columnCount > 1 ? ListView.NoSnap : ListView.SnapOneItem
                delegate: ColumnLayout {
                    id: delegate
                    width: view.columnWidth
                    height: view.height
                    layoutIndex: index
                    title: name
                    onHandleLink: {
                        var account = accountModel.get(layout.userId)
                        if (account !== null) {
                            LH.handleLink(url, panel, account, true)
                        }
                    }
                    onOpenTweet: {
                        var account = accountModel.get(layout.userId)
                        if (account !== null) {
                            panel.openTweet(tweetId, retweetId, account, true)
                        }
                    }

                    Connections {
                        target: toolbar
                        onGoToTop: {
                            if (delegate.layoutIndex == index) {
                                delegate.scrollToTop()
                                delegate.setUnread(0)
                            }
                        }
                    }
                }
                footer: Item {
                    visible: layoutModel.count > 0
                    width: view.columnWidth
                    height: view.height

                    Button {
                        anchors.centerIn: parent
                        text: qsTr("Add column")
                        onClicked: pageStack.push(Qt.resolvedUrl("AddColumnPage.qml"), {accountModel: accountModel})
                    }
                }

                remove: Transition { FadeAnimation {} }

                NumberAnimation {
                    id: animation
                    target: view
                    property: "contentX"
                    duration: 200
                    easing.type: Easing.OutQuad
                }
            }
            Toolbar {
                id: toolbar
                columnCount: view.columnCount
                mainView: view
                onGoToIndex: view.animateToIndex(index)
            }
            RightPanel {
                id: panel
                anchors.top: parent.top; anchors.bottom: toolbar.top
                width: view.columnWidth
            }
        }
    }
}
