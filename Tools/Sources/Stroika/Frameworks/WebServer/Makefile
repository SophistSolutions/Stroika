StroikaSourceRoot	:=	$(abspath ../../../../..)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=  $(StroikaSourceRoot)Tools/Sources/Stroika/Frameworks/WebServer/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Tools/Frameworks/WebServer/


VPATH		=	$(SrcDir) $(StroikaLibDir)

SubDirs	:=				\
	HTMLViewCompiler	\


include $(StroikaSourceRoot)/Build/Lib/Make/SharedBuildRules-Default.mk


all check:
	@for SUBPROJ in $(SubDirs) ; do \
		$(MAKE) --directory $$SUBPROJ --no-print-directory --silent $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
    done	

clean clobber::
	@for SUBPROJ in $(SubDirs) ; do \
		$(MAKE) --directory $$SUBPROJ --no-print-directory --silent $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
    done	
