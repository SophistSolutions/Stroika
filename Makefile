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

.PHONY:	all libraries tools tests samples documentation third-party-components run-tests clean clobber library-clobber distclean apply-configuration-if-needed_ check-prerequisite-tools apply-configurations apply-configuration


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
APPLY_CONFIGS=$(or \
				$(CONFIGURATION), \
				$(if $(CONFIGURATION_TAGS), \
					$(shell $(StroikaDirectoryLayoutEnv) Build/Scripts/GetConfigurations --quiet --config-tags "$(CONFIGURATION_TAGS)"),\
					$(if $(filter clobber, $(MAKECMDGOALS)),\
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
	@$(ECHO) "    project-files:               -    Alias for project-files-visual-studio project-files-vs-code"
	@$(ECHO) "    project-files-vs-code:       -    Builds project files for visual studio code"
	@$(ECHO) "    project-files-visual-studio: -    Builds project files for visual studio.net"
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
	@$(ECHO) "    apply-configurations:        -    Force re-creation implied files / links for any configurations in the Configurations"
	@$(ECHO) "                                      folder (not needed, automatic)"
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
# Configuration.mk), with the configuration's preset (CMakeUserPresets.json, written as it is applied) - and its generator,
# printed as the build starts (configure's --cmake-generator: by default ninja with Visual Studio, else make). Each target
# as make's own: all, or a part - libraries, tools, tests, samples, documentation, third-party-components - with what it
# needs; -jN, N jobs at once (just -j, from make 3.81: one per processor). Without CONFIGURATION: each configuration make
# builds by default (or each TAGS= names). Until Stroika v3.0d26 make built Stroika itself - as Makefile-Legacy still
# does, until 3.0d26 ships.
#
BUILD_TARGETS_ = all libraries tools tests samples third-party-components

# the configuration's cmake, and the ctest beside it: with Visual Studio, Visual Studio's own; else those on PATH
FindCMake_ = cmake_="$$($(StroikaSourceRoot)Build/Scripts/GetConfigurationParameter $(CONFIGURATION) CMAKE)" &&\
	case "$$cmake_" in [A-Za-z]:/*) cmake_="$$(cygpath --unix "$$cmake_")";; esac &&\
	case "$$cmake_" in */*) ctest_="$$(dirname "$$cmake_")/ctest";; *) ctest_=ctest;; esac

# the jobs make was given (-jN), for cmake's build - a build of its own, given them as CMAKE_BUILD_PARALLEL_LEVEL, which
# its inner builds take too, under Ninja (CMakeLists.txt) - so not given this make's MAKEFLAGS: its jobserver, its -s and -k
FindJobs_ = jobs_="$$(echo " $$MAKEFLAGS" | sed -n 's/.* -j\([0-9][0-9]*\).*/\1/p')" &&\
	if [ -z "$$jobs_" ] && echo " $$MAKEFLAGS " | grep -q -e ' -j '; then jobs_="$$(getconf _NPROCESSORS_ONLN)"; fi

ifeq ($(CONFIGURATION),)
$(BUILD_TARGETS_):	$(StroikaIntermediateFilesRoot)PREREQUISITE_TOOLS_CHECKED_COMMON $(StroikaIntermediateFilesRoot)DEFAULT_PROJECT_FILES_BUILT $(StroikaIntermediateFilesRoot)ASSURE_DEFAULT_CONFIGURATIONS_BUILT
	@#first run all checks so any errors for missing tools appear ASAP
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL)  "Checking Prerequisites for Stroika:"
	@$(MAKE) --no-print-directory --silent apply-configurations-if-needed MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory --silent $(StroikaIntermediateFilesRoot)PREREQUISITE_TOOLS_CHECKED_COMMON $(StroikaIntermediateFilesRoot)$$i/TOOLS_CHECKED CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
	done
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory $@ CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
	done

run-tests:
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory run-tests CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
	done

clean clobber library-clobber:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika $(call FUNCTION_CAPITALIZE_WORD,$@):"
ifeq ($(CONFIGURATION_TAGS),)
	@#clobber, of every configuration: also what make built before Stroika v3.0d26 (its components' CURRENT ...)
	@if [ "$@" = "clobber" ] ; then \
		rm -rf $(StroikaBuildsRoot)*;\
		$(MAKE) --directory ThirdPartyComponents --no-print-directory clobber CONFIGURATION= MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
	fi
endif
	@for i in $(APPLY_CONFIGS) ; do\
		$(MAKE) --no-print-directory $@ CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
	done
else
$(BUILD_TARGETS_):	$(StroikaIntermediateFilesRoot)PREREQUISITE_TOOLS_CHECKED_COMMON $(StroikaIntermediateFilesRoot)DEFAULT_PROJECT_FILES_BUILT $(StroikaIntermediateFilesRoot)ASSURE_DEFAULT_CONFIGURATIONS_BUILT apply-configuration-if-needed_ $(StroikaIntermediateFilesRoot)$(CONFIGURATION)/TOOLS_CHECKED
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Stroika $@ {$(CONFIGURATION)}:"
	@# and its preset written if missing (the configuration applied by Stroika before v3.0d26, whose ApplyConfiguration wrote none)
	@grep -qF '"name": "$(CONFIGURATION)"' $(StroikaSourceRoot)CMakeUserPresets.json 2>/dev/null || $(MAKE) --no-print-directory apply-configuration
	@$(FindCMake_) && "$$cmake_" --preset $(CONFIGURATION)
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "CMake generator: $$(sed -n 's/^CMAKE_GENERATOR:INTERNAL=//p' $(StroikaIntermediateFilesRoot)$(CONFIGURATION)/CMake/CMakeCache.txt 2>/dev/null) (configure's --cmake-generator)"
	@$(FindCMake_) && $(FindJobs_) && env -u MAKEFLAGS -u MFLAGS $${jobs_:+CMAKE_BUILD_PARALLEL_LEVEL=$$jobs_} "$$cmake_" --build --preset $(CONFIGURATION) $(if $(filter-out all,$@),--target $@)
