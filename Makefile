StroikaSourceRoot	:=	$(abspath .)/

# before SharedMakeVariables-Default.mk, which would make it all - and stop a plain make (help) with no build root
.DEFAULT_GOAL := help

include Build/Lib/Make/SharedMakeVariables-Default.mk

ifeq ($(filter 3.81, $(firstword $(sort $(MAKE_VERSION) 3.81))),)
$(info Warning - version of GNU Make - $(MAKE_VERSION) - appears too old)
endif


#not parallel because submakefiles use parallelism, but generally best to sequence these top level requests. Like if you say
# make clobber all you don't want those to happen at the same time. And make libraries samples wouldn't really work since all the libraries
# have to be built before the samples etc...
.NOTPARALLEL:

.PHONY:	all libraries tools tests samples documentation third-party-components run-tests skel-test install clean clobber library-clobber distclean apply-configuration-if-needed_ check-prerequisite-tools apply-configurations apply-configuration


SHELL=/bin/bash

# Only applies to run-tests
export TEST_FAILURES_CAUSE_FAILED_MAKE?=1

#
# Often useful in looking at builds to see how long things took
#
ECHO_TIMESTAMPS?=1

#
# QUICK_BUILD=1 skips some steps you might occasionally want todo (done in regtests).
#
export QUICK_BUILD?=1

#Handy shortcut
CONFIGURATION_TAGS?=$(TAGS)

# use = not := cuz rarely used more than once, and commonly never, and when called without configs,needs
# re-evaluation (make default-configurations internally called - assure...)
# Without CONFIGURATION or TAGS: the configurations built by default - but for clobber and reconfigure every one, as what
# they fix (a stale compiler path, say) is in each
APPLY_CONFIGS=$(or \
				$(CONFIGURATION), \
				$(if $(CONFIGURATION_TAGS), \
					$(shell $(StroikaDirectoryLayoutEnv) Build/Scripts/GetConfigurations --quiet --config-tags "$(CONFIGURATION_TAGS)"),\
					$(if $(filter clobber reconfigure, $(MAKECMDGOALS)),\
						$(shell $(StroikaDirectoryLayoutEnv) Build/Scripts/GetConfigurations --quiet --all --quiet),\
						$(shell $(StroikaDirectoryLayoutEnv) Build/Scripts/GetConfigurations --quiet --all-default)\
					)\
				)\
			)

