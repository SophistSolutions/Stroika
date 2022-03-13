export TOP_ROOT?=$(shell realpath ../../)/
export StroikaRoot?=$(shell realpath StroikaRoot)/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

all:
	@$(StroikaRoot)ScriptsLib/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) "ThirdPartyComponents/Stroika-Import Build {$(CONFIGURATION)}:"
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@$(MAKE) --directory=StroikaRoot --no-print-directory libraries CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) QUICK_BUILD=1
	@$(StroikaRoot)ScriptsLib/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) "ThirdPartyComponents/Stroika-Import Build Stroika CONFIGURATION=$(CONFIGURATION)...done"


clean clobber:
	@$(StroikaRoot)ScriptsLib/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) "ThirdPartyComponents/Stroika-Import $(call FUNCTION_CAPITALIZE_WORD,$@) {$(CONFIGURATION)}:"
	@# For make clobber, do recursively, but for make clean, don't do anything, because the current stroika make clean afterwards requires a full rebuild
	@# (buggy IMHO) - so don't aggravate that - but avoid it for now (isuse is make depend related) -- LGP 2019-01-20
	@#    BUT - make clean of thirdpartycomponents works fine
	@if [ "$@" = "clean" ] ; then \
		$(MAKE) --directory=StroikaRoot/ThirdPartyComponents --no-print-directory --silent $@ CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
	fi
	@if [ "$@" = "clobber" ] ; then \
		$(MAKE) --directory=StroikaRoot --no-print-directory --silent $@ CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
	fi
