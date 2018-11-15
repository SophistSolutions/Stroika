.NOTPARALLEL:
.PHONY:	tests documentation all check clobber libraries assure-default-configurations apply-configuration-if-needed_ check-prerequisite-tools apply-configurations


SHELL=/bin/bash


MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO?=	$(shell ScriptsLib/GetDefaultShellVariable.sh ECHO)
ECHO_BUILD_LINES?=0


.DEFAULT_GOAL := help

#Handy shortcut
CONFIGURATION_TAGS?=$(TAGS)


APPLY_CONFIGS=$(or $(CONFIGURATION), $(shell ScriptsLib/GetConfigurations --config-tags "$(CONFIGURATION_TAGS)"))


help:
	@$(ECHO) "Help for making Stroika:"
	@$(ECHO) "Targets:"
	@$(ECHO) "    all:                         -    Builds everything"
	@$(ECHO) "    check:                       -    Checks everything was built properly"
	@$(ECHO) "    clean:"
	@$(ECHO) "    clobber:"
	@$(ECHO) "    libraries:                   -    Builds Stroika foundation & frameworks, and any things it depends on (like third-party-components)"
	@$(ECHO) "    project-files:               -    Alias for project-files-visual-studio project-files-qt-creator"
	@$(ECHO) "    project-files-visual-studio: -    Builds project files for visual studio.net"
	@$(ECHO) "    project-files-qt-creator(*): -    Builds project project-files-qt-creator (also project-files-qt-creator-load"
	@$(ECHO) "                                      project-files-qt-creator-save)"
	@$(ECHO) "    tests:"
	@$(ECHO) "    format-code:                 -    Run astyle on source code, and update it to conform to Stroika code formatting standards"
	@$(ECHO) "    samples:"
	@$(ECHO) "    documentation:"
	@$(ECHO) "    third-party-components:"
	@$(ECHO) "    run-tests:                   -    [REMOTE=] - eg. REMOTE=lewis@localhost;"
	@$(ECHO) "                                      [VALGRIND=memcheck or helgrind, etc] to run with valgrind"
	@$(ECHO) "                                      (EXTRA_VALGRIND_OPTIONS= can be used with valgrind)"
	@$(ECHO) "                                      OR VALGRIND_SUPPRESSIONS=\"Valgrind-MemCheck-Common.supp Valgrind-MemCheck-BlockAllocation.supp\""
	@$(ECHO) "                                      OR VALGRIND_SUPPRESSIONS=\"Valgrind-Helgrind-Common.supp\""
	@$(ECHO) "                                      EG: VALGRIND_SUPPRESSIONS=\"Valgrind-Helgrind-Common.supp\" make VALGRIND=helgrind run-tests"
	@$(ECHO) "    apply-configurations:        -    Force re-creation implied files / links for any configurations in the Configurations"
	@$(ECHO) "                                      folder (not needed, automatic)"
	@$(ECHO) "    default-configurations:      -    Creates the default configurations in Configurations folder; [DEFAULT_CONFIGURATION_ARGS]')"
	@$(ECHO) "                                      e.g. DEFAULT_CONFIGURATION_ARGS=--help OR"
	@$(ECHO) "                                      DEFAULT_CONFIGURATION_ARGS='--openssl-extraargs purify --block-allocation disable'"
	@$(ECHO) "    list-configurations:         -    prints all available configurations (each can be used as arg to CONFIGURAITON= make lines)"
	@$(ECHO) "    list-configuration-tags:     -    prints a list of all configurtion tags (configuration tags impute groups of configurations)"
	@$(ECHO) "    check-prerequisite-tools:    -    Check the tools needed to build Stroika are installed."
	@$(ECHO) "Special Variables:               -    Extra params you can pass to the make line that may help..."
	@$(ECHO) "    CONFIGURATION=XYZ            -    Causes only the configuration XYZ to have the targetted goal applied (e.g. make all CONFIGURATION=Debug)"
	@$(ECHO) "    CONFIGURATION_TAGS=TAGS      -    Causes only the configurations with tags TAGS to have the targetted goal applied (e.g. make all CONFIGURATION_TAGS=Windows)"
	@$(ECHO) "                                      NB: TAGS is a handy shortcut for CONFIGURAITON_TAGS, so you can say make all TAGS=\"Windows 64\""
	@$(ECHO) "    ECHO_BUILD_LINES=1           -    Causes make lines to be echoed which can help makefile debugging"
	@$(ECHO) "    MAKE_INDENT_LEVEL=0          -    Helpful to neaten formatting when multiple levels of makes calling Stroika make"


