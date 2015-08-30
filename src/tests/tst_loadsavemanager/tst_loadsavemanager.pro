TEMPLATE = app
TARGET = tst_loadsavemanager

QT = core testlib

include(../../config.pri)

INCLUDEPATH += ../../lib
LIBS += -L../../lib -lharbour-twablet

SOURCES += tst_loadsavemanager.cpp
