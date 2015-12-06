include	../Library/Configuration.mk
include	../Library/SharedBuildRules.mk
include	../Library/SharedMakeVariables.mk



SrcDir				=	$(StroikaRoot)Samples/SystemPerformanceClient/
Includes				+=	-I.

TARGETEXE				=	$(StroikaRoot)Builds/$(CONFIGURATION)/Samples_SystemPerformanceClient


VPATH	=			$(SrcDir)/Sources/


Objs	=	\
	$(ObjDir)SystemPerformanceClient.o\


all:	$(TARGETEXE)


$(TARGETEXE):	$(Objs)
	@echo "      Linking $(subst $(StroikaRoot),\$$StroikaRoot/, $(TARGETEXE))..."
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(ECHO) "         $(Linker) $(LinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerArgs) $(StroikaLibsWithSupportLibs)";\
	fi
	@$(Linker) $(LinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerArgs) $(StroikaLibsWithSupportLibs)


