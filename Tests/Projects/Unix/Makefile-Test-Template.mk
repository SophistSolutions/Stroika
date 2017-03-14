SrcDir				=	$(StroikaRoot)Tests/$(TEST_NUM)/
ObjDir				=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Tests/$(TEST_NUM)/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk


MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO?=$(shell $(StroikaRoot)ScriptsLib/GetDefaultShellVariable.sh ECHO)
ECHO_BUILD_LINES?=0


ifndef TEST_NUM
	TEST_NUM		=	_UNDEFINED_FIX_INCLUDER_OF_TEMPLATE_TO_SPECIFY_THIS_
endif


Includes				+=	-I.

MAKE_INDENT_LEVEL?=$(MAKELEVEL)

TARGETEXE				=	$(StroikaRoot)Builds/$(CONFIGURATION)/Test$(TEST_NUM)


VPATH	=			$(SrcDir):$(SrcDir)../TestHarness/


Objs	=	\
	$(ObjDir)Test.o\
	$(ObjDir)TestHarness.o\
	$(ObjDir)SimpleClass.o\


all:	$(ObjDir) $(TARGETEXE)


$(TARGETEXE):	$(Objs)
	@$(StroikaRoot)/ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Linking $(subst $(StroikaRoot),\$$StroikaRoot/, $(TARGETEXE))..."
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)/ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && $(ECHO) "$(Linker) $(LinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerArgs) $(StroikaLibsWithSupportLibs)";\
	fi
	@$(Linker) $(LinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerArgs) $(StroikaLibsWithSupportLibs)
