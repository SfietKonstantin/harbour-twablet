project(twablet)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_AUTOMOC TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Qml REQUIRED)

add_definitions(-DQT_NO_CAST_FROM_ASCII)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${QT_INCLUDES}
)

set(${PROJECT_NAME}_Core_SRCS
    globals.h
    qobjectutils.h
    irepositorylistener.h
    iloadsave.h
    account.cpp
    layout.cpp
    tweet.cpp
    quotedtweet.cpp
    user.cpp
    entity.cpp
    entityvisitor.cpp
    mediaentity.cpp
    urlentity.cpp
    usermentionentity.cpp
    hashtagentity.cpp
    list.cpp
    query.cpp
    loadsavemanager.cpp
    repository.h
    accountrepository.cpp
    layoutrepository.cpp
    tweetrepository.h
    userrepository.h
    listrepository.h
    containerkey.cpp
    tweetrepositorycontainer.cpp
    userrepositorycontainer.cpp
    listrepositorycontainer.cpp
    iqueryexecutor.h
    irepositoryqueryhandler.h
    repositoryqueryhandlerfactory.cpp
    tweetrepositoryqueryhandler.cpp
    tweetsearchqueryhandler.cpp
    userrepositoryqueryhandler.cpp
    listrepositoryqueryhandler.cpp
    iitemqueryhandler.h
    itemqueryhandlerfactory.cpp
    iitemlistener.h
    itemquerycontainer.cpp
    tweetitemqueryhandler.cpp
    useritemqueryhandler.cpp
)

set(${PROJECT_NAME}_Private_SRCS
    private/maputil.h
    private/debughelper.cpp
    private/twitterdatautil.cpp
    private/twitterqueryutil.cpp
    private/networkqueryexecutor.cpp
    private/repositoryquerycallback.h
    private/repositoryqueryhandlerutil.cpp
    private/conversionutil.cpp
    private/itemquerycallback.h
)

set(${PROJECT_NAME}_Qml_SRCS
    qml/accountobject.cpp
    qml/layoutobject.cpp
    qml/tweetobject.cpp
    qml/quotedtweetobject.cpp
    qml/userobject.cpp
    qml/mediaobject.cpp
    qml/listobject.cpp
    qml/querytypeobject.cpp
    qml/imodel.h
    qml/model.h
    qml/accountmodel.cpp
    qml/layoutmodel.cpp
    qml/tweetmodel.cpp
    qml/usermodel.cpp
    qml/listmodel.cpp
    qml/mediamodel.cpp
    qml/querytypemodel.cpp
    qml/accountselectionmodel.cpp
    qml/iaccountrepositorycontainerobject.h
    qml/ilayoutcontainerobject.h
    qml/itweetrepositorycontainerobject.h
    qml/iuserrepositorycontainerobject.h
    qml/ilistrepositorycontainerobject.h
    qml/iitemquerycontainerobject.h
    qml/twitterauthentification.cpp
    qml/datarepositoryobject.cpp
    qml/datarepositoryobjectmap.h
    qml/descriptionformatter.cpp
    qml/entitiesformatter.cpp
    qml/tweetformatter.cpp
    qml/quotedtweetformatter.cpp
    qml/iquerywrapperobject.h
    qml/querywrappervisitor.cpp
    qml/tweetmodelquerywrapperobject.cpp
    qml/usermodelquerywrapperobject.cpp
    qml/listmodelqueryobject.cpp
    qml/iqueryitem.h
    qml/queryitem.h
    qml/queryitemfactory.h
    qml/tweetqueryitem.cpp
    qml/userqueryitem.cpp
    qml/tweetquerywrapperobject.cpp
    qml/statusupdatequerywrapperobject.cpp
    qml/tweetspecificquerywrapperobject.cpp
    qml/userquerywrapperobject.cpp
    qml/userspecificquerywrapperobject.cpp
)

add_library(${PROJECT_NAME} STATIC
    ${${PROJECT_NAME}_Core_SRCS}
    ${${PROJECT_NAME}_Private_SRCS}
    ${${PROJECT_NAME}_Qml_SRCS}
)

qt5_use_modules(${PROJECT_NAME} Core Network Qml)

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE
)
