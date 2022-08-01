cmake_minimum_required(VERSION 3.0)

project(ros_topics_typ)

set(CMAKE_BUILD_TYPE Debug)
add_library(libros_topics_typ STATIC libros_topics_typ.c)
target_include_directories(libros_topics_typ PUBLIC ..)
add_executable(ros_topics_typ termination.c ros_topics_typ.c exos_ros_topics_typ.c)
target_include_directories(ros_topics_typ PUBLIC ..)
target_link_libraries(ros_topics_typ libros_topics_typ zmq exos-api)

install(TARGETS ros_topics_typ RUNTIME DESTINATION /home/user/ros_topics_typ)

set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME exos-comp-ros_topics_typ)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "ros_topics_typ summary")
set(CPACK_PACKAGE_DESCRIPTION "Some description")
set(CPACK_PACKAGE_VENDOR "Your Company")

set(CPACK_PACKAGE_VERSION_MAJOR 1)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 0)
set(CPACK_PACKAGE_FILE_NAME exos-comp-ros_topics_typ_1.0.0_amd64)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Your Name")

set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)

