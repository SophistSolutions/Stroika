StroikaRoot	:=	$(abspath ../../)/

include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

SrcDir			:=	$(StroikaRoot)Samples/SQL/
ObjDir			:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-SQL/


TARGETDIR		:=	${StroikaPlatformTargetBuildDir}Samples-SQL/
TARGETEXE		:=	$(TARGETDIR)SQL${EXE_SUFFIX}

Objs	:=	\
	$(ObjDir)Main${OBJ_SUFFIX}\
	$(ObjDir)ComputerNetwork${OBJ_SUFFIX}\
	$(ObjDir)DirectEmployeesDB${OBJ_SUFFIX}\
	$(ObjDir)ORMEmployeesDB${OBJ_SUFFIX}\
	$(ObjDir)ThreadTest${OBJ_SUFFIX}\


include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk

### @todo not sure we need this anymore - REVISIT - and grep/search for these macros (StroikaLibs) and StroikaFoundationLib
# ONLY depend on foundation library, not frameworks library
StroikaLibs					=	$(StroikaFoundationLib)

VPATH	=			$(SrcDir)Sources/



all:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Samples/SQL:"
	@mkdir -p $(TARGETDIR)
	@$(MAKE) --silent $(TARGETEXE) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))


check:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL) + 1)) -n "Checking Samples/SQL...               "
	@$(StroikaRoot)ScriptsLib/CheckFileExists $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]";


$(TARGETEXE):	$(ObjDir) $(Objs) $(StroikaLibs)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Linking `$(StroikaRoot)ScriptsLib/SubstituteBackVariables $@`" "... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1))  '$(call DEFAULT_LINK_LINE, $@, stroika-frameworks)';\
	fi
	@$(call DEFAULT_LINK_LINE, $@, stroika-frameworks)
