CONFIG += c++11
DEFINES += QT_NO_CAST_FROM_ASCII
CONFIG(testing) {
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS_DEBUG += -lgcov -coverage
}
