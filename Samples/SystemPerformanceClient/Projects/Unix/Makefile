include	../Library/Configuration.mk
include	../Library/SharedBuildRules.mk
include	../Library/SharedMakeVariables.mk



SrcDir				=	$(StroikaRoot)Samples/SystemPerformanceClient/
Includes				+=	-I.

TARGETEXE				=	$(StroikaRoot)Builds/DefaultConfiguration/Samples_SystemPerformanceClient


VPATH	=			$(SrcDir)/Sources/


Objs	=	\
	$(ObjDir)SystemPerformanceClient.o\


all:	$(TARGETEXE)


$(TARGETEXE):	$(Objs)
	@echo "      Linking $(TARGETEXE)..."
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    echo "         $(Linker) $(LinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerArgs) $(StroikaLibsWithSupportLibs)";\
	fi
	@$(Linker) $(LinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerArgs) $(StroikaLibsWithSupportLibs)


