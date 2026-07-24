StroikaRoot	:=	$(abspath ../../)/
SrcDir		:=	$(StroikaRoot)Samples/WebService/
ObjDir		:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-WebService/

include $(StroikaRoot)Build/Lib/Make/SharedMakeVariables-Default.mk


CXXFLAGS				+=	-I$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-WebService/)

TARGETDIR		:=	${StroikaPlatformTargetBuildDir}Samples-WebService/
TARGETEXE		:=	$(TARGETDIR)WebService${EXE_SUFFIX}


VPATH	=			$(SrcDir)Sources/


Objs	:=	\
	$(ObjDir)IWSAPI${OBJ_SUFFIX}\
	$(ObjDir)Main${OBJ_SUFFIX}\
	$(ObjDir)Model${OBJ_SUFFIX}\
	$(ObjDir)WebServer${OBJ_SUFFIX}\
	$(ObjDir)WSImpl${OBJ_SUFFIX}\


include $(StroikaRoot)Build/Lib/Make/SharedBuildRules-Default.mk


VERSION_FILE_IN=$(StroikaRoot)STROIKA_VERSION
VERSION_FILE_OUT=$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-WebService/AppVersion.h
$(VERSION_FILE_OUT):	$(VERSION_FILE_IN)
	@mkdir -p $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-WebService
	@$(StroikaRoot)Build/Scripts/MakeVersionFile $(VERSION_FILE_IN) $(VERSION_FILE_OUT) AppCPlusPlusVersion

$(Objs):	$(VERSION_FILE_OUT)



all:
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Samples/WebService:"
	@mkdir -p $(TARGETDIR)
	@$(MAKE) --silent $(TARGETEXE) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))


check:
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL) + 1)) -n "Checking Samples/WebService...                "
	@$(StroikaRoot)Build/Scripts/CheckFileExists $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]";


$(TARGETEXE):	$(ObjDir) $(Objs) $(StroikaLibs)
	@$(StroikaRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Linking `$(StroikaRoot)Build/Scripts/SubstituteBackVariables $@`" "... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_LINK_LINE, $@, stroika-frameworks)';\
	fi
	@$(call DEFAULT_LINK_LINE, $@, stroika-frameworks)

