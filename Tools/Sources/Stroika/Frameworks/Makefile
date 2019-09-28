StroikaRoot=$(abspath ../../../../)/

ifneq ($(CONFIGURATION),)
	include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

SrcDir	=   $(StroikaRoot)Tools/Sources/Stroika/Frameworks/
ObjDir	=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Tools/Frameworks/


include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)/ScriptsLib/SharedMakeVariables-Default.mk


VPATH		=	$(SrcDir)

SubDirs=				\
	WebServer	\


include $(StroikaRoot)/ScriptsLib/SharedBuildRules-Default.mk


all check:	$(SubDirs)
	@for SUBPROJ in $(SubDirs) ; do \
		$(MAKE) --directory $$SUBPROJ --no-print-directory $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
    done	

clean clobber::	$(SubDirs)
	@for SUBPROJ in $(SubDirs) ; do \
		$(MAKE) --directory $$SUBPROJ --no-print-directory $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
    done	


