# CedarLogic
CedarLogic is Cedarville University's digital logic simulator.

# Getting Started
  1. Install Visual Studio 2015 (Community Edition is free)
  2. Download wxWidgets 2.8.12 source for Windows.
 3a. 3b is unnecessary if you install wxWidgets to
    * `c:\wxWidgets-2.8.12`.
 3b. Set Windows environment variables (assuming you install the binaries to `C:\`, if not adjust accordingly)
    * `WXADDITIONS = C:\wxWidgets-2.8.12\additions`
    * `WXWIN = C:\wxWidgets-2.8.12`
  4. Clone this repository.
  5. Copy `wxSetup\config.vc` (from the cloned repo) to `%WXWIN%\build\msw`.
  6. Copy `wxSetup\setup.h` (from the cloned repo) to `%WXWIN%\include\wx\msw`.
  7. Copy `wxSetup\window.cpp` (from the cloned repo) to `%WXWIN%\src\msw`
  8. Run `Developer Command Prompt for VS2015`, navigate to `%WXWIN%\build\msw`.
  9. Run `nmake -f makefile.vc`
 10. When that's done, you should be able to build and run CedarLogic in Visual Studio.
 11. If you see errors about glcanvas not being defined, delete your wxwidgets directory and restart.
