StroikaRoot:=$(abspath .)/

include ScriptsLib/Makefile-Common.mk

ifneq ($(CONFIGURATION),)
	#no error if missing cuz could be doing make clobber
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif


ifeq ($(filter 3.81, $(firstword $(sort $(MAKE_VERSION) 3.81))),)
$(info Warning - version of GNU Make - $(MAKE_VERSION) - appears too old)
endif


#not parallel because submakefiles use parallelism, but generally best to sequence these top level requests. Like if you say
# make clobber all you don't want those to happen at the same time. And make libraries samples wouldn't really work since all the libraries
# have to be built before the samples etc...
.NOTPARALLEL:

.PHONY:	tests documentation all check distclean clobber libraries apply-configuration-if-needed_ check-prerequisite-tools apply-configurations apply-configuration


SHELL=/bin/bash


# Only applies to run-tests
export TEST_FAILURES_CAUSE_FAILED_MAKE?=1


.DEFAULT_GOAL := help

#Handy shortcut
CONFIGURATION_TAGS?=$(TAGS)

# use = not := cuz rarely used more than once, and commonly never, and when called without configs,needs
# re-evaluation (make default-configurations internally called - assure...)
APPLY_CONFIGS=$(or \
				$(CONFIGURATION), \
				$(if $(CONFIGURATION_TAGS), \
					$(shell ScriptsLib/GetConfigurations --quiet --config-tags "$(CONFIGURATION_TAGS)"),\
					$(if $(filter clobber, $(MAKECMDGOALS)),\
						$(shell ScriptsLib/GetConfigurations --quiet --all --quiet),\
						$(shell ScriptsLib/GetConfigurations --quiet --all-default)\
					)\
				)\
			)

help:
	@$(ECHO) "Help for making Stroika:"
	@$(ECHO) "Targets:"
	@$(ECHO) "    all:                         -    Builds everything"
	@$(ECHO) "    check:                       -    Checks everything was built properly"
	@$(ECHO) "    clean:"
	@$(ECHO) "    clobber:"
	@$(ECHO) "    distclean:"
	@$(ECHO) "    reconfigure:                 -    Rebuild configuration files from the command-lines that built them before"
	@$(ECHO) "    libraries:                   -    Builds Stroika foundation & frameworks, and any things it depends on (like third-party-components)"
	@$(ECHO) "    project-files:               -    Alias for project-files-visual-studio project-files-qt-creator"
	@$(ECHO) "    project-files-vs-code:       -    Builds project files for visual studio code"
	@$(ECHO) "    project-files-visual-studio: -    Builds project files for visual studio.net"
	@$(ECHO) "    project-files-qt-creator(*): -    Builds project project-files-qt-creator (also project-files-qt-creator-load"
	@$(ECHO) "                                      project-files-qt-creator-save)"
	@$(ECHO) "    tests:"
	@$(ECHO) "    format-code:                 -    Run Scripts/FormatCode (clang-format) on source code, and update it to conform to Stroika code formatting standards"
	@$(ECHO) "    samples:"
	@$(ECHO) "    documentation:"
	@$(ECHO) "    third-party-components:"
	@$(ECHO) "    run-tests:                   -    [REMOTE=] - eg. REMOTE=lewis@localhost;"
	@$(ECHO) "                                      [VALGRIND=memcheck, etc] to run with valgrind"
	@$(ECHO) "                                      (EXTRA_VALGRIND_OPTIONS= can be used with valgrind)"
	@$(ECHO) "                                      OR VALGRIND_SUPPRESSIONS=\"Valgrind-MemCheck-Common.supp Valgrind-MemCheck-BlockAllocation.supp\""
	@$(ECHO) "                                      EG: VALGRIND_SUPPRESSIONS=\"Valgrind-MemCheck-Common.supp\" make VALGRIND=memcheck run-tests"
	@$(ECHO) "    apply-configurations:        -    Force re-creation implied files / links for any configurations in the Configurations"
	@$(ECHO) "                                      folder (not needed, automatic)"
	@$(ECHO) "    default-configurations:      -    Creates the default configurations in Configurations folder; see ./configure -help for environment variables"
	@$(ECHO) "                                      e.g. EXTRA_CONFIGURE_ARGS='--openssl-extraargs purify --block-allocation disable' make default-configurations"
	@$(ECHO) "                                      OR EXTRA_CONFIGURE_ARGS='--platform VisualStudio.Net-2022' make default-configurations"
	@$(ECHO) "                                      OR PLATFORM='VisualStudio.Net-2022' make default-configurations"
	@$(ECHO) "    list-configurations:         -    prints all available configurations (each can be used as arg to CONFIGURAITON= make lines)"
	@$(ECHO) "    list-configuration-tags:     -    prints a list of all configurtion tags (configuration tags impute groups of configurations)"
	@$(ECHO) "    check-prerequisite-tools:    -    Check the tools needed to build Stroika are installed."
	@$(ECHO) "Special Variables:               -    Extra params you can pass to the make line that may help..."
	@$(ECHO) "    CONFIGURATION=XYZ            -    Causes only the configuration XYZ to have the targetted goal applied (e.g. make all CONFIGURATION=Debug)"
	@$(ECHO) "    CONFIGURATION_TAGS=TAGS      -    Causes only the configurations with tags TAGS to have the targetted goal applied (e.g. make all CONFIGURATION_TAGS=Windows)"
	@$(ECHO) "                                      NB: TAGS is a handy shortcut for CONFIGURAITON_TAGS, so you can say make all TAGS=\"Windows 64\""
	@$(ECHO) "    ECHO_BUILD_LINES=1           -    Causes make lines to be echoed which can help makefile debugging"
	@$(ECHO) "    WRITE_PREPROCESSOR_OUTPUT=1  -    Write next to each object file a corresponding .i (preprocessor output) file; useful to reproduce and narrow compiler bugs (for report especially)"
	@$(ECHO) "    MAKE_INDENT_LEVEL=0          -    Helpful to neaten formatting when multiple levels of makes calling Stroika make"
	@$(ECHO) "    QUICK_BUILD=1                -    Defaults=0, but if =1, skip some optional build steps (like openssl tests, CURRENT folders; may skip detecting changes in libraries etc; used for some CI testing)"
	@$(ECHO) "    TEST_FAILURES_CAUSE_FAILED_MAKE=0"
	@$(ECHO) "                                      only applies to make run-tests, and prevents test failures from stopping make (like make -k on run-tests)"


