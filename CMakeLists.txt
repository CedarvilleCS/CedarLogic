
cmake_minimum_required(VERSION 3.8)
project(CedarLogic)


set(CEDARLOGIC_MAJOR 2)
set(CEDARLOGIC_MINOR 3)
set(CEDARLOGIC_PATCH 7)

add_definitions(
	-DCEDARLOGIC_MAJOR=${CEDARLOGIC_MAJOR}
	-DCEDARLOGIC_MINOR=${CEDARLOGIC_MINOR}
	-DCEDARLOGIC_PATCH=${CEDARLOGIC_PATCH}
)


### 
### *.cpp and *.h files
### 
set(Sources
	icon.rc
	src/version.cpp
	src/version.h
	src/gui/autoSaveThread.cpp
	src/gui/autoSaveThread.h
	src/gui/CircuitParse.cpp
	src/gui/CircuitParse.h
	src/gui/commands.h
	src/gui/gateImage.cpp
	src/gui/gateImage.h
	src/gui/gl_defs.cpp
	src/gui/gl_defs.h
	src/gui/gl_wrapper.h
	src/gui/GUICanvas.cpp
	src/gui/GUICanvas.h
	src/gui/GUICircuit.cpp
	src/gui/GUICircuit.h
	src/gui/guiGate.cpp
	src/gui/guiGate.h
	src/gui/guiText.cpp
	src/gui/guiText.h
	src/gui/guiWire.cpp
	src/gui/guiWire.h
	src/gui/klsBBox.h
	src/gui/klsClipboard.cpp
	src/gui/klsClipboard.h
	src/gui/klsCollisionChecker.cpp
	src/gui/klsCollisionChecker.h
	src/gui/klsGLCanvas.cpp
	src/gui/klsGLCanvas.h
	src/gui/klsMessage.h
	src/gui/klsMiniMap.cpp
	src/gui/klsMiniMap.h
	src/gui/LibraryParse.cpp
	src/gui/LibraryParse.h
	src/gui/MainApp.cpp
	src/gui/MainApp.h
	src/gui/MainFrame.cpp
	src/gui/MainFrame.h
	src/gui/OscopeCanvas.cpp
	src/gui/OscopeCanvas.h
	src/gui/OscopeFrame.cpp
	src/gui/OscopeFrame.h
	src/gui/PaletteCanvas.cpp
	src/gui/PaletteCanvas.h
	src/gui/PaletteFrame.cpp
	src/gui/PaletteFrame.h
	src/gui/paramDialog.cpp
	src/gui/paramDialog.h
	src/gui/RamPopupDialog.cpp
	src/gui/RamPopupDialog.h
	src/gui/threadLogic.cpp
	src/gui/threadLogic.h
	src/gui/wireSegment.cpp
	src/gui/wireSegment.h
	src/gui/XMLParser.cpp
	src/gui/XMLParser.h
	src/gui/command/cmdAddTab.cpp
	src/gui/command/cmdAddTab.h
	src/gui/command/cmdConnectWire.cpp
	src/gui/command/cmdConnectWire.h
	src/gui/command/cmdCreateGate.cpp
	src/gui/command/cmdCreateGate.h
	src/gui/command/cmdCreateWire.cpp
	src/gui/command/cmdCreateWire.h
	src/gui/command/cmdDeleteGate.cpp
	src/gui/command/cmdDeleteGate.h
	src/gui/command/cmdDeleteSelection.cpp
	src/gui/command/cmdDeleteSelection.h
	src/gui/command/cmdDeleteTab.cpp
	src/gui/command/cmdDeleteTab.h
	src/gui/command/cmdDeleteWire.cpp
	src/gui/command/cmdDeleteWire.h
	src/gui/command/cmdDisconnectWire.cpp
	src/gui/command/cmdDisconnectWire.h
	src/gui/command/cmdMoveGate.cpp
	src/gui/command/cmdMoveGate.h
	src/gui/command/cmdMoveSelection.cpp
	src/gui/command/cmdMoveSelection.h
	src/gui/command/cmdMoveWire.cpp
	src/gui/command/cmdMoveWire.h
	src/gui/command/cmdPasteBlock.cpp
	src/gui/command/cmdPasteBlock.h
	src/gui/command/cmdSetParams.cpp
	src/gui/command/cmdSetParams.h
	src/gui/command/cmdWireSegDrag.cpp
	src/gui/command/cmdWireSegDrag.h
	src/gui/command/klsCommand.cpp
	src/gui/command/klsCommand.h
	src/gui/GLFont/glfont2.cpp
	src/gui/GLFont/glfont2.h
	src/logic/logic_circuit.cpp
	src/logic/logic_circuit.h
	src/logic/logic_defaults.h
	src/logic/logic_event.cpp
	src/logic/logic_event.h
	src/logic/logic_gate.cpp
	src/logic/logic_gate.h
	src/logic/logic_junction.cpp
	src/logic/logic_junction.h
	src/logic/logic_values.h
	src/logic/logic_wire.cpp
	src/logic/logic_wire.h
)


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
set(CPACK_PACKAGE_VERSION_MAJOR "${CEDARLOGIC_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${CEDARLOGIC_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${CEDARLOGIC_PATCH}")
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
