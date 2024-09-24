cmake_minimum_required(VERSION 3.21)

function(get_all_targets var)
    set(targets)
    get_all_targets_recursive(targets ${CMAKE_CURRENT_SOURCE_DIR})
    set(${var} ${targets} PARENT_SCOPE)
endfunction()

macro(get_all_targets_recursive targets dir)
    get_property(subdirectories DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
    foreach(subdir ${subdirectories})
        get_all_targets_recursive(${targets} ${subdir})
    endforeach()

    get_property(current_targets DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)
    list(APPEND ${targets} ${current_targets})
endmacro()

function(copy_all_dl_to_bin _target)
    # We copy the target result to its own location (which does nothing)
    # as an empty TARGET_RUNTIME_DLLS could cause errors with CMake
    add_custom_command(TARGET ${_target} PRE_LINK
    COMMAND ${CMAKE_COMMAND} -E copy
        "$<TARGET_FILE_DIR:${_target}>;$<TARGET_RUNTIME_DLLS:${_target}>"
        "$<TARGET_FILE_DIR:${_target}>;$<TARGET_FILE_DIR:${_target}>"
    COMMAND_EXPAND_LISTS
    )
endfunction()

# C++20 is required
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(Colt-Cpp VERSION 0.0.5.0 LANGUAGES CXX C)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Save all the files to compile
file(GLOB_RECURSE ColtHeaders "include/*.h")
file(GLOB_RECURSE ColtUnits "include/*.cpp")
file(GLOB_RECURSE ColtUnitTests "test/*.cpp")

add_executable(coltcpp_tests
  ${ColtHeaders} ${ColtUnits} ${ColtUnitTests}
)

add_library(coltcpp ${ColtHeaders} ${ColtUnits})
set_target_properties(coltcpp PROPERTIES OUTPUT_NAME libcoltcpp PREFIX "")


# Define COLT_DEBUG_BUILD for debug config
target_compile_definitions(
  coltcpp PUBLIC $<$<CONFIG:Debug>:COLT_DEBUG> $<$<CONFIG:Debug>:COLT_DEBUG_BUILD> _CRT_SECURE_NO_WARNINGS
)
target_compile_definitions(
  coltcpp_tests PUBLIC $<$<CONFIG:Debug>:COLT_DEBUG> $<$<CONFIG:Debug>:COLT_DEBUG_BUILD> _CRT_SECURE_NO_WARNINGS
  COLT_TESTING
)

target_compile_options(
  coltcpp PUBLIC $<$<CXX_COMPILER_ID:MSVC>:/Zc:preprocessor>
  PUBLIC $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
)

target_compile_options(
  coltcpp_tests PUBLIC $<$<CXX_COMPILER_ID:MSVC>:/Zc:preprocessor>
  PUBLIC $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
)

if (NOT MSVC)
    set_target_properties(coltcpp_tests PROPERTIES LINK_FLAGS "-Wl,-export-dynamic")
endif()

target_include_directories(coltcpp_tests PUBLIC "${PROJECT_SOURCE_DIR}/include/")
target_include_directories(coltcpp PUBLIC "${PROJECT_SOURCE_DIR}/include")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT coltcpp_tests)
message(STATUS "Setting up libraries...")

###########################
# Add MPIR
message(STATUS "Adding 'MPIR'...")
# Include MPIR library
include(ExternalProject)

set(prefix "${CMAKE_BINARY_DIR}/deps")
set(MPIR_LIBRARY "${prefix}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}mpir${CMAKE_STATIC_LIBRARY_SUFFIX}")
set(MPIR_INCLUDE_DIR "${prefix}/include")

ExternalProject_Add(mpir
    PREFIX "${prefix}"
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    DOWNLOAD_NO_PROGRESS TRUE
    DOWNLOAD_NAME mpir-cmake.tar.gz
    URL https://github.com/chfast/mpir/archive/cmake.tar.gz
    URL_HASH SHA256=d32ea73cb2d8115a8e59b244f96f29bad7ff03367162b660bae6495826811e06
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DCMAKE_INSTALL_LIBDIR=lib
        -DCMAKE_BUILD_TYPE=Release
        -DMPIR_GMP=On
    BUILD_BYPRODUCTS "${MPIR_LIBRARY}"
)

# Specify the directories where header files are located
target_include_directories(coltcpp PUBLIC ${MPIR_INCLUDE_DIR})
target_include_directories(coltcpp_tests PUBLIC ${MPIR_INCLUDE_DIR})

# Link MPIR library
add_dependencies(coltcpp mpir)
add_dependencies(coltcpp_tests mpir)
target_link_libraries(coltcpp PUBLIC ${MPIR_LIBRARY})
target_link_libraries(coltcpp_tests PUBLIC ${MPIR_LIBRARY})
###########################

###########################
# Add {fmt}
message(STATUS "Adding 'fmt'...")
add_subdirectory("${PROJECT_SOURCE_DIR}/libraries/fmt")
target_link_libraries(coltcpp PUBLIC fmt::fmt)
target_link_libraries(coltcpp_tests PUBLIC fmt::fmt)
set_target_properties(fmt PROPERTIES OUTPUT_NAME libfmt PREFIX "")
###########################

