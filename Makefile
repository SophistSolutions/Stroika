.NOTPARALLEL:
.PHONY:	tests documentation all check clobber libraries assure-default-configurations apply-configuration-if-needed_
.FORCE:	check-prerequisite-tools
.FORCE:	apply-configurations


MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO?=	$(shell ScriptsLib/GetDefaultShellVariable.sh ECHO)
ECHO_BUILD_LINES?=0


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
	@$(ECHO) "    project-files-qt-creator(*): -    Builds project project-files-qt-creator (also project-files-qt-creator-load project-files-qt-creator-save)"
	@$(ECHO) "    tests:"
	@$(ECHO) "    format-code:                 -    Run astyle on source code, and update it to conform to Stroika code formatting standards"
	@$(ECHO) "    samples:"
	@$(ECHO) "    documentation:"
	@$(ECHO) "    third-party-components:"
	@$(ECHO) "    run-tests:                   -    [REMOTE=] - eg. REMOTE=lewis@localhost;"
	@$(ECHO) "                                      [VALGRIND=memcheck or helgrind, etc] to run with valgrind (EXTRA_VALGRIND_OPTIONS= can be used with valgrind)"
	@$(ECHO) "                                      OR VALGRIND_SUPPRESSIONS=\"Valgrind-MemCheck-Common.supp Valgrind-MemCheck-BlockAllocation.supp\""
	@$(ECHO) "                                      OR VALGRIND_SUPPRESSIONS=\"Valgrind-Helgrind-Common.supp\""
	@$(ECHO) "                                      EG: VALGRIND_SUPPRESSIONS=\"Valgrind-Helgrind-Common.supp\" make VALGRIND=helgrind run-tests"
	@$(ECHO) "    apply-configurations:        -    Force re-creation implied files / links for any configurations in the Configurations folder (not needed, automatic)"
	@$(ECHO) "    default-configurations:      -    Creates the default configurations in Configurations folder; [DEFAULT_CONFIGURATION_ARGS]')"
	@$(ECHO) "                                      e.g. DEFAULT_CONFIGURATION_ARGS=--help OR"
	@$(ECHO) "                                      DEFAULT_CONFIGURATION_ARGS='--openssl-extraargs purify --block-allocation disable'"
	@$(ECHO) "    check-prerequisite-tools:    -    Check the tools needed to build Stroika are installed."
	@$(ECHO) "Special Variables:               -    Extra params you can pass to the make line that may help..."
	@$(ECHO) "    ECHO_BUILD_LINES=1           -    Causes make lines to be echoed which can help makefile debugging"
	@$(ECHO) "    MAKE_INDENT_LEVEL=0          -    Helpful to neaten formatting when multiple levels of makes calling Stroika make"


ifeq ($(CONFIGURATION),)
all:		IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_ALL assure-default-configurations-exist_
	@#first run all checks so any errors for missing tools appear ASAP
	@ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Checking Prerequisites for Stroika/All:"
	@$(MAKE) --no-print-directory --silent apply-configurations-if-needed MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES)
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory --silent IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_ALL IntermediateFiles/$$i/TOOLS_CHECKED CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES) || exit $$?;\
	done
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Making Stroika/All {$$i}:";\
		$(MAKE) --no-print-directory all CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES) || exit $$?;\
	done
else
all:		IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_ALL assure-default-configurations-exist_ libraries tools samples tests documentation check
endif


check: assure-default-configurations-exist_
ifeq ($(CONFIGURATION),)
ifeq ($(MAKECMDGOALS),check)
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Stroika/Check {$$i}:";\
		$(MAKE) --no-print-directory check CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
	done
endif
else
	@ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Checking Stroika {$(CONFIGURATION)}:"
	@$(MAKE) --directory ThirdPartyComponents --no-print-directory check CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Library --no-print-directory check CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Tools --no-print-directory check CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Tests --no-print-directory check CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
	@$(MAKE) --directory Samples --no-print-directory check CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
