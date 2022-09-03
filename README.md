
# Summary

CedarLogic is a digital logic simulator made for university classroom instruction. It includes all the basic gates, buses, JK and D flip flops, muxes, decoders, and a [Z80 micro-processor](https://en.wikipedia.org/wiki/Zilog_Z80). At [Cedarville University](https://www.cedarville.edu/) it has been used by Computer Architecture 1 students to build and simulate a full [mano-machine](https://en.wikipedia.org/wiki/Mano_machine). 

Please feel free to contribute tickets and pull requests.

## Build Instructions for Windows

Linux builds are possible, but it is painful, and a succesful route is not documented at present.

### Required Programs

- [ ] [Visual Studio](https://visualstudio.microsoft.com/downloads/) 2015 or newer. Used to build and edit code.
- [ ] [CMake](https://cmake.org/download/) used to generate Visual Studio build file from CMake cross-platform build definition.

### Build Dependencies

- Install [NSIS](https://nsis.sourceforge.io/Download) (used to build installer)

### Build wxWidgets 2.8.12

1. Download [wxWidgets 2.8.12](https://github.com/wxWidgets/wxWidgets/releases/download/v2.8.12/wxMSW-2.8.12-Setup.exe) source for windows.
1. Run wxWidgets source installer.
1. (Optional) [Set windows environment variable](https://www.onmsft.com/how-to/how-to-set-an-environment-variable-in-windows-10) `WXWIN` to point to your wxWidgets directory.
1. Edit `%WXWIN%\include\wx\msw\setup.h` so that `wxUSE_GLCANVAS` is `1` instead of `0`.
1. Edit `%WXWIN%\src\msw\window.cpp` so that it does not include `pbt.h`.
1. Open the Developer Command Prompt for Visual Studio - not Powershell. If Visual Studio is installed, this is installed too. If not, google.
(NOT cmd or Powershell!) and run [nmake](https://docs.microsoft.com/en-us/cpp/build/reference/nmake-reference):
	
```PS
cd %WXWIN%\build\msw
nmake /f makefile.vc               USE_OPENGL=1 RUNTIME_LIBS=static
nmake /f makefile.vc BUILD=release USE_OPENGL=1 RUNTIME_LIBS=static
```

### Build CedarLogic Installer

1. Clone the CedarLogic git repo ([clone](https://www.git-scm.com/docs/git-clone) it if you haven't already.

3. In the repo root, make the build directory: `mkdir build && cd build`. (If you are unfamiliar with this ecosystem, yes you do need this, the alternative makes a massive mess)

3. Run `cmake .. -A Win32` (If you did not set the windows environment variable earlier, also add `-Dwxwin=C:/wxWidgets-2.8.12` to set `wxwin` variable to the base of the wxWidgets install path).

4. There is now a Visual Studio Solution File in the `build` directory. Open the solution file with Visual Studio.

5. Select the `CedarLogic` solution (there are many)

6. Select `Release` from the configuration menu.

7. Right click on the `PACKAGE` target and choose `Build` to create a new installer in the `build` directory.

8. There is now a CedarLogic installer executable in the `build` directory. If you run
the installer, you will have the latest semi-stable version of CedarLogic installed. See
the next section for development.

## Developing CedarLogic

Assuming you've gone through all the steps to build this for Windows, you will want to
develop this awesome app as well. To develop, you need a feedback loop. That is, a way
to make a change, and see if that change helped. Short feedback loops are essential and 
the driving variable in your developer velocity. (and in developer velocity, velocity and quality go together, they are not opposed, see the book _Accelerate_)

It is time-intensive to re-install CedarLogic each time you wish to test a code change. 
There is also an executable in the `build/<whatever build type you picked, like Release>`
folder. That executable would run, except a library or two aren't in the correct 
relative paths for it to do so. You can reconstruct the correct dependency relationships
by copying everything from the CedarLogic program directory (where it was installed) 
into your `build/Release` or `build/Debug` folders. As of this writing, that is just the `res` directory. With the `res` directory copied into `build/Release`, that CedarLogic executable should run.

You can do the same thing for `Debug` builds and the like.

You can also build many other solutions, which come with cool things like the ability 
to unit test portions of the code, and whatnot. We are trying to break the code apart
into multiple libraries that do not share memory to make reasoning about the code and
updating it and testing it much easier.

Here is a table of what each solution within the CedarLogic solution you opened does:

| Name | Purpose |
| :--- | :------ | 
| ALL_BUILD | Does what the name implies, probably builds everything. TODO: verify |
| Catch2 | Builds the Catch2 unit test framework which is brought in via CMake's attempt at a module-ish build-time dependency pull system. It's a dependency for unit tests, not something we modify. |
| Catch2WithMain | Another piece of Catch2 |
| CedarLogic | This creates a CedarLogic executable in `build/Release` or `/Debug`, whatever you've picked. |
| INSTALL | Attempts to install CedarLogic on your computer. |
| Logic | Builds the new logic core, which is not yet integrated with the rest of CedarLogic |
| PACKAGE | Builds the installer |
| test_logic | Builds the new logic core's unit tests, which run as an executable and report back the result. This uses Catch2. Note, you'll want to run the test_logic executable in a terminal or you'll lose the results in an instant. |
| XMLParser | Is a step towards breaking the XML parsing out into it's own library, with an identifiable interface so we can swap out serialization backends without worrying about what else we broke. There are good reasons to even hope for a day when we don't use XML at all and switch to JSON or YAML. |
| ZERO_CHECK | This may be a null operation. If memory serves rightly, CMake on Windows at least automatically creates all the all-caps build targets, and this may be a target that exists but is useless. |

If you want to debug CedarLogic and do so prettily, do a `Debug` type of build, and 
then look up how to set the Visual Studio settings to run the CedarLogic.exe you built
from the debug button.

## History

CedarLogic was developed by [Cedarville University](https://www.cedarville.edu/) professors for classroom instruction. It was released on [Source Forge](https://sourceforge.net/projects/cedarlogic/) in 2006 where  it's enjoyed long-lived popularity, over 20,000 downloads in 2019 alone.

The latest version available on Source Forge was released in 2011/12 and has been the public face of CedarLogic. That [version](https://sourceforge.net/projects/cedarlogic/files/) is still by far the most stable version of CedarLogic. Internally, senior design teams took a couple shots at improving it with added features, unfortunately accompanied by added bugs. The code-base at present has their improvements but also the accompanying instabilities.

The chief objective of development at this point is to fix any known bugs and stabilize the product before considering further improvements.

### Original Developers
 - [Dr. Keith Shomper](https://www.cedarville.edu/Academic-Schools-and-Departments/Engineering-and-Computer-Science/Faculty/Faculty/Shomper-Keith.aspx) 
 - [Matt Lewellyn](https://github.com/guruofgentoo)
 - [Benjamin Sprague](https://github.com/realmadsci)
 - [Dr. Clint Kohl](https://www.cedarville.edu/Academic-Schools-and-Departments/Engineering-and-Computer-Science/Faculty/Faculty/Kohl-Clinton.aspx)

## Version Summaries.

Run `git tag` to view all the tagged versions of CedarLogic.

**NOTE:** Old versions of this project are availible on [source forge](https://sourceforge.net/projects/cedarlogic/). The latest version on source forge is notable as it was stable though missing some later features.

v2.2017.01.16 - Buggy first release with busses.

v2.2017.01.18 - Fixed bugs.

v2.2017.05.02 - Broken version with lots of good, but enough bad to warrant abandonment.

v2.2018.01.26 - Branch from v2.2017.01.18, stable bus support with an installer and file associations.

v2.3.5 - Patch release to re-enable grayscale printing.
