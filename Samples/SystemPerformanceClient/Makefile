StroikaRoot	:=	$(abspath ../../)/
SrcDir		:=	$(StroikaRoot)Samples/SystemPerformanceClient/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-SystemPerformanceClient/

include $(StroikaRoot)Build/ScriptsLib/SharedMakeVariables-Default.mk


TARGETDIR		:=	${StroikaPlatformTargetBuildDir}Samples-SystemPerformanceClient/
TARGETEXE		:=	$(TARGETDIR)SystemPerformanceClient${EXE_SUFFIX}

VPATH	=			$(SrcDir)Sources/


Objs	=	\
	$(ObjDir)SystemPerformanceClient${OBJ_SUFFIX}\


include $(StroikaRoot)Build/ScriptsLib/SharedBuildRules-Default.mk


all:
	@$(StroikaRoot)Build/ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Samples/SystemPerformanceClient:"
	@mkdir -p $(TARGETDIR)
	@$(MAKE) --silent $(TARGETEXE) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))


check:
	@$(StroikaRoot)Build/ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL) + 1)) -n "Checking Samples/SystemPerformanceClient...  "
	@$(StroikaRoot)Build/ScriptsLib/CheckFileExists $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]";


$(TARGETEXE):	$(ObjDir) $(Objs) $(StroikaLibs)
	@$(StroikaRoot)Build/ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Linking `$(StroikaRoot)Build/ScriptsLib/SubstituteBackVariables $@`" "... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)Build/ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_LINK_LINE, $@, stroika-frameworks)';\
	fi
	@$(call DEFAULT_LINK_LINE, $@, stroika-frameworks)


