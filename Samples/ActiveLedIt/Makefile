StroikaRoot=$(abspath ../../)/

ifneq ($(CONFIGURATION),)
	#no error if missing cuz could be doing make clobber
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

SrcDir				=	$(StroikaRoot)Samples/ActiveLedIt/
ObjDir				=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-ActiveLedIt/

TARGETDIR			=	$(StroikaRoot)Builds/$(CONFIGURATION)/Samples-ActiveLedIt/
TARGETEXE			=	$(TARGETDIR)SophistSolutions.ActiveLedIt.dll


VPATH	=			$(SrcDir)Sources/


Objs	=	\
	$(ObjDir)ActiveLedIt${OBJ_SUFFIX}\
	$(ObjDir)ActiveLedItControl${OBJ_SUFFIX}\
	$(ObjDir)ActiveLedItPpg${OBJ_SUFFIX}\
	$(ObjDir)FontMenu${OBJ_SUFFIX}\
	$(ObjDir)LedItView${OBJ_SUFFIX}\
	$(ObjDir)Toolbar${OBJ_SUFFIX}\
	$(ObjDir)UserConfigCommands${OBJ_SUFFIX}\


RC_OUTPUT_=$(ObjDir)ActiveLedIt.res


ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
CXXFLAGS += /D_WINDOWS
CXXFLAGS += /DWIN32
CXXFLAGS += /DSTRICT
CXXFLAGS += /D_WIN32_WINNT=0x0501
Objs += ${RC_OUTPUT_}
endif


ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
CXXFLAGS += /I"$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(ObjDir))"
endif


include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk

#OCX ActiveX controls are DLLs. I THINK MSFT requires this to somehow get defined (but unclear where/for what)
CXXFLAGS +=   /D_WINDLL


all:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Samples/ActiveLedIt:"
	@mkdir -p $(TARGETDIR)
	@$(MAKE) --silent $(TARGETEXE) $(TARGETDIR)ActiveLedIt.cab MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))


check:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL) + 1)) -n "Checking Samples/ActiveLedIt...              "
	@$(StroikaRoot)ScriptsLib/CheckFileExists $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]";


# Cannot be done generically through configuration cuz a given build of Stroika might contain EXEs targetting multiple subsystems
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
EXTRA_SUFFIX_LINKER_ARGS+= /SUBSYSTEM:Windows
# OCX is a DLL, doesnt seem needed but examples pass this arg
EXTRA_SUFFIX_LINKER_ARGS+= /DLL
#flags from sample MSBUILD ActiveLedIt build link line I dont think are needed
#EXTRA_SUFFIX_LINKER_ARGS+= /DYNAMICBASE
#EXTRA_SUFFIX_LINKER_ARGS+= /MACHINE:X86 /SAFESEH:NO 
endif

MIDL_OUTPUT_=$(ObjDir)ActiveLedIt.tlb $(ObjDir)ActiveLedIt_i.c $(ObjDir)ActiveLedIt_h.h

$(ObjDir)ActiveLedIt_h.h $(ObjDir)ActiveLedIt_i.c:	$(ObjDir)ActiveLedIt.tlb

$(Objs):	$(ObjDir)ActiveLedIt_i.c $(ObjDir)ActiveLedIt_h.h


MIDL_FLAGS +=	/env win32
RC_FLAGS+=	/I"$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(ObjDir))"
# not sure if TLBID matters, but old msbuild script set it
EXTRA_SUFFIX_LINKER_ARGS+=/TLBID:1

$(ObjDir)ActiveLedIt.tlb:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL)  "Running MIDL compiler on "`$(StroikaRoot)ScriptsLib/SubstituteBackVariables $(SrcDir)Sources/ActiveLedIt.odl` "; see "`$(StroikaRoot)ScriptsLib/SubstituteBackVariables $(ObjDir)MIDL-Build.out`" ..."
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_MIDL_LINE,$(SrcDir)Sources/ActiveLedIt.odl,$(ObjDir)ActiveLedIt.tlb,$(ObjDir)ActiveLedIt_h.h,$(ObjDir)ActiveLedIt_i.c)';\
	fi
	@$(call DEFAULT_MIDL_LINE,$(SrcDir)Sources/ActiveLedIt.odl,$(ObjDir)ActiveLedIt.tlb,$(ObjDir)ActiveLedIt_h.h,$(ObjDir)ActiveLedIt_i.c) > $(ObjDir)MIDL-Build.out 2>&1


