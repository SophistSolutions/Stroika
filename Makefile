StroikaRoot=$(abspath .)/

include ScriptsLib/Makefile-Common.mk

ifeq ($(filter 3.81, $(firstword $(sort $(MAKE_VERSION) 3.81))),)
$(info Warning - version of GNU Make - $(MAKE_VERSION) - appears too old)
endif


#not parallel because submakefiles use parallelism, but generally best to sequence these top level requests. Like if you say
# make clobber all you don't want those to happen at the same time. And make libraries samples wouldn't really work since all the libraries
# have to be built before the samples etc...
.NOTPARALLEL:

.PHONY:	tests documentation all check clobber libraries assure-default-configurations apply-configuration-if-needed_ check-prerequisite-tools apply-configurations apply-configuration


SHELL=/bin/bash


ECHO_BUILD_LINES?=0

# Only applies to run-tests
export TEST_FAILURES_CAUSE_FAILED_MAKE?=1


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
	@$(ECHO) "    reconfigure:                 -    Rebuild configuration files from the command-lines that built them before"
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
	@$(ECHO) "    default-configurations:      -    Creates the default configurations in Configurations folder; see ./configure -help for environment variables"
	@$(ECHO) "                                      e.g. EXTRA_CONFIGURE_ARGS='--openssl-extraargs purify --block-allocation disable' make default-configurations"
	@$(ECHO) "                                      OR EXTRA_CONFIGURE_ARGS='--platform VisualStudio.Net-2019' make default-configurations"
	@$(ECHO) "    list-configurations:         -    prints all available configurations (each can be used as arg to CONFIGURAITON= make lines)"
	@$(ECHO) "    list-configuration-tags:     -    prints a list of all configurtion tags (configuration tags impute groups of configurations)"
	@$(ECHO) "    check-prerequisite-tools:    -    Check the tools needed to build Stroika are installed."
	@$(ECHO) "Special Variables:               -    Extra params you can pass to the make line that may help..."
	@$(ECHO) "    CONFIGURATION=XYZ            -    Causes only the configuration XYZ to have the targetted goal applied (e.g. make all CONFIGURATION=Debug)"
	@$(ECHO) "    CONFIGURATION_TAGS=TAGS      -    Causes only the configurations with tags TAGS to have the targetted goal applied (e.g. make all CONFIGURATION_TAGS=Windows)"
	@$(ECHO) "                                      NB: TAGS is a handy shortcut for CONFIGURAITON_TAGS, so you can say make all TAGS=\"Windows 64\""
	@$(ECHO) "    ECHO_BUILD_LINES=1           -    Causes make lines to be echoed which can help makefile debugging"
	@$(ECHO) "    MAKE_INDENT_LEVEL=0          -    Helpful to neaten formatting when multiple levels of makes calling Stroika make"
	@$(ECHO) "    QUICK_BUILD=1                -    Defaults=0, but if =1, skip some optional build steps (like openssl tests, CURRENT folders; used for some CI testing)"
	@$(ECHO) "    TEST_FAILURES_CAUSE_FAILED_MAKE=0"
	@$(ECHO) "                                      only applies to make run-tests, and prevents test failures from stopping make (like make -k on run-tests)"


ifeq ($(CONFIGURATION),)
all:		IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_ALL assure-default-configurations-exist_
	@#first run all checks so any errors for missing tools appear ASAP
	@ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL)  "Checking Prerequisites for Stroika:"
	@$(MAKE) --no-print-directory --silent apply-configurations-if-needed MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory --silent IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_ALL IntermediateFiles/$$i/TOOLS_CHECKED CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
	done
	@for i in $(APPLY_CONFIGS) ; do\
		ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Stroika all {$$i}:";\
		$(MAKE) --no-print-directory all CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
	done
else
all:		IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_ALL assure-default-configurations-exist_ libraries tools tests samples documentation check
endif


check: assure-default-configurations-exist_
ifeq ($(CONFIGURATION),)
ifeq ($(MAKECMDGOALS),check)
	@for i in $(APPLY_CONFIGS) ; do\
		ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika/Check {$$i}:";\
		$(MAKE) --no-print-directory check CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
	done
