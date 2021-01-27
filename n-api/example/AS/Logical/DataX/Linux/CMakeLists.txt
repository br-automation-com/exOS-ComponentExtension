cmake_minimum_required(VERSION 3.0)

project(datax C)

add_executable(datax datax.c termination.c)
target_include_directories(datax PUBLIC ..)
target_link_libraries(datax zmq exos-api)

install(TARGETS datax RUNTIME DESTINATION /home/user)

set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME exos-comp-datax)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "datax summary")
set(CPACK_PACKAGE_DESCRIPTION "Some description")
set(CPACK_PACKAGE_VENDOR "Your Organization")

set(CPACK_PACKAGE_VERSION_MAJOR 1)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 0)
set(CPACK_PACKAGE_FILE_NAME exos-comp-datax-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "your name")

set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)

