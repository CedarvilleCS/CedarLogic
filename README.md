
# CedarLogic
## About
CedarLogic is a digital logic simulator often used in university classroom instruction. It includes all the basic gates, buses, JK and D flip flops, muxes, decoders, and a Z80 processor. At [Cedarville University](https://www.cedarville.edu/) it has been used by Computer Architecture 1 students to build and simulate a full [mano-machine](https://en.wikipedia.org/wiki/Mano_machine). 

It has recently been open sourced and is being actively maintained by @joe-sonrichard. Please feel free to contribute tickets and pull requests.

## History
CedarLogic was developed by [Cedarville University](https://www.cedarville.edu/) professors for classroom instruction. It was released on [Source Forge](https://sourceforge.net/projects/cedarlogic/) in 2006 where  it's enjoyed long-lived popularity, over 20,000 downloads in 2019 alone.

The latest version availible on Source Forge was released in 2011/12 and has been the public face of CedarLogic. Internally, senior design teams took a couple shots at improving it with added features, unfortunately accompanied by added bugs.

In February 2020 Cedarville Unversity agreed to open source the project with @joe-sonrichard heading up  the project. As of this writing, the chief objective is to fix any known bugs and stabalize the product before considering further improvements.

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

1. First, you need to install NSIS.

2. Next, go to the root of the git repo.

3. Run `mkdir build && cd build && cmake .. "Visual Studio 15 2017"`.

4. There is now a visual studio solution in the `build` directory. Open the solution file.

5. Right-click on the project titled `CedarLogic` and select `Set as Startup Project`.

6. Select `Release` from the configuration menu.

7. Right click on the `Package` target to create a new installer in the `build` directory.

8. There is now a CedarLogic installer executable in the `build` directory.

# Version Summaries.

Run `git tag` to view all the tagged versions of CedarLogic.

**NOTE:** Old versions of this project are availible on [source forge](https://sourceforge.net/projects/cedarlogic/)

v2.2017.01.16 - Buggy first release with busses.

v2.2017.01.18 - Fixed bugs.

v2.2017.05.02 - Borken version with lots of good, but enough bad to warrant abandonment.

v2.2018.01.26 - Branch from v2.2017.01.18, stable bus support with an installer and file associations.

v2.3.5 - Patch release to re-enable grayscale printing.
