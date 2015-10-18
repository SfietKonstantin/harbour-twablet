project(twablet-tests)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_AUTOMOC TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(GTest REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Qml REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(Threads)
find_program(LCOV_PATH lcov)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${QT_INCLUDES}
    ${GTEST_INCLUDE_DIRS}
    ${twablet_INCLUDE_DIRS}
)

# Compiling gmock from source
set(gmock_SRCS
    /usr/src/gmock/gmock-all.cc
)

set(${PROJECT_NAME}_SRCS
    testrepositorylistener.h
    tst_main.cpp
    tst_account.cpp
    tst_layout.cpp
    tst_loadsavemanager.cpp
    mockqueryexecutor.h
    tst_tweetrepository.cpp
    tst_query.cpp
)

add_executable(${PROJECT_NAME}
    ${${PROJECT_NAME}_SRCS}
    ${gmock_SRCS}
)
target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Network
    Qt5::Qml
    Qt5::Test
    ${GTEST_BOTH_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    twablet
)
qt5_use_modules(${PROJECT_NAME} Core Network Qml Test)

add_test(NAME pre COMMAND ${LCOV_PATH} --directory ${CMAKE_BINARY_DIR} --zerocounters)
add_test(tests ${PROJECT_NAME})
