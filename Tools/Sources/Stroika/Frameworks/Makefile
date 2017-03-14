include ../../../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

SrcDir						=   $(StroikaRoot)Tools/Sources/Stroika/Frameworks/
ObjDir						=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Tools/Frameworks/

include $(StroikaRoot)/Library/Projects/Unix/SharedBuildRules-Default.mk
include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk


VPATH		=	$(SrcDir)

SubDirs=				\
	WebServer	\


all:	$(SubDirs)
	@for SUBPROJ in $(SubDirs) ; do \
		($(MAKE) --directory $$SUBPROJ --no-print-directory $@ CONFIGURATION=$(CONFIGURATION));\
    done	

clean clobber::	$(SubDirs)
	@for SUBPROJ in $(SubDirs) ; do \
		($(MAKE) --directory $$SUBPROJ --no-print-directory $@ CONFIGURATION=$(CONFIGURATION));\
    done	


