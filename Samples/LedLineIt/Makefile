StroikaRoot=$(abspath ../../)/

ifneq ($(CONFIGURATION),)
	include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk

SrcDir				=	$(StroikaRoot)Samples/LedLineIt/
ObjDir				=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-LedLineIt/

include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk


TARGETDIR	=	$(StroikaRoot)Builds/$(CONFIGURATION)/Samples-LedLineIt/
TARGETEXE	=	$(TARGETDIR)LedLineIt${EXE_SUFFIX}


VPATH	=			$(SrcDir)/Sources/

RC_OUTPUT_=$(ObjDir)LedLineIt.res


Objs	=	\

ifeq ($(qFeatureFlag_ATLMFC), use-system)
Objs+= $(ObjDir)FontMenu${OBJ_SUFFIX}
Objs+= $(ObjDir)LedLineItApplication${OBJ_SUFFIX}
Objs+= $(ObjDir)LedLineItDocFrame${OBJ_SUFFIX}
Objs+= $(ObjDir)LedLineItDocument${OBJ_SUFFIX}
Objs+= $(ObjDir)LedLineItInPlaceFrame${OBJ_SUFFIX}
Objs+= $(ObjDir)LedLineItMainFrame${OBJ_SUFFIX}
Objs+= $(ObjDir)LedLineItServerItem${OBJ_SUFFIX}
Objs+= $(ObjDir)LedLineItView${OBJ_SUFFIX}
Objs+= $(ObjDir)Options${OBJ_SUFFIX}
Objs += ${RC_OUTPUT_}
endif



ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
CXXFLAGS += /D_WINDOWS 
CXXFLAGS += /DWIN32 
CXXFLAGS += /DSTRICT
CXXFLAGS += /D_WIN32_WINNT=0x0501 
endif


all:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Samples/LedLineIt:"
	@mkdir -p $(TARGETDIR)
	@$(MAKE) --silent $(TARGETEXE) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))


check:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL) + 1)) -n "Checking Samples/LedLineIt...                "
	@$(StroikaRoot)ScriptsLib/CheckFileExists $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]";


MIDL_OUTPUT_=$(ObjDir)LedLineIt.tlb $(ObjDir)LedLineIt_i.c $(ObjDir)LedLineIt_h.h

$(ObjDir)LedLineIt_h.h $(ObjDir)LedLineIt_i.c:	$(ObjDir)LedLineIt.tlb

$(Objs):	$(ObjDir)LedLineIt_i.c $(ObjDir)LedLineIt_h.h


MIDL_FLAGS +=	/env win32


$(ObjDir)LedLineIt.tlb:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL)  "Running MIDL compiler on "`$(StroikaRoot)ScriptsLib/SubstituteBackVariables $(SrcDir)/Sources/LedLineIt.odl` "; see "`$(StroikaRoot)ScriptsLib/SubstituteBackVariables $(ObjDir)MIDL-Build.out`" ..."
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_MIDL_LINE,$(SrcDir)/Sources/LedLineIt.odl,$(ObjDir)LedLineIt.tlb,$(ObjDir)LedLineIt_h.h,$(ObjDir)LedLineIt_i.c)';\
	fi
	@$(call DEFAULT_MIDL_LINE,$(SrcDir)/Sources/LedLineIt.odl,$(ObjDir)LedLineIt.tlb,$(ObjDir)LedLineIt_h.h,$(ObjDir)LedLineIt_i.c) > $(ObjDir)MIDL-Build.out 2>&1


# English US culture
RC_FLAGS += /l 0x0409

$(ObjDir)LedLineIt.res:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL)  "Running RC compiler on "`$(StroikaRoot)ScriptsLib/SubstituteBackVariables $(SrcDir)/Sources/LedLineIt.rc` "; see "`$(StroikaRoot)ScriptsLib/SubstituteBackVariables $(ObjDir)RC-Build.out`" ..."
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_RC_LINE,$(SrcDir)Sources/LedLineIt.rc,$(ObjDir)LedLineIt.res)';\
	fi
	@$(call DEFAULT_RC_LINE,$(SrcDir)Sources/LedLineIt.rc,$(ObjDir)LedLineIt.res) > $(ObjDir)RC-Build.out 2>&1





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


