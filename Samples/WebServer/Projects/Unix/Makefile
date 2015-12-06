include	../Library/Configuration.mk
include	../Library/SharedBuildRules.mk
include	../Library/SharedMakeVariables.mk



SrcDir				=	$(StroikaRoot)Samples/WebServer/
Includes				+=	-I.

TARGETEXE				=	$(StroikaRoot)Builds/$(CONFIGURATION)/Samples_WebServer


VPATH	=			$(SrcDir)/Sources/


Objs	=	\
	$(ObjDir)WebServer.o\


all:
	@$(StroikaRoot)/ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Building Samples/SystemPerformanceClient:"
	@$(MAKE) --silent $(TARGETEXE) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES)


$(TARGETEXE):	$(Objs)
	@echo "      Linking $(subst $(StroikaRoot),\$$StroikaRoot/, $(TARGETEXE))..."
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    echo "         $(Linker) $(LinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerArgs) $(StroikaLibsWithSupportLibs)";\
	fi
	@$(Linker) $(LinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerArgs) $(StroikaLibsWithSupportLibs)


