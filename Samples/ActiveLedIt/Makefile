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

$(ObjDir)ActiveLedIt_h.h:	$(ObjDir)ActiveLedIt.tlb
$(ObjDir)ActiveLedIt_i.c:	$(ObjDir)ActiveLedIt.tlb


###tmphack - must replace with list of all C includes from configfile
### MAYBE ADD CPPFLAGS (like CXXFLAGS), and use for includes and defines

CPPFLAGS=
#CPPFLAGS	+=	/D"NDEBUG"
CPPFLAGS+=	/I"C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.22.27905/ATLMFC/include" /I"C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.22.27905/include" /I"C:/Program Files (x86)/Windows Kits/NETFXSDK/4.7.2/include/um" /I"C:/Program Files (x86)/Windows Kits/10/include/10.0.18362.0/ucrt" /I"C:/Program Files (x86)/Windows Kits/10/include/10.0.18362.0/shared" /I"C:/Program Files (x86)/Windows Kits/10/include/10.0.18362.0/um" /I"C:/Program Files (x86)/Windows Kits/10/include/10.0.18362.0/winrt" /I"C:/Program Files (x86)/Windows Kits/10/include/10.0.18362.0/cppwinrt"

MIDL_FLAGS_=	${CPPFLAGS}
MIDL_FLAGS_+=	/nologo
MIDL_FLAGS_+=	/env win32
MIDL_FLAGS_+=	/W1
MIDL_FLAGS_+=	/char signed
MIDL_FLAGS_+=	/I'$(call FUNCTION_CONVERT_FILES_TO_COMPILER_NATIVE,$(StroikaRoot)IntermediateFiles/$(CONFIGURATION))' 
MIDL_FLAGS_+=	/I'$(call FUNCTION_CONVERT_FILES_TO_COMPILER_NATIVE,$(StroikaRoot)Library/Sources)' 


$(ObjDir)ActiveLedIt.tlb:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL)  "Running MIDL compiler on "`$(StroikaRoot)ScriptsLib/SubstituteBackVariables $(SrcDir)/Sources/ActiveLedIt.odl` "; see "`$(StroikaRoot)ScriptsLib/SubstituteBackVariables $(ObjDir)MIDL-Build.out`" ..."
	PATH='$(TOOLS_PATH_ADDITIONS):$(PATH)';\
	INCLUDES='$(WIN_INCLUDES_PATH)'; \
		midl \
			/iid '$(call FUNCTION_CONVERT_FILES_TO_COMPILER_NATIVE,$(ObjDir)ActiveLedIt_i.c)'\
			$(MIDL_FLAGS_)\
			/h '$(call FUNCTION_CONVERT_FILES_TO_COMPILER_NATIVE,$(ObjDir)ActiveLedIt_h.h)' \
			/tlb '$(call FUNCTION_CONVERT_FILES_TO_COMPILER_NATIVE,$(ObjDir)ActiveLedIt_h.tlb)' \
			/target "NT60" \
			'$(call FUNCTION_CONVERT_FILES_TO_COMPILER_NATIVE,$(SrcDir)/Sources/ActiveLedIt.odl)' \
			> $(ObjDir)MIDL-Build.out 2>&1



$(ObjDir)ActiveLedIt${OBJ_SUFFIX}:	$(ObjDir)ActiveLedIt_i.c $(ObjDir)ActiveLedIt_h.h
$(ObjDir)LedItView${OBJ_SUFFIX}:	$(ObjDir)ActiveLedIt_i.c $(ObjDir)ActiveLedIt_h.h


$(TARGETEXE):	$(ObjDir) $(Objs) $(StroikaLibs) $(ObjDir)ActiveLedIt.tlb
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Linking `$(StroikaRoot)ScriptsLib/SubstituteBackVariables $@`" "... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_LINK_LINE,$@)';\
	fi
	@$(call DEFAULT_LINK_LINE,$@)


clean::
	@rm -f ${MIDL_OUTPUT_} $(ObjDir)MIDL-Build.out