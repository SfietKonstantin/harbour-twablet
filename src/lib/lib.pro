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
    qml/imodel.h \
    qml/model.h \
    twitteraccount.h \
    twitteraccountrepository.h \
    qml/twitteraccountobject.h \
    qml/twitteraccountmodel.h \
    twitterquery.h \
    qml/twitterqueryobject.h \
    qml/twitterquerylistmodel.h \
    layout.h \
    layoutrepository.h \
    qml/layoutobject.h \
    qml/layoutmodel.h \
    twittertweet.h \
    twittertweetcentralrepository.h \
    twittertweetrepository.h \
    qml/twittertweetobject.h \
    qml/twittertweetmodel.h \
    qml/twitterdatarepositoryobject.h \
    qml/twitterdatarepositoryobjectrepository.h \
    itwitterqueryhandler.h \
    twitterhometimelinequeryhandler.h \
    twittermentionstimelinequeryhandler.h

SOURCES += \
    private/twitterdatautil.cpp \
    private/twitterqueryutil.cpp \
    qml/twitterauthentification.cpp \
    loadsavemanager.cpp \
    twitteraccount.cpp \
    twitteraccountrepository.cpp \
    qml/twitteraccountobject.cpp \
    qml/twitteraccountmodel.cpp \
    twitterquery.cpp \
    qml/twitterqueryobject.cpp \
    qml/twitterquerylistmodel.cpp \
    layout.cpp \
    layoutrepository.cpp \
    qml/layoutobject.cpp \
    qml/layoutmodel.cpp \
    twittertweet.cpp \
    qml/twittertweetobject.cpp \
    qml/twittertweetmodel.cpp \
    twittertweetcentralrepository.cpp \
    qml/twitterdatarepositoryobject.cpp \
    twitterhometimelinequeryhandler.cpp \
    twittermentionstimelinequeryhandler.cpp
