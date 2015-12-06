.NOTPARALLEL:
.PHONY:	tests documentation all check clobber libraries assure-default-configurations apply-configuration-if-needed_
.FORCE:	check-tools
.FORCE:	apply-configurations


MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO?=	$(shell ScriptsLib/GetDefaultShellVariable.sh ECHO)


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
	@$(ECHO) "    run-tests:                   -    [REMOTE=] - eg. REMOTE=lewis@localhost; [VALGRIND=1] to run with valgrind (EXTRA_VALGRIND_OPTIONS= can be used with valgrind)"
	@$(ECHO) "    apply-configurations:        -    Create implied files / links for any configurations in the Configurations folder (forces a rebuild of configs) - not neeeded - automatic"
	@$(ECHO) "    default-configurations:      -    Creates the default configurations in Configurations folder; [DEFAULT_CONFIGURATION_ARGS=--help])"
	@$(ECHO) "    check-tools:                 -    Check the tools needed to build Stroika are installed."
	@$(ECHO) "Special Variables:               -    Extra params you can pass to the make line that may help..."
	@$(ECHO) "    ECHO_BUILD_LINES=1           -    Causes make lines to be echoed which can help makefile debugging"


all:		IntermediateFiles/TOOLS_CHECKED assure-default-configurations-exist_
ifeq ($(CONFIGURATION),)
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory all CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done
else
	@$(MAKE) --no-print-directory libraries CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
	@$(MAKE) --no-print-directory tools CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
	@$(MAKE) --no-print-directory samples CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
	@$(MAKE) --no-print-directory tests CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
	@$(MAKE) --no-print-directory documentation CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
	@$(MAKE) --no-print-directory check CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
endif


check: assure-default-configurations-exist_
ifeq ($(CONFIGURATION),)
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory check CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done
else
	@ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Checking Stroika {$(CONFIGURATION)}:"
	@$(MAKE) --directory ThirdPartyComponents --no-print-directory check CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) MAKEFLAGS=
	@$(MAKE) --directory Library --no-print-directory check CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) MAKEFLAGS=
	@$(MAKE) --directory Tools --no-print-directory check CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) MAKEFLAGS=
	@$(MAKE) --directory Tests --no-print-directory check CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) MAKEFLAGS=
	@$(MAKE) --directory Samples --no-print-directory check CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) MAKEFLAGS=
endif


clean:
ifeq ($(CONFIGURATION),)
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory clean CONFIGURATION=$$i;\
	done
else
	@$(MAKE) --directory ThirdPartyComponents --no-print-directory clean CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) MAKEFLAGS=
	@$(MAKE) --directory Library --no-print-directory clean CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) MAKEFLAGS=
	@$(MAKE) --directory Tools --no-print-directory clean CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) MAKEFLAGS=
	@$(MAKE) --directory Tests --no-print-directory clean CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) MAKEFLAGS=
	@$(MAKE) --directory Samples --no-print-directory clean CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) MAKEFLAGS=
endif


