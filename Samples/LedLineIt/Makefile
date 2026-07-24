StroikaRoot	:=	$(abspath ../../)/
SrcDir		:=	$(StroikaRoot)Samples/LedLineIt/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-LedLineIt/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk
include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk

TARGETDIR	:=	${StroikaPlatformTargetBuildDir}Samples-LedLineIt/
TARGETEXE	:=	$(TARGETDIR)LedLineIt${EXE_SUFFIX}


VPATH	=			$(SrcDir)Sources/

RC_OUTPUT_=$(ObjDir)LedLineIt.res


Objs	:=	\

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



ifeq (Windows,$(findstring Windows,$(TargetPlatforms)))
# AVOID "_WIN32_WINNT not defined. Defaulting to _WIN32_WINNT_MAXVER (see WinSDKVer.h)"
CXXFLAGS += -D_WIN32_WINNT=0x0501
# Maybe helps avoid bugs?
CXXFLAGS += -DSTRICT
endif


all:
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Samples/LedLineIt:"
	@mkdir -p $(TARGETDIR)
	@$(MAKE) --silent $(TARGETEXE) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))


check:
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL) + 1)) -n "Checking Samples/LedLineIt...                "
	@$(StroikaRoot)Build/Scripts/CheckFileExists $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]";


MIDL_OUTPUT_=$(ObjDir)LedLineIt.tlb $(ObjDir)LedLineIt_i.c $(ObjDir)LedLineIt_h.h

$(ObjDir)LedLineIt_h.h $(ObjDir)LedLineIt_i.c:	$(ObjDir)LedLineIt.tlb

$(Objs):	$(ObjDir)LedLineIt_i.c $(ObjDir)LedLineIt_h.h


MIDL_FLAGS +=	-env win32


$(ObjDir)LedLineIt.tlb:
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL)  "Running MIDL compiler on "`$(StroikaRoot)Build/Scripts/SubstituteBackVariables $(SrcDir)Sources/LedLineIt.odl` "; see "`$(StroikaRoot)Build/Scripts/SubstituteBackVariables $(ObjDir)MIDL-Build.out`" ..."
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_MIDL_LINE,$(SrcDir)Sources/LedLineIt.odl,$(ObjDir)LedLineIt.tlb,$(ObjDir)LedLineIt_h.h,$(ObjDir)LedLineIt_i.c)';\
	fi
	@$(call DEFAULT_MIDL_LINE,$(SrcDir)Sources/LedLineIt.odl,$(ObjDir)LedLineIt.tlb,$(ObjDir)LedLineIt_h.h,$(ObjDir)LedLineIt_i.c) > $(ObjDir)MIDL-Build.out 2>&1


# English US culture
RC_FLAGS += -l 0x0409

$(ObjDir)LedLineIt.res:
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL)  "Running RC compiler on "`$(StroikaRoot)Build/Scripts/SubstituteBackVariables $(SrcDir)Sources/LedLineIt.rc` "; see "`$(StroikaRoot)Build/Scripts/SubstituteBackVariables $(ObjDir)RC-Build.out`" ..."
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_RC_LINE,$(SrcDir)Sources/LedLineIt.rc,$(ObjDir)LedLineIt.res)';\
	fi
	@$(call DEFAULT_RC_LINE,$(SrcDir)Sources/LedLineIt.rc,$(ObjDir)LedLineIt.res) > $(ObjDir)RC-Build.out 2>&1


# Cannot be done generically through configuration cuz a given build of Stroika might contain EXEs targetting multiple subsystems
ifeq (Windows,$(findstring Windows,$(TargetPlatforms)))
LinkerArgs_ExtraSuffix_	+= -SUBSYSTEM:Windows
LinkerArgs_ExtraSuffix_+= -ENTRY:wWinMainCRTStartup
endif


$(TARGETEXE):	$(ObjDir) $(Objs) $(StroikaLibs)
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Linking `$(StroikaRoot)Build/Scripts/SubstituteBackVariables $@`" "... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_LINK_LINE, $@, stroika-frameworks, ${LinkerArgs_ExtraSuffix_})';\
	fi
	@$(call DEFAULT_LINK_LINE, $@, stroika-frameworks, ${LinkerArgs_ExtraSuffix_})


