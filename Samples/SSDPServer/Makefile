include ../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir				=	$(StroikaRoot)Samples/SSDPServer/
ObjDir				=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples_SSDPServer/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk


MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO?=$(shell $(StroikaRoot)ScriptsLib/GetDefaultShellVariable.sh ECHO)
ECHO_BUILD_LINES?=0


Includes				+=	-I.

TARGETEXE				=	$(StroikaRoot)Builds/$(CONFIGURATION)/Samples_SSDPServer


VPATH	=			$(SrcDir)/Sources/


Objs	=	\
	$(ObjDir)SSDPServer.o\


all:
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Building Samples/SSDPServer:"
	@$(MAKE) --silent $(TARGETEXE) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES)


$(TARGETEXE):	$(ObjDir) $(Objs)
	@$(StroikaRoot)/ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Linking $(shell $(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh `realpath --canonicalize-missing $@`)..."
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)/ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) "$(Linker) $(LinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerArgs) $(StroikaLibsWithSupportLibs)";\
	fi
	@$(Linker) $(LinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerArgs) $(StroikaLibsWithSupportLibs)


