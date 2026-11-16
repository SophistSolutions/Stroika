StroikaSourceRoot	:=	$(abspath ../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Samples/Traceroute/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Samples-Traceroute/


TARGETDIR		:=	${StroikaPlatformTargetBuildDir}Samples-Traceroute/
TARGETEXE		:=	$(TARGETDIR)Traceroute${EXE_SUFFIX}


VPATH	=			$(SrcDir)Sources/


Objs	=	\
	$(ObjDir)Traceroute${OBJ_SUFFIX}\


include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Samples/Traceroute:"
	@mkdir -p $(TARGETDIR)
	@$(MAKE) --silent $(TARGETEXE) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))


check:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL) + 1)) -n "Checking Samples/Traceroute...               "
	@$(StroikaSourceRoot)Build/Scripts/CheckFileExists $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]";


$(TARGETEXE):	$(ObjDir) $(Objs) $(StroikaLibs)
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Linking `$(StroikaSourceRoot)Build/Scripts/SubstituteBackVariables $@`" "... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaSourceRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_LINK_LINE, $@, stroika-frameworks)';\
	fi
	@$(call DEFAULT_LINK_LINE, $@, stroika-frameworks)
