export StroikaSourceRoot	:=	$(abspath ../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir				:=	$(StroikaSourceRoot)Tests/$(TEST_NUM)/
ObjDir				:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Tests/$(TEST_NUM)/
PackageDependencies :=  stroika-frameworks

ifndef TEST_NUM
	TEST_NUM		=	_UNDEFINED_FIX_INCLUDER_OF_TEMPLATE_TO_SPECIFY_THIS_
endif

TARGETEXE				=	${StroikaPlatformTargetBuildDir}Tests/Test$(TEST_NUM)${EXE_SUFFIX}

vpath %.h $(SrcDir)
vpath %.cpp $(SrcDir)


Objs	=	\
	$(ObjDir)Test${OBJ_SUFFIX}\


# For embeddings/resources (xxd)
CPPFLAGS       +=     -I${ObjDir_ToolsSafe}


include $(StroikaSourceRoot)/Build/Lib/Make/SharedBuildRules-Default.mk

all:	$(ObjDir) $(TARGETEXE)


check:
	@$(StroikaSourceRoot)/Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "Test $(TEST_NUM): $(shell $(StroikaSourceRoot)Tests/Scripts/PrintTestName $(TEST_NUM)) :  "
	@$(StroikaSourceRoot)Build/Scripts/CheckFileExists $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]";


# Cannot be done generically through configuration cuz a given build of Stroika might contain EXEs targetting multiple subsystems
ifeq (Windows,$(findstring Windows,$(TargetPlatforms)))
LinkerArgs_ExtraSuffix+= -SUBSYSTEM:CONSOLE
endif


$(TARGETEXE):	$(Objs) $(StroikaLibs)
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Linking Test `$(StroikaSourceRoot)Build/Scripts/SubstituteBackVariables $@` (`$(StroikaSourceRoot)Tests/Scripts/PrintTestName ${TEST_NUM}`) ... "
	@mkdir -p $(dir $@)
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaSourceRoot)Build/Scripts/PrintProgressLine $$((($(MAKE_INDENT_LEVEL)+2))) '$(call DEFAULT_LINK_LINE, $@, stroika-frameworks)';\
	fi
	@$(call DEFAULT_LINK_LINE, $@, stroika-frameworks)
