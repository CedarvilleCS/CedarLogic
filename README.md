
# CedarLogic

CedarLogic is Cedarville University's digital logic simulator.

# Getting Ready to Compile

1. Install `Visual Studio 2015` or newer.

2. Download `wxWidgets 2.8.12` source for windows. [link](http://www.wxwidgets.org/downloads/)

3. Run wxWidgets source installer or extract zip.

4. Set windows environment variable `WXWIN` to point to your wxWidgets directory.

5. Edit `%WXWIN%\include\wx\msw\setup.h` so that `wxUSE_GLCANVAS` is `1` instead of `0`.

6. Edit `%WXWIN%\src\msw\window.cpp` so that it does not include `pbt.h`.

7. Open an `x86 native tools` command prompt and run these commands.
	
		cd %WXWIN%\build\msw
		nmake /f makefile.vc               USE_OPENGL=1 RUNTIME_LIBS=static
		nmake /f makefile.vc BUILD=release USE_OPENGL=1 RUNTIME_LIBS=static

8. Install Wix with its Visual Studio Extentions. [link](http://wixtoolset.org/releases/)

9. Now you should be able to open CedarLogic in Visual Studio, build it in debug or release, and create an installer.