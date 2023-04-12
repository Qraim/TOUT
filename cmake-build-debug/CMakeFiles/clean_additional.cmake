# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/Tout_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/Tout_autogen.dir/ParseCache.txt"
  "Tout_autogen"
  )
endif()
