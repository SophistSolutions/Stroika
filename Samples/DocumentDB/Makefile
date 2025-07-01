StroikaRoot	:=	$(abspath ../../)/
SrcDir		:=	$(StroikaRoot)Samples/DocumentDB/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-DocumentDB/

include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk



TARGETDIR		:=	${StroikaPlatformTargetBuildDir}Samples-DocumentDB/
TARGETEXE		:=	$(TARGETDIR)DocumentDB${EXE_SUFFIX}

Objs	:=	\
	$(ObjDir)Main${OBJ_SUFFIX}\
	$(ObjDir)ComputerNetwork${OBJ_SUFFIX}\
	$(ObjDir)EmployeesDB${OBJ_SUFFIX}\


include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk

# ONLY depend on foundation library, not frameworks library
StroikaLibs					=	$(StroikaFoundationLib)

VPATH	=			$(SrcDir)Sources/



all:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Samples/DocumentDB:"
	@mkdir -p $(TARGETDIR)
	@$(MAKE) --silent $(TARGETEXE) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))


check:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL) + 1)) -n "Checking Samples/DocumentDB...               "
	@$(StroikaRoot)ScriptsLib/CheckFileExists $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]";


$(TARGETEXE):	$(ObjDir) $(Objs) $(StroikaLibs)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Linking `$(StroikaRoot)ScriptsLib/SubstituteBackVariables $@`" "... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1))  '$(call DEFAULT_LINK_LINE, $@, stroika-foundation';\
	fi
	@$(call DEFAULT_LINK_LINE, $@, stroika-foundation)
