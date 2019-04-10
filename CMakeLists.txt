cmake_minimum_required(VERSION 3.2)

project(ip_filter VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})

find_package(Threads REQUIRED)
find_package(GTest REQUIRED)
#find_package(GMock REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})
#include_directories(${GMOCK_INCLUDE_DIRS})
include_directories(inc)


configure_file(version.h.in ${PROJECT_SOURCE_DIR}/inc/version.h)

add_executable(ip_filter ${PROJECT_SOURCE_DIR}/src/main.cpp)

add_library(ip_filter_l ${PROJECT_SOURCE_DIR}/src/lib.cpp)

add_executable(test_ip_filter ${PROJECT_SOURCE_DIR}/test/test_ip_filter.cpp)

set_target_properties(ip_filter ip_filter_l test_ip_filter PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
)


target_link_libraries(ip_filter
    ip_filter_l
    Threads::Threads
)

target_link_libraries(test_ip_filter
    ${GTEST_LIBRARIES}
    ip_filter_l
    Threads::Threads
)

install(TARGETS ip_filter RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT muksunoved@mail.ru)

set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)

enable_testing()

add_test(ip_filter_tests test_ip_filter)
