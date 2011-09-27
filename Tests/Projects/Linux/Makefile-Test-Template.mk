include	../Library/Configuration.mk
include	../Library/SharedBuildRules.mk
include	../Library/SharedMakeVariables.mk


ifndef TEST_NUM=
	TEST_NUM=		=	_UNDEFINED_FIX_INCLUDER_OF_TEMPLATE_TO_SPECIFY_THIS_
endif


RelPathToDevRoot		=	../../../../
SrcDir				=	$(RelPathToDevRoot)Tests/$(TEST_NUM)/
Includes				+=	-I.

TARGETEXE				=	$(RelPathToDevRoot)Builds/Platform_Linux/Test$(TEST_NUM)


VPATH	=			$(SrcDir):$(SrcDir)../TestHarness/


Objs	=	\
	$(ObjDir)Test.o\
	$(ObjDir)TestHarness.o\
	$(ObjDir)SimpleClass.o\


all:	$(TARGETEXE)


$(TARGETEXE):	$(Objs)
	$(Linker) -o $(TARGETEXE) $(Objs) $(StroikaLibsWithSupportLibs)


