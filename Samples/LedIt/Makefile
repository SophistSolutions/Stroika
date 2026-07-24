StroikaRoot	:=	$(abspath ../../)/
SrcDir		:=	$(StroikaRoot)Samples/LedIt/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-LedIt/


include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

TARGETDIR		:=	${StroikaPlatformTargetBuildDir}Samples-LedIt/
TARGETEXE		:=	$(TARGETDIR)LedIt${EXE_SUFFIX}


VPATH	=			$(SrcDir)Sources/


Objs	:=	\

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



ifeq (Windows,$(findstring Windows,$(TargetPlatforms)))
# AVOID "_WIN32_WINNT not defined. Defaulting to _WIN32_WINNT_MAXVER (see WinSDKVer.h)"
CPPFLAGS += -D_WIN32_WINNT=0x0501
# Maybe helps avoid bugs?
CPPFLAGS += -DSTRICT
# AVOID C:\Program Files (x86)\Windows Kits\10\include\10.0.22621.0\um\transact.h(147): error C2872: 'byte': ambiguous symbol
CPPFLAGS += -DWIN32
Objs += $(ObjDir)LedIt.res
endif

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Samples/LedIt:"
	@mkdir -p $(TARGETDIR)
	@$(MAKE) --silent $(TARGETEXE) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))


check:
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL) + 1)) -n "Checking Samples/LedIt...                    "
	@$(StroikaRoot)Build/Scripts/CheckFileExists $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]";



MIDL_OUTPUT_=$(ObjDir)LedIt.tlb $(ObjDir)LedIt_i.c $(ObjDir)LedIt_h.h

$(ObjDir)LedIt_h.h $(ObjDir)LedIt_i.c:	$(ObjDir)LedIt.tlb

$(Objs):	$(ObjDir)LedIt_i.c $(ObjDir)LedIt_h.h


MIDL_FLAGS +=	-env win32

$(ObjDir)LedIt.tlb:
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL)  "Running MIDL compiler on "`$(StroikaRoot)Build/Scripts/SubstituteBackVariables $(SrcDir)Sources/LedIt.odl` "; see "`$(StroikaRoot)Build/Scripts/SubstituteBackVariables $(ObjDir)MIDL-Build.out`" ..."
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_MIDL_LINE,$(SrcDir)Sources/LedIt.odl,$(ObjDir)LedIt.tlb,$(ObjDir)LedIt_h.h,$(ObjDir)LedIt_i.c)';\
	fi
	@$(call DEFAULT_MIDL_LINE,$(SrcDir)Sources/LedIt.odl,$(ObjDir)LedIt.tlb,$(ObjDir)LedIt_h.h,$(ObjDir)LedIt_i.c) > $(ObjDir)MIDL-Build.out 2>&1


# English US culture
RC_FLAGS += /l 0x0409

$(ObjDir)LedIt.res:
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL)  "Running RC compiler on "`$(StroikaRoot)Build/Scripts/SubstituteBackVariables $(SrcDir)Sources/LedIt.rc` "; see "`$(StroikaRoot)Build/Scripts/SubstituteBackVariables $(ObjDir)RC-Build.out`" ..."
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_RC_LINE,$(SrcDir)Sources/LedIt.rc,$(ObjDir)LedIt.res)';\
	fi
	@$(call DEFAULT_RC_LINE,$(SrcDir)Sources/LedIt.rc,$(ObjDir)LedIt.res) > $(ObjDir)RC-Build.out 2>&1



# Cannot be done generically through configuration cuz a given build of Stroika might contain EXEs targetting multiple subsystems
LinkerArgs_ExtraSuffix_	:=
ifeq (Windows,$(findstring Windows,$(TargetPlatforms)))
LinkerArgs_ExtraSuffix_	+= -SUBSYSTEM:Windows
LinkerArgs_ExtraSuffix_	+= -ENTRY:wWinMainCRTStartup
endif


$(TARGETEXE):	$(ObjDir) $(Objs) $(StroikaLibs)
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Linking `$(StroikaRoot)Build/Scripts/SubstituteBackVariables $@`" "... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_LINK_LINE,$@,stroika-frameworks, , ${LinkerArgs_ExtraSuffix_})';\
	fi
	@$(call DEFAULT_LINK_LINE,$@, stroika-frameworks, , ${LinkerArgs_ExtraSuffix_}) 
