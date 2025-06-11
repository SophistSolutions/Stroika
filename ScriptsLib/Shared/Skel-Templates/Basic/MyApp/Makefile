export TOP_ROOT:=$(abspath ../)/
StroikaRoot	:=	$(TOP_ROOT)ThirdPartyComponents/Stroika/StroikaRoot/

include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk


SrcDir			:=	$(TOP_ROOT)${APP_NAME}/


SHELL=/bin/bash


all:
	@$(StroikaRoot)/ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@$(MAKE) --directory=Sources --no-print-directory all


clean clobber::
	@$(StroikaRoot)/ScriptsLib/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) "${APP_NAME} $(call FUNCTION_CAPITALIZE_WORD,$@) {$(CONFIGURATION)}:"
	@$(StroikaRoot)/ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@$(MAKE) --directory=Sources --silent $@


format-code:
	@$(StroikaRoot)ScriptsLib/FormatCode Sources *.h *.inl *.cpp
