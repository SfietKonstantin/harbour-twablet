TEMPLATE = app
TARGET = tst_twitteruser

QT = core qml testlib

include(../../config.pri)

INCLUDEPATH += ../../lib
LIBS += -L../../lib -lharbour-twablet

SOURCES += tst_twitteruser.cpp
