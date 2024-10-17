StroikaRoot:=$(abspath ../../)/

ifneq ($(CONFIGURATION),)
	#no error if missing cuz could be doing make clobber
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

SrcDir			:=	$(StroikaRoot)Samples/HTMLUI/
ObjDir			:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-HTMLUI/

#not parallel because submakefiles use parallelism, but generally best to sequence these top level requests. Like if you say
# make clobber all you don't want those to happen at the same time. And make libraries samples wouldn't really work since all the libraries
# have to be built before the samples etc...
.NOTPARALLEL:


TARGETDIR		:=	$(StroikaRoot)Builds/$(CONFIGURATION)/Samples-HTMLUI/


include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk


all :
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Samples/HTMLUI:"
	@mkdir -p $(TARGETDIR)
	@$(MAKE) --silent --directory Backend MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $@
	@$(MAKE) --silent html
	@$(MAKE) --silent installers

check:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "$(call FUNCTION_CAPITALIZE_WORD,$@) Samples/HTMLUI:"
	@$(MAKE) --silent --directory Backend MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $@

clobber::
	@$(MAKE) --silent --directory Backend MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $@
	@$(MAKE) --silent --directory html MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $@

clean::
	@$(MAKE) --silent --directory Backend MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $@


html:
ifeq ($(shell which quasar 2>/dev/null),)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Warning: ***Skipping html cuz no quasar installed:" `$(StroikaRoot)ScriptsLib/GetMessageForMissingTool quasar` ***"
	@rm -rf ${TARGETDIR}html; mkdir -p ${TARGETDIR}html
else
	@$(MAKE) --silent --directory QuasarBasedHTMLApp MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) all
endif

.PHONY: installers installer-deb installer-rpm installer-wix
installers installer-deb installer-rpm installer-wix:
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Samples/HTMLUI: Building Installers:"
	@$(MAKE) --no-print-directory --directory Installers MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $@
