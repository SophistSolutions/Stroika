StroikaSourceRoot	:=	$(abspath ../../../../../../)/

include $(StroikaSourceRoot)/Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=  $(StroikaSourceRoot)Tools/Sources/Stroika/Frameworks/WebServer/HTMLViewCompiler/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Tools/Frameworks/WebServer/HTMLViewCompiler/


VPATH = $(SrcDir)

TARGETEXE				=	${StroikaPlatformTargetBuildDir}bin/HTMLViewCompiler${EXE_SUFFIX}

Objs	:=	\
	$(ObjDir)HTMLViewCompiler${OBJ_SUFFIX}


include $(StroikaSourceRoot)/Build/Lib/Make/SharedBuildRules-Default.mk


all:	$(ObjDir) $(TARGETEXE)


$(TARGETEXE):	$(Objs) $(StroikaLibs)
	@echo "      Linking $(subst $(StroikaSourceRoot),\$$StroikaSourceRoot/, $@)..."
	@mkdir -p ${StroikaPlatformTargetBuildDir}bin
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaSourceRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_LINK_LINE, $@, stroika-foundation)';\
	fi
	@$(call DEFAULT_LINK_LINE, $@, stroika-foundation)


check:
	@$(StroikaSourceRoot)/Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL) + 1)) -n "Checking Tools/WebServer/HTMLViewCompiler...               "
	@$(StroikaSourceRoot)Build/Scripts/CheckFileExists $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]"


