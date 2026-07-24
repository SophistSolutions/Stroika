StroikaRoot	:=	$(abspath ../../../../..)/
SrcDir		:=  $(StroikaRoot)Tools/Sources/Stroika/Frameworks/WebServer/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Tools/Frameworks/WebServer/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk


VPATH		=	$(SrcDir) $(StroikaLibDir)

SubDirs	:=				\
	HTMLViewCompiler	\


include $(StroikaRoot)/Build/Lib/Make/SharedBuildRules-Default.mk


all check:
	@for SUBPROJ in $(SubDirs) ; do \
		$(MAKE) --directory $$SUBPROJ --no-print-directory --silent $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
    done	

clean clobber::
	@for SUBPROJ in $(SubDirs) ; do \
		$(MAKE) --directory $$SUBPROJ --no-print-directory --silent $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
    done	