ifeq ($(CONFIGURATION),)
all:		IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_COMMON IntermediateFiles/DEFAULT_PROJECT_FILES_BUILT IntermediateFiles/ASSURE_DEFAULT_CONFIGURATIONS_BUILT
	@#first run all checks so any errors for missing tools appear ASAP
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL)  "Checking Prerequisites for Stroika:"
	@$(MAKE) --no-print-directory --silent apply-configurations-if-needed MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory --silent IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_COMMON IntermediateFiles/$$i/TOOLS_CHECKED CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
	done
	@for i in $(APPLY_CONFIGS) ; do\
		$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Stroika all {$$i}:";\
		$(MAKE) --no-print-directory all CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
	done
else
all:		IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_COMMON IntermediateFiles/DEFAULT_PROJECT_FILES_BUILT IntermediateFiles/ASSURE_DEFAULT_CONFIGURATIONS_BUILT libraries tools samples tests documentation
endif


check: IntermediateFiles/ASSURE_DEFAULT_CONFIGURATIONS_BUILT
ifeq ($(CONFIGURATION),)
ifeq ($(MAKECMDGOALS),check)
	@for i in $(APPLY_CONFIGS) ; do\
		$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika/Check {$$i}:";\
		$(MAKE) --no-print-directory check CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
	done
endif
else
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Checking Stroika {$(CONFIGURATION)}:"
	@$(MAKE) --directory ThirdPartyComponents --no-print-directory check MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Library --no-print-directory check MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Tools --no-print-directory check MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Tests --no-print-directory check MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Samples --no-print-directory check MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif

distclean:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika $(call FUNCTION_CAPITALIZE_WORD,$@):"
ifneq ($(CONFIGURATION),)
	$(error "make distclean applies to all configurations - and deletes all configurations")
endif
	@rm -rf Builds ConfigurationFiles IntermediateFiles
	@$(MAKE) --no-print-directory --directory=ThirdPartyComponents distclean
	@$(MAKE) --no-print-directory clobber MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))

