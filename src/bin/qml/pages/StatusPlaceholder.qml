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

Item {
    id: container
    property QtObject model
    property QtObject query
    property string errorMessage
    anchors.fill: parent
    visible: query != null || model != null && model.count === 0 || internal.errorMessageAvailable

    QtObject {
        id: internal
        property bool queryLoading: container.query != null ? container.query.status === QueryItem.Loading : false
        property bool modelLoading: container.model != null ? (container.model.status === Model.Loading && container.model.count === 0) : false
        property bool queryError: container.query != null ? container.query.status === QueryItem.Error : false
        property bool modelError: container.model != null ? (container.model.status === Model.Error && container.model.count === 0) : false
        property bool errorMessageAvailable: container.errorMessage.length > 0

        property bool loading: queryLoading || modelLoading
        property bool error: queryError || modelError || errorMessageAvailable
        property string fullErrorMessage: errorMessageAvailable ? container.errorMessage : errorMessage
        property string errorMessage: container.query != null ? container.query.errorMessage : (container.model != null ? container.model.errorMessage : "")

        function retry() {
            if (container.query != null) {
                container.query.load()
            }
            if (container.model != null) {
                Repository.refresh()
            }
        }
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: internal.loading
        size: BusyIndicatorSize.Large
    }

    Text {
        anchors.centerIn: parent
        width: parent.width
        enabled: internal.error
        text: internal.fullErrorMessage
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.Wrap
        font {
            pixelSize: Theme.fontSizeExtraLarge
            family: Theme.fontFamilyHeading
        }
        color: Theme.highlightColor
        opacity: enabled ? 0.6 : 0.

        Behavior on opacity { FadeAnimation { duration: 300 } }
    }

    Button {
        id: button
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom; anchors.bottomMargin: Theme.paddingMedium
        text: qsTr("Retry")
        onClicked: internal.retry()
        visible: internal.error && !internal.errorMessageAvailable
    }
}

