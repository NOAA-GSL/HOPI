#
# Set Build system Defaults for HOPI
#

#
# -----------------------------------------------------------------------------
#                         Defaults for CMake Variables
# -----------------------------------------------------------------------------
#
# NOTE:
# These can will be overriden by the CMAKE_<variable> on the command line
# > cmake -DCMAKE_<variable>=<value>
#
set(DEFAULT_COLOR_MAKEFILE           TRUE)       # => CMAKE_COLOR_MAKEFILE
set(DEFAULT_VERBOSE_MAKEFILE         TRUE)       # => CMAKE_VERBOSE_MAKEFILE
set(DEFAULT_MESSAGE_LOG_LEVEL        "VERBOSE")  # => CMAKE_MESSAGE_LOG_LEVEL
set(DEFAULT_BUILD_TYPE               "Debug")    # => CMAKE_BUILD_TYPE

#
# -----------------------------------------------------------------------------
#                          Defaults for Build Parts
# -----------------------------------------------------------------------------
# 
# NOTE:
# These can be set directly on the command line using the standard notion
# > cmake -D<variable>=<value>
#
option(HOPI_BUILD_APPS               "Build Applications"             TRUE )
option(HOPI_BUILD_UNIT               "Build Unit Tests"               FALSE )
option(HOPI_BUILD_SCRATCH            "Build Scratch Tests"            FALSE )
option(HOPI_BUILD_DOXYGEN            "Use Doxygen to generate docs"   FALSE )

#
# -----------------------------------------------------------------------------
#                           Defaults for Compiler
# -----------------------------------------------------------------------------
# 
# NOTE:
# These can be set directly on the command line using the standard notion
# > cmake -D<variable>=<value>
#
option(HOPI_USE_NO_UNIQUE_ADDRESS    "Save Memory using [[no_unique_address]]"  TRUE )
option(HOPI_USE_INLINE               "Inline Marked Functions"                  TRUE )
option(HOPI_USE_FORCE_INLINE         "Force Inline Marked Functions"            TRUE )

#
# =============================================================================
# -----------------------------------------------------------------------------
#                     Logic to Set Dependant Variables
# -----------------------------------------------------------------------------
# =============================================================================
#

if( NOT CMAKE_BUILD_TYPE )
	set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

if( NOT CMAKE_MESSAGE_LOG_LEVEL )
	set(CMAKE_MESSAGE_LOG_LEVEL "${DEFAULT_MESSAGE_LOG_LEVEL}" CACHE STRING "Choose the type of logging." FORCE)
	set_property(CACHE CMAKE_MESSAGE_LOG_LEVEL PROPERTY STRINGS "ERROR" "WARNING" "NOTICE" "STATUS" "VERBOSE" "DEBUG" "TRACE")
endif()

if( NOT CMAKE_COLOR_MAKEFILE )
	set( CMAKE_COLOR_MAKEFILE "${DEFAULT_COLOR_MAKEFILE}" CACHE STRING "" FORCE)
	set_property(CACHE CMAKE_COLOR_MAKEFILE PROPERTY STRINGS "TRUE" "FALSE")
endif()

if( NOT CMAKE_VERBOSE_MAKEFILE )
	set( CMAKE_VERBOSE_MAKEFILE "${DEFAULT_VERBOSE_MAKEFILE}" CACHE STRING "" FORCE)
	set_property(CACHE CMAKE_VERBOSE_MAKEFILE PROPERTY STRINGS "TRUE" "FALSE")
endif()
