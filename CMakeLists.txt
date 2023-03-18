cmake_minimum_required(VERSION 3.12)
project(Colt-Cpp-Test LANGUAGES CXX VERSION 0.0.0.0)
# Set C++17 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Get the headers files and translation units
file(GLOB_RECURSE ColtCppHeaders CONFIGURE_DEPENDS "include/*.h")
file(GLOB_RECURSE ColtCppUnits CONFIGURE_DEPENDS "test/*.cpp")

add_executable(ColtCppTest
"${ColtCppHeaders}" "${ColtCppUnits}"
)

if(MSVC)
  	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ColtCppTest)
endif()

target_include_directories(ColtCppTest PUBLIC "include/")