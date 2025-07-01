export StroikaRoot	:=	$(abspath ../../)/
SrcDir				:=	$(StroikaRoot)Tests/$(TEST_NUM)/
ObjDir				:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Tests/$(TEST_NUM)/
PackageDependencies :=  stroika-frameworks

include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

ifndef TEST_NUM
	TEST_NUM		=	_UNDEFINED_FIX_INCLUDER_OF_TEMPLATE_TO_SPECIFY_THIS_
endif

TARGETEXE				=	${StroikaPlatformTargetBuildDir}Tests/Test$(TEST_NUM)${EXE_SUFFIX}

vpath %.h $(SrcDir) ../TestHarness/
vpath %.cpp $(SrcDir) ../TestHarness/


Objs	=	\
	$(ObjDir)Test${OBJ_SUFFIX}\


include $(StroikaRoot)/ScriptsLib/SharedBuildRules-Default.mk

all:	$(ObjDir) $(TARGETEXE)


check:
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "Test $(TEST_NUM): $(shell $(StroikaRoot)Tests/ScriptsLib/PrintTestName $(TEST_NUM)) :  "
	@$(StroikaRoot)ScriptsLib/CheckFileExists $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]";


# Cannot be done generically through configuration cuz a given build of Stroika might contain EXEs targetting multiple subsystems
ifeq (Windows,$(findstring Windows,$(TargetPlatforms)))
LinkerArgs_ExtraSuffix+= -SUBSYSTEM:CONSOLE
endif


$(TARGETEXE):	$(Objs) $(StroikaLibs)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Linking Test `$(StroikaRoot)ScriptsLib/SubstituteBackVariables $@` (`$(StroikaRoot)Tests/ScriptsLib/PrintTestName ${TEST_NUM}`) ... "
	@mkdir -p $(dir $@)
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintProgressLine $$((($(MAKE_INDENT_LEVEL)+2))) '$(call DEFAULT_LINK_LINE2,$@)';\
	fi
	@$(call DEFAULT_LINK_LINE2,$@)