clean clobber:
ifeq ($(CONFIGURATION),)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika $(call FUNCTION_CAPITALIZE_WORD,$@):"
ifeq ($(CONFIGURATION_TAGS),)
	@#for clobber, quickly delete all interesting stuff (if no args so unrestricted) and in third party deletes stuff like 'CURRENT' folders
	@#only delete ALL intermediate files (cuz includes Config.mk etc and forces rebuild all)
	@if [ "$@" = "clobber" ] ; then \
		rm -rf Builds/*;\
		rm -rf IntermediateFiles/*;\
		$(MAKE) --directory ThirdPartyComponents --no-print-directory clobber CONFIGURATION= MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
	fi
endif
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory $@ CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
	done
else
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika $(call FUNCTION_CAPITALIZE_WORD,$@) {$(CONFIGURATION)}:"
	@#only delete ALL intermediate files (cuz includes Config.mk etc and forces rebuild all)
	@if [ "$@" = "clobber" ] ; then \
		rm -rf IntermediateFiles/$(CONFIGURATION);\
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
libraries:	IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_COMMON IntermediateFiles/DEFAULT_PROJECT_FILES_BUILT IntermediateFiles/ASSURE_DEFAULT_CONFIGURATIONS_BUILT
	@for i in $(APPLY_CONFIGS) ; do\
		$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Making Stroika/Libraries {$$i}:";\
		$(MAKE) --no-print-directory libraries CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
	done
else
ifeq ($(QUICK_BUILD), 1)
libraries:
	@#See if files already there, and if so skip all this, and else do non-quick build
	@if [[ -f Builds/$(CONFIGURATION)/Stroika-Foundation${LIB_SUFFIX} && -f Builds/$(CONFIGURATION)/Stroika-Frameworks${LIB_SUFFIX} ]]; then \
		$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika-Foundation and Stroika-Frameworks libraries exist and QUICK_BUILD=1"; \
	else\
		$(StroikaRoot)ScriptsLib/CheckValidConfiguration $(CONFIGURATION); \
		$(MAKE) --no-print-directory --silent IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_COMMON IntermediateFiles/DEFAULT_PROJECT_FILES_BUILT IntermediateFiles/ASSURE_DEFAULT_CONFIGURATIONS_BUILT IntermediateFiles/$(CONFIGURATION)/TOOLS_CHECKED third-party-components; \
		$(MAKE) --directory Library --no-print-directory all; \
	fi
else
libraries:	IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_COMMON IntermediateFiles/DEFAULT_PROJECT_FILES_BUILT IntermediateFiles/ASSURE_DEFAULT_CONFIGURATIONS_BUILT IntermediateFiles/$(CONFIGURATION)/TOOLS_CHECKED third-party-components
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@$(MAKE) --directory Library --no-print-directory all
endif
endif



ifeq ($(CONFIGURATION),)
third-party-components:	IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_COMMON IntermediateFiles/ASSURE_DEFAULT_CONFIGURATIONS_BUILT
	@for i in $(APPLY_CONFIGS) ; do\
		$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Making Stroika/Third-party-components {$$i}:";\
		$(MAKE) --no-print-directory third-party-components CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
	done
else
third-party-components:	IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_COMMON IntermediateFiles/ASSURE_DEFAULT_CONFIGURATIONS_BUILT apply-configuration-if-needed_ IntermediateFiles/$(CONFIGURATION)/TOOLS_CHECKED
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@$(MAKE) --directory ThirdPartyComponents --no-print-directory all
endif


# As of Stroika 2.1r4, no longer automatically run project-files-qt-creator since I'm not sure its widely used
# anymore, and we may just deprecate (I no longer update the project files)
project-files:	IntermediateFiles/ASSURE_DEFAULT_CONFIGURATIONS_BUILT
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Creating (common) default project files:"
	@$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) project-files-visual-studio project-files-vs-code

project-files-vs-code:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Creating default visual-studio-code configuration files:"
ifeq ($(DETECTED_HOST_OS),Darwin)
	@rsync --update .config-default.json .config.json
else
	@cp --update .config-default.json .config.json
endif
	@$(MAKE) --silent apply-configurations MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))

project-files-visual-studio:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Creating default visual-studio project files:"
	@$(MAKE) --directory Tests --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) project-files
ifeq ($(DETECTED_HOST_OS),Darwin)
	@rsync --update Workspaces/VisualStudio.Net/Microsoft.Cpp.stroika.user-default.props Workspaces/VisualStudio.Net/Microsoft.Cpp.stroika.user.props
else
	@cp --update Workspaces/VisualStudio.Net/Microsoft.Cpp.stroika.user-default.props Workspaces/VisualStudio.Net/Microsoft.Cpp.stroika.user.props
endif

project-files-qt-creator:	project-files-qt-creator-load


project-files-qt-creator-load:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "Loading qt-creator project files ... "
	@for i in StroikaDevRoot.config StroikaDevRoot.creator StroikaDevRoot.files StroikaDevRoot.includes; do cp Library/Projects/QtCreator/$$i .; done;
	@$(ECHO) "done"

project-files-qt-creator-save:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "Saving qt-creator project files ... "
	@for i in StroikaDevRoot.config StroikaDevRoot.creator StroikaDevRoot.files StroikaDevRoot.includes; do cp $$i Library/Projects/QtCreator/ ; done;
	@$(ECHO) "done"


ifeq ($(CONFIGURATION),)
tools:	IntermediateFiles/ASSURE_DEFAULT_CONFIGURATIONS_BUILT
	@for i in $(APPLY_CONFIGS) ; do\
		$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Making Stroika/Tools {$$i}:";\
		$(MAKE) tools --no-print-directory CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
	done
else
tools:	IntermediateFiles/ASSURE_DEFAULT_CONFIGURATIONS_BUILT libraries
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@$(MAKE) --directory Tools --no-print-directory all
endif


ifeq ($(CONFIGURATION),)
tests:	IntermediateFiles/ASSURE_DEFAULT_CONFIGURATIONS_BUILT
	@for i in $(APPLY_CONFIGS) ; do\
		$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Making Stroika/Tests {$$i}:";\
		$(MAKE) tests --no-print-directory CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
	done
else
tests:	IntermediateFiles/ASSURE_DEFAULT_CONFIGURATIONS_BUILT libraries
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@$(MAKE) --directory Tests --no-print-directory tests
endif


ifeq ($(CONFIGURATION),)
samples:	IntermediateFiles/ASSURE_DEFAULT_CONFIGURATIONS_BUILT
	@for i in $(APPLY_CONFIGS) ; do\
		$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Making Stroika/Samples {$$i}:";\
		$(MAKE) samples --no-print-directory CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
	done
else
samples:	IntermediateFiles/ASSURE_DEFAULT_CONFIGURATIONS_BUILT libraries
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@$(MAKE) --directory Samples --no-print-directory samples
endif


ifeq ($(CONFIGURATION),)
run-tests:
	@for i in $(APPLY_CONFIGS) ; do\
		$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika/Run-All-Tests {$$i}:";\
		$(MAKE) --no-print-directory run-tests CONFIGURATION=$$i || exit $$?;\
	done
else
run-tests:	tests
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
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
#	We use take target files IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_COMMON and IntermediateFiles/$(CONFIGURATION)/TOOLS_CHECKED
#	to make sure each configuraiton is checked at least once before build, so we get easier to understand error messages
#	(tool x missing instead of xxx failed)
#
check-prerequisite-tools:
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
	@ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) -n && sh -c "(type jq 2> /dev/null) || (ScriptsLib/GetMessageForMissingTool jq && exit 1)"
	@if [[ "$(DETECTED_HOST_OS)" = "Cygwin" || "$(DETECTED_HOST_OS)" = "MSYS" ]] ; then\
		$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) -n && sh -c "(type dos2unix 2> /dev/null) || (ScriptsLib/GetMessageForMissingTool dos2unix && exit 1)";\
		$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) -n && sh -c "(type unix2dos 2> /dev/null) || (ScriptsLib/GetMessageForMissingTool unix2dos && exit 1)";\
	fi
ifneq (,$(findstring Darwin,$(DETECTED_HOST_OS)))
	@ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) -n && sh -c "(type gsed 2> /dev/null) || (ScriptsLib/GetMessageForMissingTool gsed && exit 1)"
endif
	@mkdir -p IntermediateFiles; touch IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_COMMON


check-prerequisite-tools-current-configuration:
ifeq ($(CONFIGURATION),)
	$(error Cannot call check-prerequisite-tools-current-configuration without a configuration argument)
endif
	@$(MAKE) --no-print-directory apply-configuration-if-needed_
	@$(MAKE) --directory ThirdPartyComponents --no-print-directory check-prerequisite-tools
	@mkdir -p IntermediateFiles/$(CONFIGURATION); touch IntermediateFiles/$(CONFIGURATION)/TOOLS_CHECKED


IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_COMMON:
	@$(MAKE) check-prerequisite-tools-common --no-print-directory

IntermediateFiles/$(CONFIGURATION)/TOOLS_CHECKED:
	@#Check BOTH the check-prerequisite-tools-common (redundantly) and check-prerequisite-tools-current-configuration
	@#because we could be using different versions of make (eg. cygwin vs. WSL) for different configurations
	@$(MAKE) --silent IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_COMMON check-prerequisite-tools-current-configuration --no-print-directory

IntermediateFiles/DEFAULT_PROJECT_FILES_BUILT:
	@$(MAKE) --no-print-directory --silent IntermediateFiles/ASSURE_DEFAULT_CONFIGURATIONS_BUILT
	@mkdir -p IntermediateFiles; touch IntermediateFiles/DEFAULT_PROJECT_FILES_BUILT


# DONT actually defaults if any configurations have already been created (but still mark as created)
IntermediateFiles/ASSURE_DEFAULT_CONFIGURATIONS_BUILT:
ifeq ($(shell ScriptsLib/GetConfigurations --quiet),)
	@$(MAKE) default-configurations --no-print-directory
endif
	@mkdir -p IntermediateFiles; touch IntermediateFiles/ASSURE_DEFAULT_CONFIGURATIONS_BUILT


apply-configuration-if-needed_:	IntermediateFiles/ASSURE_DEFAULT_CONFIGURATIONS_BUILT
ifneq ($(CONFIGURATION),)
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
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
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && PATH=.:$$PATH && `./ScriptsLib/GetConfigurationParameter $(CONFIGURATION) ConfigureCommandLine`
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
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Applying configuration {$(CONFIGURATION)}:"
	@mkdir -p "IntermediateFiles/$(CONFIGURATION)/"
	@$(StroikaRoot)ScriptsLib/ApplyConfiguration $(CONFIGURATION)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) -n "Writing \"IntermediateFiles/$(CONFIGURATION)/Stroika-Current-Version.h\" ... "
	@$(StroikaRoot)ScriptsLib/MakeVersionFile STROIKA_VERSION IntermediateFiles/$(CONFIGURATION)/Stroika-Current-Version.h StroikaLibVersion
	@echo "done"


# This variable (defaults to nothing) is added to all configurations built by make 'default-configurations' operation
# Use occasionally to generate quick debug builds etc...
DEFAULT_CONFIGURAITON_ADD2ALL=
# DEFAULT_CONFIGURAITON_ADD2ALL+= --boost no --openssl no
# DEFAULT_CONFIGURAITON_ADD2ALL+= --all-available-third-party-components
# DEFAULT_CONFIGURAITON_ADD2ALL+= --no-third-party-components

default-configurations:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Making default configurations:"
ifneq ($(findstring $(DETECTED_HOST_OS),MSYS-Cygwin),)
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ./configure Debug --config-tag Windows --config-tag x86_64 --build-by-default never --arch x86_64 --apply-default-debug-flags ${DEFAULT_CONFIGURAITON_ADD2ALL}
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ./configure Release --config-tag Windows --config-tag x86_64 --build-by-default never --arch x86_64 --apply-default-release-flags ${DEFAULT_CONFIGURAITON_ADD2ALL}
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ./configure Profile --config-tag Windows --config-tag x86_64 --build-by-default never --arch x86_64 --apply-default-release-flags --append-extra-suffix-linker-args -PROFILE ${DEFAULT_CONFIGURAITON_ADD2ALL}
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ./configure Release-Logging --config-tag Windows --config-tag x86_64 --build-by-default never --arch x86_64 --apply-default-release-flags --trace2file enable ${DEFAULT_CONFIGURAITON_ADD2ALL}
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ./configure Debug-x86 --config-tag Windows --config-tag x86 --arch x86 --build-by-default $(DETECTED_HOST_OS) --apply-default-debug-flags ${DEFAULT_CONFIGURAITON_ADD2ALL}
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ./configure Debug-x86_64 --config-tag Windows --config-tag x86_64 --build-by-default $(DETECTED_HOST_OS)  --arch x86_64 --apply-default-debug-flags ${DEFAULT_CONFIGURAITON_ADD2ALL}
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ./configure Release-x86 --config-tag Windows --config-tag x86 --arch x86 --build-by-default $(DETECTED_HOST_OS) --apply-default-release-flags ${DEFAULT_CONFIGURAITON_ADD2ALL}
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ./configure Release-x86_64 --config-tag Windows --config-tag x86_64 --arch x86_64 --build-by-default $(DETECTED_HOST_OS) --apply-default-release-flags ${DEFAULT_CONFIGURAITON_ADD2ALL}
else ifneq ($(findstring $(DETECTED_HOST_OS),Darwin),)
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ./configure Debug --build-by-default $(DETECTED_HOST_OS) --only-if-has-compiler --apply-default-debug-flags ${DEFAULT_CONFIGURAITON_ADD2ALL}
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ./configure Release --build-by-default $(DETECTED_HOST_OS) --only-if-has-compiler --apply-default-release-flags ${DEFAULT_CONFIGURAITON_ADD2ALL}
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ./configure Release-Logging --config-tag Unix --build-by-default never --only-if-has-compiler --apply-default-release-flags --trace2file enable ${DEFAULT_CONFIGURAITON_ADD2ALL}
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ./configure Release-x86_64 --arch x86_64 --config-tag x86_64 --build-by-default $(DETECTED_HOST_OS) --only-if-has-compiler --apply-default-release-flags ${DEFAULT_CONFIGURAITON_ADD2ALL}
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ./configure Release-arm64 --arch arm64 --config-tag arm --build-by-default $(DETECTED_HOST_OS) --only-if-has-compiler --apply-default-release-flags ${DEFAULT_CONFIGURAITON_ADD2ALL}
else
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ./configure Debug --config-tag Unix --build-by-default $(DETECTED_HOST_OS) --only-if-has-compiler --apply-default-debug-flags ${DEFAULT_CONFIGURAITON_ADD2ALL}
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ./configure Release --config-tag Unix --build-by-default $(DETECTED_HOST_OS) --only-if-has-compiler --apply-default-release-flags ${DEFAULT_CONFIGURAITON_ADD2ALL}
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ./configure Release-Logging --config-tag Unix --build-by-default never --only-if-has-compiler --apply-default-release-flags --trace2file enable ${DEFAULT_CONFIGURAITON_ADD2ALL}
endif



# This variable (defaults to nothing) is added to all configurations built by make 'default-configurations' operation
# Use occasionally to generate quick debug builds etc...
TEST_CONFIGURATIONS_ADD2ALL?=	--all-available-third-party-components

basic-unix-test-configurations:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Making basic-unix-test-configurations:"
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ./configure DEFAULT_CONFIG --config-tag Unix --only-if-has-compiler ${TEST_CONFIGURATIONS_ADD2ALL}
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ./configure no-third-party-components --config-tag Unix --only-if-has-compiler --no-third-party-components
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ./configure only-zlib-system-third-party-component --config-tag Unix --only-if-has-compiler --no-third-party-components --zlib system
	@$(MAKE) --silent MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) basic-unix-test-configurations_g++_versions_
	@$(MAKE) --silent MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) basic-unix-test-configurations_clang++_versions_
	@#
	@#32-bit not working now - asm bug - must debug...
	@#./configure gcc-release-32 --compiler-driver "gcc -m32" --trace2file enable --assertions enable --only-if-has-compiler --LibCurl no --OpenSSL no --Xerces no --zlib no --lzma no --extra-compiler-args -m32 --extra-linker-args -m32 --static-link-gccruntime disable;
	@#
	@$(MAKE) --silent MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) basic-unix-test-configurations_sanitizer_configs_;
	@#
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ./configure g++-optimized --only-if-has-compiler --config-tag Unix --apply-default-release-flags ${TEST_CONFIGURATIONS_ADD2ALL}
	@#
	@$(MAKE) --silent MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) basic-unix-test-configurations_valgrind_configs_


# Currently not used, but maybe test occasionally
private_compiler_versions_:
	# ./configure my-g++-8.3-debug-c++2a --config-tag Unix --compiler-driver /private-compiler-builds/gcc-8.3.0/bin/x86_64-pc-linux-gnu-gcc --apply-default-debug-flags --no-sanitize address --append-run-prefix 'LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:/private-compiler-builds/gcc-8.3.0/lib64' --only-if-has-compiler --cppstd-version c++2a

basic-unix-test-configurations_g++_versions_:
	# g++-11
	./configure g++-11-debug --config-tag Unix --compiler-driver g++-11 --apply-default-debug-flags --only-if-has-compiler --trace2file enable --cppstd-version c++20 ${TEST_CONFIGURATIONS_ADD2ALL}
	./configure g++-11-release++2b --config-tag Unix --compiler-driver g++-11 --apply-default-release-flags --only-if-has-compiler --trace2file enable --cppstd-version c++2b ${TEST_CONFIGURATIONS_ADD2ALL}
	# g++-12
	./configure g++-12-debug --config-tag Unix --compiler-driver g++-12 --apply-default-debug-flags --only-if-has-compiler --trace2file enable --cppstd-version c++20 ${TEST_CONFIGURATIONS_ADD2ALL}
	./configure g++-12-release++2b --config-tag Unix --compiler-driver g++-12 --apply-default-release-flags --only-if-has-compiler --trace2file enable --cppstd-version c++2b ${TEST_CONFIGURATIONS_ADD2ALL}
	# g++-13
	./configure g++-13-debug --config-tag Unix --compiler-driver g++-13 --apply-default-debug-flags --only-if-has-compiler --trace2file enable --cppstd-version c++20 ${TEST_CONFIGURATIONS_ADD2ALL}
	./configure g++-13-release++2b --config-tag Unix --compiler-driver g++-13 --apply-default-release-flags --only-if-has-compiler --trace2file enable --cppstd-version c++2b ${TEST_CONFIGURATIONS_ADD2ALL}
	# g++-14
	./configure g++-14-debug --config-tag Unix --compiler-driver g++-14 --apply-default-debug-flags --only-if-has-compiler --trace2file enable --cppstd-version c++20 ${TEST_CONFIGURATIONS_ADD2ALL}
	./configure g++-14-release++23 --config-tag Unix --compiler-driver g++-14 --apply-default-release-flags --only-if-has-compiler --trace2file enable --cppstd-version c++23 ${TEST_CONFIGURATIONS_ADD2ALL}

basic-unix-test-configurations_clang++_versions_:
	# clang-14
	./configure clang++-14-debug-libc++ --config-tag Unix --compiler-driver clang++-14 --apply-default-debug-flags --stdlib libc++ --only-if-has-compiler ${TEST_CONFIGURATIONS_ADD2ALL}
	./configure clang++-14-release-libstdc++ --config-tag Unix --compiler-driver clang++-14 --apply-default-release-flags --stdlib libstdc++ --only-if-has-compiler --trace2file enable ${TEST_CONFIGURATIONS_ADD2ALL}
	# clang-15
	./configure clang++-15-debug-libc++ --config-tag Unix --compiler-driver clang++-15 --apply-default-debug-flags --stdlib libc++ --only-if-has-compiler ${TEST_CONFIGURATIONS_ADD2ALL}
	./configure clang++-15-release-libstdc++ --config-tag Unix --compiler-driver clang++-15 --apply-default-release-flags --stdlib libstdc++ --only-if-has-compiler --trace2file enable ${TEST_CONFIGURATIONS_ADD2ALL}
	# clang-16
	./configure clang++-16-debug-libc++ --config-tag Unix --compiler-driver clang++-16 --apply-default-debug-flags --stdlib libc++ --only-if-has-compiler ${TEST_CONFIGURATIONS_ADD2ALL}
	./configure clang++-16-release-libstdc++ --config-tag Unix --compiler-driver clang++-16 --apply-default-release-flags --stdlib libstdc++ --only-if-has-compiler --trace2file enable ${TEST_CONFIGURATIONS_ADD2ALL}
	./configure clang++-16-debug-libstdc++-c++23 --config-tag Unix --compiler-driver clang++-16 --apply-default-debug-flags --stdlib libstdc++ --only-if-has-compiler --cppstd-version c++2b ${TEST_CONFIGURATIONS_ADD2ALL}
	./configure clang++-16-release-libc++23 --config-tag Unix --compiler-driver clang++-16 --apply-default-release-flags --stdlib libc++ --only-if-has-compiler --trace2file enable --cppstd-version c++2b ${TEST_CONFIGURATIONS_ADD2ALL}
	# clang-17
	./configure clang++-17-debug-libc++ --config-tag Unix --compiler-driver clang++-17 --apply-default-debug-flags --stdlib libc++ --only-if-has-compiler ${TEST_CONFIGURATIONS_ADD2ALL}
	./configure clang++-17-release-libstdc++ --config-tag Unix --compiler-driver clang++-17 --apply-default-release-flags --stdlib libstdc++ --only-if-has-compiler --trace2file enable ${TEST_CONFIGURATIONS_ADD2ALL}
	./configure clang++-17-debug-libstdc++-c++23 --config-tag Unix --compiler-driver clang++-17 --apply-default-debug-flags --stdlib libstdc++ --only-if-has-compiler --cppstd-version c++23 ${TEST_CONFIGURATIONS_ADD2ALL}
	./configure clang++-17-release-libc++23 --config-tag Unix --compiler-driver clang++-17 --apply-default-release-flags --stdlib libc++ --only-if-has-compiler --trace2file enable --cppstd-version c++23 ${TEST_CONFIGURATIONS_ADD2ALL}
	# clang-18
	./configure clang++-18-debug-libc++ --config-tag Unix --compiler-driver clang++-18 --apply-default-debug-flags --stdlib libc++ --only-if-has-compiler ${TEST_CONFIGURATIONS_ADD2ALL}
	./configure clang++-18-release-libstdc++ --config-tag Unix --compiler-driver clang++-18 --apply-default-release-flags --stdlib libstdc++ --only-if-has-compiler --trace2file enable ${TEST_CONFIGURATIONS_ADD2ALL}
	./configure clang++-18-debug-libstdc++-c++23 --config-tag Unix --compiler-driver clang++-18 --apply-default-debug-flags --stdlib libstdc++ --only-if-has-compiler --cppstd-version c++23 ${TEST_CONFIGURATIONS_ADD2ALL}
	./configure clang++-18-release-libc++23 --config-tag Unix --compiler-driver clang++-18 --apply-default-release-flags --stdlib libc++ --only-if-has-compiler --trace2file enable --cppstd-version c++23 ${TEST_CONFIGURATIONS_ADD2ALL}

basic-unix-test-configurations_sanitizer_configs_:
	# A few sanitize/configs (list explicit versions first as backup in case g++ doesn't work - enuf c++20 support - on this platform)
	# https://stackoverflow.com/questions/62109255/what-is-a-good-way-to-apply-multiple-sanitizers-in-cmake-with-gcc
	#
	# Note as of 2023-12-02, it appears memory sanitizer only works with clang++ (not gcc), and even that major
	# PITA to use - see https://github.com/google/sanitizers/wiki/MemorySanitizerLibcxxHowTo - must rebuild own libc++ specailly.
	# Note they do provide a dockerfile with all this setup, but still ... Not worth the trouble... --LGP 2023-12-02
	./configure g++-debug-sanitize_address_undefined_leak --config-tag Unix --only-if-has-compiler --apply-default-debug-flags --sanitize none,address,undefined,leak --trace2file enable --compiler-driver g++ ${TEST_CONFIGURATIONS_ADD2ALL}
	./configure g++-debug-sanitize_address_undefined_leak --config-tag Unix --only-if-has-compiler --apply-default-debug-flags --sanitize none,address,undefined,leak --trace2file enable ${TEST_CONFIGURATIONS_ADD2ALL}
	./configure g++-debug-sanitize_thread --config-tag Unix --only-if-has-compiler --apply-default-debug-flags --trace2file enable --cppstd-version c++20 --sanitize none,thread --compiler-driver g++ ${TEST_CONFIGURATIONS_ADD2ALL}
	./configure g++-debug-sanitize_thread --config-tag Unix --only-if-has-compiler --apply-default-debug-flags --trace2file enable --cppstd-version c++20 --sanitize none,thread ${TEST_CONFIGURATIONS_ADD2ALL}
	./configure g++-release-sanitize_address_undefined_leak --config-tag Unix --only-if-has-compiler --apply-default-release-flags --trace2file enable --cppstd-version c++20 --sanitize none,address,undefined,leak ${TEST_CONFIGURATIONS_ADD2ALL}
	./configure g++-release-sanitize_thread --config-tag Unix --only-if-has-compiler --apply-default-release-flags --trace2file enable --cppstd-version c++20 --sanitize none,thread ${TEST_CONFIGURATIONS_ADD2ALL}
	#https://stroika.atlassian.net/browse/STK-761
	if [[ `lsb_release -rs 2>/dev/null` == '20.04' || `lsb_release -rs 2>/dev/null` == '21.10' || `lsb_release -rs 2>/dev/null` == '22.04' ]] ; then ./configure g++-release-sanitize_address_undefined_leak --config-tag Unix --only-if-has-compiler --apply-default-release-flags --trace2file enable --cppstd-version c++20 --sanitize none,address,undefined,leak --lto disable ${TEST_CONFIGURATIONS_ADD2ALL}; fi;
	./configure g++-release-sanitize_thread --config-tag Unix --only-if-has-compiler --apply-default-release-flags --trace2file enable --cppstd-version c++20 --sanitize none,thread ${TEST_CONFIGURATIONS_ADD2ALL}

basic-unix-test-configurations_valgrind_configs_:
	# Builds with a few special flags to make valgrind work better
	# nb: using default installed C++ compiler cuz of matching installed libraries on host computer
	# Since Stroika v3.0d5 - disabling valgrind-debug-SSLPurify because it runs EXTREMELY SLOWLY, and has nearly no value (does work - eventually)
	# And Stroika v3.0d5 disables valgrind-debug-SSLPurify-NoBlockAlloc for the same speed reasons. Given sanitizers, very little value in memcheck and its so slow not worth it.
	#./configure valgrind-debug-SSLPurify --only-if-has-compiler --config-tag Unix --config-tag valgrind -valgrind enable --openssl use --openssl-extraargs purify --apply-default-debug-flags --trace2file enable --sanitize none ${TEST_CONFIGURATIONS_ADD2ALL}
	#./configure valgrind-debug-SSLPurify-NoBlockAlloc --only-if-has-compiler --config-tag Unix --config-tag valgrind -valgrind enable --openssl use --openssl-extraargs purify  --apply-default-debug-flags --trace2file enable --block-allocation disable --sanitize none ${TEST_CONFIGURATIONS_ADD2ALL}
	./configure valgrind-release-SSLPurify-NoBlockAlloc --only-if-has-compiler --config-tag Unix --config-tag valgrind --valgrind enable --openssl use --openssl-extraargs purify --apply-default-release-flags --trace2file disable --block-allocation disable ${TEST_CONFIGURATIONS_ADD2ALL}

raspberrypi-cross-compile-test-configurations:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Making raspberrypi-cross-compile-test-configurations:"
	@# gcc-11 ARM raspberrypi compiler
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ./configure raspberrypi-g++-11-release --config-tag Unix --config-tag raspberrypi --apply-default-release-flags --only-if-has-compiler  --compiler-driver 'arm-linux-gnueabihf-g++-11' --cross-compiling true ${TEST_CONFIGURATIONS_ADD2ALL}
	@#DUE TO qCompilerAndStdLib_arm_asan_FaultStackUseAfterScope_Buggy, omit raspberrypi-g++-11-release-sanitize_address since
	@#various attempts at supressions failed, and the suppressions DO work for g++-12 (and other cases work) - and appears to just be old compiler bug with asan stuff --LGP 2023-12-16
	@####MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ./configure raspberrypi-g++-11-release-sanitize_address --config-tag Unix --config-tag raspberrypi --apply-default-release-flags --only-if-has-compiler --trace2file enable --compiler-driver 'arm-linux-gnueabihf-g++-11' --sanitize none,address --cross-compiling true ${TEST_CONFIGURATIONS_ADD2ALL}
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ./configure raspberrypi-g++-11-debug-sanitize_undefined --config-tag Unix --config-tag raspberrypi --apply-default-debug-flags --only-if-has-compiler --trace2file enable --sanitize none,undefined --compiler-driver 'arm-linux-gnueabihf-g++-11' --cross-compiling true ${TEST_CONFIGURATIONS_ADD2ALL}
	@#
	@# gcc-12 ARM raspberrypi compiler
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ./configure raspberrypi-g++-12-release --config-tag Unix --config-tag raspberrypi --apply-default-release-flags --only-if-has-compiler  --compiler-driver 'arm-linux-gnueabihf-g++-12' --cross-compiling true ${TEST_CONFIGURATIONS_ADD2ALL}
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ./configure raspberrypi-g++-12-release-sanitize_address --config-tag Unix --config-tag raspberrypi --apply-default-release-flags --only-if-has-compiler --trace2file enable --compiler-driver 'arm-linux-gnueabihf-g++-12' --sanitize none,address --cross-compiling true ${TEST_CONFIGURATIONS_ADD2ALL}
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ./configure raspberrypi-g++-12-debug-sanitize_undefined --config-tag Unix --config-tag raspberrypi --apply-default-debug-flags --only-if-has-compiler --trace2file enable --sanitize none,undefined --compiler-driver 'arm-linux-gnueabihf-g++-12' --cross-compiling true ${TEST_CONFIGURATIONS_ADD2ALL}
	@#
	@# gcc-13 ARM raspberrypi compiler
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ./configure raspberrypi-g++-13-release --config-tag Unix --config-tag raspberrypi --apply-default-release-flags --only-if-has-compiler  --compiler-driver 'arm-linux-gnueabihf-g++-13' --cross-compiling true ${TEST_CONFIGURATIONS_ADD2ALL}
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ./configure raspberrypi-g++-13-release-sanitize_address --config-tag Unix --config-tag raspberrypi --apply-default-release-flags --only-if-has-compiler --trace2file enable --compiler-driver 'arm-linux-gnueabihf-g++-13' --sanitize none,address --cross-compiling true ${TEST_CONFIGURATIONS_ADD2ALL}
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ./configure raspberrypi-g++-13-debug-sanitize_undefined --config-tag Unix --config-tag raspberrypi --apply-default-debug-flags --only-if-has-compiler --trace2file enable --sanitize none,undefined --compiler-driver 'arm-linux-gnueabihf-g++-13' --cross-compiling true ${TEST_CONFIGURATIONS_ADD2ALL}


regression-test-configurations:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Making regression-test configurations:"
	@rm -f ConfigurationFiles/*
	@export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
	if [[ "$(DETECTED_HOST_OS)" = "Cygwin" || "$(DETECTED_HOST_OS)" = "MSYS" ]] ; then\
		$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) default-configurations;\
	else\
		$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) basic-unix-test-configurations;\
		$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) raspberrypi-cross-compile-test-configurations;\
	fi
	@# Currently not used, but maybe test occasionally
	@#(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) private_compiler_versions_


list-configurations:
	@$(ECHO) $(APPLY_CONFIGS)


list-configuration-tags:
	@ScriptsLib/GetConfigurationTags


install-realpath:
	mkdir -p /usr/local/bin
	g++ --std=c++20 -O -o /usr/local/bin/realpath BootstrapToolsSrc/realpath.cpp
