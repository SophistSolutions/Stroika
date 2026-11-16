StroikaSourceRoot	:=	$(abspath ../../../../)/

StroikaApplyConfigurationIfMissing	:=	1
include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Library/Sources/Stroika/Frameworks/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Frameworks/



VPATH		=	$(SrcDir) $(StroikaLibDir)


SubDirs:=	\
		Auth\
		Led\
		Modbus\
		NetworkMonitor\
		Service\
		SystemPerformance\
		Test\
		WebServer\
		WebService\
		UPnP\



.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s

CachedOBJSFile_ = $(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Frameworks/cached-list-objs

#
# note also cannot just invoke
#		Objs := $($(MAKE) CONFIGURATION=$(CONFIGURATION) cached-list-objs > /dev/null 2>&1 ; cat cached-list-objs ))
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
#
# Sadly none of this wildcard or file stuff appears to work right on very old gnu make, which is
# what comes with macos. Not sure what #s work/fail, but seems to work with 4.2.1 or later, and fail with
# 3.81
#
NEED_GNU_MAKE_ := 4.0.0
OK_GNU_MAKE_VERSION_ := $(filter $(NEED_GNU_MAKE_),$(firstword $(sort $(MAKE_VERSION) $(NEED_GNU_MAKE_))))

ifeq ($(OK_GNU_MAKE_VERSION_),)
# REDIRECT to avoid: warning: jobserver unavailable: using -j1.  Add '+' to parent make rule.; not good solution cuz hides errors
Objs := $(foreach dir, $(SubDirs), $(shell $(StroikaDirectoryLayoutEnv) $(MAKE) --directory $(dir) CONFIGURATION=$(CONFIGURATION) -s list-objs 2>/dev/null))
else
ifeq ($(wildcard $(CachedOBJSFile_)),)
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),clobber)
$(shell $(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Computing `$(StroikaSourceRoot)Build/Scripts/SubstituteBackVariables $(CachedOBJSFile_)`" 1>&2)
$(shell mkdir -p $(shell dirname $(CachedOBJSFile_)) )
# REDIRECT to avoid: warning: jobserver unavailable: using -j1.  Add '+' to parent make rule.; not good solution cuz hides errors
ifneq ($(findstring $(DETECTED_HOST_OS),MSYS-Cygwin),)
$(file > $(CachedOBJSFile_),$(shell cygpath --mixed $(foreach dir, $(SubDirs), $(shell $(StroikaDirectoryLayoutEnv) $(MAKE) --directory $(dir) CONFIGURATION=$(CONFIGURATION) -s list-objs 2>/dev/null))))
else
$(file > $(CachedOBJSFile_),$(foreach dir, $(SubDirs), $(shell $(StroikaDirectoryLayoutEnv) $(MAKE) --directory $(dir) CONFIGURATION=$(CONFIGURATION) -s list-objs 2>/dev/null)))
endif
endif
endif
endif
ifneq ($(wildcard $(CachedOBJSFile_)),)
Objs := $(shell cat $(CachedOBJSFile_))
endif
endif



include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk



$(ObjDir)%${OBJ_SUFFIX} : %.cpp
	@#suppress normal rule so just all_objs_ fires


$(Objs): all_objs_

.PHONY:	all_objs_
all_objs_:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Stroika Frameworks Objs {$(CONFIGURATION)}:";
	@$(MAKE) --no-print-directory -s $(SubDirs) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))


# @todo the reason the .lib file isn't rebuilt properly sometimes is that this rule doesn't
# trigger, probably because in this makefile, we don't get to see the dates changes on the obj files after the all_objs_ rule runs? and
# unlike normal makefiles, we dont see the dependencies in this makefile - just sub-makes (to a degree).
$(StroikaFrameworksLib):	$(Objs)
	@mkdir -p $(StroikaLibDir)
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Creating `$(StroikaSourceRoot)Build/Scripts/SubstituteBackVariables $@` ... "
	@rm -f $@
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then \
		$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+2)) '$(call DEFAULT_LIBRARY_GEN_LINE,$@,$(Objs))';\
	fi
	@$(call DEFAULT_LIBRARY_GEN_LINE,$@,$(Objs))


all:	$(StroikaFrameworksLib)


clean::
ifeq ($(CONFIGURATION),)
	@rm -rf $(StroikaIntermediateFilesRoot)*/Library/Frameworks
else
	@rm -rf $(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Library/Frameworks
endif


clobber::
	@for i in $(SubDirs);\
	do\
	    $(MAKE) --directory $$i --no-print-directory --silent clobber;\
	done
	@rm -f $(StroikaFrameworksLib)


Build:	all


