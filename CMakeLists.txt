project(harbour-twablet)
cmake_minimum_required(VERSION 2.8.11)

# Options
option(ENABLE_COVERAGE "Enable coverage via gcov" OFF)
option(ENABLE_DESKTOP_BUILD "Enable build on desktop" OFF)

# Configuration
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -pie -rdynamic")

if(ENABLE_TESTS)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        message("Building with tests and coverage")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
        SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -lgcov")
    endif(CMAKE_BUILD_TYPE MATCHES Debug)
endif(ENABLE_TESTS)

if(ENABLE_DESKTOP_BUILD)
    message("Building on desktop")
endif(ENABLE_DESKTOP_BUILD)

# When new CMake is out there
# set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
# set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Projects
enable_testing()
add_subdirectory(src/lib)
if(ENABLE_TESTS)
    add_subdirectory(src/tests)
endif(ENABLE_TESTS)
add_subdirectory(src/bin)
