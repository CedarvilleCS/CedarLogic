
cmake_minimum_required(VERSION 3.9)



# 
# This module defines:
# - import_library(...)
# - import_libraries(...)
# - copy_library(...)
# - copy_libraries(...)
# - install_library(...)
# - install_libraries(...)
# 



# 
# Import a dynamic/static/header-only library from binaries and/or
# include directory into a cmake library target.
# 
# Only the first execution for a library will be run.
# 
# Define before calling:
# 
# ${lib}_include  - Include directories (not required)
# ${lib}_src      - *.cpp or *.c (compile into dependent project)
# ${lib}_dynamic  - *.dll or *.so (required for dynamic libraries)
# ${lib}_static   - *.lib or *.a (required for shared(win32)/static libraries)
# ${lib}_linklibs - Extra libs (*.lib or *.a) for linker to lookup
# 
function(import_library lib)
    
    
    # Don't redefine.
    
    if (TARGET ${lib})
        message(STATUS "")
        message(STATUS "Skipping repeat import_library ${lib}")
        message(STATUS "")
    endif()
    
    
    # Make some shorthand.
    
    set(libInc ${${lib}_include})
    set(libSrc ${${lib}_src})
    set(libDyn ${${lib}_dynamic})
    set(libSta ${${lib}_static})
    set(libLink ${${lib}_linklibs})
    
    # Set defualts.
    
    if ("${libIns}" STREQUAL "")
        set(libIns "YES")
    endif()
    
    
    # Visual separation.
    
    message(STATUS "")
    message(STATUS "import_library ${lib}")
    
    
    # Decide shared/static, and make target.
    
    if (EXISTS ${libDyn})
        
        message(STATUS "${lib} is a dynamic library.")
        
        if (${WIN32})
            if (NOT EXISTS ${libSta})
                message(FATAL_ERROR "${lib} is missing an import library!")
            endif()
        endif()
        
        
        # Populate target.
        
        add_library(${lib} SHARED IMPORTED GLOBAL)
        
        set_property(TARGET ${lib}
            PROPERTY IMPORTED_LOCATION ${libDyn})
        
        if (${WIN32})
            set_property(TARGET ${lib}
                PROPERTY IMPORTED_IMPLIB ${libSta})
        endif()
        
    elseif (EXISTS ${libSta})
        
        message(STATUS "${lib} is a static library.")
        
        
        # Populate target.
        
        add_library(${lib} STATIC IMPORTED GLOBAL)
        
        set_property(TARGET ${lib}
            PROPERTY IMPORTED_LOCATION ${libSta})
    
    elseif (EXISTS ${libSrc})
        
        message(STATUS "${lib} is a source library.")
        
        
        # Populate target.
        
        add_library(${lib} INTERFACE IMPORTED GLOBAL)
        
        set_property(TARGET ${lib}
            PROPERTY INTERFACE_SOURCES ${libSrc})
        
    elseif (EXISTS ${libInc})
        
        message(STATUS "${lib} is a header-only library.")
        
        add_library(${lib} INTERFACE IMPORTED GLOBAL)
        
    else()
        
        message(FATAL_ERROR "${lib} has no dynamic/static/source/header components!")
        
    endif()
    
    
    # Add include directories to target.
    
    set_property(TARGET ${lib}
        PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${libInc})
    
    
    # Add link-interface libraries.
    
    set_property(TARGET ${lib}
        PROPERTY INTERFACE_LINK_LIBRARIES ${libLink})
    
endfunction()



# 
# Call import_library for each library parameter.
# 
function(import_libraries)
    foreach(lib IN LISTS ARGV)
        import_library(${lib})
    endforeach()
    message(STATUS "")
endfunction()


# 
# Copy lib to output directory for target after target is built.
# This does nothing when lib is not a dynamic library.
# 
# Define before calling:
# 
# ${lib}_install - YES by default. Set to NO to skip installing.
# ${lib}_dynamic - Library file to copy to output directory.
# ${lib}_depends - Extra library files to copy with main library file.
# 
function(copy_library target lib)
    
    
    # Shorthand.
    
    set(libIns ${${lib}_install})
    set(libDyn ${${lib}_dynamic})
    set(libDep ${${lib}_depends})
    
    
    # Defaults.
    
    if ("${libIns}" STREQUAL "")
        set(libIns "YES")
    endif()
    
    
    # Copy.
    
    if ("${libIns}" STREQUAL "YES")
        if (EXISTS "${libDyn}")
            add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E
                copy_if_different ${libDyn} ${libDep} "."
                COMMENT "Copying ${lib} to build directory...")
        endif()
    endif()

endfunction()


# 
# Call copy_library for each library parameter.
# 
function(copy_libraries target)
    foreach(lib IN LISTS ARGN)
        copy_library(${target} ${lib})
    endforeach()
endfunction()



# 
# Add install rules for lib.
# This does nothing when lib is not a dynamic library.
# 
# Define before calling:
# 
# ${lib}_install - YES by default. Set to NO to skip installing.
# ${lib}_dynamic - Library file to install.
# ${lib}_depends - Extra library files to install with main library file.
# 
function(install_library lib)
    
    
    # Shorthand.
    
    set(libIns ${${lib}_install})
    set(libDyn ${${lib}_dynamic})
    set(libDep ${${lib}_depends})
    
    
    # Defaults.
    
    if ("${libIns}" STREQUAL "")
        set(libIns "YES")
    endif()
    
    
    # Add install rules.
    
    if ("${libIns}" STREQUAL "YES")
        if (EXISTS "${libDyn}")
            install(FILES ${libDyn} ${libDep} DESTINATION ".")
        endif()
    endif()

endfunction()


# 
# Call install_library for each library parameter.
# 
function(install_libraries)
    foreach(lib IN LISTS ARGV)
        install_library(${lib})
    endforeach()
endfunction()

