include	../Library/Configuration.mk
include	../Library/SharedBuildRules.mk
include	../Library/SharedMakeVariables.mk



RelPathToStroikaDevRoot	=	../../../
SrcDir				=	$(RelPathToStroikaDevRoot)Samples/SSDPServer/
Includes				+=	-I.

TARGETEXE				=	$(RelPathToStroikaDevRoot)Builds/DefaultConfiguration/Samples_SSDPServer


VPATH	=			$(SrcDir)/Sources/


Objs	=	\
	$(ObjDir)SSDPServer.o\


all:	$(TARGETEXE)


$(TARGETEXE):	$(Objs)
	@echo "   Linking $(TARGETEXE)..."
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    echo "      $(Linker) $(LinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerArgs) $(StroikaLibsWithSupportLibs)";\
	fi
	@$(Linker) $(LinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerArgs) $(StroikaLibsWithSupportLibs)


