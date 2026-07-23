### MAKEFILE TODO
###		@todo 	based on WTF makefile - fix so works here
###		@todo	dont rebuild if not needed (so add dependencies)
###		@todo	use files to track various things so we can skip steps making this build faster - like if we did check
###				for preqequisites, or more importantly already built html

export TOP_ROOT:=$(abspath ../../../)/
StroikaRoot	:=	$(TOP_ROOT)

include $(StroikaRoot)Build/ScriptsLib/SharedMakeVariables-Default.mk

TARGETDIR		:=	${StroikaPlatformTargetBuildDir}Samples-HTMLUI/

PER_CONFIGURATION_BUILDS_DIR_:=${TARGETDIR}html/
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_:=$(shell realpath --canonicalize-missing $(TOP_ROOT)IntermediateFiles/$(CONFIGURATION)/Samples-HTMLUI/html)/

NEED_GNU_MAKE_ := 4.0.0
OK_GNU_MAKE_VERSION_ := $(filter $(NEED_GNU_MAKE_),$(firstword $(sort $(MAKE_VERSION) $(NEED_GNU_MAKE_))))

all:
	@$(StroikaRoot)/Build/ScriptsLib/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) "Building Samples-HTMLUI/HTML:"
ifeq ($(CONFIGURATION),)
	@echo "Cannot complete build without a configuration" && exit 1;
endif
ifeq ($(OK_GNU_MAKE_VERSION_),)
	@$(MAKE) --silent MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $(shell realpath --canonicalize-missing $(TOP_ROOT)IntermediateFiles/$(CONFIGURATION)/Samples-HTMLUI/html)
else
	@$(MAKE) --silent MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)
endif
	@$(MAKE) --silent MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)/TOOLS_CHECKED
	@${MAKE} --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ${PER_CONFIGURATION_BUILDS_DIR_}

clean clobber:
ifeq ($(CONFIGURATION),)
	@rm -rf ../IntermediateFiles/*/Samples-HTMLUI/html
	@if [ "$@"=="clobber" ] ; then \
		rm -rf ../Builds/*/Samples-HTMLUI/html;\
	fi
endif
ifneq ($(CONFIGURATION),)
	@#rm -f src/*.js
	@rm -rf $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)
	@if [ "$@"=="clobber" ] ; then \
		rm -rf $(PER_CONFIGURATION_BUILDS_DIR_);\
	fi
endif

check-prerequisites:
	@$(StroikaRoot)/Build/ScriptsLib/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) "Checking HTML prerequisites:"
	@$(StroikaRoot)/Build/ScriptsLib/PrintLevelLeader $$(($(MAKE_INDENT_LEVEL)+1)) && sh -c "(type quasar 2> /dev/null) || ($(StroikaRoot)/Build/ScriptsLib/GetMessageForMissingTool quasar && exit 1)"
	@$(StroikaRoot)/Build/ScriptsLib/PrintLevelLeader $$(($(MAKE_INDENT_LEVEL)+1)) && sh -c "(type node 2> /dev/null) || ($(StroikaRoot)/Build/ScriptsLib/GetMessageForMissingTool node && exit 1)"
	@$(StroikaRoot)Build/ScriptsLib/PrintLevelLeader $$(($(MAKE_INDENT_LEVEL)+1)) && sh -c "(type npm 2> /dev/null) || ($(StroikaRoot)Build/ScriptsLib/GetMessageForMissingTool npm && exit 1)"

${PER_CONFIGURATION_BUILDS_DIR_}: $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)
	@$(StroikaRoot)/Build/ScriptsLib/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) "npm install ( log2" `$(StroikaRoot)Build/ScriptsLib/SubstituteBackVariables ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}`NPM-Install.txt ") ..."
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_) && npm i > ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}/NPM-Install.txt 2>&1
	@# note - this build ignores .env file; used only for test/dev builds (maybe overwrite .env file with one form CONFIGURATION?)
	@rm -f $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_).env
	@$(StroikaRoot)Build/ScriptsLib/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) "Building ( log2" `$(StroikaRoot)Build/ScriptsLib/SubstituteBackVariables ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}`Build.txt ") ..."
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_) && npm run build > ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}/Build.txt 2>&1
	@$(StroikaRoot)Build/ScriptsLib/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) Copying to `$(StroikaRoot)Build/ScriptsLib/SubstituteBackVariables ${PER_CONFIGURATION_BUILDS_DIR_}` "..."
	@rm -rf ${PER_CONFIGURATION_BUILDS_DIR_} && mkdir -p ${PER_CONFIGURATION_BUILDS_DIR_} && cp -r $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)/dist/spa/* ${PER_CONFIGURATION_BUILDS_DIR_}


SRC_FILES2Link_:=
SRC_FILES2Link_ += $(wildcard .env*)
SRC_FILES2Link_ += public
SRC_FILES2Link_ += src
SRC_FILES2Link_ += Makefile
SRC_FILES2Link_ += $(wildcard *.js *.ts)
SRC_FILES2Link_ += $(wildcard *.json)
SRC_FILES2Link_ += $(wildcard *.html)
SRC_FILES2Link_ += .eslintignore .eslintrc.js .prettierrc


$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)/TOOLS_CHECKED:
	@$(MAKE) check-prerequisites --no-print-directory
	@touch $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)/TOOLS_CHECKED


$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_):
	@$(StroikaRoot)Build/ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Preparing symbolic links for " `$(StroikaRoot)Build/ScriptsLib/SubstituteBackVariables "$@"` "... "
	@rm -rf $@ && mkdir -p $@
	@for i in ${SRC_FILES2Link_}; do cp -r `pwd`/$$i $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)/$$i; done

