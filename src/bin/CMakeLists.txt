project(harbour-twablet)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_AUTOMOC TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Quick REQUIRED)

add_definitions(-DQT_NO_CAST_FROM_ASCII)

if(ENABLE_DESKTOP_BUILD)
    add_definitions(-DDESKTOP)
    qt5_add_resources(${PROJECT_NAME}_Res_SRCS
        res.qrc
    )
else(ENABLE_DESKTOP_BUILD)
    find_package(sailfishapp REQUIRED)
endif(ENABLE_DESKTOP_BUILD)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${QT_INCLUDES}
    ${SAILFISHAPP_INCLUDE_DIRS}
    ${twablet_INCLUDE_DIRS}
)

set(${PROJECT_NAME}_SRCS
    version.h
    main.cpp
    networkmonitor.cpp
)

set(${PROJECT_NAME}_Qml_SRCS
    qml/harbour-twablet.qml
    qml/pages/MainPage.qml
    qml/pages/AuthentificationPage.qml
    qml/pages/AccountPage.qml
    qml/pages/ColumnLayout.qml
    qml/pages/AddColumnPage.qml
    qml/pages/Toolbar.qml
    qml/pages/TweetDelegate.qml
    qml/cover/CoverPage.qml
    qml/pages/TwitterImage.qml
    qml/pages/StatusPlaceholder.qml
    qml/pages/StatusHeader.qml
    qml/pages/ToolbarButton.qml
    qml/pages/RightPanel.qml
    qml/pages/UserPage.qml
    qml/pages/UserPageButton.qml
    qml/pages/MiniButton.qml
    qml/pages/SimpleListItem.qml
    qml/pages/UsersPage.qml
    qml/pages/TweetsPage.qml
    qml/pages/TweetPage.qml
    qml/pages/StatusUpdater.qml
    qml/pages/ProgressIconButton.qml
    qml/pages/AboutPage.qml
    qml/pages/ImageBrowser.qml
    qml/pages/TweetHeader.qml
    qml/pages/TweetText.qml
    qml/pages/TweetFooter.qml
    qml/pages/SettingsPage.qml
    qml/pages/AccountDelegate.qml
    qml/pages/LayoutItemDelegate.qml
    qml/pages/LinkHandler.js
)

set(${PROJECT_NAME}_Data_SRCS
    data/home.svg
    data/mail.svg
    data/inbox.svg
    data/search.svg
    data/me.svg
    data/background.png
    data/logo.png
)

add_executable(${PROJECT_NAME}
    ${${PROJECT_NAME}_SRCS}
    ${${PROJECT_NAME}_Res_SRCS}
    ${${PROJECT_NAME}_Qml_SRCS}
    ${${PROJECT_NAME}_Data_SRCS}
)
target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Gui
    Qt5::Network
    Qt5::Quick
    ${SAILFISHAPP_LIBRARIES}
    twablet
)
qt5_use_modules(${PROJECT_NAME} Core Gui Network Quick)

# Deployment
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(FILES ${PROJECT_NAME}.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
install(FILES ${PROJECT_NAME}.png DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/86x86/apps)
install(DIRECTORY qml DESTINATION ${CMAKE_INSTALL_PREFIX}/share/harbour-twablet)
install(DIRECTORY data DESTINATION ${CMAKE_INSTALL_PREFIX}/share/harbour-twablet)