help:
	@$(ECHO) "Help for making Stroika:"
	@$(ECHO) "Targets:"
	@$(ECHO) "    all:                         -    Builds everything (with CMake: Stroika's CMakeLists.txt; make -f Makefile-Legacy for make's own build, until Stroika v3.0d26 ships)"
	@$(ECHO) "    clean:                       -    Removes what was built but not installed (objects, the third-party components' sources and build trees) - not the libraries and programs"
	@$(ECHO) "    clobber:                     -    Removes all that was built"
	@$(ECHO) "    distclean:"
	@$(ECHO) "    reconfigure:                 -    Rebuild configuration files from the command-lines that built them before"
	@$(ECHO) "    libraries:                   -    Builds Stroika foundation & frameworks, and any things it depends on (like third-party-components)"
	@$(ECHO) "    library-clobber:             -    Stroika's own build afresh next time - its libraries, and all built on them - the third-party components kept"
	@$(ECHO) "    tools:                       -    Builds the tools (HTMLViewCompiler)"
	@$(ECHO) "    tests:                       -    Builds the regression tests"
	@$(ECHO) "    format-code:                 -    Run Scripts/FormatCode (clang-format) on source code, and update it to conform to Stroika code formatting standards"
	@$(ECHO) "    samples:                     -    Builds the samples (and their installers)"
	@$(ECHO) "    documentation:               -    Builds the documentation (doxygen's), if doxygen is installed - needing no configuration"
	@$(ECHO) "    third-party-components:      -    Builds the configuration's third-party components"
	@$(ECHO) "    run-tests:                   -    Builds the regression tests, then runs them (ctest, one at a time) [REMOTE=] - eg. REMOTE=lewis@localhost;"
	@$(ECHO) "                                      [VALGRIND=memcheck, etc] to run with valgrind"
	@$(ECHO) "                                      (EXTRA_VALGRIND_OPTIONS= can be used with valgrind)"
	@$(ECHO) "                                      EG: VALGRIND_SUPPRESSIONS=\"Valgrind-MemCheck-Common.supp\" make VALGRIND=memcheck run-tests"
	@$(ECHO) "    skel-test:                   -    Builds the libraries, then a new Skel application on them - as a package manager's Stroika - and runs it"
	@$(ECHO) "                                      (CONFIGURATION= PREFIX=: on that configuration's installation there, make install's, instead)"
	@$(ECHO) "    install:                     -    Installs one configuration (CONFIGURATION=) in PREFIX=: its libraries, tools, headers and"
	@$(ECHO) "                                      CMake package, with its third-party components - needing neither the source tree nor the build"
	@$(ECHO) "    apply-configurations:        -    Writes what each configuration implies - its toolchain file, its CMake preset (which"
	@$(ECHO) "                                      Visual Studio's File > Open > Folder lists) and VS Code's settings - making the default"
	@$(ECHO) "                                      configurations first, if there are none. Run it before opening an IDE"
	@$(ECHO) "    default-configurations:      -    Creates the default configurations in Configurations folder; see ./configure -help for environment variables"
	@$(ECHO) "                                      e.g. EXTRA_CONFIGURE_ARGS='--openssl-extraargs purify --block-allocation disable' make default-configurations"
	@$(ECHO) "                                      OR EXTRA_CONFIGURE_ARGS='--build-platform VisualStudio.Net-2022' make default-configurations"
	@$(ECHO) "                                      OR PLATFORM='VisualStudio.Net-2022' make default-configurations"
	@$(ECHO) "    list-configurations:         -    prints all available configurations (each can be used as arg to CONFIGURATION= make lines)"
	@$(ECHO) "    list-configuration-tags:     -    prints a list of all configurtion tags (configuration tags impute groups of configurations)"
	@$(ECHO) "    check-prerequisite-tools:    -    Check the tools needed to build Stroika are installed."
	@$(ECHO) "Special Variables:               -    Extra params you can pass to the make line that may help..."
	@$(ECHO) "    CONFIGURATION=XYZ            -    Causes only the configuration XYZ to have the targetted goal applied (e.g. make all CONFIGURATION=Debug)"
	@$(ECHO) "    CONFIGURATION_TAGS=TAGS      -    Causes only the configurations with tags TAGS to have the targetted goal applied (e.g. make all CONFIGURATION_TAGS=Windows)"
	@$(ECHO) "                                      NB: TAGS is a handy shortcut for CONFIGURAITON_TAGS, so you can say make all TAGS=\"Windows 64\""
	@$(ECHO) "    MAKE_INDENT_LEVEL=0          -    Helpful to neaten formatting when multiple levels of makes calling Stroika make"
	@$(ECHO) "    ECHO_BUILD_LINES=1, WRITE_PREPROCESSOR_OUTPUT=1, QUICK_BUILD=1 - make's own build only (make -f Makefile-Legacy)"
	@$(ECHO) "    TEST_FAILURES_CAUSE_FAILED_MAKE=0"
	@$(ECHO) "                                      only applies to make run-tests, and prevents test failures from stopping make (like make -k on run-tests)"

	@$(ECHO) "    EXTRA_CONFIGURE_ARGS=        -    really proceeded by configure (only when its invoked); but handy to override values (eg --all-available-third-party-components --boost no)"