ifeq ($(CONFIGURATION),)
all:		IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_ALL assure-default-configurations-exist_
	@#first run all checks so any errors for missing tools appear ASAP
	@ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL)  "Checking Prerequisites for Stroika:"
	@$(MAKE) --no-print-directory --silent apply-configurations-if-needed MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES)
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory --silent IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_ALL IntermediateFiles/$$i/TOOLS_CHECKED CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES) || exit $$?;\
	done
	@for i in $(APPLY_CONFIGS) ; do\
		ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Building Stroika all {$$i}:";\
		$(MAKE) --no-print-directory all CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES) || exit $$?;\
	done
else
all:		IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_ALL assure-default-configurations-exist_ libraries tools tests samples documentation check
endif


check: assure-default-configurations-exist_
ifeq ($(CONFIGURATION),)
ifeq ($(MAKECMDGOALS),check)
	@for i in $(APPLY_CONFIGS) ; do\
		ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Stroika/Check {$$i}:";\
		$(MAKE) --no-print-directory check CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
	done
endif
else
	@ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Checking Stroika {$(CONFIGURATION)}:"
	@$(MAKE) --directory ThirdPartyComponents --no-print-directory check CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Library --no-print-directory check CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Tools --no-print-directory check CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Tests --no-print-directory check CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Samples --no-print-directory check CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif


clean:
ifeq ($(CONFIGURATION),)
	@ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Stroika Clean:"
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory clean CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
	done
else
	@ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Stroika Clean {$(CONFIGURATION)}:"
	@$(MAKE) --directory ThirdPartyComponents --no-print-directory clean CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Library --no-print-directory clean CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Tools --no-print-directory clean CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Tests --no-print-directory clean CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Samples --no-print-directory clean CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif


clobber:
ifeq ($(CONFIGURATION),)
	@ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Stroika Clobber:"
