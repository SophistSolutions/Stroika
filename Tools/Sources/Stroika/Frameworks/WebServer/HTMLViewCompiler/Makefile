StroikaRoot=$(abspath ../../../../../../)/

ifneq ($(CONFIGURATION),)
	include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

SrcDir						=   $(StroikaRoot)Tools/Sources/Stroika/Frameworks/WebServer/HTMLViewCompiler/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Tools/Frameworks/WebServer/HTMLViewCompiler/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk


VPATH = $(SrcDir)

TARGETEXE				=	$(StroikaRoot)Builds/$(CONFIGURATION)/HTMLViewCompiler${EXE_SUFFIX}


ifeq (Unix,$(ProjectPlatformSubdir))
Objs	=	\
	$(ObjDir)HTMLViewCompiler${OBJ_SUFFIX}
else
Objs	=	
endif


all:	$(ObjDir) $(TARGETEXE)


$(TARGETEXE):	$(Objs) $(StroikaLibs)
ifeq ($(USE_MSBUILD), 1)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(( $(MAKE_INDENT_LEVEL)+1)) -n "Building Stroika Tools-Frameworks-WebServer-HTMLViewCompiler..."
	@$(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:NoSummary ../../../../../Projects/$(ProjectPlatformSubdir)/StroikaTools-Frameworks-WebServer-HTMLViewCompiler.vcxproj /p:`$(StroikaRoot)ScriptsLib/GetVisualStudioConfigLine $(CONFIGURATION)` /target:Build"
	@$(ECHO) done
else
	@echo "      Linking $(subst $(StroikaRoot),\$$StroikaRoot/, $@)..."
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    echo "         $(call DEFAULT_LINK_LINE, $@)";\
	fi
	@$(call DEFAULT_LINK_LINE, $@)
endif


check:
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL) + 1)) -n "Checking Tools/WebServer/HTMLViewCompiler...               "
	@$(StroikaRoot)ScriptsLib/CheckFileExists $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]"


