StroikaRoot=$(abspath ../../)/

ifneq ($(CONFIGURATION),)
	include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk

SrcDir				=	$(StroikaRoot)Samples/WebServer/
ObjDir				=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-WebServer/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk

CXXFLAGS				+=	-I$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-WebServer/

TARGETDIR				=	$(StroikaRoot)Builds/$(CONFIGURATION)/Samples-WebServer/
TARGETEXE				=	$(TARGETDIR)WebServer${EXE_SUFFIX}


VPATH	=			$(SrcDir)/Sources/


Objs	=	\
	$(ObjDir)WebServer${OBJ_SUFFIX}\


VERSION_FILE_IN=$(StroikaRoot)STROIKA_VERSION
VERSION_FILE_OUT=$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-WebServer/AppVersion.h
$(VERSION_FILE_OUT):	$(VERSION_FILE_IN)
	@mkdir -p $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-WebServer
	@$(StroikaRoot)ScriptsLib/MakeVersionFile $(VERSION_FILE_IN) $(VERSION_FILE_OUT) AppCPlusPlusVersion kAppVersion __Stroika_Sample_WebServer_AppVersion_H__

$(Objs):	$(VERSION_FILE_OUT)



all:
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Samples/WebServer:"
	@mkdir -p $(TARGETDIR)
ifeq (Unix,$(ProjectPlatformSubdir))
	@$(MAKE) --silent $(TARGETEXE) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
else
	@$(MAKE) --silent $(VERSION_FILE_OUT)
	@if [[ "$(CONFIGURATION)" == "Debug-U-32" || "$(CONFIGURATION)" == "Release-U-32" || "$(CONFIGURATION)" == "Debug-U-64" || "$(CONFIGURATION)" == "Release-U-64" ]] ; then\
		$(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:NoSummary Projects/$(ProjectPlatformSubdir)/WebServer.vcxproj /p:`$(StroikaRoot)ScriptsLib/GetVisualStudioConfigLine $(CONFIGURATION)` /target:Build";\
	else\
		$(StroikaRoot)/ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "SKIPPED for unsupported configuration";\
	fi
endif
	@mkdir -p $(TARGETDIR)html && cp -r sample-html-folder/* $(TARGETDIR)html/


check:
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL) + 1)) -n "Checking Samples/WebServer...                "
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



$(TARGETEXE):	$(ObjDir) $(Objs) $(StroikaLibs)
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Linking `$(StroikaRoot)ScriptsLib/SubstituteBackVariables $@`" "... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)/ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "$(Linker) $(StroikaLinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerSuffixArgs)";\
	fi
	@$(Linker) $(StroikaLinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerSuffixArgs)


