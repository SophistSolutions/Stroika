# DEPRECATED: make's own build (run by Makefile-Legacy), kept until Stroika v3.0d26 ships - Stroika builds with CMake (CMakeLists.txt)
StroikaSourceRoot	:=	$(abspath ../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir			:=	$(StroikaSourceRoot)Samples/SQL/
ObjDir			:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Samples-SQL/


TARGETDIR		:=	${StroikaPlatformTargetBuildDir}Samples-SQL/
TARGETEXE		:=	$(TARGETDIR)SQL${EXE_SUFFIX}

Objs	:=	\
	$(ObjDir)Main${OBJ_SUFFIX}\
	$(ObjDir)ComputerNetwork${OBJ_SUFFIX}\
	$(ObjDir)DirectEmployeesDB${OBJ_SUFFIX}\
	$(ObjDir)ORMEmployeesDB${OBJ_SUFFIX}\
	$(ObjDir)ThreadTest${OBJ_SUFFIX}\


include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk

### @todo not sure we need this anymore - REVISIT - and grep/search for these macros (StroikaLibs) and StroikaFoundationLib
# ONLY depend on foundation library, not frameworks library
StroikaLibs					=	$(StroikaFoundationLib)

VPATH	=			$(SrcDir)Sources/



all:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Samples/SQL:"
	@mkdir -p $(TARGETDIR)
	@$(MAKE) --silent $(TARGETEXE) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))


check:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL) + 1)) -n "Checking Samples/SQL...               "
	@$(StroikaSourceRoot)Build/Scripts/CheckFileExists $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]";


$(TARGETEXE):	$(ObjDir) $(Objs) $(StroikaLibs)
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Linking `$(StroikaSourceRoot)Build/Scripts/SubstituteBackVariables $@`" "... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaSourceRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1))  '$(call DEFAULT_LINK_LINE, $@, stroika-frameworks)';\
	fi
	@$(call DEFAULT_LINK_LINE, $@, stroika-frameworks)
