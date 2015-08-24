include	../Library/Configuration.mk
include	../Library/SharedBuildRules.mk
include	../Library/SharedMakeVariables.mk


ifndef TEST_NUM
	TEST_NUM		=	_UNDEFINED_FIX_INCLUDER_OF_TEMPLATE_TO_SPECIFY_THIS_
endif


RelPathToStroikaDevRoot	=	../../../
SrcDir				=	$(RelPathToStroikaDevRoot)Tests/$(TEST_NUM)/
Includes				+=	-I.

TARGETEXE				=	$(RelPathToStroikaDevRoot)Builds/DefaultConfiguration/Test$(TEST_NUM)


VPATH	=			$(SrcDir):$(SrcDir)../TestHarness/


Objs	=	\
	$(ObjDir)Test.o\
	$(ObjDir)TestHarness.o\
	$(ObjDir)SimpleClass.o\


all:	$(TARGETEXE)


$(TARGETEXE):	$(Objs)
	@echo "   Linking $(TARGETEXE)..."
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    echo "      $(Linker) $(LinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerArgs) $(StroikaLibsWithSupportLibs)";\
	fi
	@$(Linker) $(LinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerArgs) $(StroikaLibsWithSupportLibs)
