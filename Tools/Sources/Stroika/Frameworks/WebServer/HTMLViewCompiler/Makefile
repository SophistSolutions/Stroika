include ../../../../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir						=   $(StroikaRoot)Tools/Sources/Stroika/Frameworks/WebServer/HTMLViewCompiler/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Tools/Frameworks/WebServer/HTMLViewCompiler/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk



VPATH = $(SrcDir)

TARGETEXE				=	$(StroikaRoot)Builds/$(CONFIGURATION)/HTMLViewCompiler


Objs	=	\
	$(ObjDir)HTMLViewCompiler.o\

all:	$(ObjDir) $(TARGETEXE)


$(TARGETEXE):	$(Objs)
	@echo "      Linking $(subst $(StroikaRoot),\$$StroikaRoot/, $(TARGETEXE))..."
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    echo "         $(Linker) $(LinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerArgs) $(StroikaLibsWithSupportLibs)";\
	fi
	@$(Linker) $(LinkerPrefixArgs) -o $(TARGETEXE) $(Objs) $(StroikaLinkerArgs) $(StroikaLibsWithSupportLibs)