ifeq (${ECHO_TIMESTAMPS},1)
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "$(call FUNCTION_CAPITALIZE_WORD,$@)-Done: `date +%T`"
endif

run-tests:	tests
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Running Stroika's tests {$(CONFIGURATION)}:"
	@$(FindCMake_) &&\
	if ! "$$ctest_" --preset $(CONFIGURATION); then\
		if [ "$${TEST_FAILURES_CAUSE_FAILED_MAKE:-1}" -ne 0 ]; then exit 1; fi;\
		echo "   ***SOME TESTS FAILED*** (exit suppressed by TEST_FAILURES_CAUSE_FAILED_MAKE)";\
	fi
ifeq (${ECHO_TIMESTAMPS},1)
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) Run-Done: `date +%T`
endif

# library-clobber: Stroika's own build afresh next time - its libraries, and all built on them - the third-party
# components kept; clean: that, and the components' sources and build trees too - not what each installed, nor its stamps
# (so not built again: a component whose recipe then changes needs clobber), nor WIX's (its programs, which the
# installers run). Each keeps the libraries and programs already built, until built again. clobber: all of it.
clean library-clobber:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika $(call FUNCTION_CAPITALIZE_WORD,$@) {$(CONFIGURATION)}:"
	@rm -rf $(StroikaIntermediateFilesRoot)$(CONFIGURATION)/CMake/Stroika $(StroikaIntermediateFilesRoot)$(CONFIGURATION)/CMake/Stroika-prefix
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
	@$(if $(CONFIGURATION),$(FindCMake_),cmake_=cmake) && doxygen_="$$(command -v doxygen)";\
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
	@$(MAKE) --no-print-directory --directory=ThirdPartyComponents distclean
	@$(MAKE) --no-print-directory clobber MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@# last: the makes above need the build root; ConfigurationFiles/ is where the configurations were before Stroika v3.0d26
	@rm -rf $(patsubst %/,%,$(StroikaBuildsRoot) $(StroikaIntermediateFilesRoot)) $(TOP_ROOT)ConfigurationFiles

project-files:	$(StroikaIntermediateFilesRoot)ASSURE_DEFAULT_CONFIGURATIONS_BUILT
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Creating (common) default project files:"
	@$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) project-files-visual-studio project-files-vs-code

project-files-vs-code:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Creating default visual-studio-code configuration files:"
ifeq ($(DETECTED_HOST_OS),Darwin)
	@rsync --update .config-default.json .config.json
else
	@cp --update .config-default.json .config.json
endif
	@$(MAKE) --silent apply-configurations MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))

project-files-visual-studio:
	@$(StroikaSourceRoot)Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Creating default visual-studio project files:"
	@mkdir -p Workspaces/VisualStudio.Net
	@$(MAKE) --directory Tests --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) project-files
ifeq ($(DETECTED_HOST_OS),Darwin)
	@rsync --update Workspaces/VisualStudio.Net/Microsoft.Cpp.stroika.user-default.props Workspaces/VisualStudio.Net/Microsoft.Cpp.stroika.user.props
else
	@cp --update Workspaces/VisualStudio.Net/Microsoft.Cpp.stroika.user-default.props Workspaces/VisualStudio.Net/Microsoft.Cpp.stroika.user.props
endif

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
	@$(MAKE) --directory ThirdPartyComponents --no-print-directory check-prerequisite-tools
	@mkdir -p $(StroikaIntermediateFilesRoot)$(CONFIGURATION); touch $(StroikaIntermediateFilesRoot)$(CONFIGURATION)/TOOLS_CHECKED


$(StroikaIntermediateFilesRoot)PREREQUISITE_TOOLS_CHECKED_COMMON:
	@$(MAKE) check-prerequisite-tools-common --no-print-directory

$(StroikaIntermediateFilesRoot)$(CONFIGURATION)/TOOLS_CHECKED:
	@#Check BOTH the check-prerequisite-tools-common (redundantly) and check-prerequisite-tools-current-configuration
	@#because we could be using different versions of make (eg. cygwin vs. WSL) for different configurations
	@$(MAKE) --silent $(StroikaIntermediateFilesRoot)PREREQUISITE_TOOLS_CHECKED_COMMON check-prerequisite-tools-current-configuration --no-print-directory

$(StroikaIntermediateFilesRoot)DEFAULT_PROJECT_FILES_BUILT:
	@$(MAKE) --no-print-directory --silent $(StroikaIntermediateFilesRoot)ASSURE_DEFAULT_CONFIGURATIONS_BUILT
	@mkdir -p $(StroikaIntermediateFilesRoot); touch $(StroikaIntermediateFilesRoot)DEFAULT_PROJECT_FILES_BUILT


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
		$(MAKE) --no-print-directory reconfigure CONFIGURATION=$$i;\
	done
else
	@Build/Scripts/PrintProgressLine $(MAKE_INDENT_LEVEL) "Reconfiguring $(CONFIGURATION):"
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) && PATH=.:$$PATH && eval "$$(./Build/Scripts/GetConfigurationParameter $(CONFIGURATION) ConfigureCommandLine)"
endif



apply-configurations:
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
