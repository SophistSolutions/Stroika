# DEPRECATED: make's own build (run by Makefile-Legacy), kept until Stroika v3.0d26 ships - Stroika builds with CMake (CMakeLists.txt)
StroikaSourceRoot	:=	$(abspath ../../)/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir		:=	$(StroikaSourceRoot)Samples/WebService/
ObjDir		:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Samples-WebService/


CXXFLAGS				+=	-I$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Samples-WebService/)

TARGETDIR		:=	${StroikaPlatformTargetBuildDir}Samples-WebService/
TARGETEXE		:=	$(TARGETDIR)WebService${EXE_SUFFIX}


VPATH	=			$(SrcDir)Sources/


Objs	:=	\
	$(ObjDir)IWSAPI${OBJ_SUFFIX}\
	$(ObjDir)Main${OBJ_SUFFIX}\
	$(ObjDir)Model${OBJ_SUFFIX}\
	$(ObjDir)WebServer${OBJ_SUFFIX}\
	$(ObjDir)WSImpl${OBJ_SUFFIX}\


include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk


VERSION_FILE_IN=$(StroikaSourceRoot)STROIKA_VERSION
VERSION_FILE_OUT=$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Samples-WebService/AppVersion.h
$(VERSION_FILE_OUT):	$(VERSION_FILE_IN)
	@mkdir -p $(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Samples-WebService
	@$(StroikaSourceRoot)Build/Scripts/MakeVersionFile $(VERSION_FILE_IN) $(VERSION_FILE_OUT) AppCPlusPlusVersion

$(Objs):	$(VERSION_FILE_OUT)



all:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Samples/WebService:"
	@mkdir -p $(TARGETDIR)
	@$(MAKE) --silent $(TARGETEXE) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))


check:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL) + 1)) -n "Checking Samples/WebService...                "
	@$(StroikaSourceRoot)Build/Scripts/CheckFileExists $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]";


$(TARGETEXE):	$(ObjDir) $(Objs) $(StroikaLibs)
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Linking `$(StroikaSourceRoot)Build/Scripts/SubstituteBackVariables $@`" "... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaSourceRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_LINK_LINE, $@, stroika-frameworks)';\
	fi
	@$(call DEFAULT_LINK_LINE, $@, stroika-frameworks)

