include ../../../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir						=   $(StroikaRoot)Tools/Sources/Stroika/Frameworks/WebServer/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Tools/Frameworks/WebServer/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk



VPATH		=	$(SrcDir) $(StroikaLibDir)

SubDirs=				\
	HTMLViewCompiler	\



all:	$(SubDirs)
	@for SUBPROJ in $(SubDirs) ; do \
		($(MAKE) --directory $$SUBPROJ --no-print-directory --silent $@ CONFIGURATION=$(CONFIGURATION));\
    done	

clean clobber::	$(SubDirs)
	@for SUBPROJ in $(SubDirs) ; do \
		($(MAKE) --directory $$SUBPROJ --no-print-directory --silent $@ CONFIGURATION=$(CONFIGURATION));\
    done	


