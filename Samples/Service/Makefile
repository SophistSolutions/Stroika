StroikaRoot=$(abspath ../../)/

ifneq ($(CONFIGURATION),)
	include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif


SrcDir				=	$(StroikaRoot)Samples/Service/
ObjDir				=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-Service/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk


CXXFLAGS				+=	-I$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-Service/

TARGETDIR				=	$(StroikaRoot)Builds/$(CONFIGURATION)/Samples-Service/
TARGETEXE				=	$(TARGETDIR)Service${EXE_SUFFIX}


VPATH	=			$(SrcDir)/Sources/


Objs	=	\
	$(ObjDir)Service${OBJ_SUFFIX}\
	$(ObjDir)Main${OBJ_SUFFIX}\


VERSION_FILE_IN=$(StroikaRoot)STROIKA_VERSION
VERSION_FILE_OUT=$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-Service/AppVersion.h
$(VERSION_FILE_OUT):	$(VERSION_FILE_IN)
	@mkdir -p $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-Service
	@$(StroikaRoot)ScriptsLib/MakeVersionFile $(VERSION_FILE_IN) $(VERSION_FILE_OUT) AppCPlusPlusVersion kAppVersion __Stroika_Sample_ServiceSample_AppVersion_H__

$(Objs):	$(VERSION_FILE_OUT)


.PHONY: all
all:
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Samples/Service:"
	@mkdir -p $(TARGETDIR)
ifeq (Unix,$(ProjectPlatformSubdir))
	@$(MAKE) --silent $(TARGETEXE) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --silent installers MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
else
	@$(MAKE) --silent $(VERSION_FILE_OUT)
	@if [[ "$(CONFIGURATION)" == "Debug-U-32" || "$(CONFIGURATION)" == "Release-U-32" || "$(CONFIGURATION)" == "Debug-U-64" || "$(CONFIGURATION)" == "Release-U-64" ]] ; then\
		$(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:NoSummary Projects/$(ProjectPlatformSubdir)/Service.vcxproj /p:`$(StroikaRoot)ScriptsLib/GetVisualStudioConfigLine $(CONFIGURATION)` /target:Build";\
		$(MAKE) --silent installers MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
	else\
		$(StroikaRoot)/ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "SKIPPED for unsupported configuration";\
	fi
endif


check:
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL) + 1)) -n "Checking Samples/Service...            "
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	@if [[ "$(CONFIGURATION)" == "Debug-U-32" || "$(CONFIGURATION)" == "Release-U-32" || "$(CONFIGURATION)" == "Debug-U-64" || "$(CONFIGURATION)" == "Release-U-64" ]] ; then\
		$(StroikaRoot)ScriptsLib/CheckFileExists $(TARGETEXE);\
		$(ECHO) "[SUCCEEDED]";\
	else\
		$(ECHO) "[SKIPPED]";\
	fi
else
	@$(StroikaRoot)ScriptsLib/CheckFileExists $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]";
endif


clobber::
	@rm -rf $(TARGETDIR)


ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
else
$(TARGETEXE):	$(Objs) $(StroikaLibs)
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Linking `$(StroikaRoot)ScriptsLib/SubstituteBackVariables $@`" "... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)/ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "$(Linker) $(StroikaLinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerSuffixArgs)";\
	fi
	@$(Linker) $(StroikaLinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerSuffixArgs)
endif


.PHONY:  installers installer-deb installer-rpm installer-wix
installers installer-deb installer-rpm installer-wix:   $(TARGETEXE)
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Installers:"
	@$(MAKE) --no-print-directory --directory Installers MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $@
