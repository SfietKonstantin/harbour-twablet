TEMPLATE = app
TARGET = tst_layout

QT = core qml testlib

include(../../config.pri)

INCLUDEPATH += ../../lib
LIBS += -L../../lib -lharbour-twablet

SOURCES += tst_layout.cpp
