StroikaRoot=$(abspath ../../)/

ifneq ($(CONFIGURATION),)
	include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

SrcDir				=	$(StroikaRoot)Samples/SimpleLedTest/
ObjDir				=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-SimpleLedTest/


TARGETDIR	=	$(StroikaRoot)Builds/$(CONFIGURATION)/Samples-SimpleLedTest/
TARGETEXE	=	$(TARGETDIR)SimpleLedTest${EXE_SUFFIX}


VPATH	=			$(SrcDir)/Sources/


RC_OUTPUT_=$(ObjDir)Test.res



ifeq (Windows,$(findstring Windows,$(TARGET_PLATFORMS)))
Objs+= $(ObjDir)Test${OBJ_SUFFIX}
endif


ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
CXXFLAGS += /D_WINDOWS 
CXXFLAGS += /DWIN32 
CXXFLAGS += /DSTRICT
CXXFLAGS += /D_WIN32_WINNT=0x0501
Objs += ${RC_OUTPUT_}
endif


include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk



all:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Samples/SimpleLedTest:"
	@mkdir -p $(TARGETDIR)
ifeq (${USE_MSBUILD},1)
	if [[ "$(CONFIGURATION)" == "Debug-U-32" || "$(CONFIGURATION)" == "Release-U-32" ]] ; then\
		$(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:NoSummary Projects/$(ProjectPlatformSubdir)/SimpleLedTest.vcxproj /p:`$(StroikaRoot)ScriptsLib/GetVisualStudioConfigLine $(CONFIGURATION)` /target:Build";\
	else\
		$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "SKIPPED for unsupported configuration";\
	fi
else
	@$(MAKE) --silent $(TARGETEXE) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif


check:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL) + 1)) -n "Checking Samples/SimpleLedTest...            "
ifeq (${USE_MSBUILD},1)
	@if [[ "$(CONFIGURATION)" == "Debug-U-32" || "$(CONFIGURATION)" == "Release-U-32" ]] ; then\
		$(StroikaRoot)ScriptsLib/CheckFileExists $(TARGETEXE);\
		$(ECHO) "[SUCCEEDED]";\
	else\
		$(ECHO) "[SKIPPED]";\
	fi
else
	@$(StroikaRoot)ScriptsLib/CheckFileExists $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]";
endif


# English US culture
RC_FLAGS += /l 0x0409

$(ObjDir)Test.res:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL)  "Running RC compiler on "`$(StroikaRoot)ScriptsLib/SubstituteBackVariables $(SrcDir)/Sources/LedIt.rc` "; see "`$(StroikaRoot)ScriptsLib/SubstituteBackVariables $(ObjDir)RC-Build.out`" ..."
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_RC_LINE,$(SrcDir)Sources/Test.rc,$(ObjDir)Test.res)';\
	fi
	@$(call DEFAULT_RC_LINE,$(SrcDir)Sources/Test.rc,$(ObjDir)Test.res) > $(ObjDir)RC-Build.out 2>&1


# Cannot be done generically through configuration cuz a given build of Stroika might contain EXEs targetting multiple subsystems
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
EXTRA_SUFFIX_LINKER_ARGS+= /SUBSYSTEM:Windows
endif

$(TARGETEXE):	$(ObjDir) $(Objs) $(StroikaLibs)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Linking `$(StroikaRoot)ScriptsLib/SubstituteBackVariables $@`" "... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_LINK_LINE,$@)';\
	fi
	@$(call DEFAULT_LINK_LINE,$@)
