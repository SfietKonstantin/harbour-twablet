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
    qml/pages/MainPage.qml \
    qml/pages/AccountsPage.qml \
    qml/pages/AuthentificationPage.qml \
    qml/pages/ColumnLayout.qml \
    qml/pages/AddColumnPage.qml \
    qml/cover/CoverPage.qml

CONFIG += sailfishapp_i18n

TRANSLATIONS += translations/harbour-twablet-de.ts