#
# The build is CMake's (#1157): Stroika's CMakeLists.txt, one build directory per configuration (CMake/, beside its
# Configuration.mk), with the configuration's preset (CMakeUserPresets.json, written as it is applied) - and its
# generator, printed as the build starts (configure's --cmake-generator: by default ninja with Visual Studio, else
# make). Configured each time, which builds the third-party components first. Each target as make's own: all, or a part
# (libraries, tools, tests, samples, documentation, third-party-components) with what it needs; -jN, N jobs at once
# (just -j, from make 3.81: one per processor), the third-party components' builds too. Without CONFIGURATION: each
# configuration make builds by default (or each TAGS= names). Until Stroika v3.0d26 make built Stroika itself - as
# Makefile-Legacy still does, until 3.0d26 ships.
#
BUILD_TARGETS_ = all libraries tools tests samples third-party-components

ifeq ($(CONFIGURATION),)
$(BUILD_TARGETS_):	$(StroikaIntermediateFilesRoot)PREREQUISITE_TOOLS_CHECKED_COMMON $(StroikaIntermediateFilesRoot)ASSURE_DEFAULT_CONFIGURATIONS_BUILT
	@#first run all checks so any errors for missing tools appear ASAP
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL)  "Checking Prerequisites for Stroika:"
	@$(MAKE) --no-print-directory --silent apply-configurations-if-needed MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory --silent $(StroikaIntermediateFilesRoot)PREREQUISITE_TOOLS_CHECKED_COMMON $(StroikaIntermediateFilesRoot)$$i/TOOLS_CHECKED CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
	done
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory $@ CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
	done

run-tests skel-test:
	@if [ "$@" = "skel-test" ] && [ -n "$(PREFIX)" ] ; then echo "make skel-test PREFIX=: say which configuration is installed there, CONFIGURATION=<name>"; exit 1; fi
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory $@ CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
	done

install:
	@echo "make install: say which configuration, CONFIGURATION=<name> - an installation holds one"; exit 1

clean clobber library-clobber:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika $(call FUNCTION_CAPITALIZE_WORD,$@):"
ifeq ($(CONFIGURATION_TAGS),)
	@#clobber, of every configuration: also what make's own build (Makefile-Legacy; before Stroika v3.0d26, make) left -
	@#its components' build trees, and their sources, extracted into the source tree (CURRENT, ORIGS; a part-done extraction)
	@if [ "$@" = "clobber" ] ; then \
		rm -rf $(StroikaBuildsRoot)* $(StroikaIntermediateFilesRoot)*/ThirdPartyComponents;\
		cd $(StroikaSourceRoot)ThirdPartyComponents && rm -rf */CURRENT */ORIGS */ORIGs Xerces/xerces-c-* sqlite/sqlite-amalgamation-*;\
	fi
endif
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory $@ CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
	done
else
$(BUILD_TARGETS_):	$(StroikaIntermediateFilesRoot)PREREQUISITE_TOOLS_CHECKED_COMMON $(StroikaIntermediateFilesRoot)ASSURE_DEFAULT_CONFIGURATIONS_BUILT apply-configuration-if-needed_ $(StroikaIntermediateFilesRoot)$(CONFIGURATION)/TOOLS_CHECKED
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Stroika $@ {$(CONFIGURATION)}:"
	@# and its preset written if missing (the configuration applied by Stroika before v3.0d26, whose ApplyConfiguration wrote none)
	@grep -qF '"name": "$(CONFIGURATION)"' $(StroikaSourceRoot)CMakeUserPresets.json 2>/dev/null || $(MAKE) --no-print-directory apply-configuration
	@$(StroikaFindCMake) && $(StroikaFindJobs) && env -u MAKEFLAGS -u MFLAGS $${jobs_:+CMAKE_BUILD_PARALLEL_LEVEL=$$jobs_} "$$cmake_" --preset $(CONFIGURATION) --log-level=NOTICE
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "CMake generator: $$(sed -n 's/^CMAKE_GENERATOR:INTERNAL=//p' $(StroikaIntermediateFilesRoot)$(CONFIGURATION)/CMake/CMakeCache.txt 2>/dev/null) (configure's --cmake-generator)"
	@$(StroikaFindCMake) && $(StroikaFindJobs) && env -u MAKEFLAGS -u MFLAGS $${jobs_:+CMAKE_BUILD_PARALLEL_LEVEL=$$jobs_} "$$cmake_" --build --preset $(CONFIGURATION) $(if $(filter-out all,$@),--target $@)
