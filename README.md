
# CedarLogic

CedarLogic is Cedarville University's digital logic simulator.

# Getting Ready to Compile

1. Install `Visual Studio 2015` or newer.

2. Download `wxWidgets 3.1.0` source for windows. [link](http://www.wxwidgets.org/downloads/)

3. Run wxWidgets source installer or extract zip.

4. Set windows environment variable `WXWIN` to point to your wxWidgets directory.
   This step is not strictly necessary. Without WXWIN defined, the visual studio
   project file will look in `C:\wxWidgets-3.1.0`. It is, however, reccomended that
   you set this environment variable to make the next step (a little bit) easier.

5. Open an `x86 native tools` command prompt and run these commands.

		cd %WXWIN%\build\msw
		nmake /f makefile.vc               RUNTIME_LIBS=static
		nmake /f makefile.vc BUILD=release RUNTIME_LIBS=static

6. Install Wix with its Visual Studio Extentions. [link](http://wixtoolset.org/releases/)

7. Now you should be able to open CedarLogic in Visual Studio, build it in debug or release, and create an installer.