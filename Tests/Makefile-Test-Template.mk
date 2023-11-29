export StroikaRoot?=$(abspath ../../)/

ifneq ($(CONFIGURATION),)
	#no error if missing cuz could be doing make clobber
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)/ScriptsLib/SharedMakeVariables-Default.mk

SrcDir				=	$(StroikaRoot)Tests/$(TEST_NUM)/
ObjDir				=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Tests/$(TEST_NUM)/

ifndef TEST_NUM
	TEST_NUM		=	_UNDEFINED_FIX_INCLUDER_OF_TEMPLATE_TO_SPECIFY_THIS_
endif

TARGETEXE				=	$(StroikaRoot)Builds/$(CONFIGURATION)/Tests/Test$(TEST_NUM)${EXE_SUFFIX}

vpath %.h $(SrcDir) ../TestHarness/
vpath %.cpp $(SrcDir) ../TestHarness/


Objs	=	\
	$(ObjDir)Test${OBJ_SUFFIX}\
	$(ObjDir)NotCopyable${OBJ_SUFFIX}\
	$(ObjDir)SimpleClass${OBJ_SUFFIX}\
	$(ObjDir)TestHarness${OBJ_SUFFIX}\


include $(StroikaRoot)/ScriptsLib/SharedBuildRules-Default.mk

# ONLY depend on foundation library, not frameworks library
StroikaLibs					=	$(StroikaFoundationLib)


all:	$(ObjDir) $(TARGETEXE)


check:
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "Test $(TEST_NUM): $(shell $(StroikaRoot)Tests/ScriptsLib/PrintTestName $(TEST_NUM)) :  "
	@$(StroikaRoot)ScriptsLib/CheckFileExists $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]";


# Cannot be done generically through configuration cuz a given build of Stroika might contain EXEs targetting multiple subsystems
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
LinkerArgs_ExtraSuffix+= /SUBSYSTEM:CONSOLE
endif


$(TARGETEXE):	$(Objs) $(StroikaLibs)
	@$(StroikaRoot)/ScriptsLib/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) "Linking `$(StroikaRoot)ScriptsLib/SubstituteBackVariables $@`" "... "
	@mkdir -p $(dir $@)
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_LINK_LINE,$@)';\
	fi
	@$(call DEFAULT_LINK_LINE,$@)
