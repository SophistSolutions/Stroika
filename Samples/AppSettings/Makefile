StroikaRoot		:=	$(abspath ../../)/
SrcDir			:=	$(StroikaRoot)Samples/AppSettings/
ObjDir			:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-AppSettings/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk


TARGETDIR		:=	${StroikaPlatformTargetBuildDir}Samples-AppSettings/
TARGETEXE		:=	$(TARGETDIR)AppSettings${EXE_SUFFIX}

# ONLY depend on foundation library, not frameworks library
StroikaLibs					=	$(StroikaFoundationLib)

VPATH	=			$(SrcDir)Sources/


Objs	:=	\
	$(ObjDir)AppConfiguration${OBJ_SUFFIX}\
	$(ObjDir)MoreConfiguration${OBJ_SUFFIX}\
	$(ObjDir)Main${OBJ_SUFFIX}\

include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


all:
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Samples/AppSettings:"
	@mkdir -p $(TARGETDIR)
	@$(MAKE) --silent $(TARGETEXE) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))


check:
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL) + 1)) -n "Checking Samples/AppSettings...           "
	@$(StroikaRoot)Build/Scripts/CheckFileExists $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]";


$(TARGETEXE):	 $(ObjDir) $(Objs) $(StroikaLibs)
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Linking `$(StroikaRoot)Build/Scripts/SubstituteBackVariables $@`" "... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_LINK_LINE, $@, stroika-foundation)';\
	fi
	@$(call DEFAULT_LINK_LINE, $@, stroika-foundation)