endif
else
	@ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Checking Stroika {$(CONFIGURATION)}:"
	@$(MAKE) --directory ThirdPartyComponents --no-print-directory check MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Library --no-print-directory check MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Tools --no-print-directory check MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Tests --no-print-directory check MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Samples --no-print-directory check MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif


clean clobber:
ifeq ($(CONFIGURATION),)
	@ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika $(call FUNCTION_CAPITALIZE_WORD,$@):"
ifeq ($(CONFIGURATION_TAGS),)
	@#for clobber, quickly delete all interesting stuff (if no args so unrestricted) and in third party deletes stuff like 'CURRENT' folders
	@rm -rf IntermediateFiles/*
	@if [ "$@" == "clobber" ] ; then \
		$(MAKE) --directory ThirdPartyComponents --no-print-directory clobber CONFIGURATION= MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
	fi
endif
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory $@ CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
	done
else
	@ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika $(call FUNCTION_CAPITALIZE_WORD,$@) {$(CONFIGURATION)}:"
	@rm -rf IntermediateFiles/$(CONFIGURATION)
	@if [ "$@" == "clobber" ] ; then \
		rm -rf Builds/$(CONFIGURATION);\
	fi
	@$(MAKE) --directory ThirdPartyComponents --no-print-directory $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Library --no-print-directory $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Tools --no-print-directory $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Tests --no-print-directory $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Samples --no-print-directory $@ MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif


documentation:
	@$(MAKE) --directory Documentation --no-print-directory all


ifeq ($(CONFIGURATION),)
libraries:	IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_ALL assure-default-configurations-exist_
	@for i in $(APPLY_CONFIGS) ; do\
		ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Making Stroika/Libraries {$$i}:";\
		$(MAKE) --no-print-directory libraries CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
	done
else
libraries:	IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_ALL assure-default-configurations-exist_ IntermediateFiles/$(CONFIGURATION)/TOOLS_CHECKED third-party-components
	@ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@$(MAKE) --directory Library --no-print-directory all
endif



ifeq ($(CONFIGURATION),)
third-party-components:	IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_ALL assure-default-configurations-exist_
	@for i in $(APPLY_CONFIGS) ; do\
		ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Making Stroika/Third-party-components {$$i}:";\
		$(MAKE) --no-print-directory third-party-components CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
	done
else
third-party-components:	IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_ALL assure-default-configurations-exist_ apply-configuration-if-needed_ IntermediateFiles/$(CONFIGURATION)/TOOLS_CHECKED
	@ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@$(MAKE) --directory ThirdPartyComponents --no-print-directory all
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
		ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Making Stroika/Tools {$$i}:";\
		$(MAKE) tools --no-print-directory CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
	done
else
tools:	assure-default-configurations-exist_ libraries
	@ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@$(MAKE) --directory Tools --no-print-directory all
endif


ifeq ($(CONFIGURATION),)
tests:	assure-default-configurations-exist_
	@for i in $(APPLY_CONFIGS) ; do\
		ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Making Stroika/Tests {$$i}:";\
		$(MAKE) tests --no-print-directory CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
	done
else
tests:	assure-default-configurations-exist_ libraries
	@ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@$(MAKE) --directory Tests --no-print-directory tests
endif


ifeq ($(CONFIGURATION),)
samples:	assure-default-configurations-exist_
	@for i in $(APPLY_CONFIGS) ; do\
		ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Making Stroika/Samples {$$i}:";\
		$(MAKE) samples --no-print-directory CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
	done
else
samples:	assure-default-configurations-exist_ libraries
	@ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@$(MAKE) --directory Samples --no-print-directory samples
endif


ifeq ($(CONFIGURATION),)
run-tests:
	@for i in $(APPLY_CONFIGS) ; do\
		ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika/Run-All-Tests {$$i}:";\
		$(MAKE) --no-print-directory run-tests CONFIGURATION=$$i || exit $$?;\
	done
else
run-tests:	tests
	@ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@$(MAKE) --directory Tests --no-print-directory run-tests
endif



format-code:
	@$(ECHO) Formatting Code:
	@ScriptsLib/FormatCode Library *.h *.inl *.cpp
	@ScriptsLib/FormatCode Samples *.h *.inl *.cpp
	@ScriptsLib/FormatCode Tests *.h *.inl *.cpp
	@ScriptsLib/FormatCode Tools *.h *.inl *.cpp
	@ScriptsLib/FormatCode BootstrapToolsSrc *.h *.inl *.cpp




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
	@$(MAKE) --no-print-directory check-prerequisite-tools-common CONFIGURATION=$$i
ifeq ($(CONFIGURATION),)
	@#no need to run apply-configurations-if-needed, but  looks neater in output if we run first, and then do checks
	@$(MAKE) --no-print-directory apply-configurations-if-needed MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory check-prerequisite-tools-current-configuration CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
	done
else
	@$(MAKE) --no-print-directory check-prerequisite-tools-current-configuration MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif


check-prerequisite-tools-common:
	@# used to check with "type X 2> /dev/null" and still do sometimes, but trouble is on WSL, this prints it finds file, if it finds non-functional cygwin version - so best to check if it actually runs
	@ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Checking for installed tools:"
	@ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) -n && sh -c "(type sed 2> /dev/null) || (ScriptsLib/GetMessageForMissingTool sed && exit 1)"
	@ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) -n && sh -c "(type wget 2> /dev/null) || (ScriptsLib/GetMessageForMissingTool wget && exit 1)"
	@ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) -n && sh -c "(type perl 2> /dev/null) || (ScriptsLib/GetMessageForMissingTool perl && exit 1)"
	@ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) -n && sh -c "(type tar 2> /dev/null) || (ScriptsLib/GetMessageForMissingTool tar && exit 1)"
	@ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) -n && sh -c "(type patch 2> /dev/null) || (ScriptsLib/GetMessageForMissingTool patch && exit 1)"
	@ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) -n && sh -c "(type tr 2> /dev/null) || (ScriptsLib/GetMessageForMissingTool tr && exit 1)"
	@ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) -n && sh -c "(pkg-config --version 1> /dev/null 2> /dev/null && type pkg-config 2> /dev/null) || (ScriptsLib/GetMessageForMissingTool pkg-config && exit 1)"
	@ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) -n && sh -c "(type realpath 2> /dev/null) || (ScriptsLib/GetMessageForMissingTool realpath && exit 1)"
ifneq (,$(findstring CYGWIN,$(shell uname)))
	@ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) -n && sh -c "(type dos2unix 2> /dev/null) || (ScriptsLib/GetMessageForMissingTool dos2unix && exit 1)"
	@ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) -n && sh -c "(type unix2dos 2> /dev/null) || (ScriptsLib/GetMessageForMissingTool unix2dos && exit 1)"
endif
	@mkdir -p IntermediateFiles
	@touch IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_ALL


check-prerequisite-tools-current-configuration:
ifeq ($(CONFIGURATION),)
	$(error Cannot call check-prerequisite-tools-current-configuration without a configuration argument)
endif
	@$(MAKE) --no-print-directory apply-configuration-if-needed_
	@$(MAKE) --directory ThirdPartyComponents --no-print-directory check-prerequisite-tools
	@mkdir -p IntermediateFiles/$(CONFIGURATION)
	@touch IntermediateFiles/$(CONFIGURATION)/TOOLS_CHECKED


IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_ALL:
	@$(MAKE) check-prerequisite-tools-common --no-print-directory

IntermediateFiles/$(CONFIGURATION)/TOOLS_CHECKED:
	@#Check BOTH the check-prerequisite-tools-common (redundantly) and check-prerequisite-tools-current-configuration
	@#because we could be using different versions of make (eg. cygwin vs. WSL) for different configurations
	@$(MAKE) check-prerequisite-tools-common check-prerequisite-tools-current-configuration --no-print-directory


assure-default-configurations-exist_:
ifeq ($(shell ScriptsLib/GetConfigurations),)
	@$(MAKE) default-configurations --no-print-directory
endif


apply-configuration-if-needed_:	assure-default-configurations-exist_
ifneq ($(CONFIGURATION),)
	@ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@$(MAKE) --no-print-directory --silent IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif


#
# The configuration file contains a command-line used to generate it, and a bunch of variables
# used for the build process. This reconfigure target re-runs the configure program using the same
# arguments that were used to originally build the makefile
#
reconfigure:
ifeq ($(CONFIGURATION),)
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory reconfigure CONFIGURATION=$$i;\
	done
else
	@ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Reconfiguring $(CONFIGURATION):"
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && PATH=.:$$PATH && `./ScriptsLib/GetConfigurationParameter $(CONFIGURATION) Configure-Command-Line`
endif

#
# Each time we release a new version, its probbaly  good idea to regenerate your configuration files.
#
ifneq ($(CONFIGURATION),)
ConfigurationFiles/$(CONFIGURATION).xml:	STROIKA_VERSION
	@$(MAKE) reconfigure
endif


apply-configurations:
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory apply-configuration CONFIGURATION=$$i;\
	done

apply-configurations-if-needed:
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory --silent IntermediateFiles/$$i/Configuration.mk CONFIGURATION=$$i;\
	done


IntermediateFiles/$(CONFIGURATION)/Configuration.mk:	ConfigurationFiles/$(CONFIGURATION).xml
	@$(MAKE) --no-print-directory apply-configuration
	

apply-configuration:
ifeq ($(CONFIGURATION),)
	$(error Cannot call apply-configuration without a configuration argument)
endif
	@ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Applying configuration {$(CONFIGURATION)}:"
	@mkdir -p "IntermediateFiles/$(CONFIGURATION)/"
	@ScriptsLib/ApplyConfiguration $(CONFIGURATION)
	@ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) -n "Writing \"IntermediateFiles/$(CONFIGURATION)/Stroika-Current-Version.h\" ... "
	@ScriptsLib/MakeVersionFile STROIKA_VERSION IntermediateFiles/$(CONFIGURATION)/Stroika-Current-Version.h StroikaLibVersion
	@echo "done"


UNAME_DASH_O_=$(shell uname -o 2>/dev/null || true)

default-configurations:
	@ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Making default configurations:"
	@export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
	if [ "$(UNAME_DASH_O_)" = "Cygwin" ] ; then\
		./configure Debug-U-32 --config-tag Windows --config-tag 32 --arch x86 --apply-default-debug-flags;\
		./configure Debug-U-64 --config-tag Windows --config-tag 64 --arch x86_64 --apply-default-debug-flags;\
		./configure Release-Logging-U-32 --config-tag Windows --config-tag 32 --arch x86 --apply-default-release-flags --trace2file enable;\
		./configure Release-Logging-U-64 --config-tag Windows --config-tag 64 --arch x86_64 --apply-default-release-flags --trace2file enable;\
		./configure Release-U-32 --config-tag Windows --config-tag 32 --arch x86 --apply-default-release-flags;\
		./configure Release-U-64 --config-tag Windows --config-tag 64 --arch x86_64 --apply-default-release-flags;\
	else\
		./configure Debug --config-tag Unix --apply-default-debug-flags;\
		./configure Release --config-tag Unix --apply-default-release-flags;\
	fi


basic-unix-test-configurations:
	@ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Making basic-unix-test-configurations:"
	@export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
	./configure DEFAULT_CONFIG --config-tag Unix --only-if-has-compiler;\
	./configure no-third-party-components --config-tag Unix --only-if-has-compiler --LibCurl no --lzma no --zlib no --OpenSSL no --sqlite no --Xerces no --boost no;\
	./configure only-zlib-system-third-party-component --config-tag Unix --only-if-has-compiler --LibCurl no --lzma no --zlib system --OpenSSL no --sqlite no --Xerces no --boost no;\
	make --silent basic-unix-test-configurations_g++_versions_;\
	make --silent basic-unix-test-configurations_clang++_versions_;\
	#\
	#32-bit not working now - asm bug - must debug...\
	#./configure gcc-release-32 --compiler-driver "gcc -m32" --trace2file enable --assertions enable --only-if-has-compiler --LibCurl no --OpenSSL no --Xerces no --zlib no --lzma no --extra-compiler-args -m32 --extra-linker-args -m32 --static-link-gccruntime disable;\
	#\
	make --silent basic-unix-test-configurations_sanitizer_configs_;\
	#\
	./configure g++-optimized --only-if-has-compiler --config-tag Unix --apply-default-release-flags;\
	#\
	make --silent basic-unix-test-configurations_valgrind_configs_;\


basic-unix-test-configurations_g++_versions_:
	# g++-8
	./configure g++-8-debug-c++17 --config-tag Unix --compiler-driver g++-8 --apply-default-debug-flags --only-if-has-compiler --trace2file enable --cppstd-version c++17
	./configure g++-8-release-c++17 --config-tag Unix --compiler-driver g++-8 --apply-default-release-flags --only-if-has-compiler --cppstd-version c++17
	./configure g++-8-debug-c++2a --config-tag Unix --compiler-driver g++-8 --apply-default-debug-flags --only-if-has-compiler --trace2file enable --cppstd-version c++2a
	./configure my-g++-8.3-debug-c++2a --config-tag Unix --compiler-driver /private-compiler-builds/gcc-8.3.0/bin/x86_64-pc-linux-gnu-gcc --apply-default-debug-flags --no-sanitize address --append-run-prefix 'LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:/private-compiler-builds/gcc-8.3.0/lib64' --only-if-has-compiler --cppstd-version c++2a
	# A few sanitize/configs (must do explicit -g++-8 versions for ubuntu 1804 since its default g++ is 7 and not supported any longer)
	./configure g++-8-release-sanitize_address_undefined --config-tag Unix --only-if-has-compiler --compiler-driver g++-8 --apply-default-release-flags --trace2file enable --cppstd-version c++17 --sanitize none,address,undefined
	./configure g++-8-release-sanitize_thread_undefined --config-tag Unix --only-if-has-compiler --compiler-driver g++-8 --apply-default-release-flags --trace2file enable --cppstd-version c++17 --sanitize none,thread,undefined --append-run-prefix "TSAN_OPTIONS=suppressions=$(StroikaRoot)/Tests/ThreadSanitizerSuppressions.supp"
	# g++-9
	./configure g++-9-debug-c++17 --config-tag Unix --compiler-driver g++-9 --apply-default-debug-flags --only-if-has-compiler --trace2file enable --cppstd-version c++17
	./configure g++-9-release-c++17 --config-tag Unix --compiler-driver g++-9 --apply-default-release-flags --only-if-has-compiler --cppstd-version c++17
	./configure g++-9-debug-c++2a --config-tag Unix --compiler-driver g++-9 --apply-default-debug-flags --only-if-has-compiler --trace2file enable --cppstd-version c++2a
	# g++-10
	./configure g++-10-debug-c++17 --config-tag Unix --compiler-driver g++-10 --apply-default-debug-flags --only-if-has-compiler --trace2file enable --cppstd-version c++17
	./configure g++-10-release-c++17 --config-tag Unix --compiler-driver g++-10 --apply-default-release-flags --only-if-has-compiler --cppstd-version c++17
	./configure g++-10-debug-c++2a --config-tag Unix --compiler-driver g++-10 --apply-default-debug-flags --only-if-has-compiler --trace2file enable --cppstd-version c++2a

basic-unix-test-configurations_clang++_versions_:
	#
	# clang++-6
	./configure clang++-6-debug-libc++ --config-tag Unix --compiler-driver clang++-6.0 --apply-default-debug-flags --stdlib libc++ --only-if-has-compiler --trace2file enable
	./configure clang++-6-release-libstdc++ --config-tag Unix --compiler-driver clang++-6.0 --apply-default-release-flags --stdlib libstdc++  --only-if-has-compiler --trace2file enable
	# clang++-7
	./configure clang++-7-debug-libc++ --config-tag Unix --compiler-driver clang++-7 --apply-default-debug-flags --stdlib libc++ --only-if-has-compiler --trace2file enable
	./configure clang++-7-debug-libc++-c++2a --config-tag Unix --compiler-driver clang++-7 --apply-default-debug-flags --stdlib libc++ --only-if-has-compiler --trace2file enable --cppstd-version c++2a
	./configure clang++-7-release-libstdc++ --config-tag Unix --compiler-driver clang++-7 --apply-default-release-flags --stdlib libstdc++  --only-if-has-compiler --trace2file enable
	# --no-sanitize and --lto disable calls for clang7 just because of how I built it...
	./configure my-clang++-7-debug-libc++ --config-tag Unix --compiler-driver /private-compiler-builds/clang-7.0.0/bin/clang++ --apply-default-debug-flags --stdlib libc++ --only-if-has-compiler --trace2file enable --no-sanitize address --no-sanitize undefined
	./configure my-clang++-7-debug-libc++-c++2a --config-tag Unix --compiler-driver /private-compiler-builds/clang-7.0.0/bin/clang++ --apply-default-debug-flags --stdlib libc++ --only-if-has-compiler --trace2file enable --no-sanitize address --no-sanitize undefined --cppstd-version c++2a
	./configure my-clang++-7-release-libstdc++ --config-tag Unix --compiler-driver /private-compiler-builds/clang-7.0.0/bin/clang++ --apply-default-release-flags --stdlib libstdc++  --only-if-has-compiler --trace2file enable --lto disable
	# clang-8
	./configure clang++-8-debug-libc++ --config-tag Unix --compiler-driver clang++-8 --apply-default-debug-flags --stdlib libc++ --only-if-has-compiler --trace2file enable
	./configure clang++-8-debug-libc++-c++2a --config-tag Unix --compiler-driver clang++-8 --apply-default-debug-flags --stdlib libc++ --only-if-has-compiler --trace2file enable --cppstd-version c++2a
	./configure clang++-8-release-libstdc++ --config-tag Unix --compiler-driver clang++-8 --apply-default-release-flags --stdlib libstdc++  --only-if-has-compiler --trace2file enable
	# clang-9
	./configure clang++-9-debug-libc++ --config-tag Unix --compiler-driver clang++-9 --apply-default-debug-flags --stdlib libc++ --only-if-has-compiler --trace2file enable
	./configure clang++-9-debug-libc++-c++2a --config-tag Unix --compiler-driver clang++-9 --apply-default-debug-flags --stdlib libc++ --only-if-has-compiler --trace2file enable --cppstd-version c++2a
	./configure clang++-9-release-libstdc++ --config-tag Unix --compiler-driver clang++-9 --apply-default-release-flags --stdlib libstdc++  --only-if-has-compiler --trace2file enable
	# clang-10
	./configure clang++-10-debug-libc++ --config-tag Unix --compiler-driver clang++-10 --apply-default-debug-flags --stdlib libc++ --only-if-has-compiler --trace2file enable
	./configure clang++-10-debug-libc++-c++2a --config-tag Unix --compiler-driver clang++-10 --apply-default-debug-flags --stdlib libc++ --only-if-has-compiler --trace2file enable --cppstd-version c++2a
	./configure clang++-10-release-libstdc++ --config-tag Unix --compiler-driver clang++-10 --apply-default-release-flags --stdlib libstdc++  --only-if-has-compiler --trace2file enable
	# clang-11
	./configure clang++-11-debug-libc++ --config-tag Unix --compiler-driver clang++-11 --apply-default-debug-flags --stdlib libc++ --only-if-has-compiler --trace2file enable
	./configure clang++-11-debug-libc++-c++2a --config-tag Unix --compiler-driver clang++-11 --apply-default-debug-flags --stdlib libc++ --only-if-has-compiler --trace2file enable --cppstd-version c++2a
	./configure clang++-11-release-libstdc++ --config-tag Unix --compiler-driver clang++-11 --apply-default-release-flags --stdlib libstdc++  --only-if-has-compiler --trace2file enable

basic-unix-test-configurations_sanitizer_configs_:
	# A few sanitize/configs (must do explicit -g++-8 versions for ubuntu 1804 since its default g++ is 7 and not supported any longer)
	./configure g++-debug-sanitize_leak --config-tag Unix --only-if-has-compiler --apply-default-debug-flags --sanitize none,leak --trace2file enable
	./configure g++-debug-sanitize_address --config-tag Unix --only-if-has-compiler --apply-default-debug-flags --sanitize none,address,undefined --trace2file enable
	if [[ $$(lsb_release -rs 2>/dev/null) == "20.10" ]]; then\
		ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Skipping configuration g++-debug-sanitize_thread to WORKAROUND qCompiler_SanitizerDoubleLockWithConditionVariables_Buggy - TSAN just too many false positives to be useful";\
	else\
		./configure g++-debug-sanitize_thread --config-tag Unix --only-if-has-compiler --apply-default-debug-flags --trace2file enable --cppstd-version c++17 --sanitize none,thread,undefined --append-run-prefix "TSAN_OPTIONS=suppressions=$(StroikaRoot)/Tests/ThreadSanitizerSuppressions.supp";\
	fi
	./configure g++-debug-sanitize_undefined --config-tag Unix --only-if-has-compiler --apply-default-debug-flags --sanitize none,address,undefined --trace2file enable
	./configure g++-release-sanitize_address_undefined --config-tag Unix --only-if-has-compiler --apply-default-release-flags --trace2file enable --cppstd-version c++17 --sanitize none,address,undefined
	if [[ $$(lsb_release -rs 2>/dev/null) == "20.10" ]]; then\
		ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Skipping configuration g++-release-sanitize_thread_undefined to WORKAROUND qCompiler_SanitizerDoubleLockWithConditionVariables_Buggy- TSAN just too many false positives to be useful";\
	else\
		./configure g++-release-sanitize_thread_undefined --config-tag Unix --only-if-has-compiler --apply-default-release-flags --trace2file enable --cppstd-version c++17 --sanitize none,thread,undefined --append-run-prefix "TSAN_OPTIONS=suppressions=$(StroikaRoot)/Tests/ThreadSanitizerSuppressions.supp";\
	fi

basic-unix-test-configurations_valgrind_configs_:
	###Builds with a few special flags to make valgrind work better
	#nb: using default installed C++ compiler cuz of matching installed libraries on host computer
	./configure g++-valgrind-debug-SSLPurify --only-if-has-compiler --config-tag Unix --config-tag valgrind -valgrind enable --openssl use --openssl-extraargs purify --apply-default-debug-flags --trace2file enable --sanitize none
	./configure g++-valgrind-debug-SSLPurify-NoBlockAlloc --only-if-has-compiler --config-tag Unix --config-tag valgrind -valgrind enable --openssl use --openssl-extraargs purify  --apply-default-debug-flags --trace2file enable --block-allocation disable --sanitize none
	##https://stroika.atlassian.net/browse/STK-674 - avoid warning
	##https://stroika.atlassian.net/browse/STK-702 - avoid another warning (lto disable for valgrind)
	./configure g++-valgrind-release-SSLPurify-NoBlockAlloc --only-if-has-compiler --config-tag Unix --config-tag valgrind --valgrind enable --openssl use --openssl-extraargs purify --apply-default-release-flags --trace2file disable --block-allocation disable -lto disable
	# A few valgrind (must do explicit -g++-8 versions for ubuntu 1804 since its default g++ is 7 and not supported any longer)
	./configure g++--8-valgrind-debug-SSLPurify --compiler-driver g++-8 --only-if-has-compiler --config-tag Unix --config-tag valgrind -valgrind enable --openssl use --openssl-extraargs purify --apply-default-debug-flags --trace2file enable --sanitize none


raspberrypi-cross-compile-test-configurations:
	@ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Making raspberrypi-cross-compile-test-configurations:"
	@export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
	#\
	# --append-CXXFLAGS -Wno-psabi JUST FOR ARM GCC6 and GCC7 - https://stroika.atlassian.net/browse/STK-627\
	#./configure raspberrypi-g++-7 --config-tag Unix --config-tag raspberrypi --apply-default-debug-flags --only-if-has-compiler --trace2file enable --compiler-driver 'arm-linux-gnueabihf-g++-7' --cross-compiling true --sanitize none --append-CXXFLAGS -Wno-psabi;\
	#qCompiler_Sanitizer_stack_use_after_scope_on_arm_Buggy - SEE https://stroika.atlassian.net/browse/STK-500 - RETEST WHEN WE HAVE GCC8\
	#./configure raspberrypi-g++-7-sanitize --apply-default-debug-flags --only-if-has-compiler --trace2file enable --sanitize address,undefined --compiler-driver 'arm-linux-gnueabihf-g++-7' --cross-compiling true --append-run-prefix 'LD_PRELOAD=/usr/lib/arm-linux-gnueabihf/libasan.so.4' --append-CXXFLAGS -Wno-psabi;\
	./configure raspberrypi-g++-7-sanitize_undefined --config-tag Unix --config-tag raspberrypi --apply-default-debug-flags --only-if-has-compiler --trace2file enable --sanitize none,undefined --compiler-driver 'arm-linux-gnueabihf-g++-7' --cross-compiling true --append-CXXFLAGS -Wno-psabi;\
	#./configure raspberrypi-valgrind-g++-7-SSLPurify-NoBlockAlloc --config-tag Unix --config-tag raspberrypi --config-tag valgrind --apply-default-release-flags --only-if-has-compiler --trace2file disable --compiler-driver 'arm-linux-gnueabihf-g++-7' --valgrind enable --block-allocation disable --openssl use --openssl-extraargs purify --cross-compiling true --append-CXXFLAGS -Wno-psabi;\
	#\
	# --append-CXXFLAGS -Wno-psabi JUST FOR ARM GCC6 and GCC7 - https://stroika.atlassian.net/browse/STK-627 \
	#qCompiler_Sanitizer_stack_use_after_scope_on_arm_Buggy - SEE https://stroika.atlassian.net/browse/STK-500 - RETEST WHEN WE HAVE GCC8\
	#./configure raspberrypi-g++-8 --apply-default-debug-flags --only-if-has-compiler --trace2file enable --compiler-driver 'arm-linux-gnueabihf-g++-8' --cross-compiling true --append-run-prefix 'LD_PRELOAD=/usr/lib/arm-linux-gnueabihf/libasan.so.5' --append-CXXFLAGS -Wno-psabi;\
	#./configure raspberrypi-g++-8-release-sanitize_address --config-tag Unix --config-tag raspberrypi --apply-default-release-flags --only-if-has-compiler --trace2file enable --compiler-driver 'arm-linux-gnueabihf-g++-8' --sanitize none,address --cross-compiling true --append-run-prefix 'LD_PRELOAD=/usr/lib/arm-linux-gnueabihf/libasan.so.5';\
	./configure raspberrypi-g++-8-release-sanitize_address --config-tag Unix --config-tag raspberrypi --apply-default-release-flags --only-if-has-compiler --trace2file enable --compiler-driver 'arm-linux-gnueabihf-g++-8' --sanitize none,address --cross-compiling true;\
	./configure raspberrypi-g++-8-debug-sanitize_undefined --config-tag Unix --config-tag raspberrypi --apply-default-debug-flags --only-if-has-compiler --trace2file enable --sanitize none,undefined --compiler-driver 'arm-linux-gnueabihf-g++-8' --cross-compiling true --append-CXXFLAGS -Wno-psabi;\
	##Couldn't get tsan to link (/usr/bin/arm-linux-gnueabihf-ld: cannot find -ltsan) - so retry on next ubuntu release\
	##DISABLE VALGRIND ALTOGETHER ON RASPBERRYPI - https://stroika.atlassian.net/browse/STK-703\
	#./configure raspberrypi-valgrind-g++-8-SSLPurify-NoBlockAlloc --config-tag Unix --config-tag raspberrypi --config-tag valgrind --apply-default-release-flags --only-if-has-compiler --trace2file disable --compiler-driver 'arm-linux-gnueabihf-g++-8' --valgrind enable --block-allocation disable --openssl use --openssl-extraargs purify --cross-compiling true;\
	#\
	# gcc-9 ARM raspberrypi compiler (ubuntu 20.04)\
	./configure raspberrypi-g++-9-release-sanitize_address --config-tag Unix --config-tag raspberrypi --apply-default-release-flags --only-if-has-compiler --trace2file enable --compiler-driver 'arm-linux-gnueabihf-g++-9' --sanitize none,address --cross-compiling true;\
	./configure raspberrypi-g++-9-debug-sanitize_undefined --config-tag Unix --config-tag raspberrypi --apply-default-debug-flags --only-if-has-compiler --trace2file enable --sanitize none,undefined --compiler-driver 'arm-linux-gnueabihf-g++-9' --cross-compiling true --append-CXXFLAGS -Wno-psabi;\
	#\
	# gcc-10 ARM raspberrypi compiler (ubuntu 20.04)\
	./configure raspberrypi-g++-10-release-sanitize_address --config-tag Unix --config-tag raspberrypi --apply-default-release-flags --only-if-has-compiler --trace2file enable --compiler-driver 'arm-linux-gnueabihf-g++-10' --sanitize none,address --cross-compiling true;\
	./configure raspberrypi-g++-10-debug-sanitize_undefined --config-tag Unix --config-tag raspberrypi --apply-default-debug-flags --only-if-has-compiler --trace2file enable --sanitize none,undefined --compiler-driver 'arm-linux-gnueabihf-g++-10' --cross-compiling true --append-CXXFLAGS -Wno-psabi;\


regression-test-configurations:
	@ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Making regression-test configurations:"
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
	g++ --std=c++17 -O -o /usr/local/bin/realpath BootstrapToolsSrc/realpath.cpp
