StroikaRoot	:=	$(abspath ../../../../)/
SrcDir		:=	$(StroikaRoot)Library/Sources/Stroika/Foundation/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/

ifneq ($(CONFIGURATION),)
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),clobber)
ifeq ($(wildcard $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk),)
$(shell $(MAKE) --directory $(StroikaRoot) --no-print-directory CONFIGURATION=$(CONFIGURATION) --silent apply-configuration)
endif
endif
endif
endif

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk


VPATH		=	$(SrcDir) $(StroikaLibDir)

SubDirs:=				\
	Cache				\
	Characters			\
	Common				\
	Configuration		\
	Containers			\
	Cryptography		\
	Database			\
	DataExchange		\
	Debug				\
	Execution			\
	IO					\
	Linguistics			\
	Math				\
	Memory				\
	Streams				\
	Time				\
	Traversal			\


CachedOBJSFile_ = $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/cached-list-objs

#
# note also cannot just invoke
#		Objs := $($(MAKE) cached-list-objs > /dev/null 2>&1 ; cat cached-list-objs ))
# cuz results in recursive invoke of list cached-list-objs
# So do redirect to the file unconditionally if it doesn't exist
#ifneq ($(shell if [ -e $(CachedOBJSFile_) ]; then echo true; fi),true)
# If cache file doesn't exist, create it, and then use it to cache the list of objs associated with Stroika-Foundation
# COULD have rule with dependencies, but thats harder, and list should change very rarely, and easy to just make clean in
# that case
# 
# Tried using $(file < $(CachedOBJSFile_)), but that errored out sometimes, nad never debugged why.
#
#		-- LGP 2019-10-27
#
# Sadly none of this wildcard or file stuff appears to work right on very old gnu make, which is
# what comes with macos. Not sure what #s work/fail, but seems to work with 4.2.1 or later, and fail with
# 3.81
#
NEED_GNU_MAKE_ := 4.0.0
OK_GNU_MAKE_VERSION_ := $(filter $(NEED_GNU_MAKE_),$(firstword $(sort $(MAKE_VERSION) $(NEED_GNU_MAKE_))))

ifeq ($(OK_GNU_MAKE_VERSION_),)
# REDIRECT to avoid: warning: jobserver unavailable: using -j1.  Add '+' to parent make rule.; not good solution cuz hides errors
Objs := $(foreach dir, $(SubDirs), $(shell $(MAKE) --directory $(dir) CONFIGURATION=$(CONFIGURATION) -s list-objs 2>/dev/null))
else
ifeq ($(wildcard $(CachedOBJSFile_)),)
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),clobber)
$(shell $(StroikaRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Computing `$(StroikaRoot)Build/Scripts/SubstituteBackVariables $(CachedOBJSFile_)`" 1>&2)
$(shell mkdir -p $(shell dirname $(CachedOBJSFile_)) )
# REDIRECT to avoid: warning: jobserver unavailable: using -j1.  Add '+' to parent make rule.; not good solution cuz hides errors
ifneq ($(findstring $(DETECTED_HOST_OS),MSYS-Cygwin),)
$(file > $(CachedOBJSFile_),$(shell cygpath --mixed $(foreach dir, $(SubDirs), $(shell $(MAKE) --directory $(dir) CONFIGURATION=$(CONFIGURATION) -s list-objs 2>/dev/null))))
else
$(file > $(CachedOBJSFile_),$(foreach dir, $(SubDirs), $(shell $(MAKE) --directory $(dir) CONFIGURATION=$(CONFIGURATION) -s list-objs 2>/dev/null)))
endif
endif
endif
endif
ifneq ($(wildcard $(CachedOBJSFile_)),)
Objs := $(shell cat $(CachedOBJSFile_))
endif
endif

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


$(Objs):	all_objs_


$(ObjDir)%${OBJ_SUFFIX} : %.cpp
	@#suppress normal rule so just all_objs_ fires


.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s


# @todo fix - this iterates over all the subdirs calling make, which is a little slow and perhaps unneeded -- LGP 2019-10-27
.PHONY: all_objs_
all_objs_:
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Stroika Foundation Objs {$(CONFIGURATION)}:";
	@$(MAKE) --no-print-directory -s $(SubDirs) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))


# @todo the reason the .lib file isn't rebuilt properly sometimes is that this rule doesn't
# trigger, probably because in this makefile, we don't get to see the dates changes on the obj files after the all_objs_ rule runs? and
# unlike normal makefiles, we dont see the dependencies in this makefile - just sub-makes (to a degree).
$(StroikaFoundationLib):	$(Objs)
	@mkdir -p $(StroikaLibDir)
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Creating `$(StroikaRoot)Build/Scripts/SubstituteBackVariables $@` ... "
	@rm -f $@
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then \
		$(StroikaRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+2)) '$(call DEFAULT_LIBRARY_GEN_LINE,$@,$(Objs))';\
	fi
	@$(call DEFAULT_LIBRARY_GEN_LINE,$@,$(Objs))




all:	$(StroikaFoundationLib)


check:
	@$(StroikaRoot)Build/Scripts/CheckFileExists $(StroikaFoundationLib)
	@$(ECHO) "[SUCCEEDED]"


clean::
ifeq ($(CONFIGURATION),)
	@rm -rf $(StroikaRoot)IntermediateFiles/*/Library/Foundation
else
	@rm -rf $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation
endif


clobber::
	@rm -f $(StroikaFoundationLib)
