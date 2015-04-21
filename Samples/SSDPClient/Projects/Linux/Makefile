include	../Library/Configuration.mk
include	../Library/SharedBuildRules.mk
include	../Library/SharedMakeVariables.mk



RelPathToStroikaDevRoot	=	../../../
SrcDir				=	$(RelPathToStroikaDevRoot)Samples/SSDPClient/
Includes				+=	-I.

TARGETEXE				=	$(RelPathToStroikaDevRoot)Builds/DefaultConfiguration/Samples_SSDPClient


VPATH	=			$(SrcDir)/Sources/


Objs	=	\
	$(ObjDir)SSDPClient.o\


all:	$(TARGETEXE)


$(TARGETEXE):	$(Objs)
	$(Linker) $(LinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerArgs) $(StroikaLibsWithSupportLibs)


