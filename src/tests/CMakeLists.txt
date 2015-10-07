project(twablet-tests)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_AUTOMOC TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Qml REQUIRED)
find_package(Qt5Test REQUIRED)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${QT_INCLUDES}
    ${twablet_INCLUDE_DIRS}
)

set(${PROJECT_NAME}_account_SRCS
    tst_account/tst_account.cpp
)

add_executable(${PROJECT_NAME}_account
    ${${PROJECT_NAME}_account_SRCS}
)
target_link_libraries(${PROJECT_NAME}_account
    Qt5::Core
    Qt5::Network
    Qt5::Qml
    Qt5::Test
    twablet
)
qt5_use_modules(${PROJECT_NAME}_account Core Network Qml Test)

add_test(account ${PROJECT_NAME}_account)
