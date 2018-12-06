export StroikaRoot?=$(shell realpath ../../)/

ifneq ($(CONFIGURATION),)
	include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk
	ProjectPlatformSubdir			=	$(shell $(StroikaRoot)ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" ProjectPlatformSubdir)
endif

SrcDir				=	$(StroikaRoot)Samples/WebServer/
ObjDir				=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples_WebServer/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk


MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO?=$(shell $(StroikaRoot)ScriptsLib/GetDefaultShellVariable.sh ECHO)
ECHO_BUILD_LINES?=0

Includes				+=	-I.

TARGETDIR				=	$(StroikaRoot)Builds/$(CONFIGURATION)/Samples-WebServer/
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
TARGETEXE				=	$(TARGETDIR)WebServer.exe
else
TARGETEXE				=	$(TARGETDIR)WebServer
endif


VPATH	=			$(SrcDir)/Sources/


Objs	=	\
	$(ObjDir)WebServer.o\


all:
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Building Samples/WebServer:"
	@mkdir -p $(TARGETDIR)
ifeq (Unix,$(ProjectPlatformSubdir))
	@$(MAKE) --silent $(TARGETEXE) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES)
else
	@if [[ "$(CONFIGURATION)" == "Debug-U-32" || "$(CONFIGURATION)" == "Release-U-32" || "$(CONFIGURATION)" == "Debug-U-64" || "$(CONFIGURATION)" == "Release-U-64" ]] ; then\
		perl $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:NoSummary Projects/$(ProjectPlatformSubdir)/WebServer.vcxproj /p:`$(StroikaRoot)ScriptsLib/GetVisualStudioConfigLine.pl $(CONFIGURATION)` /target:Build";\
	else\
		$(StroikaRoot)/ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) "SKIPPED for unsupported configuration";\
	fi
endif
	@mkdir -p $(TARGETDIR)html && cp -r sample-html-folder/* $(TARGETDIR)html/


check:
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL) + 1)) -n "Checking Samples/WebServer...                "
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	@if [[ "$(CONFIGURATION)" == "Debug-U-32" || "$(CONFIGURATION)" == "Release-U-32" || "$(CONFIGURATION)" == "Debug-U-64" || "$(CONFIGURATION)" == "Release-U-64" ]] ; then\
		$(StroikaRoot)ScriptsLib/CheckFileExists.sh $(TARGETEXE);\
		$(ECHO) "[SUCCEEDED]";\
	else\
		$(ECHO) "[SKIPPED]";\
	fi
else
	@$(StroikaRoot)ScriptsLib/CheckFileExists.sh $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]";
endif



$(TARGETEXE):	$(ObjDir) $(Objs) $(StroikaLibs)
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Linking `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh $@`" "... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)/ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) "$(Linker) $(StroikaLinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerSuffixArgs)";\
	fi
	@$(Linker) $(StroikaLinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerSuffixArgs)


