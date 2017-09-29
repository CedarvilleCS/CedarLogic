
# Tyler Drake here,
# This cmake project is strictly an installer using CPack.
# I used CPack because I am familiar with it.
# I wasn't familiar with cmake or cpack during senior design.
# You should build cedar logic using the visual studio solution file.

cmake_minimum_required(VERSION 3.9)
project(CedarLogicInstaller)

install(DIRECTORY "res" DESTINATION ".")
install(FILES "Debug/CedarLogic.exe" DESTINATION ".")

include(CPack)