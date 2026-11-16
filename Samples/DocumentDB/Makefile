StroikaSourceRoot	:=	$(abspath ../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Samples/DocumentDB/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Samples-DocumentDB/


TARGETDIR		:=	${StroikaPlatformTargetBuildDir}Samples-DocumentDB/
TARGETEXE		:=	$(TARGETDIR)DocumentDB${EXE_SUFFIX}

Objs	:=	\
	$(ObjDir)Main${OBJ_SUFFIX}\
	$(ObjDir)ComputerNetwork${OBJ_SUFFIX}\
	$(ObjDir)EmployeesDB${OBJ_SUFFIX}\


include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk

# ONLY depend on foundation library, not frameworks library
StroikaLibs					=	$(StroikaFoundationLib)

VPATH	=			$(SrcDir)Sources/



all:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Samples/DocumentDB:"
	@mkdir -p $(TARGETDIR)
	@$(MAKE) --silent $(TARGETEXE) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))


check:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL) + 1)) -n "Checking Samples/DocumentDB...               "
	@$(StroikaSourceRoot)Build/Scripts/CheckFileExists $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]";


$(TARGETEXE):	$(ObjDir) $(Objs) $(StroikaLibs)
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Linking `$(StroikaSourceRoot)Build/Scripts/SubstituteBackVariables $@`" "... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaSourceRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1))  '$(call DEFAULT_LINK_LINE, $@, stroika-foundation)';\
	fi
	@$(call DEFAULT_LINK_LINE, $@, stroika-foundation)
