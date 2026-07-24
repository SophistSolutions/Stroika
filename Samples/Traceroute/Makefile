StroikaRoot	:=	$(abspath ../../)/
SrcDir		:=	$(StroikaRoot)Samples/Traceroute/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-Traceroute/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk


TARGETDIR		:=	${StroikaPlatformTargetBuildDir}Samples-Traceroute/
TARGETEXE		:=	$(TARGETDIR)Traceroute${EXE_SUFFIX}


VPATH	=			$(SrcDir)Sources/


Objs	=	\
	$(ObjDir)Traceroute${OBJ_SUFFIX}\


include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Samples/Traceroute:"
	@mkdir -p $(TARGETDIR)
	@$(MAKE) --silent $(TARGETEXE) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))


check:
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL) + 1)) -n "Checking Samples/Traceroute...               "
	@$(StroikaRoot)Build/Scripts/CheckFileExists $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]";


$(TARGETEXE):	$(ObjDir) $(Objs) $(StroikaLibs)
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Linking `$(StroikaRoot)Build/Scripts/SubstituteBackVariables $@`" "... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_LINK_LINE, $@, stroika-frameworks)';\
	fi
	@$(call DEFAULT_LINK_LINE, $@, stroika-frameworks)
