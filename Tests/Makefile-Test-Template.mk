export StroikaRoot?=$(realpath ../../)/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk

ifneq ($(CONFIGURATION),)
	include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

SrcDir				=	$(StroikaRoot)Tests/$(TEST_NUM)/
ObjDir				=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Tests/$(TEST_NUM)/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk


ifndef TEST_NUM
	TEST_NUM		=	_UNDEFINED_FIX_INCLUDER_OF_TEMPLATE_TO_SPECIFY_THIS_
endif


Includes				+=	-I.

# @todo - no need for extra level of folders here on windows, I think. Even if .pdb file gets stuck in same directory its OK. MAYBE in BOTH cases
#        add Tests folder, so $(CONFIGURATION)/Tests/Test1${EXE_SUFFIX}
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
TARGETEXE				=	$(StroikaRoot)Builds/$(CONFIGURATION)/Test$(TEST_NUM)/Test$(TEST_NUM)${EXE_SUFFIX}
else
TARGETEXE				=	$(StroikaRoot)Builds/$(CONFIGURATION)/Test$(TEST_NUM)
endif


VPATH	=			$(SrcDir):$(SrcDir)../TestHarness/


Objs	=	\
	$(ObjDir)Test${OBJ_SUFFIX}\
	$(ObjDir)NotCopyable${OBJ_SUFFIX}\
	$(ObjDir)SimpleClass${OBJ_SUFFIX}\
	$(ObjDir)TestHarness${OBJ_SUFFIX}\


all:	$(ObjDir) $(TARGETEXE)


check:
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "Test $(TEST_NUM): $(shell $(StroikaRoot)Tests/ScriptsLib/PrintTestName $(TEST_NUM)) :  "
	@$(StroikaRoot)ScriptsLib/CheckFileExists $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]";


$(TARGETEXE):	$(Objs) $(StroikaLibs)
	@$(StroikaRoot)/ScriptsLib/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) "Linking `$(StroikaRoot)ScriptsLib/SubstituteBackVariables $@`" "... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintLevelLeader $$(($(MAKE_INDENT_LEVEL)+1)) && $(ECHO) "$(Linker) $(StroikaLinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerSuffixArgs)";\
	fi
	@$(Linker) $(StroikaLinkerPrefixArgs) -o $@ $(Objs) $(StroikaLinkerSuffixArgs)
