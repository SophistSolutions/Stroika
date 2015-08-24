include	../Library/Configuration.mk
include	../Library/SharedBuildRules.mk
include	../Library/SharedMakeVariables.mk



RelPathToStroikaDevRoot	=	../../../
SrcDir				=	$(RelPathToStroikaDevRoot)Samples/WebServer/
Includes				+=	-I.

TARGETEXE				=	$(RelPathToStroikaDevRoot)Builds/DefaultConfiguration/Samples_WebServer


VPATH	=			$(SrcDir)/Sources/


Objs	=	\
	$(ObjDir)WebServer.o\


all:	$(TARGETEXE)


$(TARGETEXE):	$(Objs)
	@echo "   Linking $(TARGETEXE)..."
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    echo "      $(Linker) $(LinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerArgs) $(StroikaLibsWithSupportLibs)";\
	fi
	@$(Linker) $(LinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerArgs) $(StroikaLibsWithSupportLibs)


