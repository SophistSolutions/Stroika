include ../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir				=	$(StroikaRoot)Samples/SimpleService/
ObjDir				=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples_SimpleService/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk

SHELL=/bin/bash


MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO?=$(shell $(StroikaRoot)ScriptsLib/GetDefaultShellVariable.sh ECHO)
ECHO_BUILD_LINES?=0


Includes				+=	-I.

TARGETEXE				=	$(StroikaRoot)Builds/$(CONFIGURATION)/Samples_SimpleService


VPATH	=			$(SrcDir)/Sources/


Objs	=	\
	$(ObjDir)Service.o\
	$(ObjDir)Main.o\


VERSION_FILE_IN=$(StroikaRoot)STROIKA_VERSION
VERSION_FILE_OUT=$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-SimpleService/AppVersion.h
$(VERSION_FILE_OUT):	$(VERSION_FILE_IN)
	@$(StroikaRoot)ScriptsLib/MakeVersionFile.sh $(VERSION_FILE_IN) $(VERSION_FILE_OUT) AppCPlusPlusVersion kAppVersion __Stroika_Sample_ServiceSample_AppVersion_H__

$(Objs):	$(VERSION_FILE_OUT)


all:
	@$(StroikaRoot)/ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Building Samples/SimpleService:"
	@$(MAKE) --silent $(TARGETEXE) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES)


$(TARGETEXE):	$(ObjDir) $(Objs)
	@$(StroikaRoot)/ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Linking $(subst $(StroikaRoot),\$$StroikaRoot/, $(TARGETEXE))..."
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)/ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && $(ECHO) "$(Linker) $(LinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerArgs) $(StroikaLibsWithSupportLibs)";\
	fi
	@$(Linker) $(LinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerArgs) $(StroikaLibsWithSupportLibs)
