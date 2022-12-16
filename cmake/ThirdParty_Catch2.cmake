#
########################################################################
#
# ThirdParty Configuration
#
# Catch2 - Catch2 Testing Framework
#
########################################################################
#

#
# Parameters
#
SET(LIBRAY_NAME "Catch2")
SET(GIT_WEBSITE "https://github.com/catchorg/Catch2.git")
SET(GIT_VERSION "v3.2.0")

#
# Display our message
#
MESSAGE(VERBOSE " ")
MESSAGE(VERBOSE "Fetching ... ")
MESSAGE(VERBOSE "Library : ${LIBRAY_NAME}")
MESSAGE(VERBOSE "GitHub  : ${GIT_WEBSITE}")
MESSAGE(VERBOSE "Version : ${GIT_VERSION}")

#
# Set argparse environment variables
#
#set(INSTALL_GTEST OFF)

#
# DownLoad (if not already done)
#
include(FetchContent)
FetchContent_Declare(
	${LIBRAY_NAME}
    GIT_REPOSITORY ${GIT_WEBSITE}
	GIT_TAG        ${GIT_VERSION}
)
FetchContent_MakeAvailable(${LIBRAY_NAME})

MESSAGE(VERBOSE "Done")