# English US culture
RC_FLAGS += /l 0x0409

$(ObjDir)ActiveLedIt.res:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL)  "Running RC compiler on "`$(StroikaRoot)ScriptsLib/SubstituteBackVariables $(SrcDir)Sources/ActiveLedIt.rc` "; see "`$(StroikaRoot)ScriptsLib/SubstituteBackVariables $(ObjDir)RC-Build.out`" ..."
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_RC_LINE,$(SrcDir)Sources/ActiveLedIt.rc,$(ObjDir)ActiveLedIt.res)';\
	fi
	@$(call DEFAULT_RC_LINE,$(SrcDir)Sources/ActiveLedIt.rc,$(ObjDir)ActiveLedIt.res) > $(ObjDir)RC-Build.out 2>&1


# without this the regsvr32 will fail; tried adding explict dllexport() in c++ code but couldnt find the right syntax to make it work
EXTRA_SUFFIX_LINKER_ARGS += /DEF:Sources/ActiveLedIt.def

$(TARGETEXE):	$(ObjDir) $(Objs) $(StroikaLibs) $(ObjDir)ActiveLedIt.tlb
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Linking `$(StroikaRoot)ScriptsLib/SubstituteBackVariables $@`" "... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_LINK_LINE,$@)';\
	fi
	@$(call DEFAULT_LINK_LINE,$@)




$(TARGETDIR)ActiveLedIt.cab: $(TARGETEXE)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "$@ ; see $(ObjDir)CAB-Build.OUT"` ... "
	@rm -rf $(ObjDir)CabBld
	@mkdir $(ObjDir)CabBld
	@rm -f $(ObjDir)CabBld/ActiveLedIt.inf
	@sed s/FILE_VERSION_STRING/%1,%2,%3,%4/g < ActiveLedItTemplate.inf > $(ObjDir)CabBld/ActiveLedIt.inf
	@# Make the CAB file (-s leaves room to be signed)
	@rm -f $(ObjDir)CabBld.ddf
	@cp $(TARGETEXE) $(ObjDir)CabBld
	@echo ".OPTION EXPLICIT     ; Generate errors" >> $(ObjDir)CabBld/CabBld.ddf
	@echo ".Set CabinetNameTemplate=ActiveLedIt.cab"      >> $(ObjDir)CabBld/CabBld.ddf
	@echo ".set DiskDirectoryTemplate=CDROM ; All cabinets in a single directory">> $(ObjDir)CabBld/CabBld.ddf
	@echo ".Set CompressionType=MSZIP;** All files are compressed in cabinet files">> $(ObjDir)CabBld/CabBld.ddf
	@echo ".Set UniqueFiles=\"OFF\"">> $(ObjDir)CabBld/CabBld.ddf
	@echo ".Set Cabinet=on">> $(ObjDir)CabBld/CabBld.ddf
	@echo ".Set DiskDirectory1=AAA">> $(ObjDir)CabBld/CabBld.ddf
	@echo "SophistSolutions.ActiveLedIt.dll" >> $(ObjDir)CabBld/CabBld.ddf
	@echo ActiveLedIt.inf >> $(ObjDir)CabBld/CabBld.ddf
	@cd $(ObjDir)/CabBld && makecab /F CabBld.ddf > ../CAB-Build.OUT 2>&1
	@mv $(ObjDir)/CabBld/AAA/ActiveLedIt.cab $@


register:	$(TARGETEXE)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Registering `$(StroikaRoot)ScriptsLib/SubstituteBackVariables $(TARGETEXE)`" "... "
	@#NOTE - if you run into errors - this typically needs to be run as administrator (run as administrator shell that invokes this)
	@#regsvr32 /s /c "$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(TARGETEXE))"
	@regsvr32 /c "$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(TARGETEXE))"


launch-sample:
	"C:/Program Files/Internet Explorer/iexplore.exe" 'file://$(shell cygpath --absolute --windows Samples/HTML-JavaScript/CommandSupportTester.html)'