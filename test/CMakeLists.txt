cmake_minimum_required(VERSION 3.12)
project(Colt-Cpp-Test LANGUAGES CXX VERSION 0.0.1.1)
# Set C++17 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Get the headers files and translation units
file(GLOB_RECURSE ColtCppHeaders CONFIGURE_DEPENDS "../include/*.h")
file(GLOB_RECURSE ColtCppUnits   CONFIGURE_DEPENDS "../include/*.cpp")
file(GLOB         ColtCppUnitsT  CONFIGURE_DEPENDS "../test/*.cpp")

message("")

message(STATUS "Importing scnlib...")
add_subdirectory("../libraries/scnlib" "scnlib")
message(STATUS "Finished importing scnlib!\n")

message(STATUS "Importing {fmt}...")
add_subdirectory("../libraries/fmt" "fmt")
message(STATUS "Finished importing {fmt}!\n")

add_executable(ColtCppTest
    "${ColtCppHeaders}" "${ColtCppUnits}" "${ColtCppUnitsT}"
)

target_link_libraries(
    ColtCppTest scn::scn fmt::fmt
)

if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ColtCppTest)
    target_compile_options(ColtCppTest PUBLIC
      "/external:anglebrackets" "/external:W0" #To remove warnings from <files>
      "/Zc:preprocessor" #Use conforming C++ preprocessor
    )
endif()

target_include_directories(ColtCppTest PUBLIC
  "../include/"
  "../libraries/fmt/include"
  "../libraries/scnlib/include"
)
