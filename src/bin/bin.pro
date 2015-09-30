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
    qml/pages/Toolbar.qml \
    qml/pages/TweetDelegate.qml \
    qml/cover/CoverPage.qml \
    qml/pages/TwitterImage.qml \
    qml/pages/StatusPlaceholder.qml \
    qml/pages/StatusHeader.qml \
    qml/pages/ToolbarButton.qml \
    qml/pages/RightPanel.qml \
    qml/pages/UserPage.qml \
    qml/pages/MiniButton.qml \
    qml/pages/UsersPage.qml \
    qml/pages/TweetPage.qml \
    qml/pages/LinkHandler.js


DATA += \
    data/home.svg \
    data/mail.svg \
    data/inbox.svg \
    data/search.svg \
    data/me.svg \
    data/background.png

data.files = $${DATA}
data.path = /usr/share/harbour-twablet/data
INSTALLS += data

CONFIG += sailfishapp_i18n

TRANSLATIONS += translations/harbour-twablet-de.ts
