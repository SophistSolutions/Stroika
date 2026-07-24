StroikaRoot	:=	$(abspath ../../)/
SrcDir		:=	$(StroikaRoot)Samples/HTMLUI/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-HTMLUI/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk


#not parallel because submakefiles use parallelism, but generally best to sequence these top level requests. Like if you say
# make clobber all you don't want those to happen at the same time. And make libraries samples wouldn't really work since all the libraries
# have to be built before the samples etc...
.NOTPARALLEL:


TARGETDIR		:=	${StroikaPlatformTargetBuildDir}Samples-HTMLUI/


include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all :
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Samples/HTMLUI:"
	@mkdir -p $(TARGETDIR)
	@$(MAKE) --silent --directory Backend --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $@
	@$(MAKE) --silent html
	@$(MAKE) --silent installers

check:
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "$(call FUNCTION_CAPITALIZE_WORD,$@) Samples/HTMLUI:"
	@$(MAKE) --silent --directory Backend --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $@

clobber::
	@$(MAKE) --silent --directory Backend --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $@
	@$(MAKE) --silent --directory QuasarBasedHTMLApp --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $@

clean::
	@$(MAKE) --silent --directory Backend --no-print-directory  MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $@
	@$(MAKE) --silent --directory QuasarBasedHTMLApp --no-print-directory  MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $@


html:
ifeq ($(shell which node 2>/dev/null),)
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Warning: ***Skipping html cuz no nodejs installed:" `$(StroikaRoot)Build/Scripts/GetMessageForMissingTool node` "***"
	@rm -rf ${TARGETDIR}html; mkdir -p ${TARGETDIR}html
else ifeq ($(shell if `test $(shell node -v | cut -d'.' -f1 | tr -d 'v') -ge 22`; then echo x; fi),)
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Warning: ***Skipping html cuz no nodejs older than 22 installed:" `$(StroikaRoot)Build/Scripts/GetMessageForMissingTool quasar` "***"
	@rm -rf ${TARGETDIR}html; mkdir -p ${TARGETDIR}html
else ifeq ($(shell which quasar 2>/dev/null),)
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Warning: ***Skipping html cuz no quasar installed:" `$(StroikaRoot)Build/Scripts/GetMessageForMissingTool quasar` "***"
	@rm -rf ${TARGETDIR}html; mkdir -p ${TARGETDIR}html
else
	@$(MAKE) --silent --directory QuasarBasedHTMLApp --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) all
endif

.PHONY: installers installer-deb installer-rpm installer-wix
installers installer-deb installer-rpm installer-wix:
	@$(StroikaRoot)/Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Samples/HTMLUI: Building Installers:"
	@$(MAKE) --no-print-directory --directory Installers --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $@
