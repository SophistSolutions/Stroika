StroikaRoot=$(abspath ../../)/

ifneq ($(CONFIGURATION),)
	include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

SrcDir				=	$(StroikaRoot)Samples/LedIt/
ObjDir				=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-LedIt/

TARGETDIR				=	$(StroikaRoot)Builds/$(CONFIGURATION)/Samples-LedIt/
TARGETEXE				=	$(TARGETDIR)LedIt${EXE_SUFFIX}


VPATH	=			$(SrcDir)/Sources/


Objs	=	\


RC_OUTPUT_=$(ObjDir)LedIt.res


ifeq ($(qFeatureFlag_ATLMFC), use-system)
Objs+= $(ObjDir)ColorMenu${OBJ_SUFFIX}
Objs+= $(ObjDir)FormatToolbar${OBJ_SUFFIX}
Objs+= $(ObjDir)LedItApplication${OBJ_SUFFIX}
Objs+= $(ObjDir)LedItControlItem${OBJ_SUFFIX}
Objs+= $(ObjDir)LedItDocument${OBJ_SUFFIX}
Objs+= $(ObjDir)LedItInPlaceFrame${OBJ_SUFFIX}
Objs+= $(ObjDir)LedItMainFrame${OBJ_SUFFIX}
Objs+= $(ObjDir)LedItServerItem${OBJ_SUFFIX}
Objs+= $(ObjDir)LedItView${OBJ_SUFFIX}
Objs+= $(ObjDir)Main${OBJ_SUFFIX}
Objs+= $(ObjDir)Options${OBJ_SUFFIX}
Objs+= $(ObjDir)RulerToolbar${OBJ_SUFFIX}
Objs+= $(ObjDir)Units${OBJ_SUFFIX}
endif

ifeq (MacOS.Net,$(findstring MacOS,$(TARGET_PLATFORMS)))
Objs+= $(ObjDir)FilteredFilePicker${OBJ_SUFFIX}
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
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Samples/LedIt:"
	@mkdir -p $(TARGETDIR)
ifeq (${USE_MSBUILD},1)
	if [[ "$(CONFIGURATION)" == "Debug-U-32" || "$(CONFIGURATION)" == "Release-U-32" ]] ; then\
		$(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:NoSummary Projects/$(ProjectPlatformSubdir)/LedIt.vcxproj /p:`$(StroikaRoot)ScriptsLib/GetVisualStudioConfigLine $(CONFIGURATION)` /target:Build";\
	else\
		$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "SKIPPED for unsupported configuration";\
	fi
else
	@$(MAKE) --silent $(TARGETEXE) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif



check:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL) + 1)) -n "Checking Samples/LedIt...                    "
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



MIDL_OUTPUT_=$(ObjDir)LedIt.tlb $(ObjDir)LedIt_i.c $(ObjDir)LedIt_h.h

$(ObjDir)LedIt_h.h $(ObjDir)LedIt_i.c:	$(ObjDir)LedIt.tlb

$(Objs):	$(ObjDir)LedIt_i.c $(ObjDir)LedIt_h.h


MIDL_FLAGS +=	/env win32

$(ObjDir)LedIt.tlb:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL)  "Running MIDL compiler on "`$(StroikaRoot)ScriptsLib/SubstituteBackVariables $(SrcDir)/Sources/LedIt.odl` "; see "`$(StroikaRoot)ScriptsLib/SubstituteBackVariables $(ObjDir)MIDL-Build.out`" ..."
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_MIDL_LINE,$(SrcDir)Sources/LedIt.odl,$(ObjDir)LedIt.tlb,$(ObjDir)LedIt_h.h,$(ObjDir)LedIt_i.c)';\
	fi
	@$(call DEFAULT_MIDL_LINE,$(SrcDir)Sources/LedIt.odl,$(ObjDir)LedIt.tlb,$(ObjDir)LedIt_h.h,$(ObjDir)LedIt_i.c) > $(ObjDir)MIDL-Build.out 2>&1


# English US culture
RC_FLAGS += /l 0x0409

$(ObjDir)LedIt.res:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL)  "Running RC compiler on "`$(StroikaRoot)ScriptsLib/SubstituteBackVariables $(SrcDir)/Sources/LedIt.rc` "; see "`$(StroikaRoot)ScriptsLib/SubstituteBackVariables $(ObjDir)RC-Build.out`" ..."
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_RC_LINE,$(SrcDir)Sources/LedIt.rc,$(ObjDir)LedIt.res)';\
	fi
	@$(call DEFAULT_RC_LINE,$(SrcDir)Sources/LedIt.rc,$(ObjDir)LedIt.res) > $(ObjDir)RC-Build.out 2>&1



# Cannot be done generically through configuration cuz a given build of Stroika might contain EXEs targetting multiple subsystems
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
EXTRA_SUFFIX_LINKER_ARGS+= /SUBSYSTEM:Windows
EXTRA_SUFFIX_LINKER_ARGS+= /ENTRY:wWinMainCRTStartup
endif

$(TARGETEXE):	$(ObjDir) $(Objs) $(StroikaLibs)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Linking `$(StroikaRoot)ScriptsLib/SubstituteBackVariables $@`" "... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_LINK_LINE,$@)';\
	fi
	@$(call DEFAULT_LINK_LINE,$@)
