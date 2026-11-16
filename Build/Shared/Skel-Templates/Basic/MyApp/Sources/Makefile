export TOP_ROOT:=$(abspath ../../)/
StroikaSourceRoot	:=	$(TOP_ROOT)ThirdPartyComponents/Stroika/StroikaRoot/

include $(StroikaSourceRoot)Build/Lib/Make/SharedMakeVariables-Default.mk

SrcDir			:=	$(TOP_ROOT)${APP_NAME}/Sources/
ObjDir			:=	$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/${APP_NAME}/

include $(StroikaSourceRoot)Build/Lib/Make/SharedBuildRules-Default.mk

CXXFLAGS += -I"$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/${APP_NAME}/)"


TARGETEXEDir	:=	${StroikaPlatformTargetBuildDir}${APP_NAME}/
TARGETEXE		:=	$(TARGETEXEDir)${APP_NAME}${EXE_SUFFIX}


SubDirs:=\



SUBDIR_OBJS := $(foreach dir, $(SubDirs), $(shell $(MAKE) CONFIGURATION=$(CONFIGURATION) --directory $(dir) -s list-objs))


Objs	:=	\
	$(ObjDir)Main${OBJ_SUFFIX}\


VERSION_FILE_IN=$(TOP_ROOT)VERSION
VERSION_FILE_OUT=$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/${APP_NAME}/AppVersion.h
$(VERSION_FILE_OUT):	$(VERSION_FILE_IN)
	@mkdir -p $(StroikaIntermediateFilesRoot)$(CONFIGURATION)/${APP_NAME}
	@$(StroikaSourceRoot)Build/Scripts/MakeVersionFile $(VERSION_FILE_IN) $(VERSION_FILE_OUT) AppCPlusPlusVersion

$(ObjDir)Main${OBJ_SUFFIX} $(ObjDir)Service${OBJ_SUFFIX}:		$(VERSION_FILE_OUT)


all:
	@$(StroikaSourceRoot)/Build/Scripts/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) "Building ${APP_NAME}"
	@mkdir -p $(TARGETEXEDir)
	@$(MAKE) --silent $(VERSION_FILE_OUT) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@for s in ${SubDirs}; do $(MAKE) --directory $${s} -s MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) all; done
	@$(MAKE) --silent $(TARGETEXE) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))


Objs += $(SUBDIR_OBJS)

$(TARGETEXE):	$(Objs) $(StroikaLibs)
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Linking `$(StroikaSourceRoot)Build/Scripts/SubstituteBackVariables $@`" "... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaSourceRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_LINK_LINE, $@, stroika-frameworks)';\
	fi
	@$(call DEFAULT_LINK_LINE, $@, stroika-frameworks)


clean::
	@rm -f $(SUBDIR_OBJS)
