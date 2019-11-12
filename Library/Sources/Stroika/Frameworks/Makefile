StroikaRoot=$(abspath ../../../../)/

ifneq ($(CONFIGURATION),)
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

SrcDir	=	$(StroikaRoot)Library/Sources/Stroika/Frameworks/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

ifeq ($(WIN_USE_PROGRAM_DATABASE),1)
CXXFLAGS+=/Fd$(call FUNCTION_CONVERT_FILES_TO_COMPILER_NATIVE,$(StroikaRoot)Builds/$(CONFIGURATION)/Stroika-Frameworks.pdb)
endif


VPATH		=	$(SrcDir) $(StroikaLibDir)


SubDirs=	\
		Led\
		Modbus\
		NetworkMonitor\
		Service\
		SystemPerformance\
		WebServer\
		WebService\
		UPnP\



.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s

CachedOBJSFile_ = $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Frameworks/cached-list-objs

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
ifeq ($(wildcard $(CachedOBJSFile_)),)
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),clobber)
$(shell $(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Computing $(CachedOBJSFile_)" 1>&2)
$(shell mkdir -p $(shell dirname $(CachedOBJSFile_)) )
$(file > $(CachedOBJSFile_),$(foreach dir, $(SubDirs), $(shell $(MAKE) --directory $(dir) CONFIGURATION=$(CONFIGURATION) -s list-objs 2>/dev/null)))
endif
endif
endif
ifneq ($(wildcard $(CachedOBJSFile_)),)
Objs := $(shell cat $(CachedOBJSFile_))
endif



include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk



$(ObjDir)%${OBJ_SUFFIX} : %.cpp
	@#suppress normal rule so just all_objs_ fires


$(Objs): all_objs_

.PHONY:	all_objs_
all_objs_:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Stroika Frameworks Objs:";
	@$(MAKE) --no-print-directory -s $(SubDirs) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))


$(StroikaFrameworksLib):	$(Objs)
	@mkdir -p $(StroikaLibDir)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Creating `$(StroikaRoot)ScriptsLib/SubstituteBackVariables $@` ... "
	@rm -f $@
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then \
		$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+2)) '$(call DEFAULT_LIBRARY_GEN_LINE,$@,$(Objs))';\
	fi
	@$(call DEFAULT_LIBRARY_GEN_LINE,$@,$(Objs))


all:	$(StroikaFrameworksLib)


clean::
ifeq ($(CONFIGURATION),)
	@rm -rf $(StroikaRoot)IntermediateFiles/*/Frameworks
else
	@rm -rf $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Frameworks
endif


clobber::
ifeq (Unix,$(ProjectPlatformSubdir))
	@for i in $(SubDirs);\
	do\
	    $(MAKE) --directory $$i --no-print-directory --silent clobber;\
	done
	@rm -f $(StroikaFrameworksLib)
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
endif


Build:	all