ifeq (${ECHO_TIMESTAMPS},1)
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "$(call FUNCTION_CAPITALIZE_WORD,$@)-Done: `date +%T`"
endif

run-tests:	tests
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Running Stroika's tests {$(CONFIGURATION)}:"
	@# each test's whole output, as RunTest reports it, a level in (PREFIX_) - which the regression runs' summaries count
	@# (Build/Scripts/RegressionTests): ctest's verbose output, less what ctest adds (Tests/Scripts/FilterCTestOutput)
	@set -o pipefail; $(StroikaFindCMake) &&\
	if ! PREFIX_="$$($(StroikaSourceRoot)Build/Scripts/PrintLevelLeader $$(($(MAKE_INDENT_LEVEL)+1)))" "$$ctest_" --preset $(CONFIGURATION) --verbose | $(StroikaSourceRoot)Tests/Scripts/FilterCTestOutput; then\
		if [ "$${TEST_FAILURES_CAUSE_FAILED_MAKE:-1}" -ne 0 ]; then exit 1; fi;\
		echo "   ***SOME TESTS FAILED*** (exit suppressed by TEST_FAILURES_CAUSE_FAILED_MAKE)";\
	fi
ifeq (${ECHO_TIMESTAMPS},1)
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) Run-Done: `date +%T`
endif

# A new Skel application, with no Stroika of its own (as a package manager would supply it), built on this one's package
# and run (unless cross-compiled): what an application sees of Stroika's CMake package, which Stroika's own tests and samples
# - part of its build - never read. Made afresh each time, in the configuration's IntermediateFiles; its build in CMake/App
# there, its program in the configuration's Builds directory (ApplyConfiguration --application-root). Given PREFIX=, built
# instead on the installation there (make install): its toolchain file and package - and its program in its build
# directory, as an installation has no Builds directory (StroikaConfig-Installed.cmake)
SkelTestProgram_ = $(if $(PREFIX),$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/CMake/App/SkelApp/SkelApp,$(StroikaBuildsRoot)$(CONFIGURATION)/SkelApp/SkelApp)
skel-test:	libraries
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building a Skel application on Stroika's $(if $(PREFIX),installation in $(PREFIX),package) {$(CONFIGURATION)}:"
	@rm -rf "$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/SkelApp" "$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/CMake/App"
	@$(StroikaSourceRoot)Build/Scripts/Skel --appRoot "$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/SkelApp" --stroikaSource none > /dev/null
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $(StroikaSourceRoot)Build/Scripts/ApplyConfiguration --application-root "$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/SkelApp" $(CONFIGURATION)
	@$(StroikaFindCMake) && $(StroikaFindJobs) && $(if $(PREFIX),$(StroikaFindPrefix) &&) cd "$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/SkelApp" &&\
		env -u MAKEFLAGS -u MFLAGS $${jobs_:+CMAKE_BUILD_PARALLEL_LEVEL=$$jobs_} "$$cmake_" --preset $(CONFIGURATION) --log-level=NOTICE\
			$(if $(PREFIX),"-DCMAKE_TOOLCHAIN_FILE=$$prefix_/lib/cmake/Stroika/Toolchain.cmake" "-DCMAKE_PREFIX_PATH=$$prefix_") &&\
		env -u MAKEFLAGS -u MFLAGS $${jobs_:+CMAKE_BUILD_PARALLEL_LEVEL=$$jobs_} "$$cmake_" --build --preset $(CONFIGURATION)
ifneq ($(CrossCompiling),true)
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "Running $(SkelTestProgram_)"
	@"$(SkelTestProgram_)"
endif