endif


clean:
ifeq ($(CONFIGURATION),)
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory clean CONFIGURATION=$$i;\
	done
else
	@$(MAKE) --directory ThirdPartyComponents --no-print-directory clean CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
	@$(MAKE) --directory Library --no-print-directory clean CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
	@$(MAKE) --directory Tools --no-print-directory clean CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
	@$(MAKE) --directory Tests --no-print-directory clean CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
	@$(MAKE) --directory Samples --no-print-directory clean CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
endif


clobber:
ifeq ($(CONFIGURATION),)
	@ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Stroika Clobber..."
	@rm -rf IntermediateFiles/*
	@rm -rf Builds/*
	@$(MAKE) --directory ThirdPartyComponents --no-print-directory clobber CONFIGURATION= MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))
else
	@ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Stroika Clobber {$(CONFIGURATION)}..."
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
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Making Stroika/Libraries {$$i}:";\
		$(MAKE) --no-print-directory libraries CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES) || exit $$?;\
	done
else
libraries:	IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_ALL assure-default-configurations-exist_ IntermediateFiles/$(CONFIGURATION)/TOOLS_CHECKED third-party-components
	@$(MAKE) --directory Library --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES)
endif



ifeq ($(CONFIGURATION),)
third-party-components:	IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_ALL assure-default-configurations-exist_
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Making Stroika/Third-party-components {$$i}:";\
		$(MAKE) --no-print-directory third-party-components CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES) || exit $$?;\
	done
else
third-party-components:	IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_ALL assure-default-configurations-exist_ apply-configuration-if-needed_ IntermediateFiles/$(CONFIGURATION)/TOOLS_CHECKED
	@$(MAKE) --directory ThirdPartyComponents --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES)
endif


project-files:	project-files-visual-studio project-files-qt-creator


project-files-visual-studio:
	@$(MAKE) --directory Tests --no-print-directory project-files


project-files-qt-creator:	project-files-qt-creator-load


project-files-qt-creator-load:
	@$(ECHO) -n "Loading qt-creator project files..."
	@for i in StroikaDevRoot.config StroikaDevRoot.creator StroikaDevRoot.files StroikaDevRoot.includes; do cp Library/Projects/QtCreator/$$i .; done;
	@$(ECHO) "done"


project-files-qt-creator-save:
	@$(ECHO) -n "Saving qt-creator project files..."
	@for i in StroikaDevRoot.config StroikaDevRoot.creator StroikaDevRoot.files StroikaDevRoot.includes; do cp $$i Library/Projects/QtCreator/ ; done;
	@$(ECHO) "done"


ifeq ($(CONFIGURATION),)
tools:	assure-default-configurations-exist_
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Making Stroika/Tools {$$i}:";\
		$(MAKE) tools --no-print-directory CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES) || exit $$?;\
	done
else
tools:	assure-default-configurations-exist_ libraries
	@$(MAKE) --directory Tools --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES)
endif


ifeq ($(CONFIGURATION),)
tests:	assure-default-configurations-exist_
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Making Stroika/Tests {$$i}:";\
		$(MAKE) tests --no-print-directory CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES) || exit $$?;\
	done
else
tests:	assure-default-configurations-exist_ libraries
	@$(MAKE) --directory Tests --no-print-directory tests CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES)
endif


ifeq ($(CONFIGURATION),)
samples:	assure-default-configurations-exist_
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Making Stroika/Samples {$$i}:";\
		$(MAKE) samples --no-print-directory CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES) || exit $$?;\
	done
else
samples:	assure-default-configurations-exist_ libraries
	@$(MAKE) --directory Samples --no-print-directory samples CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES)
endif


ifeq ($(CONFIGURATION),)
run-tests:
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Stroika/Run-All-Tests {$$i}:";\
		$(MAKE) --no-print-directory run-tests CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES) || exit $$?;\
	done
else
run-tests:	tests
	@$(MAKE) --directory Tests --no-print-directory run-tests CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES)
endif



format-code:
	@$(ECHO) Running Astyle...
	@for i in Library Samples Tests Tools;\
	do\
		ScriptsLib/FormatCode.sh "$$i/*.h" "$$i/*.cpp" "$$i/*.inl";\
	done




#
#check-prerequisite-tools
#	is broken into check-prerequisite-tools-common - which checks all stroika prerequisites, and
#	check-prerequisite-tools-current-configuration which checks a given argument CONFIGURATION.
#
#	We use take target files IntermediateFiles/PREREQUISITE_TOOLS_CHECKED_ALL and IntermediateFiles/$(CONFIGURATION)/TOOLS_CHECKED
#	to make sure each configuraiton is checked at least once before build, so we get easier to understand error messages
#	(tool x missing instead of xxx failed)
#
check-prerequisite-tools:
	@$(MAKE) --no-print-directory check-prerequisite-tools-common CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES)
ifeq ($(CONFIGURATION),)
	@#no need to run apply-configurations-if-needed, but  looks neater in output if we run first, and then do checks
	@$(MAKE) --no-print-directory apply-configurations-if-needed MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES)
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory check-prerequisite-tools-current-configuration CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES) || exit $$?;\
	done
else
	@$(MAKE) --no-print-directory check-prerequisite-tools-current-configuration CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES)
endif


check-prerequisite-tools-common:
	@ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Checking for installed tools:"
	@ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && sh -c "(type sed 2> /dev/null) || (echo 'Missing sed' && exit 1)"
	@ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && sh -c "(type wget 2> /dev/null) || (echo 'Missing wget' && exit 1)"
	@ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && sh -c "(type perl 2> /dev/null) || (echo 'Missing perl' && exit 1)"
	@ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && sh -c "(type tar 2> /dev/null) || (echo 'Missing tar' && exit 1)"
	@ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && sh -c "(type patch 2> /dev/null) || (echo 'Missing patch' && exit 1)"
	@ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && sh -c "(type realpath 2> /dev/null) || (echo 'Missing realpath' && exit 1)"
ifneq (,$(findstring CYGWIN,$(shell uname)))
	@ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && sh -c "(type dos2unix 2> /dev/null) || (echo 'Missing dos2unix' && exit 1)"
	@ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && sh -c "(type unix2dos 2> /dev/null) || (echo 'Missing unix2dos' && exit 1)"
endif
	@ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && $(ECHO) "All Required-Always Tools Present"
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
ifeq ($(shell ScriptsLib/GetConfigurations.sh),)
	@$(MAKE) default-configurations --no-print-directory MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
endif


apply-configuration-if-needed_:	assure-default-configurations-exist_
ifneq ($(CONFIGURATION),)
	@$(MAKE) --no-print-directory --silent IntermediateFiles/$(CONFIGURATION)/APPLIED_CONFIGURATION CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);
endif

apply-configurations:
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory apply-configuration CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done

apply-configurations-if-needed:
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory --silent IntermediateFiles/$$i/APPLIED_CONFIGURATION CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done


IntermediateFiles/$(CONFIGURATION)/APPLIED_CONFIGURATION:	ConfigurationFiles/$(CONFIGURATION).xml
	@$(MAKE) --no-print-directory apply-configuration CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
	

apply-configuration:
ifeq ($(CONFIGURATION),)
	$(error Cannot call apply-configuration without a configuration argument)
endif
	@ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Applying configuration {$(CONFIGURATION)}:"
	@mkdir -p "IntermediateFiles/$(CONFIGURATION)/"
	@perl ScriptsLib/ApplyConfiguration.pl $(CONFIGURATION)
	@ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && $(ECHO) "Writing \"IntermediateFiles/$(CONFIGURATION)/Stroika-Current-Version.h\""
	@ScriptsLib/MakeVersionFile.sh STROIKA_VERSION IntermediateFiles/$(CONFIGURATION)/Stroika-Current-Version.h StroikaLibVersion
	@touch IntermediateFiles/$(CONFIGURATION)/APPLIED_CONFIGURATION


default-configurations:
	@ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) Making default configurations...
	@export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
	if [ `uname -o` = "Cygwin" ] ; then\
		./configure Debug-U-32 --apply-default-debug-flags --trace2file disable $(DEFAULT_CONFIGURATION_ARGS);\
		./configure Debug-U-64 --apply-default-debug-flags --trace2file disable $(DEFAULT_CONFIGURATION_ARGS);\
		./configure Release-DbgMemLeaks-U-32 --apply-default-release-flags $(DEFAULT_CONFIGURATION_ARGS);\
		./configure Release-Logging-U-32 --apply-default-release-flags --trace2file enable $(DEFAULT_CONFIGURATION_ARGS);\
		./configure Release-Logging-U-64 --apply-default-release-flags --trace2file enable $(DEFAULT_CONFIGURATION_ARGS);\
		./configure Release-U-32 --apply-default-release-flags $(DEFAULT_CONFIGURATION_ARGS);\
		./configure Release-U-64 --apply-default-release-flags $(DEFAULT_CONFIGURATION_ARGS);\
	else\
		#./configure DefaultConfiguration $(DEFAULT_CONFIGURATION_ARGS);\
		./configure Debug --apply-default-debug-flags $(DEFAULT_CONFIGURATION_ARGS);\
		./configure Release --apply-default-release-flags $(DEFAULT_CONFIGURATION_ARGS);\
	fi
	

regression-test-configurations:
	@ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) Making regression-test configurations...
	@rm -f ConfigurationFiles/*
	@export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1));\
	if [ `uname -o` = "Cygwin" ] ; then\
		./configure Debug-U-32 --apply-default-debug-flags --trace2file disable;\
		./configure Debug-U-64 --apply-default-debug-flags --trace2file disable;\
		./configure Release-DbgMemLeaks-U-32 --apply-default-release-flags;\
		./configure Release-Logging-U-32 --apply-default-release-flags --trace2file enable;\
		./configure Release-Logging-U-64 --apply-default-release-flags --trace2file enable;\
		./configure Release-U-32 --apply-default-release-flags;\
		./configure Release-U-64 --apply-default-release-flags;\
	else\
		./configure DEFAULT_CONFIG;\
		./configure no-third-party-components --LibCurl no --lzma no --zlib no --OpenSSL no --sqlite no --Xerces no;\
		./configure malloc-guard --malloc-guard true;\
		#\
		#./configure gcc-4.9-release --compiler-driver g++-4.9 --apply-default-release-flags --only-if-has-compiler --trace2file enable;\
		#\
		./configure my-gcc-5.3.0-release --compiler-driver ~/gcc-5.3.0/bin/x86_64-unknown-linux-gnu-gcc --apply-default-release-flags --only-if-has-compiler --trace2file enable;\
		#./configure my-gcc-5.3.0-debug-c++17 --compiler-driver ~/gcc-5.3.0/bin/x86_64-unknown-linux-gnu-gcc --apply-default-debug-flags --only-if-has-compiler --trace2file enable --cppstd-version-flag --std=c++1z;\
		./configure my-gcc-6.1.0-debug-c++17 --compiler-driver ~/gcc-6.1.0/bin/x86_64-pc-linux-gnu-gcc --apply-default-debug-flags --only-if-has-compiler --trace2file enable --cppstd-version-flag --std=c++17;\
		./configure my-gcc-6.1.0-release-c++17 --compiler-driver ~/gcc-6.1.0/bin/x86_64-pc-linux-gnu-gcc --apply-default-release-flags --only-if-has-compiler --cppstd-version-flag --std=c++17;\
		#\
		#LTO not working for my private builds of clang- no matter\
		./configure my-clang-3.7.1-release --compiler-driver ~/clang-3.7.1/bin/clang++ --apply-default-release-flags --only-if-has-compiler --lto disable --cppstd-version-flag --std=c++1y;\
		#./configure clang++-3.7-debug --compiler-driver clang++-3.7 --apply-default-debug-flags --only-if-has-compiler --trace2file enable --cppstd-version-flag --std=c++1y;\
		./configure my-clang-3.8.1-release --compiler-driver ~/clang-3.8.1/bin/clang++ --apply-default-release-flags --only-if-has-compiler --lto disable --cppstd-version-flag --std=c++1y;\
		./configure clang-3.8-debug --compiler-driver clang++-3.8 --apply-default-debug-flags --only-if-has-compiler --trace2file enable --cppstd-version-flag --std=c++1y;\
		#\
		#32-bit not working now - asm bug - must debug...\
		#./configure gcc-release-32 --compiler-driver "gcc -m32" --trace2file enable --assertions enable --only-if-has-compiler --LibCurl no --OpenSSL no --Xerces no --zlib no --lzma no --extra-compiler-args -m32 --extra-linker-args  -m32 --static-link-gccruntime disable;\
		#\
		# maybe add these directly to config\
		# NB: muse use noblockalloc for gcc-debug-threadsanitize because we need to annoate and thats NYI - so sanitizer knows about free\
		# see https://stroika.atlassian.net/browse/STK-500 for why  --no-sanitize vptr\
		#OR maybe I have a bug with the stream/iostream convert code - wait a bit and test more later--LGP 2016-08-17\
		./configure gcc-debug-sanitize --apply-default-debug-flags --only-if-has-compiler --trace2file enable --cppstd-version-flag --std=c++17 --sanitize address,undefined --no-sanitize vptr;\
		./configure gcc-release-sanitize --apply-default-release-flags --only-if-has-compiler --trace2file enable --cppstd-version-flag --std=c++17 --sanitize address,undefined --no-sanitize vptr;\
		#./configure gcc-debug-threadsanitize --apply-default-debug-flags --only-if-has-compiler --trace2file enable --cppstd-version-flag --std=c++17 --block-allocation disable --extra-compiler-args -fsanitize=thread --extra-linker-args -fsanitize=thread;\
		#\
		###Builds with a few specail flags to make valgrind work better\
		./configure VALGRIND_LatestGCC_Dbg_SSLPurify --compiler-driver ~/gcc-6.1.0/bin/x86_64-pc-linux-gnu-gcc --cppstd-version-flag --std=c++17 --only-if-has-compiler --valgrind enable --openssl use --openssl-extraargs purify --apply-default-debug-flags --trace2file enable;\
		./configure VALGRIND_LatestGCC_Release_SSLPurify_NoBlockAlloc --compiler-driver ~/gcc-6.1.0/bin/x86_64-pc-linux-gnu-gcc --cppstd-version-flag --std=c++17 --only-if-has-compiler --valgrind enable --openssl use --openssl-extraargs purify  --apply-default-release-flags --lto disable --trace2file disable --block-allocation disable;\
		#\
		./configure raspberrypi-gcc-5 --apply-default-debug-flags --only-if-has-compiler --trace2file enable --compiler-driver 'arm-linux-gnueabihf-g++-5' --cross-compiling true;\
		# see https://stroika.atlassian.net/browse/STK-500 for why  --no-sanitize vptr\
		./configure raspberrypi-gcc-sanitize --apply-default-debug-flags --only-if-has-compiler --trace2file enable --sanitize address,undefined --no-sanitize vptr --compiler-driver 'arm-linux-gnueabihf-g++-5' --cross-compiling true;\
		./configure raspberrypi_valgrind_gcc-5_NoBlockAlloc --apply-default-release-flags --only-if-has-compiler --trace2file disable --compiler-driver 'arm-linux-gnueabihf-g++-5' --valgrind enable --block-allocation disable --cross-compiling true;\
	fi
