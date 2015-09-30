TEMPLATE = lib
TARGET = harbour-twablet

CONFIG += static

include(../config.pri)

CONFIG(use_mock_server):{
DEFINES += USE_MOCK_SERVER
}

QT = core network

HEADERS += \
    globals.h \
    qobjectutils.h \
    private/twitterdatautil.h \
    private/twitterqueryutil.h \
    qml/twitterauthentification.h \
    repository.h \
    ilistener.h \
    iloadsave.h \
    loadsavemanager.h \
    qml/imodel.h \
    qml/model.h \
    account.h \
    accountrepository.h \
    qml/accountobject.h \
    qml/accountmodel.h \
    query.h \
    qml/queryobject.h \
    qml/querylistmodel.h \
    layout.h \
    layoutrepository.h \
    qml/layoutobject.h \
    qml/layoutmodel.h \
    tweet.h \
    tweetcentralrepository.h \
    tweetrepository.h \
    qml/tweetobject.h \
    qml/tweetmodel.h \
    qml/datarepositoryobject.h \
    qml/datarepositoryobjectmap.h \
    iqueryhandler.h \
    hometimelinequeryhandler.h \
    mentionstimelinequeryhandler.h \
    user.h \
    qml/userobject.h \
    mediaentity.h \
    qml/mediaobject.h \
    qml/mediamodel.h \
    qml/abstractqueryitem.h \
    qml/statusupdatequeryitem.h \
    qml/accountselectionmodel.h \
    qml/userqueryitem.h \
    private/timeutil.h \
    qml/descriptionformatter.h \
    entity.h \
    urlentity.h \
    qml/entitiesformatter.h \
    usermentionentity.h \
    qml/tweetformatter.h \
    searchqueryhandler.h \
    hashtagentity.h \
    favoritesqueryhandler.h \
    usertimelinequeryhandler.h \
    abstracttweetqueryhandler.h \
    userrepository.h \
    qml/usermodel.h \
    usercentralrepository.h \
    abstractuserqueryhandler.h \
    friendsqueryhandler.h \
    followersqueryhandler.h \
    qml/tweetqueryitem.h

SOURCES += \
    private/twitterdatautil.cpp \
    private/twitterqueryutil.cpp \
    qml/twitterauthentification.cpp \
    loadsavemanager.cpp \
    account.cpp \
    accountrepository.cpp \
    qml/accountobject.cpp \
    qml/accountmodel.cpp \
    query.cpp \
    qml/queryobject.cpp \
    qml/querylistmodel.cpp \
    layout.cpp \
    layoutrepository.cpp \
    qml/layoutobject.cpp \
    qml/layoutmodel.cpp \
    tweet.cpp \
    qml/tweetobject.cpp \
    qml/tweetmodel.cpp \
    tweetcentralrepository.cpp \
    qml/datarepositoryobject.cpp \
    hometimelinequeryhandler.cpp \
    mentionstimelinequeryhandler.cpp \
    user.cpp \
    qml/userobject.cpp \
    mediaentity.cpp \
    qml/mediaobject.cpp \
    qml/mediamodel.cpp \
    qml/abstractqueryitem.cpp \
    qml/statusupdatequeryitem.cpp \
    qml/accountselectionmodel.cpp \
    qml/userqueryitem.cpp \
    qml/descriptionformatter.cpp \
    urlentity.cpp \
    entity.cpp \
    qml/entitiesformatter.cpp \
    usermentionentity.cpp \
    qml/tweetformatter.cpp \
    searchqueryhandler.cpp \
    hashtagentity.cpp \
    favoritesqueryhandler.cpp \
    usertimelinequeryhandler.cpp \
    abstracttweetqueryhandler.cpp \
    qml/usermodel.cpp \
    usercentralrepository.cpp \
    abstractuserqueryhandler.cpp \
    friendsqueryhandler.cpp \
    followersqueryhandler.cpp \
    qml/tweetqueryitem.cpp
