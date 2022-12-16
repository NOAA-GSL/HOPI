#
# set_common_properties(target)
#
# Sets properties that are common to either library or executable targets. This
# includes:
#
# - Add compiler definitions and appropriate warning levels to gcc-like
#   compilers (e.g. clang)
# - Define versions for the target
# - Make sure that -fPIC is enabled for library code if building shared
#   libraries.
#
# Arguments:
#   - `target`: target name
#
function(set_common_properties name)

    # Global Setting
    target_compile_options(${name} PRIVATE -Wall -Wextra)                    # Enable all warnings
    # target_compile_options(${name} PRIVATE -Wno-sign-compare)              # Disable warnings about comparing signed and unsigned
    # target_compile_options(${name} PRIVATE -Wno-narrowing -Wno-conversion) # Disable warnings about narrowing of data types
    target_compile_options(${name} PRIVATE -Werror)                          # Error on Warning
    target_compile_options(${name} PRIVATE -Wfatal-errors)                   # Stop at 1st Error Encountered

    # GNU Compiler
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${name} PRIVATE -Wpedantic)
    endif()

    # Clang Compiler
    # if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # 
    # endif()

    # Intel Compiler
    # if((CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    #
    # endif()

    # Static libraries must be compiled with position independent code on 64 bit Linux.
    if( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
        set_property(TARGET ${name} APPEND PROPERTY COMPILE_FLAGS "-fPIC")
    endif()

    # Add Common Libraries to Targets
    target_link_libraries(${name} 
        PRIVATE
            "$<$<BOOL:${Boost_SERIALIZATION_FOUND}>:Boost::serialization>"
		    "$<$<BOOL:${Boost_MPI_FOUND}>:Boost::mpi>"
            "$<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>"
            "$<$<BOOL:${MPI_CXX_FOUND}>:MPI::MPI_CXX>"
    )

endfunction()



#
# add_cxx_library(name
#                 DESCRIPTION <description>
#                 SUMMARY <summary>
#                 DEPENDS dep1 dep2 ...
#                 SOURCES src1 src2 ...
#                 HEADERS head1 head2 ...)
#
# Adds a new library to a component with the supplied component dependencies and
# sources files. A new component will be set up automatically with a lower-case
# name: e.g. if the supplied library name is `LibUtilities` the corresponding
# component is `libutilities`.
#
# Arguments:
#   - `name`: target name to construct
#   - `SUMMARY`: a brief summary of the library
#   - `DESCRIPTION`: a more detailed description of the library
#   - `DEPENDS`: a list of components on which this target depends on
#   - `SOURCES`: a list of source files for this target
#   - `HEADERS`: a list of header files for this target. These will be
#     automatically put into a `dev` package.
#
function(add_cxx_library name)

    # Parse the function arguments
    cmake_parse_arguments(CXX "SHARED" "DESCRIPTION;SUMMARY" "DEPENDS;SOURCES;HEADERS" ${ARGN})

    add_library(${name} "$<$<BOOL:${CXX_SHARED}>:SHARED>" ${CXX_SOURCES} ${CXX_HEADERS})
    add_library(${CMAKE_PROJECT_NAME}::${name} ALIAS ${name})

    #
    # Include directories for building library
    # Note: 
    # - The configure.hpp file is within ${MY_INSTALL_SRCDIR}
    #   until it gets installed to ${MY_INSTALL_INCDIR} at the end.
    #
    target_include_directories(${name}
	    PUBLIC  
		    "$<INSTALL_INTERFACE:${MY_INSTALL_INCDIR}>"
		    "$<BUILD_INTERFACE:${MY_PROJECT_INCDIR}>"
		   # "$<BUILD_INTERFACE:${MY_INSTALL_SRCDIR}>"
    )

    # Infer component name from lower-case library name, variables should use upper-case.
    string(TOLOWER ${name} CXX_COMPONENT)

    # Add name to a list so that we know for constructing dependencies
    set(CXX_LIBRARIES ${CXX_LIBRARIES} ${name} CACHE INTERNAL "")

    # Set common flags etc.
    set_common_properties(${name})

    # If we have dependencies then link against them.
    if(CXX_DEPENDS)
        target_link_libraries(${name} LINK_PUBLIC ${CXX_DEPENDS})
    endif()

    # Install headers
    foreach(HEADER ${UtilityHeaders})
        string(REGEX MATCH "(.*)[/\\]" DIR ${HEADER})
        install(FILES ${HEADER}
                DESTINATION ${MY_INSTALL_INCDIR}/hopi/${name}/${DIR}
                COMPONENT dev)
    endforeach()

    # Install Library
    install(TARGETS ${name}
        EXPORT CXX_LIBRARIES
        RUNTIME DESTINATION ${MY_INSTALL_BINDIR} COMPONENT ${CXX_COMPONENT} OPTIONAL
        ARCHIVE DESTINATION ${MY_INSTALL_LIBDIR} COMPONENT ${CXX_COMPONENT} OPTIONAL
        LIBRARY DESTINATION ${MY_INSTALL_LIBDIR} COMPONENT ${CXX_COMPONENT} OPTIONAL
    )

endfunction()

#
# add_cxx_test(name [LENGTHY])
#
# Adds a test with a given name.  The Test Definition File should be in a
# subdirectory called Tests relative to the CMakeLists.txt file calling this
# functions. The test file should be called NAME.tst, where NAME is given as a
# parameter to this function. If the LENGTHY flag is given, the test will only be
# run if `add_cxx_test_ALL` is enabled.
#
# Arguments:
#   - `name`: name of the test file
#   - `LENGTHY`: denotes a test that requires extended runtime.
#
function(add_cxx_test name)

    # Parse the function arguments
    cmake_parse_arguments(CXX "LENGTHY" "WORK_DIRECTORY" "SOURCES;DEPENDS" ${ARGN})

    if (NOT CXX_LENGTHY OR add_cxx_test_ALL)
        set(test_target ${name})     # Target becomes xxxx
        set(test_source ${name}.cpp) # Source becomes xxxx.cpp
        add_executable(${test_target} ${test_source})

        if(CXX_DEPENDS)
            target_link_libraries(${test_target} ${CXX_DEPENDS})
        endif()
        target_link_libraries(${test_target} Catch2WithMain)

        if(NOT CXX_WORK_DIRECTORY)
            add_test(NAME ${name} COMMAND ${test_target})
        else()
            add_test(NAME ${name} COMMAND ${test_target} WORKING_DIRECTORY ${CXX_WORK_DIRECTORY})
        endif()
    endif()
endfunction()

#
# add_cxx_executable(name [LENGTHY])
#
# Adds an executable with a given name.
#
# Arguments:
#   - `name`: name of the test file
#
function(add_cxx_executable name)

    # Parse the function arguments
    cmake_parse_arguments(EXE "" "DESCRIPTION;SUMMARY" "DEPENDS;SOURCES" ${ARGN})

    add_executable(${name} ${EXE_SOURCES})
    if(EXE_DEPENDS)
        target_link_libraries(${name} LINK_PUBLIC ${EXE_DEPENDS})
    endif()
endfunction()

