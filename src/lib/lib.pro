TEMPLATE = lib
TARGET = harbour-twablet

CONFIG += static

include(../config.pri)

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
    twitteruser.h \
    twitteruserrepository.h \
    qml/twitterdatarepositoryobject.h \
    qml/twitteruserobject.h \
    qml/twitterusermodel.h \
    twitterquery.h \
    qml/twitterqueryobject.h \
    layout.h \
    layoutrepository.h \
    qml/layoutobject.h \
    qml/layoutmodel.h \
    qml/twitterquerylistmodel.h \
    qml/imodel.h \
    qml/model.h \
    qml/twitterdatarepositoryobjectrepository.h \
    twittertweet.h \
    twittertweetcentralrepository.h \
    itwitterqueryhandler.h \
    twittertimelinequeryhandler.h \
    twittertweetrepository.h \
    qml/twittertweetobject.h \
    qml/twittertweetmodel.h

SOURCES += \
    private/twitterdatautil.cpp \
    private/twitterqueryutil.cpp \
    qml/twitterauthentification.cpp \
    loadsavemanager.cpp \
    twitteruser.cpp \
    twitteruserrepository.cpp \
    qml/twitterdatarepositoryobject.cpp \
    qml/twitteruserobject.cpp \
    qml/twitterusermodel.cpp \
    twitterquery.cpp \
    qml/twitterqueryobject.cpp \
    layout.cpp \
    layoutrepository.cpp \
    qml/layoutobject.cpp \
    qml/layoutmodel.cpp \
    qml/twitterquerylistmodel.cpp \
    twittertweet.cpp \
    twittertweetcentralrepository.cpp \
    twittertimelinequeryhandler.cpp \
    qml/twittertweetmodel.cpp \
    qml/twittertweetobject.cpp