# The configuration installed in PREFIX (cmake --install; Library/CMakeLists.txt says what): an installation needing neither
# this source tree nor its build, as a package manager installs Stroika. A Visual Studio generator's build installs the
# build type its preset builds (ApplyConfiguration): Debug where assertions are on. A Windows cmake is given Windows paths.
install:	install-prefix-given_ libraries tools
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Installing Stroika {$(CONFIGURATION)} in $(PREFIX):"
	@$(StroikaFindCMake) && $(StroikaFindPrefix) &&\
	config_= &&\
	if grep -q '^CMAKE_CONFIGURATION_TYPES:' "$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/CMake/CMakeCache.txt"; then\
		case "$$($(StroikaSourceRoot)Build/Scripts/GetConfigurationParameter $(CONFIGURATION) AssertionsEnabled)" in 1|true) config_=Debug;; *) config_=Release;; esac;\
	fi &&\
	"$$cmake_" --install "$$(Path_ "$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/CMake")" --prefix "$$prefix_" $${config_:+--config $$config_}

# before building anything
install-prefix-given_:
	@if [ -z "$(PREFIX)" ] ; then echo "make install: say where, PREFIX=<directory>"; exit 1; fi

# library-clobber: Stroika's own build afresh next time - its libraries, and all built on them (an application's too) -
# the third-party components' build kept; clean: that, and the components' sources and build trees too - not what each
# installed, nor its stamps (so not built again: a component whose recipe then changes needs clobber), nor WIX's (its
# programs, which the installers run). Each keeps the libraries and programs already built, until built again. clobber:
# all of it.
clean library-clobber:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika $(call FUNCTION_CAPITALIZE_WORD,$@) {$(CONFIGURATION)}:"
	@if [ -d $(StroikaIntermediateFilesRoot)$(CONFIGURATION)/CMake ]; then\
		find $(StroikaIntermediateFilesRoot)$(CONFIGURATION)/CMake -mindepth 1 -maxdepth 1 ! -name ThirdParty -exec rm -rf {} +;\
	fi
	@if [ "$@" = "clean" ] ; then\
		for stamp_ in $(StroikaIntermediateFilesRoot)$(CONFIGURATION)/CMake/ThirdParty/*/stamp ; do\
			component_="$${stamp_%/stamp}";\
			if [ -d "$$stamp_" ] && [ "$${component_##*/}" != WIX ]; then rm -rf "$$component_/src" "$$component_/build"; fi;\
		done;\
	fi

clobber:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika $(call FUNCTION_CAPITALIZE_WORD,$@) {$(CONFIGURATION)}:"
	@rm -rf $(StroikaIntermediateFilesRoot)$(CONFIGURATION) ${StroikaPlatformTargetBuildDir}
endif

