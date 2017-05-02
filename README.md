
# Important!!!

This readme has had a volatile history. If you use git to check-out an old version of CedarLogic, please
re-read the readme if compilation fails.

# CedarLogic

CedarLogic is Cedarville University's digital logic simulator.

# Getting Ready to Compile

1. Install `Visual Studio 2015` or newer.

2. Download `wxWidgets 3.1.0` source for windows. [link](http://www.wxwidgets.org/downloads/)

3. Extract wxWidgets to `C:\wxWidgets-3.1.0`. You can change this directory by setting
   the environment variable WXWIN.

4. Open an `x86 native tools` command prompt and run these commands.

		cd %WXWIN%\build\msw
		nmake /f makefile.vc               RUNTIME_LIBS=static
		nmake /f makefile.vc BUILD=release RUNTIME_LIBS=static

5. Install Wix with its Visual Studio Extentions. [link](http://wixtoolset.org/releases/)

6. Now you should be able to open CedarLogic in Visual Studio, build it in debug or release, and create an installer.

# For more information...

There are a bunch of working documents, some notes, the google docs for the help-menu surveys, and unused
assets in the CedarLogic google Team-Drive.