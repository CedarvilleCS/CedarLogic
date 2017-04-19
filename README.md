
# CedarLogic

CedarLogic is Cedarville University's digital logic simulator.

# Getting Ready to Compile

1. Install `Visual Studio 2015` or newer.

2. Download `wxWidgets 3.1.0` source for windows. [link](http://www.wxwidgets.org/downloads/)

3. Download `glew-2.0.0` binaries for windows. [link](http://glew.sourceforge.net/)

4. Extract wxWidgets to `C:\wxWidgets-3.1.0`. You can change this directory by setting
   the environment variable WXWIN.

5. Extract glew to `C:\glew-2.0.0`. You can change this directory by setting
   the environment variable GLEWWIN.

6. Open an `x86 native tools` command prompt and run these commands.

		cd %WXWIN%\build\msw
		nmake /f makefile.vc               RUNTIME_LIBS=static
		nmake /f makefile.vc BUILD=release RUNTIME_LIBS=static

7. Install Wix with its Visual Studio Extentions. [link](http://wixtoolset.org/releases/)

8. Now you should be able to open CedarLogic in Visual Studio, build it in debug or release, and create an installer.