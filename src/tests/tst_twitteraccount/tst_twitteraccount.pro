TEMPLATE = app
TARGET = tst_twitteraccount

QT = core qml testlib

include(../../config.pri)

INCLUDEPATH += ../../lib
LIBS += -L../../lib -lharbour-twablet

SOURCES += tst_twitteraccount.cpp
