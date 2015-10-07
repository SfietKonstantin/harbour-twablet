# - Find sailfishapp
# Find sailfishapp.
# Adapted from https://cmake.org/Wiki/CMake:How_To_Find_Libraries#Piggybacking_on_pkg-config
# this only works under Linux, thanks to pkg-config.
#
# Once done this will define
# SAILFISHAPP_FOUND - System has sailfishapp
# SAILFISHAPP_INCLUDE_DIRS - The sailfishapp include directories
# SAILFISHAPP_LIBRARIES - The libraries needed to use sailfishapp
# SAILFISHAPP_DEFINITIONS - Compiler switches required for using sailfishapp

find_package(PkgConfig REQUIRED)
pkg_check_modules(PC_SAILFISHAPP QUIET sailfishapp)
set(PC_SAILFISHAPP_DEFINITIONS ${PC_SAILFISHAPP_CFLAGS_OTHER})

find_path(SAILFISHAPP_INCLUDE_DIR sailfishapp.h
    HINTS ${PC_SAILFISHAPP_INCLUDEDIR} ${PC_SAILFISHAPP_INCLUDE_DIRS}
    PATH_SUFFIXES libsailfishapp
)

find_library(SAILFISHAPP_LIBRARY NAMES sailfishapp libsailfishapp
    HINTS ${PC_SAILFISHAPP_LIBDIR} ${PC_SAILFISHAPP_LIBRARY_DIRS}
)

set(SAILFISHAPP_LIBRARIES ${SAILFISHAPP_LIBRARY})
set(SAILFISHAPP_INCLUDE_DIRS ${SAILFISHAPP_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set SAILFISHAPP_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(sailfishapp DEFAULT_MSG SAILFISHAPP_LIBRARY SAILFISHAPP_INCLUDE_DIR)
mark_as_advanced(SAILFISHAPP_INCLUDE_DIR SAILFISHAPP_LIBRARY)
