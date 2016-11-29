
# CedarLogic

CedarLogic is Cedarville University's digital logic simulator.

# Getting Ready to Compile

1. Install `Visual Studio 2015` or newer.

2. Download `wxWidgets 2.8.12` source for windows. [link](http://www.wxwidgets.org/downloads/)

3. Run wxWidgets source installer or extract zip.

4. Skip step 5 if you used the installer and did not change the extraction directory.

5. Set windows environment variable `WXWIN` to point to your wxWidgets directory.

6. Edit `%WXWIN%\include\wx\msw\setup.h` so that `wxUSE_GLCANVAS` is `1` instead of `0`.

7. Edit `%WXWIN%\src\msw\window.cpp` so that it does not include `pbt.h`.

8. Open an `x86 native tools` command prompt and navigate to `%WXWIN%\build\msw`

9. In the command prompt, run these commands.

        nmake /f makefile.vc               USE_OPENGL=1 RUNTIME_LIBS=static                  MONOLITHIC=1
        nmake /f makefile.vc BUILD=release USE_OPENGL=1 RUNTIME_LIBS=static                  MONOLITHIC=1

8. Open an `x64 native tools` command prompt and navigate to `%WXWIN%\build\msw`

9. In the command prompt, run these commands.

        nmake /f makefile.vc               USE_OPENGL=1 RUNTIME_LIBS=static TARGET_CPU=AMD64 MONOLITHIC=1
        nmake /f makefile.vc BUILD=release USE_OPENGL=1 RUNTIME_LIBS=static TARGET_CPU=AMD64 MONOLITHIC=1

10. Now you should be able to build CedarLogic debug and release versions for both 32- and 64-bit architectures.