clobber:
	@ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Stroika Clobber..."
	@rm -rf IntermediateFiles/*
	@rm -rf Builds/*
	@#OK if no configuration or given configuration (handled in submake file)
	@$(MAKE) --directory ThirdPartyComponents --no-print-directory clobber CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)


documentation:
	@$(MAKE) --directory Documentation --no-print-directory all MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)


libraries:	IntermediateFiles/TOOLS_CHECKED assure-default-configurations-exist_
ifeq ($(CONFIGURATION),)
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory libraries CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done
else
	@$(MAKE) --no-print-directory apply-configuration-if-needed_ CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
	@$(MAKE) --directory ThirdPartyComponents --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
	@$(MAKE) --directory Library --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
endif



third-party-components:	IntermediateFiles/TOOLS_CHECKED assure-default-configurations-exist_
ifeq ($(CONFIGURATION),)
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory third-party-components CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done
else
	@$(MAKE) --no-print-directory apply-configuration-if-needed_ CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
	@$(MAKE) --directory ThirdPartyComponents --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
endif


project-files:	project-files-visual-studio project-files-qt-creator


project-files-visual-studio:
	@$(MAKE) --directory Tests --no-print-directory MAKEFLAGS= project-files


project-files-qt-creator:	project-files-qt-creator-load


project-files-qt-creator-load:
	@$(ECHO) -n "Loading qt-creator project files..."
	@for i in StroikaDevRoot.config StroikaDevRoot.creator StroikaDevRoot.files StroikaDevRoot.includes; do cp Library/Projects/QtCreator/$$i .; done;
	@$(ECHO) "done"


project-files-qt-creator-save:
	@$(ECHO) -n "Saving qt-creator project files..."
	@for i in StroikaDevRoot.config StroikaDevRoot.creator StroikaDevRoot.files StroikaDevRoot.includes; do cp $$i Library/Projects/QtCreator/ ; done;
	@$(ECHO) "done"


tools:	assure-default-configurations-exist_
ifeq ($(CONFIGURATION),)
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) tools --no-print-directory CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done
else
	@$(MAKE) --no-print-directory apply-configuration-if-needed_ CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
	@$(MAKE) --directory ThirdPartyComponents --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
	@$(MAKE) --directory Library --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
	@$(MAKE) --directory Tools --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
endif


tests:	assure-default-configurations-exist_
ifeq ($(CONFIGURATION),)
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) tests --no-print-directory CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done
else
	@$(MAKE) --no-print-directory apply-configuration-if-needed_ CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
	@$(MAKE) --directory ThirdPartyComponents --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
	@$(MAKE) --directory Library --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
	@$(MAKE) --directory Tests --no-print-directory tests CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
endif


samples:	assure-default-configurations-exist_
ifeq ($(CONFIGURATION),)
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) samples --no-print-directory CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done
else
	@$(MAKE) --no-print-directory apply-configuration-if-needed_ CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
	@$(MAKE) --directory ThirdPartyComponents --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
	@$(MAKE) --directory Library --no-print-directory all CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
	@$(MAKE) --directory Samples --no-print-directory samples CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
endif


run-tests:	tests
ifeq ($(CONFIGURATION),)
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --directory Tests --no-print-directory run-tests CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done
else
	@$(MAKE) --directory Tests --no-print-directory run-tests CONFIGURATION=$(CONFIGURATION) MAKEFLAGS=  MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
endif


ASTYLE_ARGS=
ASTYLE_ARGS+=	--style=stroustrup
#ASTYLE_ARGS+=	--style=whitesmith
#ASTYLE_ARGS+=	--style=kr
#ASTYLE_ARGS+=	--style=java

ASTYLE_ARGS+=	--convert-tabs
ASTYLE_ARGS+=	--indent-namespaces
ASTYLE_ARGS+=	--indent-cases
ASTYLE_ARGS+=	--indent-switches
ASTYLE_ARGS+=	--indent-preprocessor
ASTYLE_ARGS+=	--pad-oper
ASTYLE_ARGS+=	--break-closing-brackets
ASTYLE_ARGS+=	--keep-one-line-blocks
ASTYLE_ARGS+=	--indent=spaces
ASTYLE_ARGS+=	--preserve-date

ASTYLE_ARGS+=	--align-pointer=type
ASTYLE_ARGS+=	--align-reference=type
ASTYLE_ARGS+=	--mode=c
ASTYLE_ARGS+=	--suffix=none

format-code:
	@$(ECHO) Running Astyle...
	@for i in Library Samples Tests Tools;\
	do\
		astyle $(ASTYLE_ARGS) --recursive "$$i/*.h" "$$i/*.cpp" "$$i/*.inl" --formatted || true;\
	done


# useful internal check to make sure users dont run/build while missing key components that will
# just fail later
# but dont check if already checked
IntermediateFiles/TOOLS_CHECKED:
	@$(MAKE) check-tools --no-print-directory MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
	@mkdir -p IntermediateFiles
	@touch IntermediateFiles/TOOLS_CHECKED

# Force TOOLS_CHECKED test
check-tools:
	@#NOTE - we used to check for libtool, but thats only sometimes needed and we dont know if needed until after this rule (config based); its checked/warned about later
	@# no point in checking make ;-)
	@ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Checking for installed tools:"
	@ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && sh -c "type sed"
	@ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && sh -c "type wget"
	@ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && sh -c "type perl"
	@ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && sh -c "type tar"
	@ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && sh -c "type patch"
ifneq (,$(findstring CYGWIN,$(shell uname)))
	@ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && sh -c "type dos2unix"
	@ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && sh -c "type unix2dos"
endif
	@ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && $(ECHO) "All Required Tools Present"


assure-default-configurations-exist_:
ifeq ($(shell ScriptsLib/GetConfigurations.sh),)
	@$(MAKE) default-configurations --no-print-directory MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)
endif


apply-configuration-if-needed_:	assure-default-configurations-exist_
	@$(MAKE) --no-print-directory --silent IntermediateFiles/$(CONFIGURATION)/APPLIED_CONFIGURATION CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);


apply-configurations:
	@for i in `ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory apply-configuration CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
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
	
