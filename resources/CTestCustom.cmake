file (STRINGS "@CMAKE_BINARY_DIR@/CTestTestfile.cmake" LINES)
file(WRITE "@CMAKE_BINARY_DIR@/CTestTestfile.cmake" "")

foreach(LINE IN LISTS LINES)
  string(REGEX REPLACE ".*simdutf.*" "" STRIPPED "${LINE}")
  file(APPEND "@CMAKE_BINARY_DIR@/CTestTestfile.cmake" "${STRIPPED}\n")
endforeach()