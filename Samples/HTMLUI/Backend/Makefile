StroikaRoot:=$(abspath ../../../)/

ifneq ($(CONFIGURATION),)
	#no error if missing cuz could be doing make clobber
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

SrcDir			:=	$(StroikaRoot)Samples/HTMLUI/Backend/
ObjDir			:=	$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-HTMLUI/Backend/
ResDir			:=	$(ObjDir)Resources/

CXXFLAGS				+=	-I$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-HTMLUI/Backend/)

TARGETDIR		:=	$(StroikaRoot)Builds/$(CONFIGURATION)/Samples-HTMLUI/Backend/
TARGETHTMLDIR	:=	$(StroikaRoot)Builds/$(CONFIGURATION)/Samples-HTMLUI/html/
TARGETEXE		:=	$(TARGETDIR)HTMLUI${EXE_SUFFIX}


VPATH	=			$(SrcDir)/Sources/


SubDirs:=\

SUBDIR_OBJS := $(foreach dir, $(SubDirs), $(shell $(MAKE) CONFIGURATION=$(CONFIGURATION) --directory $(dir) -s list-objs))

# For .embed files
CPPFLAGS += -I"$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(ResDir))"

Objs	:=	\
	$(ObjDir)AppConfiguration${OBJ_SUFFIX}\
	$(ObjDir)OperationalStatistics${OBJ_SUFFIX}\
	$(ObjDir)Main${OBJ_SUFFIX}\
	$(ObjDir)Model${OBJ_SUFFIX}\
	$(ObjDir)Service${OBJ_SUFFIX}\
	$(ObjDir)WebServer${OBJ_SUFFIX}\
	$(ObjDir)WSImpl${OBJ_SUFFIX}\

include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk


VERSION_FILE_IN=$(StroikaRoot)STROIKA_VERSION
VERSION_FILE_OUT=$(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-HTMLUI/Backend/AppVersion.h
$(VERSION_FILE_OUT):	$(VERSION_FILE_IN)
	@mkdir -p $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-HTMLUI
	@$(StroikaRoot)ScriptsLib/MakeVersionFile $(VERSION_FILE_IN) $(VERSION_FILE_OUT) AppCPlusPlusVersion

$(Objs):	$(VERSION_FILE_OUT)


Resources_Src:=\
	../OpenAPI/api.json \

### assumes filenames all unique no matter what dir they come from
Resources_EmbedFiles:=\
	 $(patsubst %,%.embed,$(patsubst %,$(ResDir)%,$(notdir $(Resources_Src))))

# $(info Resources_EmbedFiles=$(Resources_EmbedFiles))

# easilest to just compile all if any out of date
$(Resources_EmbedFiles):
	@mkdir -p $(ResDir)
	@for i in $(Resources_Src) ; do \
		(cd `dirname $$i`; xxd -i `basename $$i` ${ResDir}`basename $$i`.embed) ; \
	done


$(ObjDir)WSImpl${OBJ_SUFFIX}:	${Resources_EmbedFiles}


# Objs += $(SUBDIR_OBJS)

all:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Samples/HTMLUI/Backend:"
	@mkdir -p $(TARGETDIR)
	@for s in ${SubDirs}; do $(MAKE) --directory $${s} -s MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) all; done
	@$(MAKE) --silent $(TARGETEXE) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	mkdir -p ${TARGETHTMLDIR}; ln -s ${TARGETHTMLDIR} ${TARGETDIR}html


check:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL) + 1)) -n "Checking Samples/HTMLUI/Backend/...                "
	@$(StroikaRoot)ScriptsLib/CheckFileExists $(TARGETEXE)
	@$(ECHO) "[SUCCEEDED]";


$(TARGETEXE):	$(ObjDir) $(Objs) $(StroikaLibs)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Linking `$(StroikaRoot)ScriptsLib/SubstituteBackVariables $@`" "... "
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_LINK_LINE,$@)';\
	fi
	@$(call DEFAULT_LINK_LINE,$@)


clean::
	@rm -rf $(ObjDir)