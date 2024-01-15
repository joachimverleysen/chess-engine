# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/schaken_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/schaken_autogen.dir/ParseCache.txt"
  "schaken_autogen"
  )
endif()
