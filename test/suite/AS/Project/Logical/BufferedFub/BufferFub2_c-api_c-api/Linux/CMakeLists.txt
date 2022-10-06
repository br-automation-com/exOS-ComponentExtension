cmake_minimum_required(VERSION 3.0)

project(bufferfub2)

set(CMAKE_BUILD_TYPE Debug)
add_executable(bufferfub2 termination.c bufferfub2.c exos_bufferfub2.c)
target_include_directories(bufferfub2 PUBLIC ..)
target_link_libraries(bufferfub2 zmq exos-api)

install(TARGETS bufferfub2 RUNTIME DESTINATION /home/user/bufferfub2)

set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME exos-comp-bufferfub2)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "BufferFub2 summary")
set(CPACK_PACKAGE_DESCRIPTION "Some description")
set(CPACK_PACKAGE_VENDOR "Your Company")

set(CPACK_PACKAGE_VERSION_MAJOR 1)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 0)
set(CPACK_PACKAGE_FILE_NAME exos-comp-bufferfub2_1.0.0_amd64)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Your Name")

set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)

