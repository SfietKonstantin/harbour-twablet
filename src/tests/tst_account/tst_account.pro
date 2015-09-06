TEMPLATE = app
TARGET = tst_account

QT = core qml testlib

include(../../config.pri)

INCLUDEPATH += ../../lib
LIBS += -L../../lib -lharbour-twablet

SOURCES += tst_account.cpp
