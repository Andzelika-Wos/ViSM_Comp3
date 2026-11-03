# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\ViSM_Comp3_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\ViSM_Comp3_autogen.dir\\ParseCache.txt"
  "ViSM_Comp3_autogen"
  )
endif()
