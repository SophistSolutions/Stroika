### MAKEFILE TODO
###		@todo 	based on WTF makefile - fix so works here
###		@todo	dont rebuild if not needed (so add dependencies)
###		@todo	use files to track various things so we can skip steps making this build faster - like if we did check
###				for preqequisites, or more importantly already built html


StroikaRoot:=$(abspath ../../../)/

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

TARGETDIR		:=	$(StroikaRoot)Builds/$(CONFIGURATION)/Samples-HTMLUI/

PER_CONFIGURATION_BUILDS_DIR:=$(shell realpath --canonicalize-missing $(StroikaRoot)Builds/$(CONFIGURATION)/Samples-HTMLUI/html)/
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_:=$(shell realpath --canonicalize-missing $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Samples-HTMLUI/html)/

all:
	@$(StroikaRoot)/ScriptsLib/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) "Samples-HTMLUI/HTML Build:"
ifeq ($(CONFIGURATION),)
	@echo "Cannot complete build without a configuraiton" && exit 1;
endif
	@$(MAKE) --silent MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)
	@$(MAKE) --silent MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)/TOOLS_CHECKED
	@$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) update-components_
	@${MAKE} --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) build-dist_

clean clobber:
	@$(StroikaRoot)/ScriptsLib/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) "Samples-HTMLUI/HTML $(call FUNCTION_CAPITALIZE_WORD,$@) {$(CONFIGURATION)}:"
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
		rm -rf $(PER_CONFIGURATION_BUILDS_DIR);\
	fi
endif

check-prerequisites:
	@$(StroikaRoot)/ScriptsLib/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) "Checking HTML prerequisites:"
	@$(StroikaRoot)/ScriptsLib/PrintLevelLeader $$(($(MAKE_INDENT_LEVEL)+1)) && sh -c "(type quasar 2> /dev/null) || ($(StroikaRoot)/ScriptsLib/GetMessageForMissingTool quasar && exit 1)"
	@$(StroikaRoot)/ScriptsLib/PrintLevelLeader $$(($(MAKE_INDENT_LEVEL)+1)) && sh -c "(type node 2> /dev/null) || ($(StroikaRoot)/ScriptsLib/GetMessageForMissingTool node && exit 1)"
	@$(StroikaRoot)ScriptsLib/PrintLevelLeader $$(($(MAKE_INDENT_LEVEL)+1)) && sh -c "(type npm 2> /dev/null) || ($(StroikaRoot)ScriptsLib/GetMessageForMissingTool npm && exit 1)"

update-components_: $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)
	@$(StroikaRoot)/ScriptsLib/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "npm install ( log2 ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}/NPM-Install.txt ) ..."`
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_) && npm i > ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}/NPM-Install.txt 2>&1

build-dist_: $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)
	@$(StroikaRoot)ScriptsLib/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "Building ( log2 ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}/Build.txt ) ..."`
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_) && npm run build > ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}/Build.txt 2>&1
	@$(StroikaRoot)ScriptsLib/PrintLevelLeader $(MAKE_INDENT_LEVEL) && $(ECHO) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "Copying to ${PER_CONFIGURATION_BUILDS_DIR} ..."`
	@rm -rf ${PER_CONFIGURATION_BUILDS_DIR} && mkdir -p ${PER_CONFIGURATION_BUILDS_DIR} && cp -r $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)/dist/spa/* ${PER_CONFIGURATION_BUILDS_DIR}


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
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "Preparing symbolic links for $@"` "... "
	@rm -rf $@ && mkdir -p $@
	@for i in ${SRC_FILES2Link_}; do cp -r `pwd`/$$i $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)/$$i; done

