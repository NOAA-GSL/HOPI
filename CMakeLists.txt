
#
#---------------------------------------------------------------------
# Set Version Numbers
#---------------------------------------------------------------------
#
cmake_minimum_required(VERSION 3.20.0 FATAL_ERROR)
set(HOPI_VERSION_MAJOR 2022)
set(HOPI_VERSION_MINOR   01)
set(HOPI_VERSION_PATCH    0)

#
#---------------------------------------------------------------------
# Handle case where HOPI is a SubProject to larger CMake Project
#---------------------------------------------------------------------
#
set(HOPI_VERSION "${HOPI_VERSION_MAJOR}.${HOPI_VERSION_MINOR}.${HOPI_VERSION_PATCH}")
if( HOPI_LOADED AND (NOT HOPI_LOADED STREQUAL ${HOPI_VERSION}) )
	message(FATAL_ERROR "You are mixing HOPI versions. Loaded is ${HOPI_LOADED}, expected ${HOPI_VERSION}")
endif()
if( HOPI_LOADED )
	return() # Stop processing file, avoids nesting the whole file
endif()
set(HOPI_LOADED ${HOPI_VERSION})

get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
  set (HOPI_LOADED ${HOPI_LOADED} PARENT_SCOPE)
endif()
mark_as_advanced(HOPI_LOADED)

#
#---------------------------------------------------------------------
# Project Setup
#---------------------------------------------------------------------
#
project("HOPI" VERSION ${HOPI_LOADED} LANGUAGES CXX)

#
#---------------------------------------------------------------------
# Set location of *.cmake modules
#---------------------------------------------------------------------
#
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

#
#---------------------------------------------------------------------
# Load CMake Scripts
#---------------------------------------------------------------------
#
include(InSourceBuild)
include(BuildDefaults)
include(DefineTextColor)
include(InternalUtils)

#
#---------------------------------------------------------------------
# Directory Report
#---------------------------------------------------------------------
#
# CMAKE directories are from top level CMakeLists.txt
# PROJECT directories are for recent project call
#
message(VERBOSE "")
message(VERBOSE "--------------------- Directory Report -----------------------")
message(VERBOSE "")
message(VERBOSE "--- Top Level Directories ---")
message(VERBOSE "CMAKE_SOURCE_DIR  = ${CMAKE_SOURCE_DIR}")
message(VERBOSE "CMAKE_BINARY_DIR  = ${CMAKE_BINARY_DIR}")
message(VERBOSE "")
message(VERBOSE "--- Project Level Directories ---")
message(VERBOSE "PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}")
message(VERBOSE "PROJECT_BINARY_DIR = ${PROJECT_BINARY_DIR}")

#---------------------------------------------------------------------
# Locations of Installation & Report
#---------------------------------------------------------------------

# Source locations in source tree
set(MY_PROJECT_PREFIX ${PROJECT_SOURCE_DIR})
set(MY_PROJECT_APPDIR ${MY_PROJECT_PREFIX}/apps)
set(MY_PROJECT_INCDIR ${MY_PROJECT_PREFIX}/library)
set(MY_PROJECT_SRCDIR ${MY_PROJECT_PREFIX}/library)
set(MY_PROJECT_TSTDIR ${MY_PROJECT_PREFIX}/test)
set(MY_PROJECT_DOCDIR ${MY_PROJECT_PREFIX}/docs)

# Where to place all libraries
#set(MY_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(MY_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
set(MY_INSTALL_INCDIR ${MY_INSTALL_PREFIX}/include)
set(MY_INSTALL_BINDIR ${MY_INSTALL_PREFIX}/bin)
set(MY_INSTALL_SRCDIR ${MY_INSTALL_PREFIX}/src)
set(MY_INSTALL_LIBDIR ${MY_INSTALL_PREFIX}/lib)
set(MY_INSTALL_TSTDIR ${MY_INSTALL_PREFIX}/test)
set(MY_INSTALL_DOCDIR ${MY_INSTALL_PREFIX}/docs)

message(VERBOSE "")
message(VERBOSE "--------------------- Using Directories ----------------------")
message(VERBOSE " ")
message(VERBOSE "--- Original Locations ---")
message(VERBOSE "MY_PROJECT_PREFIX = ${MY_PROJECT_PREFIX}")
message(VERBOSE "MY_PROJECT_INCDIR = ${MY_PROJECT_INCDIR}")
message(VERBOSE "MY_PROJECT_SRCDIR = ${MY_PROJECT_SRCDIR}")
message(VERBOSE "MY_PROJECT_TSTDIR = ${MY_PROJECT_TSTDIR}")
message(VERBOSE "MY_PROJECT_DOCDIR = ${MY_PROJECT_DOCDIR}")
message(VERBOSE " ")
message(VERBOSE "--- Installation Locations ---")
message(VERBOSE "MY_INSTALL_PREFIX = ${MY_INSTALL_PREFIX}")
message(VERBOSE "MY_INSTALL_INCDIR = ${MY_INSTALL_INCDIR}")
message(VERBOSE "MY_INSTALL_SRCDIR = ${MY_INSTALL_SRCDIR}")
message(VERBOSE "MY_INSTALL_LIBDIR = ${MY_INSTALL_LIBDIR}")
message(VERBOSE "MY_INSTALL_TSTDIR = ${MY_INSTALL_TSTDIR}")
message(VERBOSE "MY_INSTALL_DOCDIR = ${MY_INSTALL_DOCDIR}")

#---------------------------------------------------------------------
# Detect Library includes, flags, etc.
#---------------------------------------------------------------------
include(FetchContent)

if( HOPI_BUILD_UNIT )
	include(ThirdParty_Catch2)
	include(CTest)
endif()

message(VERBOSE "")
message(VERBOSE "---------------------------- MPI -------------------------------")
message(VERBOSE "")
find_package(MPI REQUIRED)

message(VERBOSE "")
message(VERBOSE "--------------------------- Boost ------------------------------")
message(VERBOSE "")
find_package(_Boost REQUIRED)

message(VERBOSE "")
message(VERBOSE "-------------------------- DOxygen -----------------------------")
message(VERBOSE "")
if( HOPI_BUILD_DOXYGEN )
	find_package(Doxygen REQUIRED)
	message(VERBOSE "Doxygen Found   = ${DOXYGEN_FOUND}")
	message(VERBOSE "Doxygen Version = ${DOXYGEN_VERSION}")	
else()
	message(VERBOSE "${Magenta}\t\t\t      Not Used ${ColorReset}")
endif()

#---------------------------------------------------------------------
# 
#---------------------------------------------------------------------

message(VERBOSE "")
message(VERBOSE "================================================================")
message(VERBOSE "                    Configuration Complete                      ")
message(VERBOSE "================================================================")
message(VERBOSE "")

# Header only installation
add_subdirectory(library/hopi)

# Build Applications from Library
if( HOPI_BUILD_APPS )
	message(VERBOSE "Configured to build - Applications")
	add_subdirectory(apps)
endif()

# Build Scratch things for Library
if( HOPI_BUILD_SCRATCH )
	message(VERBOSE "Configured to build - Scratch Things")
	add_subdirectory(test/scratch)
endif()

if( HOPI_BUILD_DOXYGEN AND DOXYGEN_FOUND )
	message(VERBOSE "Configured to build - Documentation")
	add_subdirectory(docs)
endif()
