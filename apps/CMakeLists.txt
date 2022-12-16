
add_cxx_executable("driver" SOURCES "main.cpp" DEPENDS "${CMAKE_PROJECT_NAME}::hopi")

install(TARGETS "driver" DESTINATION "${MY_INSTALL_BINDIR}")