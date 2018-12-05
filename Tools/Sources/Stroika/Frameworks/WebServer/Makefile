export StroikaRoot=$(shell realpath ../../../../..)/

ifneq ($(CONFIGURATION),)
include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk
endif

SrcDir						=   $(StroikaRoot)Tools/Sources/Stroika/Frameworks/WebServer/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Tools/Frameworks/WebServer/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk



VPATH		=	$(SrcDir) $(StroikaLibDir)

SubDirs=				\
	HTMLViewCompiler	\



all check:	$(SubDirs)
	@for SUBPROJ in $(SubDirs) ; do \
		$(MAKE) --directory $$SUBPROJ --no-print-directory --silent $@ CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
    done	

clean clobber::	$(SubDirs)
	@for SUBPROJ in $(SubDirs) ; do \
		$(MAKE) --directory $$SUBPROJ --no-print-directory --silent $@ CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
    done	


