StroikaRoot=$(abspath ../../../../)/

ifneq ($(CONFIGURATION),)
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

SrcDir	=	$(StroikaRoot)Library/Sources/Stroika/Foundation/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation/

VPATH		=	$(SrcDir) $(StroikaLibDir)

SubDirs=				\
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

# redirect errors on list-objs cuz could fail due to directories not existing yet (#include) and not worth fixing each
# sub-make - easier to just ignore in this one case from here
# -- LGP 2019-01-09
#Objs := $(foreach dir, $(SubDirs), $(shell $(MAKE) --directory $(dir) CONFIGURATION=$(CONFIGURATION) -s list-objs 2>/dev/null))

# note also cannot just invoke
#		Objs := $($(MAKE) CONFIGURATION=$(CONFIGURATION) cached-list-objs > /dev/null 2>&1 ; cat cached-list-objs ))
# cuz results in recursive invoke of list cached-list-objs
# So do redirect to the file unconditionally if it doesn't exist
#ifneq ($(shell if [ -e $(CachedOBJSFile_) ]; then echo true; fi),true)
ifeq ($(wildcard $(CachedOBJSFile_)),)
#$(shell $(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Computing $(CachedOBJSFile_)")
$(shell mkdir -p $(shell dirname $(CachedOBJSFile_)) )
$(file > $(CachedOBJSFile_),$(foreach dir, $(SubDirs), $(shell $(MAKE) --directory $(dir) CONFIGURATION=$(CONFIGURATION) -s list-objs 2>/dev/null)))
endif
#ifeq ($(shell if [ -e $(CachedOBJSFile_) ]; then echo true; fi),true)
ifneq ($(wildcard $(CachedOBJSFile_)),)
# no good reason for file < to fail, but it does sometimes ;-) --LGP 2019-10-27
#Objs := $(file < $(CachedOBJSFile_))
Objs := $(shell cat $(CachedOBJSFile_))
endif

include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk


$(Objs):	all_objs_


$(ObjDir)%${OBJ_SUFFIX} : %.cpp
	@#suppress normal rule so just all_objs_ fires


.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s


# @todo fix - this iterates over all the subdirs calling make, which is a little slow and perhaps unneeded -- LGP 2019-10-27
.PHONY: all_objs_
all_objs_:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Stroika Foundation Objs:";
	@$(MAKE) --no-print-directory -s $(SubDirs) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))


$(StroikaFoundationLib):	$(Objs)
	@mkdir -p $(StroikaLibDir)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Creating `$(StroikaRoot)ScriptsLib/SubstituteBackVariables $@` ... "
	@rm -f $@
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then \
		$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+2)) '$(call DEFAULT_LIBRARY_GEN_LINE,$@,$(Objs))';\
	fi
	@$(call DEFAULT_LIBRARY_GEN_LINE,$@,$(Objs))




all:	$(StroikaFoundationLib)


check:
	@$(StroikaRoot)ScriptsLib/CheckFileExists $(StroikaFoundationLib)
	@$(ECHO) "[SUCCEEDED]"


clean::
ifeq ($(CONFIGURATION),)
	@rm -rf $(StroikaRoot)IntermediateFiles/*/Library/Foundation
else
	@rm -rf $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Foundation
endif


clobber::
	@rm -f $(StroikaFoundationLib)
