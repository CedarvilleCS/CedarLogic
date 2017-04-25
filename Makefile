
### [ App Name ] ====================================================

appName := CedarLogic



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

cflags += -std=c++14
cflags += -Wall
cflags += -I $(srcDirCpp)
cflags += $(shell wx-config --cxxflags)
cflags += -Wno-potentially-evaluated-expression

### [ Linker Flags ] ================================================

lflags += -lGL -lGLEW -lGLU $(shell wx-config --libs all)







include .makefile
