export StroikaRoot?=$(shell realpath ../../)/

ifneq ($(CONFIGURATION),)
	include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk

SrcDir				=	$(StroikaRoot)Samples/Containers/
ObjDir				=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-Containers/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk

Includes				+=	-I.

TARGETDIR				=	$(StroikaRoot)Builds/$(CONFIGURATION)/Samples-Containers/
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
TARGETEXE				=	$(TARGETDIR)Containers.exe
else
TARGETEXE				=	$(TARGETDIR)Containers
endif


VPATH	=			$(SrcDir)/Sources/


Objs	=	\
	$(ObjDir)Main.o\
	\
	$(ObjDir)Collection.o\
	$(ObjDir)Mapping.o\
	$(ObjDir)Sequence.o\
	$(ObjDir)Set.o\


all:
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Building Samples/Containers:"
	@mkdir -p $(TARGETDIR)
ifeq (Unix,$(ProjectPlatformSubdir))
	@$(MAKE) --silent $(TARGETEXE) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
else
	@if [[ "$(CONFIGURATION)" == "Debug-U-32" || "$(CONFIGURATION)" == "Release-U-32" || "$(CONFIGURATION)" == "Debug-U-64" || "$(CONFIGURATION)" == "Release-U-64" ]] ; then\
		$(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:NoSummary Projects/$(ProjectPlatformSubdir)/Containers.vcxproj /p:`$(StroikaRoot)ScriptsLib/GetVisualStudioConfigLine $(CONFIGURATION)` /target:Build";\
	else\
		$(StroikaRoot)/ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) "SKIPPED for unsupported configuration";\
	fi
endif


check:
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL) + 1)) -n "Checking Samples/Containers...               "
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
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Linking `$(StroikaRoot)ScriptsLib/SubstituteBackVariables $@`" "... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)/ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) "$(Linker) $(StroikaLinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerSuffixArgs)";\
	fi
	@$(Linker) $(StroikaLinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerSuffixArgs)
