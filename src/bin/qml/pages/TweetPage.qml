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
    property alias tweetId: query.tweetId
    property string retweetId // Used to mark retweeted
    property alias accountUserId: query.accountUserId
    property RightPanel panel
    function load() { queryItem.load() }

    TweetQueryItem {
        id: queryItem
        repository: Repository
        query: TweetQuery {
            id: query
            type: QueryType.ShowTweet
        }
    }

    TweetQueryItem {
        id: retweetQueryItem
        repository: Repository
        query: TweetQuery {
            type: QueryType.Retweet
            accountUserId: query.accountUserId
            tweetId: query.tweetId
        }
        onFinished: {
            Repository.setTweetRetweeted(container.tweetId)
            if (container.retweetId !== container.tweetId) {
                Repository.setTweetRetweeted(container.retweetId)
            }
            queryItem.setRetweeted(true)
        }
    }

    TweetQueryItem {
        id: favoriteQueryItem
        repository: Repository
        query: TweetQuery {
            type: queryItem.item && queryItem.item.favorited ? QueryType.Unfavorite : QueryType.Favorite
            accountUserId: query.accountUserId
            tweetId: query.tweetId
        }
        onFinished: {
            if (!item) {
                return
            }

            var favorited = item.favorited
            Repository.setTweetFavorited(container.tweetId, favorited)
            if (container.retweetId !== container.tweetId) {
                Repository.setTweetFavorited(container.retweetId, favorited)
            }
            queryItem.setFavorited(favorited)
        }
    }

    SilicaFlickable {
        anchors.fill: parent
        clip: true
        contentHeight: Math.max(column.height, container.height)

        Column {
            id: column
            opacity: queryItem.status === QueryItem.Idle && queryItem.item ? 1 : 0
            anchors.left: parent.left; anchors.right: parent.right

            PageHeader {
                title: qsTr("Tweet")
            }

            TweetDelegate {
                enabled: false
                anchors.left: parent.left; anchors.right: parent.right
                onOpenTweet: {
                    panel.openTweet(originalId, id, container.accountUserId)
                }
                tweet: queryItem.item
                onHandleLink: LH.handleLink(url, container.panel, container.accountUserId, false)
                onHandleOpenImageBrowser: panel.openImageBrowser(tweet, container.accountUserId)
                fontSize: Theme.fontSizeSmall
                fontSizeSmall: Theme.fontSizeExtraSmall
                itemSize: Theme.itemSizeSmall
            }

            Row {
               IconButton {
                   icon.source: "image://theme/icon-s-chat"
                   onClicked: replyStatusUpdater.focus()
               }
               ProgressIconButton {
                   source: "image://theme/icon-s-retweet"
                   enabled: queryItem.item ? !queryItem.item.retweeted && container.accountUserId !== queryItem.item.user.id: false
                   highlighted: down || (queryItem.item ? queryItem.item.retweeted : false)
                   busy: retweetQueryItem.status === QueryItem.Loading
                   error: retweetQueryItem.status === QueryItem.Error
                   onClicked: retweetQueryItem.load()
               }
               ProgressIconButton {
                   source: "image://theme/icon-s-favorite"
                   highlighted: down || (queryItem.item ? queryItem.item.favorited : false)
                   busy: favoriteQueryItem.status === QueryItem.Loading
                   error: favoriteQueryItem.status === QueryItem.Error
                   onClicked: favoriteQueryItem.load()
               }
            }

            StatusUpdater {
                id: replyStatusUpdater
                replyOnly: true
                inReplyTo: queryItem.item ? queryItem.item.id : ""
                inReplyScreenName: queryItem.item ? "@" + queryItem.item.user.screenName : ""
                anchors.left: parent.left; anchors.right: parent.right
            }

            Behavior on opacity {
                NumberAnimation { duration: 200 }
            }
        }

        StatusPlaceholder {
            query: query
        }

        PullDownMenu {
            MenuItem {
                text: qsTr("Open in browser")
                enabled: queryItem.item
                onClicked: Qt.openUrlExternally("https://twitter.com/" + queryItem.item.user.screenName + "/status/" + queryItem.item.id)
            }
        }

        VerticalScrollDecorator {}
    }
}

