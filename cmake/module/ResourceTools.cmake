
cmake_minimum_required(VERSION 3.9)


# 
# This module defines:
# - install_resources(...)
# - copy_resources(...)
#



# 
# Install directory.
# Directory is relative to toplevel CMakeLists.txt.
# 
function(install_resources resDir)
    install(DIRECTORY "${CMAKE_SOURCE_DIR}/${resDir}" DESTINATION ".")	
endfunction()



# 
# Copy directory after target build.
# Directory is relative to toplevel CMakeLists.txt.
# 
function(copy_resources target resDir)
    
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
        copy_directory "${CMAKE_SOURCE_DIR}/${resDir}" "${resDir}"
        COMMENT "Copying ${resDir} to build directory...")

endfunction()

