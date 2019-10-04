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


# redirect errors on list-objs cuz could fail due to directories not existing yet (#include) and not worth fixing each
# sub-make - easier to just ignore in this one case from here
# -- LGP 2019-01-09
Objs := $(foreach dir, $(SubDirs), $(shell $(MAKE) --directory $(dir) CONFIGURATION=$(CONFIGURATION) -s list-objs 2>/dev/null))


include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk


$(Objs):	all_objs_


$(ObjDir)%${OBJ_SUFFIX} : %.cpp
	@#suppress normal rule so just all_objs_ fires


.PHONY:			$(SubDirs)
$(SubDirs):
	@$(MAKE) --directory $@ -s


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
