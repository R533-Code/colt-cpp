cmake_minimum_required(VERSION 3.4)

# C++20 is required
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(Colt-Cpp VERSION 0.0.0.0 LANGUAGES CXX)

# Save all the files to compile
file(GLOB_RECURSE ColtHeaders "include/*.h")
file(GLOB_RECURSE ColtUnits "include/*.cpp")
file(GLOB_RECURSE ColtUnitTests "test/*.cpp")

add_executable(colt_helper_test
  ${ColtHeaders} ${ColtUnits} ${ColtUnitTests}
)

# The library that can be linked with
add_library(colt_helper STATIC ${ColtHeaders} ${ColtUnits})

# Define COLT_DEBUG_BUILD for debug config
target_compile_definitions(
  colt_helper PRIVATE $<$<CONFIG:Debug>:COLT_DEBUG> $<$<CONFIG:Debug>:COLT_DEBUG_BUILD> _CRT_SECURE_NO_WARNINGS
)
target_compile_definitions(
  colt_helper_test PRIVATE $<$<CONFIG:Debug>:COLT_DEBUG> $<$<CONFIG:Debug>:COLT_DEBUG_BUILD> _CRT_SECURE_NO_WARNINGS
)

target_include_directories(colt_helper_test PRIVATE "${PROJECT_SOURCE_DIR}/include/")
target_include_directories(colt_helper PRIVATE "${PROJECT_SOURCE_DIR}/include")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT colt_helper_test)
message(STATUS "Setting up libraries...")

###########################
# Add {fmt}
message(STATUS "Adding 'fmt'...")
add_subdirectory("${PROJECT_SOURCE_DIR}/libraries/fmt")
target_link_libraries(colt_helper PUBLIC fmt::fmt)
target_link_libraries(colt_helper_test PUBLIC fmt::fmt)
###########################

###########################
# Add scnlib
message(STATUS "Adding 'scnlib'...")
add_subdirectory("${PROJECT_SOURCE_DIR}/libraries/scnlib")
target_link_libraries(colt_helper PUBLIC scn::scn)
target_link_libraries(colt_helper_test PUBLIC scn::scn)
###########################

###########################
# Add gcem
message(STATUS "Adding 'gcem'...")
target_include_directories(colt_helper_test PRIVATE "${PROJECT_SOURCE_DIR}/libraries/gcem/include")
target_include_directories(colt_helper PRIVATE "${PROJECT_SOURCE_DIR}/libraries/gcem/include")
###########################

###########################
# Add hedley
message(STATUS "Adding 'hedley'...")
target_include_directories(colt_helper_test PRIVATE "${PROJECT_SOURCE_DIR}/libraries/hedley")
target_include_directories(colt_helper PRIVATE "${PROJECT_SOURCE_DIR}/libraries/hedley")
###########################

###########################
# Add ctre
message(STATUS "Adding 'ctre'...")
target_include_directories(colt_helper_test PRIVATE "${PROJECT_SOURCE_DIR}/libraries/compile-time-regular-expressions/include")
target_include_directories(colt_helper PRIVATE "${PROJECT_SOURCE_DIR}/libraries/compile-time-regular-expressions/include")
###########################

###########################
# Add date
message(STATUS "Adding 'date'...")
target_include_directories(colt_helper_test PRIVATE "${PROJECT_SOURCE_DIR}/libraries/date/include")
target_include_directories(colt_helper PRIVATE "${PROJECT_SOURCE_DIR}/libraries/date/include")
###########################

###########################
# Catch2
message(STATUS "Adding 'Catch2'...")
target_include_directories(colt_helper_test PRIVATE "${PROJECT_SOURCE_DIR}/libraries/Catch2")
add_subdirectory("${PROJECT_SOURCE_DIR}/libraries/Catch2")
target_link_libraries(colt_helper_test PRIVATE Catch2::Catch2WithMain)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(colt_helper_test)
###########################

#########################################
# COLT CONFIG
#########################################

message(STATUS "Configuring 'config.h'...")

include(TestBigEndian)
test_big_endian(IMPL_COLT_IS_BIG_ENDIAN)

# Useful macros for knowing which compiler is going to compile the code
set(IMPL_COLT_CLANG 0)
set(IMPL_COLT_GNU 0)
set(IMPL_COLT_INTEL 0)
set(IMPL_COLT_MSVC 0)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang") # AppleClang or Clang
  set(IMPL_COLT_CLANG 1)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(IMPL_COLT_GNU 1)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  set(IMPL_COLT_INTEL 1)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(IMPL_COLT_MSVC 1)
endif()

set(IMPL_COLT_OS_WINDOWS 0)
set(IMPL_COLT_OS_LINUX 0)
set(IMPL_COLT_OS_APPLE 0)

if(WIN32)
  set(IMPL_COLT_OS_WINDOWS 1)
  set(IMPL_COLT_OS_STRING "Windows")
elseif(APPLE)
  set(IMPL_COLT_OS_APPLE 1)
  set(IMPL_COLT_OS_STRING "Apple")
elseif(UNIX AND NOT APPLE) #LINUX
  set(IMPL_COLT_OS_LINUX 1)
  set(IMPL_COLT_OS_STRING "Linux")
else()
  set(IMPL_COLT_OS_STRING "UNKNOWN")
endif()

configure_file("${CMAKE_SOURCE_DIR}/resources/cmake_colt_config.in"
  "${CMAKE_SOURCE_DIR}/include/colt/macro/config.h")

message(STATUS "Finished configuring 'config.h'!")