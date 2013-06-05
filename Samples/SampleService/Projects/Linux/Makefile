include	../Library/Configuration.mk
include	../Library/SharedBuildRules.mk
include	../Library/SharedMakeVariables.mk



RelPathToStroikaDevRoot	=	../../../../
SrcDir				=	$(RelPathToStroikaDevRoot)Samples/SampleService/
Includes				+=	-I.

TARGETEXE				=	$(RelPathToStroikaDevRoot)Builds/Platform_Linux/Samples_SampleService


VPATH	=			$(SrcDir)/Sources/


Objs	=	\
	$(ObjDir)SampleService.o\


all:	$(TARGETEXE)


$(TARGETEXE):	$(Objs)
	$(Linker) -o $(TARGETEXE) $(Objs) $(StroikaLinkerArgs) $(StroikaLibsWithSupportLibs)


