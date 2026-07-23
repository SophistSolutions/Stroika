StroikaRoot	:=	$(abspath ../../../../../../)/
SrcDir		:=  $(StroikaRoot)Tools/Sources/Stroika/Frameworks/WebServer/HTMLViewCompiler/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Tools/Frameworks/WebServer/HTMLViewCompiler/

include $(StroikaRoot)/Build/ScriptsLib/SharedMakeVariables-Default.mk


VPATH = $(SrcDir)

TARGETEXE				=	${StroikaPlatformTargetBuildDir}bin/HTMLViewCompiler${EXE_SUFFIX}

Objs	:=	\
	$(ObjDir)HTMLViewCompiler${OBJ_SUFFIX}


include $(StroikaRoot)/Build/ScriptsLib/SharedBuildRules-Default.mk


all:	$(ObjDir) $(TARGETEXE)


$(TARGETEXE):	$(Objs) $(StroikaLibs)
	@echo "      Linking $(subst $(StroikaRoot),\$$StroikaRoot/, $@)..."
	@mkdir -p ${StroikaPlatformTargetBuildDir}bin
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)Build/ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_LINK_LINE, $@, stroika-foundation)';\
	fi
	@$(call DEFAULT_LINK_LINE, $@, stroika-foundation)


check:
	@$(StroikaRoot)/Build/ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL) + 1)) -n "Checking Tools/WebServer/HTMLViewCompiler...               "
	@$(StroikaRoot)Build/ScriptsLib/CheckFileExists $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]"