ifeq ($(CONFIGURATION_TAGS),)
	@rm -rf IntermediateFiles/*
	@rm -rf Builds/*
	@$(MAKE) --directory ThirdPartyComponents --no-print-directory clobber CONFIGURATION= MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
else
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory clobber CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
	done
endif
else
	@ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Stroika Clobber {$(CONFIGURATION)}:"
	@$(MAKE) --directory ThirdPartyComponents --no-print-directory clobber CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Library --no-print-directory clobber CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Tools --no-print-directory clobber CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Tests --no-print-directory clobber CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Samples --no-print-directory clobber CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif


documentation:
	@$(MAKE) --directory Documentation --no-print-directory all MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)


ifeq ($(CONFIGURATION),)
libraries:	IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_ALL assure-default-configurations-exist_
	@for i in $(APPLY_CONFIGS) ; do\
		ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Making Stroika/Libraries {$$i}:";\
		$(MAKE) --no-print-directory libraries CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES) || exit $$?;\
	done
else
libraries:	IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_ALL assure-default-configurations-exist_ IntermediateFiles/$(CONFIGURATION)/TOOLS_CHECKED third-party-components
	@ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@$(MAKE) --directory Library --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES)
endif



ifeq ($(CONFIGURATION),)
third-party-components:	IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_ALL assure-default-configurations-exist_
	@for i in $(APPLY_CONFIGS) ; do\
		ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Making Stroika/Third-party-components {$$i}:";\
		$(MAKE) --no-print-directory third-party-components CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES) || exit $$?;\
	done
else
third-party-components:	IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_ALL assure-default-configurations-exist_ apply-configuration-if-needed_ IntermediateFiles/$(CONFIGURATION)/TOOLS_CHECKED
	@ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@$(MAKE) --directory ThirdPartyComponents --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES)
endif


project-files:	project-files-visual-studio project-files-qt-creator


project-files-visual-studio:
	@$(MAKE) --directory Tests --no-print-directory project-files


project-files-qt-creator:	project-files-qt-creator-load


project-files-qt-creator-load:
	@$(ECHO) -n "Loading qt-creator project files ... "
	@for i in StroikaDevRoot.config StroikaDevRoot.creator StroikaDevRoot.files StroikaDevRoot.includes; do cp Library/Projects/QtCreator/$$i .; done;
	@$(ECHO) "done"


project-files-qt-creator-save:
	@$(ECHO) -n "Saving qt-creator project files ... "
	@for i in StroikaDevRoot.config StroikaDevRoot.creator StroikaDevRoot.files StroikaDevRoot.includes; do cp $$i Library/Projects/QtCreator/ ; done;
	@$(ECHO) "done"


ifeq ($(CONFIGURATION),)
tools:	assure-default-configurations-exist_
	@for i in $(APPLY_CONFIGS) ; do\
		ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Making Stroika/Tools {$$i}:";\
		$(MAKE) tools --no-print-directory CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES) || exit $$?;\
	done
else
tools:	assure-default-configurations-exist_ libraries
	@ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@$(MAKE) --directory Tools --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES)
endif


ifeq ($(CONFIGURATION),)
tests:	assure-default-configurations-exist_
	@for i in $(APPLY_CONFIGS) ; do\
		ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Making Stroika/Tests {$$i}:";\
		$(MAKE) tests --no-print-directory CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES) || exit $$?;\
	done
else
tests:	assure-default-configurations-exist_ libraries
	@ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@$(MAKE) --directory Tests --no-print-directory tests CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES)
endif


ifeq ($(CONFIGURATION),)
samples:	assure-default-configurations-exist_
	@for i in $(APPLY_CONFIGS) ; do\
		ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Making Stroika/Samples {$$i}:";\
		$(MAKE) samples --no-print-directory CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES) || exit $$?;\
	done
else
samples:	assure-default-configurations-exist_ libraries
	@ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@$(MAKE) --directory Samples --no-print-directory samples CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES)
endif


ifeq ($(CONFIGURATION),)
run-tests:
	@for i in $(APPLY_CONFIGS) ; do\
		ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Stroika/Run-All-Tests {$$i}:";\
		$(MAKE) --no-print-directory run-tests CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES) || exit $$?;\
	done
else
run-tests:	tests
	@ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@$(MAKE) --directory Tests --no-print-directory run-tests CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES)
endif



format-code:
	@$(ECHO) Formatting Code:
	@ScriptsLib/FormatCode.sh Library *.h *.inl *.cpp
	@ScriptsLib/FormatCode.sh Samples *.h *.inl *.cpp
	@ScriptsLib/FormatCode.sh Tests *.h *.inl *.cpp
	@ScriptsLib/FormatCode.sh Tools *.h *.inl *.cpp
	@ScriptsLib/FormatCode.sh BuildToolsSrc *.h *.inl *.cpp




#
#check-prerequisite-tools
#	is broken into check-prerequisite-tools-common - which checks all stroika prerequisites, and
#	check-prerequisite-tools-current-configuration which checks a given argument CONFIGURATION.
#
#	We use take target files IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_ALL and IntermediateFiles/$(CONFIGURATION)/TOOLS_CHECKED
#	to make sure each configuraiton is checked at least once before build, so we get easier to understand error messages
#	(tool x missing instead of xxx failed)
#
check-prerequisite-tools:Rw
	@$(MAKE) --no-print-directory check-prerequisite-tools-common CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES)
ifeq ($(CONFIGURATION),)
	@#no need to run apply-configurations-if-needed, but  looks neater in output if we run first, and then do checks
	@$(MAKE) --no-print-directory apply-configurations-if-needed MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES)
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory check-prerequisite-tools-current-configuration CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES) || exit $$?;\
	done
else
	@$(MAKE) --no-print-directory check-prerequisite-tools-current-configuration CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES)
endif


check-prerequisite-tools-common:
	@ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Checking for installed tools:"
	@ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n && sh -c "(type sed 2> /dev/null) || (ScriptsLib/GetMessageForMissingTool.sh sed && exit 1)"
	@ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n && sh -c "(type wget 2> /dev/null) || (ScriptsLib/GetMessageForMissingTool.sh wget && exit 1)"
	@ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n && sh -c "(type perl 2> /dev/null) || (ScriptsLib/GetMessageForMissingTool.sh perl && exit 1)"
	@ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n && sh -c "(type tar 2> /dev/null) || (ScriptsLib/GetMessageForMissingTool.sh tar && exit 1)"
	@ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n && sh -c "(type patch 2> /dev/null) || (ScriptsLib/GetMessageForMissingTool.sh patch && exit 1)"
	@ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n && sh -c "(type tr 2> /dev/null) || (ScriptsLib/GetMessageForMissingTool.sh tr && exit 1)"
	@ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n && sh -c "(type pkg-config 2> /dev/null) || (ScriptsLib/GetMessageForMissingTool.sh pkg-config && exit 1)"
	@ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n && sh -c "(type realpath 2> /dev/null) || (ScriptsLib/GetMessageForMissingTool.sh realpath && exit 1)"
ifneq (,$(findstring CYGWIN,$(shell uname)))
	@ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n && sh -c "(type dos2unix 2> /dev/null) || (ScriptsLib/GetMessageForMissingTool.sh dos2unix && exit 1)"
	@ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n && sh -c "(type unix2dos 2> /dev/null) || (ScriptsLib/GetMessageForMissingTool.sh unix2dos && exit 1)"
endif
	@mkdir -p IntermediateFiles
	@touch IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_ALL


check-prerequisite-tools-current-configuration:
ifeq ($(CONFIGURATION),)
	$(error Cannot call check-prerequisite-tools-current-configuration without a configuration argument)
endif
	@$(MAKE) --no-print-directory apply-configuration-if-needed_ CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES)
	@$(MAKE) --directory ThirdPartyComponents --no-print-directory check-prerequisite-tools CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES)
	@mkdir -p IntermediateFiles/$(CONFIGURATION)
	@touch IntermediateFiles/$(CONFIGURATION)/TOOLS_CHECKED


IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_ALL:
	@$(MAKE) check-prerequisite-tools-common --no-print-directory MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)

IntermediateFiles/$(CONFIGURATION)/TOOLS_CHECKED:
	@$(MAKE) check-prerequisite-tools-current-configuration --no-print-directory MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) CONFIGURATION=$(CONFIGURATION)


assure-default-configurations-exist_:
ifeq ($(shell ScriptsLib/GetConfigurations),)
	@$(MAKE) default-configurations --no-print-directory MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
endif


apply-configuration-if-needed_:	assure-default-configurations-exist_
ifneq ($(CONFIGURATION),)
	@ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@$(MAKE) --no-print-directory --silent IntermediateFiles/$(CONFIGURATION)/APPLIED_CONFIGURATION CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);
endif



apply-configurations:
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory apply-configuration CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done

apply-configurations-if-needed:
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory --silent IntermediateFiles/$$i/APPLIED_CONFIGURATION CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done


IntermediateFiles/$(CONFIGURATION)/APPLIED_CONFIGURATION:	ConfigurationFiles/$(CONFIGURATION).xml
	@$(MAKE) --no-print-directory apply-configuration CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
	

apply-configuration:
ifeq ($(CONFIGURATION),)
	$(error Cannot call apply-configuration without a configuration argument)
endif
	@ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Applying configuration {$(CONFIGURATION)}:"
	@mkdir -p "IntermediateFiles/$(CONFIGURATION)/"
	@perl ScriptsLib/ApplyConfiguration.pl $(CONFIGURATION)
	@ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n "Writing \"IntermediateFiles/$(CONFIGURATION)/Stroika-Current-Version.h\" ... "
	@ScriptsLib/MakeVersionFile.sh STROIKA_VERSION IntermediateFiles/$(CONFIGURATION)/Stroika-Current-Version.h StroikaLibVersion
	@touch IntermediateFiles/$(CONFIGURATION)/APPLIED_CONFIGURATION
	@echo "done"


UNAME_DASH_O_=$(shell uname -o 2>/dev/null || true)

default-configurations:
	@ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Making default configurations:"
	@export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
	if [ "$(UNAME_DASH_O_)" = "Cygwin" ] ; then\
		./configure Debug-U-32 --config-tag Windows --config-tag 32 --apply-default-debug-flags --trace2file disable $(DEFAULT_CONFIGURATION_ARGS);\
		./configure Debug-U-64 --config-tag Windows --config-tag 64 --apply-default-debug-flags --trace2file disable $(DEFAULT_CONFIGURATION_ARGS);\
		./configure Release-DbgMemLeaks-U-32 --config-tag Windows --config-tag 32 --apply-default-release-flags $(DEFAULT_CONFIGURATION_ARGS);\
		./configure Release-Logging-U-32 --config-tag Windows --config-tag 32 --apply-default-release-flags --trace2file enable $(DEFAULT_CONFIGURATION_ARGS);\
		./configure Release-Logging-U-64 --config-tag Windows --config-tag 64 --apply-default-release-flags --trace2file enable $(DEFAULT_CONFIGURATION_ARGS);\
		./configure Release-U-32 --config-tag Windows --config-tag 32 --apply-default-release-flags $(DEFAULT_CONFIGURATION_ARGS);\
		./configure Release-U-64 --config-tag Windows --config-tag 64 --apply-default-release-flags $(DEFAULT_CONFIGURATION_ARGS);\
	else\
		./configure Debug --config-tag Unix --apply-default-debug-flags $(DEFAULT_CONFIGURATION_ARGS);\
		./configure Release --config-tag Unix --apply-default-release-flags $(DEFAULT_CONFIGURATION_ARGS);\
	fi


basic-unix-test-configurations:
	@ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Making basic-unix-test-configurations:"
	@export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
	./configure DEFAULT_CONFIG --config-tag Unix;\
	./configure no-third-party-components --config-tag Unix --LibCurl no --lzma no --zlib no --OpenSSL no --sqlite no --Xerces no --boost no;\
	./configure only-zlib-system-third-party-component --config-tag Unix --LibCurl no --lzma no --zlib system --OpenSSL no --sqlite no --Xerces no --boost no;\
	##DISABLE TESTING BECAUSE of https://stroika.atlassian.net/browse/STK-621\
	##./configure malloc-guard --config-tag Unix --malloc-guard true;\
	#\
	./configure g++-7-debug-c++17 --config-tag Unix --compiler-driver g++-7 --apply-default-debug-flags --only-if-has-compiler --trace2file enable --cppstd-version c++17;\
	./configure g++-7-release-c++17 --config-tag Unix --compiler-driver g++-7 --apply-default-release-flags --only-if-has-compiler --cppstd-version c++17;\
	./configure g++-8-debug-c++17 --config-tag Unix --compiler-driver g++-8 --apply-default-debug-flags --only-if-has-compiler --trace2file enable --cppstd-version c++17;\
	./configure g++-8-release-c++17 --config-tag Unix --compiler-driver g++-8 --apply-default-release-flags --only-if-has-compiler --cppstd-version c++17;\
	./configure g++-8-debug-c++2a --config-tag Unix --compiler-driver g++-8 --apply-default-debug-flags --only-if-has-compiler --trace2file enable --cppstd-version c++2a;\
	#./configure my-g++-8.2-debug-c++2a --config-tag Unix --compiler-driver /private-compiler-builds/gcc-8.2.0/bin/x86_64-pc-linux-gnu-gcc --apply-default-debug-flags --no-sanitize address --append-run-prefix 'LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:/private-compiler-builds/gcc-8.2.0/lib64' --only-if-has-compiler --cppstd-version c++2a;\
	#\
	#no-sanitize undefined - due to issue with ObjectVariantMapper I cannot find better way to resolve - https://stroika.atlassian.net/browse/STK-601 - I THINK \
	./configure clang++-6-debug-libc++ --config-tag Unix --compiler-driver clang++-6.0 --apply-default-debug-flags --stdlib libc++ --only-if-has-compiler --trace2file enable --no-sanitize undefined;\
	./configure clang++-6-release-libstdc++ --config-tag Unix --compiler-driver clang++-6.0 --apply-default-release-flags --stdlib libstdc++  --only-if-has-compiler --trace2file enable;\
	./configure clang++-7-debug-libc++ --config-tag Unix --compiler-driver clang++-7 --apply-default-debug-flags --stdlib libc++ --only-if-has-compiler --trace2file enable --no-sanitize undefined;\
	./configure clang++-7-debug-libc++-c++2a --config-tag Unix --compiler-driver clang++-7 --apply-default-debug-flags --stdlib libc++ --only-if-has-compiler --trace2file enable --no-sanitize undefined --cppstd-version c++2a;\
	./configure clang++-7-release-libstdc++ --config-tag Unix --compiler-driver clang++-7 --apply-default-release-flags --stdlib libstdc++  --only-if-has-compiler --trace2file enable;\
	# --no-sanitize and --lto disable calls for clang7 just because of how I built it...\
	./configure my-clang++-7-debug-libc++ --config-tag Unix --compiler-driver /private-compiler-builds/clang-7.0.0/bin/clang++ --apply-default-debug-flags --stdlib libc++ --only-if-has-compiler --trace2file enable --no-sanitize address --no-sanitize undefined;\
	./configure my-clang++-7-debug-libc++-c++2a --config-tag Unix --compiler-driver /private-compiler-builds/clang-7.0.0/bin/clang++ --apply-default-debug-flags --stdlib libc++ --only-if-has-compiler --trace2file enable --no-sanitize address --no-sanitize undefined --cppstd-version c++2a;\
	./configure my-clang++-7-release-libstdc++ --config-tag Unix --compiler-driver /private-compiler-builds/clang-7.0.0/bin/clang++ --apply-default-release-flags --stdlib libstdc++  --only-if-has-compiler --trace2file enable --lto disable;\
	#\
	#32-bit not working now - asm bug - must debug...\
	#./configure gcc-release-32 --compiler-driver "gcc -m32" --trace2file enable --assertions enable --only-if-has-compiler --LibCurl no --OpenSSL no --Xerces no --zlib no --lzma no --extra-compiler-args -m32 --extra-linker-args -m32 --static-link-gccruntime disable;\
	#\
	# (@todo - see if --lto disable still needed on release asan/tsan (was needed for gcc 7.2)\
	./configure g++-debug-sanitize_leak --config-tag Unix --apply-default-debug-flags --sanitize none,leak --trace2file enable;\
	./configure g++-debug-sanitize_address --config-tag Unix --apply-default-debug-flags --sanitize none,address,undefined --trace2file enable;\
	./configure g++-debug-sanitize_thread --config-tag Unix --apply-default-debug-flags --trace2file enable --cppstd-version c++17 --sanitize none,thread,undefined;\
	./configure g++-debug-sanitize_undefined --config-tag Unix --apply-default-debug-flags --sanitize none,address,undefined --trace2file enable;\
	./configure g++-release-sanitize_address_undefined --config-tag Unix --apply-default-release-flags --trace2file enable --lto disable --cppstd-version c++17 --sanitize none,address,undefined;\
	./configure g++-release-sanitize_thread_undefined --config-tag Unix --apply-default-release-flags --trace2file enable --lto disable --cppstd-version c++17 --sanitize none,thread,undefined;\
	./configure g++-optimized --config-tag Unix --apply-default-release-flags;\
	#\
	###Builds with a few specail flags to make valgrind work better\
	#nb: using default installed C++ compiler cuz of mathcing installed liraries on host computer\
	./configure g++-valgrind-debug-SSLPurify --config-tag Unix --config-tag valgrind -valgrind enable --openssl use --openssl-extraargs purify --apply-default-debug-flags --trace2file enable --sanitize none;\
	./configure g++-valgrind-debug-SSLPurify-NoBlockAlloc --config-tag Unix --config-tag valgrind -valgrind enable --openssl use --openssl-extraargs purify  --apply-default-debug-flags --trace2file enable --block-allocation disable --sanitize none;\
	./configure g++-valgrind-release-SSLPurify-NoBlockAlloc --config-tag Unix --config-tag valgrind --valgrind enable --openssl use --openssl-extraargs purify  --apply-default-release-flags --lto disable --trace2file disable --block-allocation disable;
	

raspberrypi-cross-compile-test-configurations:
	@ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Making raspberrypi-cross-compile-test-configurations:"
	@export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
	#\
	# --append-compiler-warning-args -Wno-psabi JUST FOR ARM GCC6 and GCC7 - https://stroika.atlassian.net/browse/STK-627\
	#./configure raspberrypi-g++-7 --config-tag Unix --config-tag raspberrypi --apply-default-debug-flags --only-if-has-compiler --trace2file enable --compiler-driver 'arm-linux-gnueabihf-g++-7' --cross-compiling true --sanitize none --append-compiler-warning-args -Wno-psabi;\
	#qCompiler_Sanitizer_stack_use_after_scope_on_arm_Buggy - SEE https://stroika.atlassian.net/browse/STK-500 - RETEST WHEN WE HAVE GCC8\
	#./configure raspberrypi-g++-7-sanitize --apply-default-debug-flags --only-if-has-compiler --trace2file enable --sanitize address,undefined --compiler-driver 'arm-linux-gnueabihf-g++-7' --cross-compiling true --append-run-prefix 'LD_PRELOAD=/usr/lib/arm-linux-gnueabihf/libasan.so.4' --append-compiler-warning-args -Wno-psabi;\
	./configure raspberrypi-g++-7-sanitize_undefined --config-tag Unix --config-tag raspberrypi --apply-default-debug-flags --only-if-has-compiler --trace2file enable --sanitize none,undefined --compiler-driver 'arm-linux-gnueabihf-g++-7' --cross-compiling true --append-compiler-warning-args -Wno-psabi;\
	#./configure raspberrypi-valgrind-g++-7-SSLPurify-NoBlockAlloc --config-tag Unix --config-tag raspberrypi --config-tag valgrind --apply-default-release-flags --only-if-has-compiler --trace2file disable --compiler-driver 'arm-linux-gnueabihf-g++-7' --valgrind enable --block-allocation disable --openssl use --openssl-extraargs purify --cross-compiling true --append-compiler-warning-args -Wno-psabi;\
	#\
	# --append-compiler-warning-args -Wno-psabi JUST FOR ARM GCC6 and GCC7 - https://stroika.atlassian.net/browse/STK-627\
	#qCompiler_Sanitizer_stack_use_after_scope_on_arm_Buggy - SEE https://stroika.atlassian.net/browse/STK-500 - RETEST WHEN WE HAVE GCC8\
	#./configure raspberrypi-g++-8 --apply-default-debug-flags --only-if-has-compiler --trace2file enable --compiler-driver 'arm-linux-gnueabihf-g++-8' --cross-compiling true --append-run-prefix 'LD_PRELOAD=/usr/lib/arm-linux-gnueabihf/libasan.so.5' --append-compiler-warning-args -Wno-psabi;\
	./configure raspberrypi-g++-8-release-sanitize_address --config-tag Unix --config-tag raspberrypi --apply-default-release-flags --only-if-has-compiler --trace2file enable --compiler-driver 'arm-linux-gnueabihf-g++-8' --sanitize none,address --cross-compiling true --append-run-prefix 'LD_PRELOAD=/usr/lib/arm-linux-gnueabihf/libasan.so.5' --append-compiler-warning-args -Wno-psabi;\
	./configure raspberrypi-g++-8-debug-sanitize_undefined --config-tag Unix --config-tag raspberrypi --apply-default-debug-flags --only-if-has-compiler --trace2file enable --sanitize none,undefined --compiler-driver 'arm-linux-gnueabihf-g++-8' --cross-compiling true --append-compiler-warning-args -Wno-psabi;\
	##Couldn't get tsan to link (/usr/bin/arm-linux-gnueabihf-ld: cannot find -ltsan) - so retry on next ubuntu release\
	./configure raspberrypi-valgrind-g++-8-SSLPurify-NoBlockAlloc --config-tag Unix --config-tag raspberrypi --config-tag valgrind --apply-default-release-flags --only-if-has-compiler --trace2file disable --compiler-driver 'arm-linux-gnueabihf-g++-8' --valgrind enable --block-allocation disable --openssl use --openssl-extraargs purify --cross-compiling true --append-compiler-warning-args -Wno-psabi;


regression-test-configurations:
	@ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Making regression-test configurations:"
	@rm -f ConfigurationFiles/*
	@export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
	if [ "$(UNAME_DASH_O_)" = "Cygwin" ] ; then\
		$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) default-configurations;\
	else\
		$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) basic-unix-test-configurations;\
		$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) raspberrypi-cross-compile-test-configurations;\
	fi


list-configurations:
	@echo $(APPLY_CONFIGS)

list-configuration-tags:
	@ScriptsLib/GetConfigurationTags

install-realpath:
	g++ --std=c++17 -O -o /usr/local/bin/realpath BuildToolsSrc/realpath.cpp
