# Note: Below 3.11 breaks the FetchContent capability used to pull in Catch2,
# the unit testing framework.
cmake_minimum_required(VERSION 3.11)

# Name project, and set version. If updating version, change it here, and then re-generate your Visual
# Studio solution.
project(CedarLogic VERSION 2.3.8)

# This configure file is where we store version info that's auto-populated by cmake configure.
# This file is not considered part of the source code and it's stored in the build directory
# It is made visible as an external header when we add CMAKE_CURRENT_BINARY_DIR (build) in 
# include_directories. This may seem odd, but it's how they do it in the official cmake tutorial.
configure_file(CedarLogic.h.in CedarLogic.h)

# CMake modules, that is, files specifying functions we can use in CMake
# each is a .cmake file in the CMAKE_MODULE_PATH and we include them below
# so we can use their functions later on.
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/module")
include(ResourceTools)
include(VsSourceGroups)

##
# Connect external libraries
##

# Use static runtime libs.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Linking to the system version of wxWidgets is usually preferred on linux, and
# it makes the build much faster.
if (CMAKE_SYSTEM_NAME STREQUAL Linux)
    set(USE_SYSWX_DEF TRUE)
else()
    set(USE_SYSWX_DEF FALSE)
endif()
set(USE_SYSTEM_WXWIDGETS "${USE_SYSWX_DEF}" CACHE BOOL
    "Use the system version of wxWidgets or build it from soruce")

if (USE_SYSTEM_WXWIDGETS)
    find_package(wxWidgets 3.2.0 COMPONENTS html aui adv gl core base REQUIRED)
    include("${wxWidgets_USE_FILE}")
else()
    include(FetchContent)

    FetchContent_Declare(
       wxWidgets_fetch
       GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
       GIT_TAG 97e99707c5d2271a70cb686720b48dbf34ced496 # 3.2.1
       GIT_SHALLOW 1
    )

    set(wxBUILD_INSTALL OFF)
    set(wxBUILD_SHARED OFF)
    set(wxUSE_GLCANVAS ON)
    set(wxBUILD_USE_STATIC_RUNTIME ON)
    set(wxUSE_OLE ON)  # Required to use wxUSE_DRAG_AND_DROP

    # Disabled because they break the windows build
    set(wxUSE_SOCKETS OFF)
    set(wxUSE_PROTOCOL OFF)
    set(wxUSE_URL OFF)
    set(wxUSE_FS_INET OFF)
    set(wxUSE_PROTOCOL_FTP OFF)
    set(wxUSE_PROTOCOL_HTTP OFF)

    FetchContent_MakeAvailable(wxWidgets_fetch)
    set(wxWidgets_LIBRARIES wx::html wx::aui wx::adv wx::gl wx::core wx::base)
endif()

find_package(OpenGL REQUIRED)

###
### Build Config.
###

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
target_link_libraries(CedarLogic PRIVATE 
    Logic
    ${OPENGL_opengl_LIBRARY}
    ${OPENGL_glu_LIBRARY}
    ${wxWidgets_LIBRARIES}
)

# This is for internal headers
include_directories(
	"${PROJECT_SOURCE_DIR}/include/"
	"${PROJECT_SOURCE_DIR}/include/gui/"
	"${PROJECT_SOURCE_DIR}/include/logic/"
	"${PROJECT_SOURCE_DIR}/include/gui/command/"
	"${PROJECT_SOURCE_DIR}/include/gui/GLFont/"
	"${CMAKE_CURRENT_BINARY_DIR}"
)

add_subdirectory(
    "${PROJECT_SOURCE_DIR}/logic/"
)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

target_compile_definitions(CedarLogic PRIVATE "_PRODUCTION_")

if (WIN32)
    target_compile_options(CedarLogic PRIVATE /MP)
    target_compile_definitions(CedarLogic PRIVATE
	"WIN32"
	"STRICT"
	"__WXMSW__"
	"__WX__"
	"__WXDEBUG__"
	"_WINDOWS"
	"_CRT_SECURE_NO_DEPRECATE"
    )
endif()


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
