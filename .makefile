# Makefile by tylerdrake.
# Use freely

### [ Source & Output Files ] =======================================

srcCpp := $(shell find $(srcDirCpp)  -type f -name '*.cpp')
srcRes := $(shell find $(srcDirRes)  -type f)

genDep := $(srcCpp:$(srcDirCpp)%.cpp=$(genDirObj)%.o.d)
genObj := $(srcCpp:$(srcDirCpp)%.cpp=$(genDirObj)%.o)
genRes := $(srcRes:$(srcDirRes)%=$(genDirRes)%)

genDir := $(sort $(dir $(genObj)) $(dir $(genRes)))



### [ Link-Time-Optimization(LTO) additional flags ] ================

LTO ?= 0
ifeq ($(LTO), 0)
	cflags += -g
else
	cflags += -O2 -flto
endif



### [ Compiler selection ] ==========================================

COMPILER ?= 0
ifeq ($(COMPILER), 0)
	cppcomp = g++
endif
ifeq ($(COMPILER), 1)
	cppcomp = clang++
endif



### [ Print macro ] =================================================

# Print Colors and macro.
heading :='\033[1;97m'
item    :='\033[1;34m'
default :='\033[0m'
print   = @echo -e '    ' $(item)$(1)$(default)



### [ Build order ] =================================================

# Prerequisite that prints as heading. headingBob prints 'Bob'.
.PHONY : heading%
heading% :
	@echo $(heading)[ $* ]$(default)

# This is the ordering of the steps in the makefile.
.DEFAULT : all
.PHONY : all
all : toDir resToRes cppToObj objToExe

# Step. Generate output directories.
.PHONY : toDir
toDir : headingDIR $(genDir)

# Step. Copy resources into build directory.
.PHONY : resToRes
resToRes : headingRES $(genRes)

# Step. Compile cpp code into obj files. Also make submakefiles.
.PHONY : cppToObj
cppToObj : headingCPP $(genObj)

# Step. Link obj files into an executable.
.PHONY : objToExe
objToExe : headingAPP $(genApp)



### [ Build rules & recipes ] =======================================

# Make directories.
$(genDir) :
	$(call print,$@)
	@mkdir $@ -p

# Make copies of resources
$(genDirRes)% : $(srcDirRes)%
	$(call print,$@)
	@cp $< $@
	
# Make objects from cpp source. Also make submakefiles
# NOTE: Submakefiles always list the *.cpp as the first dependency.
$(genObj) :
	$(call print,$@)
	@$(cppcomp) -MM -MT $@ $(@:$(genDirObj)%.o=$(srcDirCpp)%.cpp) $(cflags) > $@.d
	@$(cppcomp) -c  -o  $@ $(@:$(genDirObj)%.o=$(srcDirCpp)%.cpp) $(cflags)

# Make executable from objects.
$(genApp) : $(genObj)
	$(call print,$@)
	@$(cppcomp) $(genObj) $(cflags) $(lflags) -o $@


### [ Other rules & recipes ] =======================================

# Create doxygen documentation.
.PHONY : doc
doc : all headingDOC
	@rm $(genDirDoc) -f -r
	@doxygen .doxyfile

# Remove generated files.
.PHONY : clean
clean : headingCLEAN
	@rm $(genDirRoot) -f -r

# Build and run.
.PHONY : run
run : all headingRUN
	@(cd $(genDirRoot) && exec ./$(appName))



# Attach to submakefiles.
-include $(genDep)
