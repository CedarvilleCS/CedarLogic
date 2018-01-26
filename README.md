
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

# Compiling and creating an installer.

1. Go to the root of the git repo.

2. Run `mkdir build && cd build && cmake .. "Visual Studio 15 2017"`.

3. There is now a visual studio solution in the `build` directory. Open the solution file.

4. Right-click on the project titled `CedarLogic` and select `Set as Startup Project`.

5. Select `Release` from the configuration menu.

6. Right click on the `Package` target to create a new installer in the `build` directory.

7. There is now a CedarLogic installer executable in the `build` directory.

# Version Summaries.

Run `git tag` to view all the tagged versions of CedarLogic.

v2.2017.01.16 - Buggy first release with busses.
v2.2017.01.18 - Fixed bugs.
v2.2017.05.02 - Borken version with lots of good, but enough bad to warrant abandonment.
v2.2018.01.26 - Branch from v2.2017.01.18, stable bus support with an installer and file associations.
