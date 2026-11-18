# DEPRECATED: make's own build (run by Makefile-Legacy), kept until Stroika v3.0d26 ships - Stroika builds with CMake (CMakeLists.txt)
StroikaSourceRoot	:=	$(abspath ../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Samples/HTMLUI/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Samples-HTMLUI/


#not parallel because submakefiles use parallelism, but generally best to sequence these top level requests. Like if you say
# make clobber all you don't want those to happen at the same time. And make libraries samples wouldn't really work since all the libraries
# have to be built before the samples etc...
.NOTPARALLEL:


TARGETDIR		:=	${StroikaPlatformTargetBuildDir}Samples-HTMLUI/


include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all :
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Samples/HTMLUI:"
	@mkdir -p $(TARGETDIR)
	@$(MAKE) --silent --directory Backend --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $@
	@$(MAKE) --silent html
	@$(MAKE) --silent installers

check:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "$(call FUNCTION_CAPITALIZE_WORD,$@) Samples/HTMLUI:"
	@$(MAKE) --silent --directory Backend --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $@

clobber::
	@$(MAKE) --silent --directory Backend --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $@
	@$(MAKE) --silent --directory QuasarBasedHTMLApp --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $@

clean::
	@$(MAKE) --silent --directory Backend --no-print-directory  MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $@
	@$(MAKE) --silent --directory QuasarBasedHTMLApp --no-print-directory  MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $@


html:
ifeq ($(shell which node 2>/dev/null),)
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Warning: ***Skipping html cuz no nodejs installed:" `$(StroikaSourceRoot)Build/Scripts/GetMessageForMissingTool node` "***"
	@rm -rf ${TARGETDIR}html; mkdir -p ${TARGETDIR}html
else ifeq ($(shell if `test $(shell node -v | cut -d'.' -f1 | tr -d 'v') -ge 22`; then echo x; fi),)
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Warning: ***Skipping html cuz nodejs is older than 22 (`node -v`):" `$(StroikaSourceRoot)Build/Scripts/GetMessageForMissingTool node` "***"
	@rm -rf ${TARGETDIR}html; mkdir -p ${TARGETDIR}html
else ifeq ($(shell which quasar 2>/dev/null),)
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Warning: ***Skipping html cuz no quasar installed:" `$(StroikaSourceRoot)Build/Scripts/GetMessageForMissingTool quasar` "***"
	@rm -rf ${TARGETDIR}html; mkdir -p ${TARGETDIR}html
else
	@$(MAKE) --silent --directory QuasarBasedHTMLApp --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) all
endif

.PHONY: installers installer-deb installer-rpm installer-wix
installers installer-deb installer-rpm installer-wix:
	@$(StroikaSourceRoot)/Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Samples/HTMLUI: Building Installers:"
	@$(MAKE) --no-print-directory --directory Installers --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $@
