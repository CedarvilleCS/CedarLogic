
# Note: updating to 3.15 breaks ms_static_runtime_libs or something around it such that we get a bunch of
# linker errors where the MT and MD mismatch.
cmake_minimum_required(VERSION 3.8)

# Name project, and set version. If updating version, change it here, and then re-generate your Visual
# Studio solution.
project(CedarLogic VERSION 2.3.7)

# This configure file is where we store version info that's auto-populated by cmake configure.
# This file is not considered part of the source code and it's stored in the build directory
# It is made visible as an external header when we add PROJECT_BINARY_DIR (build) in 
# target_include_directories. This may seem odd, but it's how they do it in the official cmake tutorial.
configure_file(CedarLogic.h.in CedarLogic.h)

# CMake modules, that is, files specifying functions we can use in CMake
# each is a .cmake file in the CMAKE_MODULE_PATH and we include them below
# so we can use their functions later on.
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/module")
include(LibraryTools) 
include(ResourceTools)
include(VsSourceGroups)
include(MsStaticRuntimeLibs)

##
# Connect external libraries
##

# Use static runtime libs.
ms_static_runtime_libs()

# Read environment variable, but if it is not set, use installer default location.
if ("$ENV{WXWIN}" STREQUAL "")
    set(ENV{WXWIN} "C:/wxWidgets-2.8.12")
endif()
set(wxwin "$ENV{WXWIN}" CACHE STRING "Path to wxWidgets install folder.")

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

link_directories("${wxwin}/lib/vc_lib")

# Glob all the source files to be added when CedarLogic executable is added later.
file(GLOB SRC_FILES 
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/gui/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/logic/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/gui/command/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/gui/GLFont/*.cpp"
	"${PROJECT_SOURCE_DIR}/include/*.h"
	"${PROJECT_SOURCE_DIR}/include/gui/*.h"
	"${PROJECT_SOURCE_DIR}/include/logic/*.h"
	"${PROJECT_SOURCE_DIR}/include/gui/command/*.h"
	"${PROJECT_SOURCE_DIR}/include/gui/GLFont/*.h"
)

add_executable(CedarLogic WIN32 "${SRC_FILES}")

# Add the external libraries for CedarLogic
target_link_libraries(CedarLogic
    PRIVATE ${Libraries})

# This is for internal headers
include_directories(	
	"${PROJECT_SOURCE_DIR}/include/"
	"${PROJECT_SOURCE_DIR}/include/gui/"
	"${PROJECT_SOURCE_DIR}/include/logic/"
	"${PROJECT_SOURCE_DIR}/include/gui/command/"
	"${PROJECT_SOURCE_DIR}/include/gui/GLFont/"
)

# These are for external headers
# Including PROJECT_BINARY_DIR gets CedarLogic.h with the version info.
target_include_directories(CedarLogic
    PRIVATE
        "${PROJECT_BINARY_DIR}"
        "${wxwin}/include/msvc"
        "${wxwin}/contrib/include"
)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

target_compile_options(CedarLogic
    PRIVATE
        /FI"${wxwin}/include/msvc/wx/setup.h"
        /FI"${wxwin}/include/wx/wxprec.h"
        /MP
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
# This exceptionally weird line is hard to understand even with the documentation.
# CMake thinks my executable is in the 'bin' subdirectory of the installation folder.
# I don't like to put it there, so I need to manually tell it my programs are up one level.
# Also, the combination of CMAKE and NSIS requires quadruple backslashes sometimes :(
# UPDATE: You can change 'bin' to '.' by setting  CPACK_NSIS_EXECUTABLES_DIRECTORY.
set(CPACK_PACKAGE_EXECUTABLES ..\\\\CedarLogic "CedarLogic" ..\\\\Uninstall "Uninstall CedarLogic")
set(CPACK_PACKAGE_VENDOR "Cedarville University")
# set(CPACK_PACKAGE_VERSION_MAJOR "${CEDARLOGIC_MAJOR}")
# set(CPACK_PACKAGE_VERSION_MINOR "${CEDARLOGIC_MINOR}")
# set(CPACK_PACKAGE_VERSION_PATCH "${CEDARLOGIC_PATCH}")
set(CPACK_PACKAGE_ICON ${CMAKE_SOURCE_DIR}/cmake\\\\InstallSmall.bmp)
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/res/LICENSE.txt)
set(CPACK_NSIS_MUI_ICON ${CMAKE_SOURCE_DIR}/cmake\\\\icon.ico)
set(CPACK_NSIS_MUI_WELCOMEFINISHPAGE_BITMAP ${CMAKE_SOURCE_DIR}/cmake\\\\InstallLarge.bmp)
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS
    "!include \\\"${CMAKE_SOURCE_DIR}/cmake/cpack\\\\FileAssociation.nsh\\\"
    \\\${registerExtension} \\\"$INSTDIR\\\\CedarLogic.exe\\\" \\\".cdl\\\" \\\"CedarLogic Project\\\""
)
set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS
    "\\\${unregisterExtension} \\\".cdl\\\" \\\"CedarLogic Project\\\""
)

include(CPack)
