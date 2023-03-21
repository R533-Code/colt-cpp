cmake_minimum_required(VERSION 3.12)
project(Colt-Cpp-Test LANGUAGES CXX VERSION 0.0.0.0)
# Set C++17 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Get the headers files and translation units
file(GLOB_RECURSE ColtCppHeaders CONFIGURE_DEPENDS "include/*.h")
file(GLOB_RECURSE ColtCppUnits CONFIGURE_DEPENDS "include/*.cpp")
file(GLOB_RECURSE ColtCppUnitsT CONFIGURE_DEPENDS "test/*.cpp")

add_executable(ColtCppTest
"${ColtCppHeaders}" "${ColtCppUnits}" "${ColtCppUnitsT}"
)

if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ColtCppTest)
    target_compile_options(ColtCppTest PUBLIC
      "/external:anglebrackets" "/external:W0" #To remove warnings from <files>
      "/Zc:preprocessor" #Use conforming C++ preprocessor
    )
endif()

target_include_directories(ColtCppTest PUBLIC "include/")

function(create_zip output_file input_files working_dir)
    add_custom_command(
        COMMAND ${CMAKE_COMMAND} -E tar "cf" "${output_file}" --format=zip -- ${input_files}
        WORKING_DIRECTORY "${working_dir}"
        OUTPUT  "${output_file}"
        DEPENDS ${input_files}
        COMMENT "Zipping to ${output_file}."
    )
endfunction()