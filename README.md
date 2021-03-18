
# About CedarLogic
CedarLogic is a digital logic simulator often used in university classroom instruction. It includes all the basic gates, buses, JK and D flip flops, muxes, decoders, and a [Z80 micro-processor](https://en.wikipedia.org/wiki/Zilog_Z80). At [Cedarville University](https://www.cedarville.edu/) it has been used by Computer Architecture 1 students to build and simulate a full [mano-machine](https://en.wikipedia.org/wiki/Mano_machine). 

Please feel free to contribute tickets and pull requests. Currently the only maintainer is @joe-sonrichard.

## History
CedarLogic was developed by [Cedarville University](https://www.cedarville.edu/) professors for classroom instruction. It was released on [Source Forge](https://sourceforge.net/projects/cedarlogic/) in 2006 where  it's enjoyed long-lived popularity, over 20,000 downloads in 2019 alone.

The latest version availible on Source Forge was released in 2011/12 and has been the public face of CedarLogic. That [version](https://sourceforge.net/projects/cedarlogic/files/) is still by far the most stable version of CedarLogic. Internally, senior design teams took a couple shots at improving it with added features, unfortunately accompanied by added bugs. The code-base at present is the has their improvements but also the accompanying instabilities.

In February 2020 Cedarville Unversity agreed to move it to GitHub with @joe-sonrichard heading up  the project. As of this writing, the chief objective is to fix any known bugs and stabalize the product before considering further improvements.

### Original Developers
 - [Dr. Keith Shomper](https://www.cedarville.edu/Academic-Schools-and-Departments/Engineering-and-Computer-Science/Faculty/Faculty/Shomper-Keith.aspx) 
 - [Matt Lewellyn](https://github.com/guruofgentoo)
 - [Benjamin Sprague](https://github.com/realmadsci)
 - [Dr. Clint Kohl](https://www.cedarville.edu/Academic-Schools-and-Departments/Engineering-and-Computer-Science/Faculty/Faculty/Kohl-Clinton.aspx)
 
 
# Build Instructions
## Getting Ready to Compile
**TODO:** Automate build process so this is antiquated (infrastructure as code)

1. Install `Visual Studio 2015` or newer. `Visual Studio 2019 Community Edition` is known to work as well.

2. Download [wxWidgets 2.8.12](https://github.com/wxWidgets/wxWidgets/releases/download/v2.8.12/wxMSW-2.8.12-Setup.exe) source for windows.

3. Run wxWidgets source installer.

4. (Optional) [Set windows environment variable](https://www.onmsft.com/how-to/how-to-set-an-environment-variable-in-windows-10) `WXWIN` to point to your wxWidgets directory.

5. Edit `%WXWIN%\include\wx\msw\setup.h` so that `wxUSE_GLCANVAS` is `1` instead of `0`.

6. Edit `%WXWIN%\src\msw\window.cpp` so that it does not include `pbt.h`.

7. Open an `x86 native tools` command prompt (like [Powershell](https://docs.microsoft.com/en-us/powershell/)) and run [nmake](https://docs.microsoft.com/en-us/cpp/build/reference/nmake-reference):
	
```PS
cd %WXWIN%\build\msw
nmake /f makefile.vc               USE_OPENGL=1 RUNTIME_LIBS=static
nmake /f makefile.vc BUILD=release USE_OPENGL=1 RUNTIME_LIBS=static
```

## Compiling and creating an installer.

1. First, you need to install [NSIS](https://nsis.sourceforge.io/Download).

2. Next, go to the root of the CedarLogic git repo ([Clone](https://www.git-scm.com/docs/git-clone)) it if you haven't already).

3. Run `mkdir build && cd build`

3. Run `cmake .. -A Win32` (If you did not set the windows environment variable earlier, also add `-Dwxwin=C:/wxWidgets-2.8.12` to set `wxwin` variable to the base of the wxWidgets install path).

4. There is now a visual studio solution in the `build` directory. Open the solution file.

5. Right-click on the project titled `CedarLogic` and select `Set as Startup Project`.

6. Select `Release` from the configuration menu.

7. Right click on the `Package` target to create a new installer in the `build` directory.

8. There is now a CedarLogic installer executable in the `build` directory.

# Version Summaries.

Run `git tag` to view all the tagged versions of CedarLogic.

**NOTE:** Old versions of this project are availible on [source forge](https://sourceforge.net/projects/cedarlogic/). The latest version on source forge is notable as it was stable though missing some later features.

v2.2017.01.16 - Buggy first release with busses.

v2.2017.01.18 - Fixed bugs.

v2.2017.05.02 - Borken version with lots of good, but enough bad to warrant abandonment.

v2.2018.01.26 - Branch from v2.2017.01.18, stable bus support with an installer and file associations.

v2.3.5 - Patch release to re-enable grayscale printing.
