TEMPLATE = app
TARGET = harbour-twablet

QT = core gui quick

include(../config.pri)

CONFIG(desktop) {
    DEFINES += DESKTOP
    RESOURCES += res.qrc
} else {
    CONFIG += sailfishapp
}

INCLUDEPATH += ../lib
LIBS += -L../lib -lharbour-twablet

SOURCES += main.cpp

OTHER_FILES += \
    qml/harbour-twablet.qml \
    pages/MainPage.qml \
    pages/AccountsPage.qml \
    pages/AuthentificationPage.qml \
    pages/ColumnLayout.qml \
    pages/AddColumnPage.qml \
    cover/CoverPage.qml

CONFIG += sailfishapp_i18n

TRANSLATIONS += translations/harbour-twablet-de.ts
