
######################################################
#   Build System Tests
######################################################

# 
# List of final applications to build
#
set(sys_files
       system.cpp
)

#
# Compiler options for each application
#
set(apps_compiler_options 
		"$<$<CXX_COMPILER_ID:AppleClang>:-Wno-comment>"
)

#
# Set the library names needed by applications
# Note:
# - These are only things not previously linked by the library
#
set(apps_linked_libraries 
	PUBLIC
		hopi
)	
		             
#
# Loop over each building & installing
#	             
FOREACH(sys_src ${sys_files})
	string(REPLACE ".cpp" "" sys_name ${sys_src})
	add_executable(${sys_name} ${sys_src})
	target_compile_options(${sys_name} 
		PRIVATE 
				${apps_compiler_options}
	)
	target_include_directories(${sys_name} PRIVATE ${apps_include_headers})
	target_link_libraries(${sys_name} ${apps_linked_libraries})
	#install(TARGETS ${sys_name} DESTINATION "${MY_INSTALL_TSTDIR}/system")
ENDFOREACH(sys_src)