# documentation: doxygen's, one for all configurations (Builds/Stroika-Doxygen-Help.tar.gz), as a configuration's all
# builds it (Documentation/CMakeLists.txt) - here needing no configuration, nor its third-party components: cmake (the
# configuration's, given CONFIGURATION; else the one on PATH) runs Documentation/BuildDocumentation.cmake, with doxygen from
# PATH. A Windows cmake is given Windows paths (cygpath --mixed); one in /usr/bin - the POSIX layer's own - POSIX paths.
documentation:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Stroika documentation:"
	@$(if $(CONFIGURATION),$(StroikaFindCMake),cmake_=cmake) && doxygen_="$$(command -v doxygen)";\
	if [ -z "$$doxygen_" ]; then\
		$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "Documentation: not built, as no doxygen is installed";\
	else\
		case "$$(command -v "$$cmake_")" in /usr/bin/*) Path_ () { echo "$$1"; };; *) Path_ () { cygpath --mixed "$$1" 2>/dev/null || echo "$$1"; };; esac;\
		"$$cmake_" "-DDOXYGEN=$$(Path_ "$$doxygen_")" "-DVERSION=$$(head -1 $(StroikaSourceRoot)STROIKA_VERSION | tr -d '\r')"\
			"-DWORK_DIR=$$(Path_ $(StroikaIntermediateFilesRoot)Doxygen)" "-DOUTPUT=$$(Path_ $(StroikaBuildsRoot)Stroika-Doxygen-Help.tar.gz)"\
			-P "$$(Path_ $(StroikaSourceRoot)Documentation/BuildDocumentation.cmake)";\
	fi



distclean:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika $(call FUNCTION_CAPITALIZE_WORD,$@):"
ifneq ($(CONFIGURATION),)
	$(error "make distclean applies to all configurations - and deletes all configurations")
endif
	@# the components' downloaded archives too - not those in git
	@cd $(StroikaSourceRoot)ThirdPartyComponents/Origs-Cache && find . -type f ! -name .gitignore ! -name 'doxygen-1.8.2-setup.exe' ! -name 'doxygen-1.8.2-Windows-Installed.tar.gz' -exec rm -f {} +
	@$(MAKE) --no-print-directory clobber MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@# last: the makes above need the build root; ConfigurationFiles/ is where the configurations were before Stroika v3.0d26
	@rm -rf $(patsubst %/,%,$(StroikaBuildsRoot) $(StroikaIntermediateFilesRoot)) $(TOP_ROOT)ConfigurationFiles

# DEPRECATED (Stroika v3.0d26): make apply-configurations
project-files project-files-vs-code:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "make $@ is DEPRECATED (Stroika v3.0d26) - make apply-configurations:"
	@$(MAKE) --no-print-directory apply-configurations MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))


format-code:
	@$(ECHO) Formatting Code:
	@Build/Scripts/FormatCode Library '*.h' '*.inl' '*.cpp'
	@Build/Scripts/FormatCode Samples '*.h' '*.inl' '*.cpp'
	@Build/Scripts/FormatCode Tests '*.h' '*.inl' '*.cpp'
	@Build/Scripts/FormatCode Tools '*.h' '*.inl' '*.cpp'
	@Build/Scripts/FormatCode Build/Tools/Src '*.h' '*.inl' '*.cpp'




#
#check-prerequisite-tools
#	is broken into check-prerequisite-tools-common - which checks all stroika prerequisites, and
#	check-prerequisite-tools-current-configuration which checks a given argument CONFIGURATION.
#
#	We use take target files IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_COMMON and IntermediateFiles/$(CONFIGURATION)/TOOLS_CHECKED
#	to make sure each configuration is checked at least once before build, so we get easier to understand error messages
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
	@$(StroikaSourceRoot)Build/Scripts/CheckPrerequisiteTools $(MAKE_INDENT_LEVEL)
	@mkdir -p $(StroikaIntermediateFilesRoot); touch $(StroikaIntermediateFilesRoot)PREREQUISITE_TOOLS_CHECKED_COMMON


check-prerequisite-tools-current-configuration:
ifeq ($(CONFIGURATION),)
	$(error Cannot call check-prerequisite-tools-current-configuration without a configuration argument)
endif
	@$(MAKE) --no-print-directory apply-configuration-if-needed_
	@# the configuration's cmake, which builds it - and whose configure steps find the rest, or say what is missing
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "Stroika Check-Prerequisite-Tools {$(CONFIGURATION)}: cmake ... "
	@$(StroikaFindCMake) && if ! type "$$cmake_" > /dev/null 2>&1; then\
		echo; $(StroikaSourceRoot)Build/Scripts/GetMessageForMissingTool cmake; echo "    (the configuration's: $$cmake_)"; exit 1;\
	fi
	@$(ECHO) done
	@mkdir -p $(StroikaIntermediateFilesRoot)$(CONFIGURATION); touch $(StroikaIntermediateFilesRoot)$(CONFIGURATION)/TOOLS_CHECKED


$(StroikaIntermediateFilesRoot)PREREQUISITE_TOOLS_CHECKED_COMMON:
	@$(MAKE) check-prerequisite-tools-common --no-print-directory

$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/TOOLS_CHECKED:
	@#Check BOTH the check-prerequisite-tools-common (redundantly) and check-prerequisite-tools-current-configuration
	@#because we could be using different versions of make (eg. cygwin vs. WSL) for different configurations
	@$(MAKE) --silent $(StroikaIntermediateFilesRoot)PREREQUISITE_TOOLS_CHECKED_COMMON check-prerequisite-tools-current-configuration --no-print-directory


# DONT actually defaults if any configurations have already been created (but still mark as created)
$(StroikaIntermediateFilesRoot)ASSURE_DEFAULT_CONFIGURATIONS_BUILT:
ifeq ($(shell $(StroikaDirectoryLayoutEnv) Build/Scripts/GetConfigurations --quiet),)
	@$(MAKE) default-configurations --no-print-directory
endif
	@mkdir -p $(StroikaIntermediateFilesRoot); touch $(StroikaIntermediateFilesRoot)ASSURE_DEFAULT_CONFIGURATIONS_BUILT


apply-configuration-if-needed_:	$(StroikaIntermediateFilesRoot)ASSURE_DEFAULT_CONFIGURATIONS_BUILT
ifneq ($(CONFIGURATION),)
	@$(StroikaSourceRoot)Build/Scripts/CheckValidConfiguration $(CONFIGURATION)
	@$(MAKE) --no-print-directory --silent apply-configuration-if-stale_ .vscode/c_cpp_properties.json $(StroikaPlatformTargetBuildDir)include/Stroika/Current-Version.h
endif


#
# The configuration file contains a command-line used to generate it, and a bunch of variables
# used for the build process. This reconfigure target re-runs the configure program using the same
# arguments that were used to originally build the makefile
#
reconfigure:
ifeq ($(CONFIGURATION),)
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory reconfigure CONFIGURATION=$$i || exit $$?;\
	done
else
	@Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Reconfiguring $(CONFIGURATION):"
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && PATH=.:$$PATH && eval "$$(./Build/Scripts/GetConfigurationParameter $(CONFIGURATION) ConfigureCommandLine)"
endif



# Each configuration applied - the default ones made first, if there are none: what it implies written - its
# Configuration.mk, toolchain file and CMake preset (CMakeUserPresets.json, which an IDE reads: Visual Studio's
# File > Open > Folder lists the configurations it holds), and VS Code's settings (.config.json too, from
# .config-default.json if newer)
apply-configurations:	$(StroikaIntermediateFilesRoot)ASSURE_DEFAULT_CONFIGURATIONS_BUILT
ifeq ($(DETECTED_HOST_OS),Darwin)
	@rsync --update $(StroikaSourceRoot).config-default.json $(StroikaSourceRoot).config.json
else
	@cp --update $(StroikaSourceRoot).config-default.json $(StroikaSourceRoot).config.json
endif
	@# a make of its own, so the configurations it applies include any made just now
	@$(MAKE) --no-print-directory apply-configurations-each_

apply-configurations-each_:
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory apply-configuration CONFIGURATION=$$i;\
	done

apply-configurations-if-needed:
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory --silent apply-configuration-if-stale_ .vscode/c_cpp_properties.json $(StroikaBuildsRoot)$$i/include/Stroika/Current-Version.h CONFIGURATION=$$i;\
	done

ifneq ($(CONFIGURATION),)
#
# Re-runs configure when STROIKA_VERSION is newer than the configuration (each new version, it is a good idea to regenerate
# the configuration files), then ApplyConfiguration when the configuration is newer than its Configuration.mk, or its
# Toolchain.cmake is missing - or ApplyConfiguration itself is newer: what it writes (Stroika-Configuration.cmake, which the
# CMake build reads) may have changed.
# Tested here, not written as rules making those files: SharedMakeVariables-Default.mk includes Configuration.mk, and make
# first remakes an included makefile it has a rule for - with the recipe running this makefile again, which would do the same.
#
.PHONY: apply-configuration-if-stale_
apply-configuration-if-stale_:
	@if [ STROIKA_VERSION -nt "$(StroikaConfigurationFilesDir)$(CONFIGURATION).xml" ] ; then $(MAKE) reconfigure; fi
	@if [ "$(StroikaConfigurationFilesDir)$(CONFIGURATION).xml" -nt "$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Configuration.mk" ] || [ ! -f "$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Toolchain.cmake" ] || [ "$(StroikaSourceRoot)Build/Scripts/ApplyConfiguration" -nt "$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/Configuration.mk" ] ; then\
		$(MAKE) --no-print-directory apply-configuration;\
	fi
	
.vscode/c_cpp_properties.json:	 $(wildcard ${StroikaPlatformTargetBuildDir}lib/pkgconfig/*.pc)
	@$(StroikaSourceRoot)Build/Scripts/CheckValidConfiguration $(CONFIGURATION)
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Applying configuration {$(CONFIGURATION)}:"
	@mkdir -p "$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/"
	@$(StroikaSourceRoot)Build/Scripts/ApplyConfiguration $(CONFIGURATION) --only-vscode 
	@$(MAKE) --no-print-directory --silent $(StroikaPlatformTargetBuildDir)include/Stroika/Current-Version.h

$(StroikaPlatformTargetBuildDir)include/Stroika/Current-Version.h:	 STROIKA_VERSION Build/Scripts/MakeVersionFile
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) -n "Writing \"$(shell $(StroikaSourceRoot)Build/Scripts/SubstituteBackVariables $(StroikaPlatformTargetBuildDir)include/Stroika/Current-Version.h)\" ... "
	@mkdir -p $(StroikaPlatformTargetBuildDir)include/Stroika/
	@$(StroikaSourceRoot)Build/Scripts/MakeVersionFile STROIKA_VERSION $(StroikaPlatformTargetBuildDir)include/Stroika/Current-Version.h StroikaLibVersion
	@echo "done"
endif


apply-configuration:
ifeq ($(CONFIGURATION),)
	$(error Cannot call apply-configuration without a configuration argument)
endif
	@$(StroikaSourceRoot)Build/Scripts/CheckValidConfiguration $(CONFIGURATION)
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Applying configuration {$(CONFIGURATION)}:"
	@mkdir -p "$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/"
	@$(StroikaSourceRoot)Build/Scripts/ApplyConfiguration $(CONFIGURATION)
	@$(MAKE) --no-print-directory --silent $(StroikaPlatformTargetBuildDir)include/Stroika/Current-Version.h


# EXTRA_CONFIGURE_ARGS is added to the front of every configure command line. Nothing to declare here:
# configure reads it out of the environment itself, and make passes both environment and command-line
# variables into a recipe's environment. Use occasionally to generate quick debug builds etc, e.g.
#	EXTRA_CONFIGURE_ARGS="--boost no --openssl no" make default-configurations
#	EXTRA_CONFIGURE_ARGS='--all-available-third-party-components' make default-configurations
#	EXTRA_CONFIGURE_ARGS='--no-third-party-components' make default-configurations

default-configurations:
	@$(StroikaSourceRoot)Build/Scripts/MakeDefaultConfigurations


# n.b. basic-unix-test-configurations and raspberrypi-cross-compile-test-configurations are also
# used on their own - Build/Scripts/RegressionTests can be pointed at an individual set of
# configurations via BUILD_CONFIGURATIONS_MAKEFILE_TARGET (see Documentation/Regression-Tests.md)
regression-test-configurations:
	@$(StroikaSourceRoot)Build/Scripts/MakeRegressionTestConfigurations

basic-unix-test-configurations:
	@$(StroikaSourceRoot)Build/Scripts/MakeRegressionTestConfigurations --basic-unix-only

raspberrypi-cross-compile-test-configurations:
	@$(StroikaSourceRoot)Build/Scripts/MakeRegressionTestConfigurations --raspberrypi-only


list-configurations:
	@$(ECHO) $(APPLY_CONFIGS)


list-configuration-tags:
	@Build/Scripts/GetConfigurationTags


install-realpath:
	mkdir -p /usr/local/bin
	g++ --std=c++20 -O -o /usr/local/bin/realpath Build/Tools/Src/realpath.cpp
