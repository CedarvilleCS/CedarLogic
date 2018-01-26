
cmake_minimum_required(VERSION 3.9)



# 
# This module defines:
# - vs_source_groups(...)
# 



# 
# Add source groupings for visual studio.
# The root is given relative to top level CMakeLists.txt.
# 
function(vs_source_groups root)
    source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${ARGN})
endfunction()

