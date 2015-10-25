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
    property alias userId: query.userId
    property alias accountUserId: query.accountUserId
    property RightPanel panel
    function load() { queryItem.load() }

    UserQueryItem {
        id: queryItem
        repository: Repository
        query: UserQuery {
            id: query
            type: QueryType.ShowUser
        }
    }

    UserQueryItem {
        id: followQueryItem
        repository: Repository
        query: UserQuery {
            type: queryItem.item && queryItem.item.following ? QueryType.Unfollow : QueryType.Follow
            accountUserId: query.accountUserId
            userId: query.userId
        }
        onFinished: {
            if (!item) {
                return
            }

            var following = item.following
            queryItem.setFollowing(following)
        }
    }

    SilicaFlickable {
        anchors.fill: parent
        clip: true
        contentHeight: column.height

        Column {
            id: column
            visible: queryItem.status === QueryItem.Idle
            anchors.left: parent.left; anchors.right: parent.right

            TwitterImage {
                id: image
                anchors.left: parent.left; anchors.right: parent.right
                height: 3 * Theme.itemSizeLarge
                source: queryItem.item ? (Screen.sizeCategory === Screen.Large ? queryItem.item.bannerUrlLarge : queryItem.item.bannerUrl) : ""

                ShaderEffect {
                    id: gradient
                    property variant source: ShaderEffectSource {
                        hideSource: true
                        sourceItem: image.image
                    }

                    property real _boundary: 1 - (Theme.paddingLarge + Theme.fontSizeLarge + Theme.paddingMedium) / height;
                    anchors.fill: image

                    fragmentShader: "
                    varying highp vec2 qt_TexCoord0;
                    uniform float qt_Opacity;
                    uniform float _boundary;
                    uniform sampler2D source;
                    void main(void)
                    {
                        lowp vec4 textureColor = texture2D(source, qt_TexCoord0.st);
                        gl_FragColor = (1. - smoothstep(0., _boundary, qt_TexCoord0.y)) * textureColor * qt_Opacity;
                    }
                    "
                }

                TwitterImage {
                    id: profilePicture
                    anchors.top: parent.top; anchors.topMargin: Theme.paddingMedium
                    anchors.right: parent.right; anchors.rightMargin: Theme.paddingMedium
                    width: Theme.itemSizeMedium
                    height: Theme.itemSizeMedium
                    source: queryItem.item ? (Screen.sizeCategory === Screen.Large ? queryItem.item.imageUrlLarge : queryItem.item.imageUrl) : ""
                }

                Image {
                    anchors.verticalCenter: profilePicture.verticalCenter
                    anchors.right: profilePicture.left; anchors.rightMargin: Theme.paddingMedium
                    source: "image://theme/icon-s-installed?" + Theme.highlightColor
                    visible: queryItem.item && queryItem.item.following
                }

                Label {
                    anchors.right: parent.right; anchors.rightMargin: Theme.paddingMedium
                    anchors.bottom: name.top; anchors.bottomMargin: Theme.paddingSmall
                    font.pixelSize: Theme.fontSizeMedium
                    color: Theme.highlightColor
                    text: queryItem.item ? "@" + queryItem.item.screenName : ""
                }

                Label {
                    id: name
                    anchors.right: parent.right; anchors.rightMargin: Theme.paddingMedium
                    anchors.bottom: parent.bottom; anchors.bottomMargin: Theme.paddingMedium
                    font.pixelSize: Theme.fontSizeLarge
                    color: Theme.highlightColor
                    text: queryItem.item ? queryItem.item.name : ""
                }
            }

            Item {
                anchors.left: parent.left; anchors.right: parent.right
                height: description.height + 2 * Theme.paddingMedium

                DescriptionFormatter {
                    id: descriptionFormatter
                    user: queryItem.item
                }

                Label {
                    id: description
                    anchors.left: parent.left; anchors.leftMargin: Theme.paddingMedium
                    anchors.right: parent.right; anchors.rightMargin: Theme.paddingMedium
                    anchors.verticalCenter: parent.verticalCenter
                    wrapMode: Text.Wrap
                    color: Theme.highlightColor
                    linkColor: Theme.primaryColor
                    text: descriptionFormatter.text
                    textFormat: Text.StyledText
                    onLinkActivated: {
                        LH.handleLink(link, container.panel, container.accountUserId, false)
                    }
                }
            }

            Row {
                anchors.left: parent.left; anchors.right: parent.right
                MiniButton {
                    width: parent.width / 3
                    text: queryItem.item ? qsTr("%n\ntweets", "", queryItem.item.statusesCount) : ""
                    onClicked: container.panel.openUserTimeline(container.userId, queryItem.item.screenName,
                                                                container.accountUserId, false)
                }
                MiniButton {
                    width: parent.width / 3
                    text: queryItem.item ? qsTr("%n\nfollowing", "", queryItem.item.friendsCount) : ""
                    onClicked: container.panel.openFriends(container.userId, queryItem.item.screenName,
                                                           container.accountUserId, false)
                }
                MiniButton {
                    width: parent.width / 3
                    text: queryItem.item ? qsTr("%n\nfollowers", "", queryItem.item.followersCount) : ""
                    onClicked: container.panel.openFollowers(container.userId, queryItem.item.screenName,
                                                             container.accountUserId, false)
                }
            }

            Item {
                anchors.left: parent.left; anchors.right: parent.right
                height: Theme.itemSizeMedium

                Label {
                    anchors.left: parent.left; anchors.leftMargin: Theme.paddingMedium
                    anchors.right: parent.right; anchors.rightMargin: Theme.paddingMedium
                    anchors.verticalCenter: parent.verticalCenter
                    truncationMode: TruncationMode.Fade
                    color: Theme.highlightColor
                    text: queryItem.item ? qsTr("%n tweets per day", "", queryItem.item.tweetsPerDay) : ""
                }
            }

            MiniButton {
                visible: queryItem.item && queryItem.item.displayUrl !== ""
                text: queryItem.item ? queryItem.item.displayUrl : ""
                onClicked: Qt.openUrlExternally(queryItem.item.url)
            }

            MiniButton {
                text: queryItem.item ? qsTr("%n favourites", "", queryItem.item.favouritesCount) : ""
                onClicked: container.panel.openFavorites(container.userId, queryItem.item.screenName,
                                                         container.accountUserId, false)
            }

            MiniButton {
                text: qsTr("Subscribed lists")
            }

            MiniButton {
                text: queryItem.item ? qsTr("Listed in %n lists", "", queryItem.item.listedCount) : ""
            }
        }

        PullDownMenu {
            visible: queryItem.status === QueryItem.Idle
            MenuItem {
                text: queryItem.item ? (queryItem.item.following ? qsTr("Unfollow @%1").arg(queryItem.item.screenName)
                                                                 : qsTr("Follow @%1").arg(queryItem.item.screenName))
                                     : ""
                onClicked: followQueryItem.load()
                enabled: followQueryItem.status === QueryItem.Idle
            }
        }

        StatusPlaceholder {
            query: queryItem
        }

        VerticalScrollDecorator {}
    }

}

