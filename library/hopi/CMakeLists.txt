

#configure_file(
#	"${MY_PROJECT_SRCDIR}/hopi/configure.hpp.in"
#	"${MY_INSTALL_INCDIR}/hopi/configure.hpp"
#)

#
# Combine header files into single list
#
set(AllHeaders
    ascii_targets.hpp
    mpixx.hpp
    partition.hpp
    spatial/bound/box.hpp
    spatial/common/truncated_multiset.hpp
    spatial/shared/index/rtree/algorithm.hpp
    spatial/shared/index/rtree/leaf.hpp
    spatial/shared/index/rtree/linear.hpp
    spatial/shared/index/rtree/node.hpp
    spatial/shared/index/rtree/page.hpp
    spatial/shared/index/rtree/quadratic.hpp
    spatial/shared/index/exhaustive.hpp
    spatial/shared/index/rtree.hpp
    spatial/shared/predicate/dispatch.hpp
    spatial/shared/predicate/distance.hpp
    spatial/shared/predicate/factories.hpp
    spatial/shared/predicate/spatial.hpp
    spatial/shared/predicate/tags.hpp
    spatial/all.hpp
)

#
# Combine source files into single list
#
set(AllSources
	ascii_targets.cpp
    mpixx.cpp
    partition.cpp
)

#
# Combine test files into single list
#
set(AllTests
)

#
# Add the library to be built+installed
#
add_cxx_library(hopi
    DESCRIPTION "This library provides core routines and integration." 
    SUMMARY     "HOPI Library"
    HEADERS     ${AllHeaders}
    SOURCES     ${AllSources}
)

#
# Add Tests
# For some reason this will not work inside each subdirectory ???
#
if( HOPI_BUILD_UNIT )
   add_cxx_tests(hopi_unit
       SOURCES ${AllTests}
       DEPENDS hopi
   )
endif()