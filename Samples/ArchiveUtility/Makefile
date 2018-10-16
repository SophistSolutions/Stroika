include ../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

ifneq ($(CONFIGURATION),)
	ProjectPlatformSubdir	=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)
endif

SrcDir				=	$(StroikaRoot)Samples/ArchiveUtility/
ObjDir				=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples_ArchiveUtility/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk



MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO?=$(shell $(StroikaRoot)ScriptsLib/GetDefaultShellVariable.sh ECHO)
ECHO_BUILD_LINES?=0

Includes				+=	-I.

TARGETDIR				=	$(StroikaRoot)Builds/$(CONFIGURATION)/Samples-ArchiveUtility/
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
TARGETEXE				=	$(TARGETDIR)ArchiveUtility.exe
else
TARGETEXE				=	$(TARGETDIR)ArchiveUtility
endif


VPATH	=			$(SrcDir)/Sources/


Objs	=	\
	$(ObjDir)ArchiveUtility.o\


all:
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Building Samples/ArchiveUtility:"
	@mkdir -p $(TARGETDIR)
ifeq (Unix,$(ProjectPlatformSubdir))
	@$(MAKE) --silent $(TARGETEXE) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES)
else
	@perl $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:NoSummary Projects/$(ProjectPlatformSubdir)/ArchiveUtility.vcxproj /p:`$(StroikaRoot)ScriptsLib/GetVisualStudioConfigLine.pl $(CONFIGURATION)` /target:Build";
endif



check:
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL) + 1)) -n "Checking Samples/ArchiveUtility...           "
	@$(StroikaRoot)ScriptsLib/CheckFileExists.sh $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]";


$(TARGETEXE):	 $(ObjDir) $(Objs) $(StroikaLibs)
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Linking `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh $@`" "... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)/ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) "$(Linker) $(StroikaLinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerSuffixArgs)";\
	fi
	@$(Linker) $(StroikaLinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerSuffixArgs)
