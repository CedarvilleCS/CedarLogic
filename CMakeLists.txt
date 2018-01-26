
cmake_minimum_required(VERSION 3.8)
project(CedarLogic)


### 
### Modules.
### 

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/module")
include(LibraryTools)
include(ResourceTools)
include(VsSourceGroups)
include(MsStaticRuntimeLibs)


### 
### Libraries.
### 

# Use static runtime libs.
ms_static_runtime_libs()

# Read environment variable, but if it is not set, use installer default location.
set(wxwin "$ENV{WXWIN}")
if ("${wxwin}" STREQUAL "")
    set(wxwin "C:/wxWidgets-2.8.12")
endif()

# Set special variables read-in by functions from LibraryTools module.
set(wx_widgets_include
    "${wxwin}/include"
)
set(wx_widgets_linklibs
    "comctl32.lib"
    "rpcrt4.lib"
    "winmm.lib"
    "wsock32.lib"
    "opengl32.lib"
    "glu32.lib"
)

# Use the variables above to define the cmake library targets.
set(Libraries
    wx_widgets
)
import_libraries(${Libraries})


###
### Build Config.
###

file(GLOB_RECURSE Sources "src/*.cpp" "src/*.h")

link_directories("${wxwin}/lib/vc_lib")

add_executable(CedarLogic WIN32 ${Sources})

target_link_libraries(CedarLogic
    PRIVATE ${Libraries})

target_include_directories(CedarLogic
    PRIVATE
        src
        "${wxwin}/include/msvc"
        "${wxwin}/contrib/include"
)

target_compile_features(CedarLogic
    PRIVATE cxx_std_11)

target_compile_options(CedarLogic
    PRIVATE
        /FI"${wxwin}/include/msvc/wx/setup.h"
        /FI"${wxwin}/include/wx/wxprec.h"
)

target_compile_definitions(CedarLogic
    PRIVATE
        "_PRODUCTION_"
        "WIN32"
        "STRICT"
        "__WXMSW__"
        "__WX__"
        "__WXDEBUG__"
        "_WINDOWS"
        "_CRT_SECURE_NO_DEPRECATE"
)

###
### Dev Setup.
###

copy_resources(CedarLogic res)
vs_source_groups(src ${Sources})


###
### Installation.
###

install(TARGETS CedarLogic RUNTIME DESTINATION ".")

install_resources(res)

set(CPACK_PACKAGE_NAME "CedarLogic")
set(CPACK_PACKAGE_EXECUTABLES ..\\\\CedarLogic "CedarLogic")
set(CPACK_PACKAGE_VENDOR "Cedarville University")
set(CPACK_PACKAGE_VERSION_MAJOR "2")
set(CPACK_PACKAGE_VERSION_MINOR "3")
set(CPACK_PACKAGE_VERSION_PATCH "3")
set(CPACK_PACKAGE_ICON ${CMAKE_SOURCE_DIR}/cmake\\\\InstallSmall.bmp)
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE.txt)
set(CPACK_NSIS_MUI_ICON ${CMAKE_SOURCE_DIR}/cmake\\\\icon.ico)
set(CPACK_NSIS_MUI_WELCOMEFINISHPAGE_BITMAP ${CMAKE_SOURCE_DIR}/cmake\\\\InstallLarge.bmp)
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)

include(CPack)
