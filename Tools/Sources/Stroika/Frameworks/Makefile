# DEPRECATED: make's own build (run by Makefile-Legacy), kept until Stroika v3.0d26 ships - Stroika builds with CMake (CMakeLists.txt)
StroikaSourceRoot	:=	$(abspath ../../../../)/

include $(StroikaSourceRoot)/Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=  $(StroikaSourceRoot)Tools/Sources/Stroika/Frameworks/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Tools/Frameworks/


VPATH		=	$(SrcDir)

SubDirs	:=				\
	WebServer	\


include $(StroikaSourceRoot)/Build/Lib/Make/SharedBuildRules-Default.mk


all check:	$(SubDirs)
	@for SUBPROJ in $(SubDirs) ; do \
		$(MAKE) --directory $$SUBPROJ --no-print-directory $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
    done	

clean clobber::	$(SubDirs)
	@for SUBPROJ in $(SubDirs) ; do \
		$(MAKE) --directory $$SUBPROJ --no-print-directory $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
    done	


