export TOP_ROOT:=$(abspath ../)/
StroikaSourceRoot	:=	$(TOP_ROOT)ThirdPartyComponents/Stroika/StroikaRoot/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


SrcDir			:=	$(TOP_ROOT)${APP_NAME}/


SHELL=/bin/bash


all:
	@$(StroikaSourceRoot)/Build/Scripts/CheckValidConfiguration $(CONFIGURATION)
	@$(MAKE) --directory=Sources --no-print-directory all


clean clobber::
	@$(StroikaSourceRoot)/Build/Scripts/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) "${APP_NAME} $(call FUNCTION_CAPITALIZE_WORD,$@) {$(CONFIGURATION)}:"
	@$(StroikaSourceRoot)/Build/Scripts/CheckValidConfiguration $(CONFIGURATION)
	@$(MAKE) --directory=Sources --silent $@


format-code:
	@$(StroikaSourceRoot)Build/Scripts/FormatCode Sources '*.h' '*.inl' '*.cpp'
