include	../Library/Configuration.mk
include	../Library/SharedBuildRules.mk
include	../Library/SharedMakeVariables.mk



RelPathToStroikaDevRoot	=	../../../
SrcDir				=	$(RelPathToStroikaDevRoot)Samples/SystemPerformanceClient/
Includes				+=	-I.

TARGETEXE				=	$(RelPathToStroikaDevRoot)Builds/DefaultConfiguration/Samples_SystemPerformanceClient


VPATH	=			$(SrcDir)/Sources/


Objs	=	\
	$(ObjDir)SystemPerformanceClient.o\


all:	$(TARGETEXE)


$(TARGETEXE):	$(Objs)
	$(Linker) $(LinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerArgs) $(StroikaLibsWithSupportLibs)


