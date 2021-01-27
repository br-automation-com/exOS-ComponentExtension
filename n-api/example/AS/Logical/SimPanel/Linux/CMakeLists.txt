cmake_minimum_required(VERSION 3.0)

project(simpanel C)

add_executable(simpanel simpanel.c termination.c)
target_include_directories(simpanel PUBLIC ..)
target_link_libraries(simpanel zmq exos-api)

install(TARGETS simpanel RUNTIME DESTINATION /home/user)

set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME exos-comp-simpanel)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "simpanel summary")
set(CPACK_PACKAGE_DESCRIPTION "Some description")
set(CPACK_PACKAGE_VENDOR "Your Organization")

set(CPACK_PACKAGE_VERSION_MAJOR 1)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 0)
set(CPACK_PACKAGE_FILE_NAME exos-comp-simpanel-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "your name")

set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)

