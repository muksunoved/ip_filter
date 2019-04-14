cmake_minimum_required(VERSION 3.2)

project(ip_filter VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})

find_package(Threads REQUIRED)

include_directories(inc)
configure_file(version.h.in ${PROJECT_SOURCE_DIR}/inc/version.h)
add_executable(ip_filter ${PROJECT_SOURCE_DIR}/src/main.cpp)

add_library(ip_filter_l 
	src/lib.cpp
	src/sorter.cpp
)

# Require c++11.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra;")
#set(COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra;-g3;-O0")
#set(CMAKE_BUILD_TYPE Debug)

set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR}/inc)

target_link_libraries(ip_filter
    ip_filter_l
    Threads::Threads
)


######################
# External Library
######################

set(EXT_PROJECTS_DIR ${PROJECT_SOURCE_DIR}/external)
add_subdirectory(${EXT_PROJECTS_DIR}/gtest)

enable_testing()

# Include the gtest library.
include_directories(
  ${gtest_SOURCE_DIR}/include
  ${gtest_SOURCE_DIR}
)

#####################
# Unit Tests
#####################

set(PROJECT_TEST_NAME test_${PROJECT_NAME})
include_directories(${GTEST_INCLUDE_DIRS} ${COMMON_INCLUDES})
file(GLOB TEST_SRC_FILES ${PROJECT_SOURCE_DIR}/test/*.cpp)
add_executable(${PROJECT_TEST_NAME} ${TEST_SRC_FILES})
add_dependencies(${PROJECT_TEST_NAME} googletest)

target_link_libraries(${PROJECT_TEST_NAME}
    ip_filter_l
    ${GTEST_LIBS_DIR}/libgtest.a
    ${GTEST_LIBS_DIR}/libgtest_main.a
    ${CMAKE_THREAD_LIBS_INIT}
    )

add_test(test1 ${PROJECT_TEST_NAME})


install(TARGETS ip_filter RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT muksunoved@mail.ru)

set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)
