StroikaRoot=$(abspath ../../)/

ifneq ($(CONFIGURATION),)
	include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

SrcDir				=	$(StroikaRoot)Samples/ActiveLedIt/
ObjDir				=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-ActiveLedIt/

TARGETDIR				=	$(StroikaRoot)Builds/$(CONFIGURATION)/Samples-ActiveLedIt/
TARGETEXE				=	$(TARGETDIR)SophistSolutions.ActiveLedIt.dll


VPATH	=			$(SrcDir)/Sources/


Objs	=	\
	$(ObjDir)ActiveLedIt${OBJ_SUFFIX}\
	$(ObjDir)ActiveLedItControl${OBJ_SUFFIX}\
	$(ObjDir)ActiveLedItPpg${OBJ_SUFFIX}\
	$(ObjDir)FontMenu${OBJ_SUFFIX}\
	$(ObjDir)LedItView${OBJ_SUFFIX}\
	$(ObjDir)Toolbar${OBJ_SUFFIX}\
	$(ObjDir)UserConfigCommands${OBJ_SUFFIX}\


ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
CXXFLAGS += /D_WINDOWS 
CXXFLAGS += /DWIN32 
CXXFLAGS += /DSTRICT
CXXFLAGS += /D_WIN32_WINNT=0x0501 
endif


ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
CXXFLAGS += /I"$(call FUNCTION_CONVERT_FILES_TO_COMPILER_NATIVE,$(ObjDir))"
endif


include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk



all:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Samples/ActiveLedIt:"
	@mkdir -p $(TARGETDIR)
ifeq (${USE_MSBUILD},1)
	@if [[ "$(CONFIGURATION)" == "Debug-U-32" || "$(CONFIGURATION)" == "Release-U-32" ]] ; then\
		$(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:NoSummary Projects/$(ProjectPlatformSubdir)/ActiveLedIt.vcxproj /p:`$(StroikaRoot)ScriptsLib/GetVisualStudioConfigLine $(CONFIGURATION)` /target:Build";\
	else\
		$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "SKIPPED for unsupported configuration";\
	fi
else
	@$(MAKE) --silent $(TARGETEXE) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif


check:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL) + 1)) -n "Checking Samples/ActiveLedIt...              "
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


# Cannot be done generically through configuration cuz a given build of Stroika might contain EXEs targetting multiple subsystems
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
EXTRA_SUFFIX_LINKER_ARGS+= /SUBSYSTEM:Windows
EXTRA_SUFFIX_LINKER_ARGS+= /ENTRY:wWinMainCRTStartup
endif


MIDL_OUTPUT_=$(ObjDir)ActiveLedIt.tlb $(ObjDir)ActiveLedIt_i.c $(ObjDir)ActiveLedIt_h.h

$(ObjDir)ActiveLedIt_h.h $(ObjDir)ActiveLedIt_i.c:	$(ObjDir)ActiveLedIt.tlb

$(Objs):	$(ObjDir)ActiveLedIt_i.c $(ObjDir)ActiveLedIt_h.h


MIDL_FLAGS +=	/env win32

$(ObjDir)ActiveLedIt.tlb:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL)  "Running MIDL compiler on "`$(StroikaRoot)ScriptsLib/SubstituteBackVariables $(SrcDir)/Sources/ActiveLedIt.odl` "; see "`$(StroikaRoot)ScriptsLib/SubstituteBackVariables $(ObjDir)MIDL-Build.out`" ..."
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) $(call DEFAULT_MIDL_LINE,$(SrcDir)/Sources/ActiveLedIt.odl,$(ObjDir)ActiveLedIt_h.tlb,$(ObjDir)ActiveLedIt_h.h,$(ObjDir)ActiveLedIt_i.c);\
	fi
	@$(call DEFAULT_MIDL_LINE,$(SrcDir)/Sources/ActiveLedIt.odl,$(ObjDir)ActiveLedIt_h.tlb,$(ObjDir)ActiveLedIt_h.h,$(ObjDir)ActiveLedIt_i.c) > $(ObjDir)MIDL-Build.out 2>&1





$(TARGETEXE):	$(ObjDir) $(Objs) $(StroikaLibs) $(ObjDir)ActiveLedIt.tlb
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Linking `$(StroikaRoot)ScriptsLib/SubstituteBackVariables $@`" "... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_LINK_LINE,$@)';\
	fi
	@$(call DEFAULT_LINK_LINE,$@)


clean::
	@rm -f ${MIDL_OUTPUT_} $(ObjDir)MIDL-Build.out