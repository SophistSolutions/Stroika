export TOP_ROOT?=$(shell realpath ../../)/
export StroikaSourceRoot?=$(shell realpath StroikaRoot)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

all:
	@$(StroikaSourceRoot)Build/Scripts/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) "ThirdPartyComponents/Stroika-Import Build {$(CONFIGURATION)}:"
	@$(StroikaSourceRoot)Build/Scripts/CheckValidConfiguration $(CONFIGURATION)
	@$(MAKE) --directory=StroikaRoot --no-print-directory libraries CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(StroikaSourceRoot)Build/Scripts/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) "ThirdPartyComponents/Stroika-Import Build Stroika CONFIGURATION=$(CONFIGURATION)...done"


clean clobber:
	@$(StroikaSourceRoot)Build/Scripts/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) "ThirdPartyComponents/Stroika-Import $(call FUNCTION_CAPITALIZE_WORD,$@) {$(CONFIGURATION)}:"
	@$(MAKE) --directory=StroikaRoot --no-print-directory --silent $@ CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
