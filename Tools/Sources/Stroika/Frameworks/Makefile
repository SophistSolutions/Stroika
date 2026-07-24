StroikaRoot	:=	$(abspath ../../../../)/
SrcDir		:=  $(StroikaRoot)Tools/Sources/Stroika/Frameworks/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Tools/Frameworks/

include $(StroikaRoot)/Build/Lib/Make/SharedMakeVariables-Default.mk


VPATH		=	$(SrcDir)

SubDirs	:=				\
	WebServer	\


include $(StroikaRoot)/Build/Lib/Make/SharedBuildRules-Default.mk


all check:	$(SubDirs)
	@for SUBPROJ in $(SubDirs) ; do \
		$(MAKE) --directory $$SUBPROJ --no-print-directory $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
    done	

clean clobber::	$(SubDirs)
	@for SUBPROJ in $(SubDirs) ; do \
		$(MAKE) --directory $$SUBPROJ --no-print-directory $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
    done	


