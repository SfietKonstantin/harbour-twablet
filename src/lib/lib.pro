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
    twittertweet.h \
    twittertweetcentralrepository.h \
    twittertweetrepository.h \
    qml/twittertweetobject.h \
    qml/twittertweetmodel.h \
    qml/twitterdatarepositoryobject.h \
    qml/twitterdatarepositoryobjectrepository.h \
    iqueryhandler.h \
    twitterhometimelinequeryhandler.h \
    twittermentionstimelinequeryhandler.h

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
    twittertweet.cpp \
    qml/twittertweetobject.cpp \
    qml/twittertweetmodel.cpp \
    twittertweetcentralrepository.cpp \
    qml/twitterdatarepositoryobject.cpp \
    twitterhometimelinequeryhandler.cpp \
    twittermentionstimelinequeryhandler.cpp
