# DEPRECATED: make's own build (run by Makefile-Legacy), kept until Stroika v3.0d26 ships - Stroika builds with CMake (CMakeLists.txt)
StroikaSourceRoot	:=	$(abspath ../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Samples/Service/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Samples-Service/


CXXFLAGS				+=	-I$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Samples-Service/)

TARGETDIR	:=	${StroikaPlatformTargetBuildDir}Samples-Service/
TARGETEXE	:=	$(TARGETDIR)Service${EXE_SUFFIX}


VPATH	=			$(SrcDir)Sources/


Objs	:=	\
	$(ObjDir)Service${OBJ_SUFFIX}\
	$(ObjDir)Main${OBJ_SUFFIX}\


include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


VERSION_FILE_IN=$(StroikaSourceRoot)STROIKA_VERSION
VERSION_FILE_OUT=$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Samples-Service/AppVersion.h
$(VERSION_FILE_OUT):	$(VERSION_FILE_IN)
	@mkdir -p $(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Samples-Service
	@$(StroikaSourceRoot)Build/Scripts/MakeVersionFile $(VERSION_FILE_IN) $(VERSION_FILE_OUT) AppCPlusPlusVersion

$(Objs):	$(VERSION_FILE_OUT)


.PHONY: all
all:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Samples/Service:"
	@mkdir -p $(TARGETDIR)
	@$(MAKE) --silent $(TARGETEXE) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --silent installers MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))


check:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL) + 1)) -n "Checking Samples/Service...            "
	@$(StroikaSourceRoot)Build/Scripts/CheckFileExists $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]";


clobber::
	@rm -rf $(TARGETDIR)


$(TARGETEXE):	$(Objs) $(StroikaLibs)
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Linking `$(StroikaSourceRoot)Build/Scripts/SubstituteBackVariables $@`" "... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaSourceRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_LINK_LINE, $@, stroika-frameworks)';\
	fi
	@$(call DEFAULT_LINK_LINE, $@, stroika-frameworks)


.PHONY:  installers installer-deb installer-rpm installer-wix
installers installer-deb installer-rpm installer-wix:   $(TARGETEXE)
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Samples/Service Installers:"
	@$(MAKE) --no-print-directory --directory Installers MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $@
