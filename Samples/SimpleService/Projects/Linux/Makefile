include	../Library/Configuration.mk
include	../Library/SharedBuildRules.mk
include	../Library/SharedMakeVariables.mk



RelPathToStroikaDevRoot	=	../../../
SrcDir				=	$(RelPathToStroikaDevRoot)Samples/SimpleService/
Includes				+=	-I.

TARGETEXE				=	$(RelPathToStroikaDevRoot)Builds/DefaultConfiguration/Samples_SimpleService


VPATH	=			$(SrcDir)/Sources/


Objs	=	\
	$(ObjDir)SimpleService.o\


all:	$(TARGETEXE)


$(TARGETEXE):	$(Objs)
	$(Linker) $(LinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerArgs) $(StroikaLibsWithSupportLibs)