###########################
# Add fast_float
message(STATUS "Adding 'fast_float'...")
add_subdirectory("${PROJECT_SOURCE_DIR}/libraries/fast_float")
###########################

###########################
# Add scnlib
set(SCN_USE_EXTERNAL_FAST_FLOAT ON CACHE BOOL "" FORCE)
message(STATUS "Adding 'scnlib'...")
add_subdirectory("${PROJECT_SOURCE_DIR}/libraries/scnlib")
target_link_libraries(coltcpp PUBLIC scn::scn)
target_link_libraries(coltcpp_tests PUBLIC scn::scn)
set_target_properties(scn PROPERTIES OUTPUT_NAME libscn PREFIX "")
###########################

###########################
# Add gcem
message(STATUS "Adding 'gcem'...")
target_include_directories(coltcpp_tests PUBLIC "${PROJECT_SOURCE_DIR}/libraries/gcem/include")
target_include_directories(coltcpp PUBLIC "${PROJECT_SOURCE_DIR}/libraries/gcem/include")
###########################

###########################
# Add zpp_bits
message(STATUS "Adding 'zpp_bits'...")
target_include_directories(coltcpp_tests PUBLIC "${PROJECT_SOURCE_DIR}/libraries/zpp_bits")
target_include_directories(coltcpp PUBLIC "${PROJECT_SOURCE_DIR}/libraries/zpp_bits")
###########################

###########################
# Add uni-algo
message(STATUS "Adding 'uni-algo'...")
add_subdirectory("${PROJECT_SOURCE_DIR}/libraries/uni-algo")
target_link_libraries(coltcpp PUBLIC uni-algo::uni-algo)
target_link_libraries(coltcpp_tests PUBLIC uni-algo::uni-algo)
set_target_properties(uni-algo PROPERTIES OUTPUT_NAME libunialgo PREFIX "")
###########################

###########################
# Add simdutf
message(STATUS "Adding 'simdutf'...")
set(SIMDUTF_ICONV OFF CACHE BOOL "simdutf use iconv." FORCE)
set(SIMDUTF_TESTS OFF CACHE BOOL "simdutf generate tests." FORCE)
set(SIMDUTF_TOOLS OFF CACHE BOOL "simdutf generate tool." FORCE)
add_subdirectory("${PROJECT_SOURCE_DIR}/libraries/simdutf")
target_link_libraries(coltcpp PUBLIC simdutf)
target_link_libraries(coltcpp_tests PUBLIC simdutf)
set_target_properties(simdutf PROPERTIES OUTPUT_NAME libsimdutf PREFIX "")
###########################

###########################
# Add hedley
message(STATUS "Adding 'hedley'...")
target_include_directories(coltcpp_tests PUBLIC "${PROJECT_SOURCE_DIR}/libraries/hedley")
target_include_directories(coltcpp PUBLIC "${PROJECT_SOURCE_DIR}/libraries/hedley")
###########################

###########################
# Add date
message(STATUS "Adding 'date'...")
target_include_directories(coltcpp_tests PUBLIC "${PROJECT_SOURCE_DIR}/libraries/date/include")
target_include_directories(coltcpp PUBLIC "${PROJECT_SOURCE_DIR}/libraries/date/include")
###########################

###########################
# Catch2
message(STATUS "Adding 'Catch2'...")
target_include_directories(coltcpp_tests PUBLIC "${PROJECT_SOURCE_DIR}/libraries/Catch2")
add_subdirectory("${PROJECT_SOURCE_DIR}/libraries/Catch2")
target_link_libraries(coltcpp_tests PUBLIC Catch2::Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(coltcpp_tests
  DISCOVERY_MODE PRE_TEST
)
###########################

# The code below hides the projects generated by the dependencies
# in the Solution Explorer View in Visual Studio

get_all_targets(ALL_TARGETS)
set(KEEP_TARGETS coltcpp coltcpp_tests simdutf fmt scn)
list(REMOVE_ITEM ALL_TARGETS  ${KEEP_TARGETS})
# Put these targets in the 'HiddenTargets' folder in the IDE.
set_target_properties(${SIMDUTF_TO_EXCLUDE} PROPERTIES EXCLUDE_FROM_ALL True)
set_target_properties(${ALL_TARGETS} PROPERTIES FOLDER HiddenTargets)

# Used to avoid dependencies from adding their own tests.
configure_file(${PROJECT_SOURCE_DIR}/resources/CTestCustom.cmake ${CMAKE_BINARY_DIR} @ONLY)
# Copy all dll to target directory
copy_all_dl_to_bin(coltcpp_tests)

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

configure_file("${PROJECT_SOURCE_DIR}/resources/cmake_colt_config.in"
  "${PROJECT_SOURCE_DIR}/include/colt/macro/config.h")

message(STATUS "Finished configuring 'config.h'!")

# Add ALIAS
add_library(colt::coltcpp ALIAS coltcpp)