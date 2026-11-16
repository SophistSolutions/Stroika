StroikaSourceRoot	:=	$(abspath ../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Samples/SimpleLedTest/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Samples-SimpleLedTest/


TARGETDIR	:=	${StroikaPlatformTargetBuildDir}Samples-SimpleLedTest/
TARGETEXE	:=	$(TARGETDIR)SimpleLedTest${EXE_SUFFIX}


VPATH	=			$(SrcDir)Sources/


RC_OUTPUT_=$(ObjDir)Test.res


ifeq (Windows,$(findstring Windows,$(TargetPlatforms)))
Objs+= $(ObjDir)Test${OBJ_SUFFIX}
Objs += ${RC_OUTPUT_}
endif


include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk



all:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Samples/SimpleLedTest:"
	@mkdir -p $(TARGETDIR)
	@$(MAKE) --silent $(TARGETEXE) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))


check:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL) + 1)) -n "Checking Samples/SimpleLedTest...            "
	@$(StroikaSourceRoot)Build/Scripts/CheckFileExists $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]";


# English US culture
RC_FLAGS += -l 0x0409

$(ObjDir)Test.res:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL)  "Running RC compiler on "`$(StroikaSourceRoot)Build/Scripts/SubstituteBackVariables $(SrcDir)Sources/LedIt.rc` "; see "`$(StroikaSourceRoot)Build/Scripts/SubstituteBackVariables $(ObjDir)RC-Build.out`" ..."
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaSourceRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_RC_LINE,$(SrcDir)Sources/Test.rc,$(ObjDir)Test.res)';\
	fi
	@$(call DEFAULT_RC_LINE,$(SrcDir)Sources/Test.rc,$(ObjDir)Test.res) > $(ObjDir)RC-Build.out 2>&1


# Cannot be done generically through configuration cuz a given build of Stroika might contain EXEs targetting multiple subsystems
ifeq (Windows,$(findstring Windows,$(TargetPlatforms)))
EXTRALINKARGS_ += -SUBSYSTEM:Windows
endif

$(TARGETEXE):	$(ObjDir) $(Objs) $(StroikaLibs)
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Linking `$(StroikaSourceRoot)Build/Scripts/SubstituteBackVariables $@`" "... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaSourceRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_LINK_LINE, $@, stroika-frameworks, ${EXTRALINKARGS_})';\
	fi
	@$(call DEFAULT_LINK_LINE, $@, stroika-frameworks, ${EXTRALINKARGS_})
