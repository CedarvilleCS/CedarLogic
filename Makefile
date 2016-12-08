
### [ App Name ] ====================================================

appName := moose



### [ Input/Ouput Paths ] ===========================================

srcDirRoot :=src/
srcDirCpp  :=$(srcDirRoot)
srcDirRes  :=res/

genDirRoot :=.gen/
genDirObj  :=$(genDirRoot)obj/
genDirRes  :=$(genDirRoot)res/
genDirDoc  :=$(genDirRoot)doc/

genApp     :=$(genDirRoot)$(appName)



### [ Compiler Flags ] ==============================================

cflags += -std=c++11
#cflags += -Wall
cflags += -I $(srcDirCpp) -I $(srcDirCpp)tool/
cflags += $(shell wx-config --cxxflags --libs all)


### [ Linker Flags ] ================================================

lflags += -lGL -lGLEW -lGLU







include .makefile
