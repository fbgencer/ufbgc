cmake_minimum_required(VERSION 3.10)

project(ufbgc VERSION 1.0.0 DESCRIPTION "ufbgc unit test framework")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -Wall")


include(GNUInstallDirs)

add_library(ufbgc SHARED
    src/ufbgc.c
)
set_target_properties(ufbgc PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER src/ufbgc.h)

configure_file(ufbgc.pc.in ufbgc.pc @ONLY)

target_include_directories(ufbgc PUBLIC src)

install(TARGETS ufbgc 
LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES ${CMAKE_BINARY_DIR}/ufbgc.pc
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)


add_executable(ufbgc_example example/example.c)
target_link_libraries (ufbgc_example LINK_PUBLIC ufbgc)
target_include_directories(ufbgc_example PUBLIC example)
target_include_directories(ufbgc_example PUBLIC src)
