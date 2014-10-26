.PHONY:	tests documentation all check clobber libraries
.FORCE:	check-tools
.FORCE:	apply-configurations


help:
	@echo "Help for making Stroika"
	@echo "Targets"
	@echo "    all:                    -    Builds everything"
	@echo "    check:                  -    Checks everything was built properly"
	@echo "    clean:"
	@echo "    clobber:"
	@echo "    libraries:              -    Builds Stroika foundation & frameworks, and any things it depends on (like third-party-libs)"
	@echo "    project-files:          -    Builds project files for things like visual studio.net etc"
	@echo "    tests:"
	@echo "    format-code:            -    Run astyle on source code, and update it to conform to Stroika code formatting standards"
	@echo "    samples:"
	@echo "    documentation:"
	@echo "    third-party-libs:"
	@echo "    run-tests:              -    [REMOTE=] - eg. REMOTE=lewis@localhost; [VALGRIND=1] to run with valgrind (EXTRA_VALGRIND_OPTIONS= can be used with valgrind)"
	@echo "    apply-configurations:   -    Create implied files / links for any configurations in the Configurations folder (forces a rebuild of configs)"
	@echo "    default-configuration:  -    Creates the default configuration in Configurations folder (target takes DEFAULT_CONFIGURATION_ARGS, for example: make default-configuration DEFAULT_CONFIGURATION_ARGS=\"--help\")"
	@echo "    check-tools:            -    Check the tools needed to build Stroika are installed."


all:		IntermediateFiles/TOOLS_CHECKED apply-configurations-if-needed libraries tools samples tests documentation
	@make check


check:
	@make --directory ThirdPartyLibs --no-print-directory MAKEFLAGS= check
	@(cd Library && perl checkall.pl)
	@(cd Tools && perl checkall.pl)
	@($MAKE) --directory Samples --no-print-directory MAKEFLAGS= check
	@($MAKE) --directory Tests --no-print-directory MAKEFLAGS= check


clean:
	@make --directory ThirdPartyLibs --no-print-directory clean
	@(cd Library; perl buildall.pl clean)
	@(cd Tools; perl buildall.pl clean)
	@($MAKE) --directory Samples --no-print-directory MAKEFLAGS= clean
	@($MAKE) --directory Tests --no-print-directory MAKEFLAGS= clean


clobber:
	@echo "Clobbering..."
	@rm -rf IntermediateFiles
	@rm -rf Builds
	@($MAKE) --directory ThirdPartyLibs --no-print-directory clobber
	@#SHOULD DO BUT BROKEN NOW - @make --directory Tests --no-print-directory MAKEFLAGS= clobber


documentation:
	@cd Documentation; cd Doxygen; perl ./RunDoxygen.pl


libraries:	IntermediateFiles/TOOLS_CHECKED apply-configurations-if-needed third-party-libs
	@cd Library; perl buildall.pl build


third-party-libs:
	@($MAKE) --directory ThirdPartyLibs --no-print-directory all


project-files:
	@($MAKE) --directory Tests --no-print-directory MAKEFLAGS= project-files


tools:	libraries
	@cd Tools; perl buildall.pl build


tests:	tools libraries
	@($MAKE) --directory Tests --no-print-directory tests

samples:	tools libraries
	@($MAKE) --directory Samples --no-print-directory samples

run-tests:	tests
	@($MAKE) --directory Tests --no-print-directory run-tests MAKEFLAGS=

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
	@echo Running Astyle...
	@for i in Library Samples Tests Tools;\
	do\
		astyle $(ASTYLE_ARGS) --recursive "$$i/*.h" "$$i/*.cpp" "$$i/*.inl" --formatted || true;\
	done


# useful internal check to make sure users dont run/build while missing key components that will
# just fail later
# but dont check if already checked
IntermediateFiles/TOOLS_CHECKED:
	@$(MAKE) check-tools --no-print-directory
	@mkdir -p IntermediateFiles
	@touch IntermediateFiles/TOOLS_CHECKED

# Force TOOLS_CHECKED test
check-tools:
	@# no point in checking make ;-)
	@echo "Checking for installed tools..."
	@echo -n "  " && sh -c "type wget"
	@echo -n "  " && sh -c "type perl"
	@echo -n "  " && sh -c "type tar"
	@# NOT SURE NEEDED ANYMORE...#sh -c "type curl"
	@echo -n "  " && sh -c "type patch"
ifneq (,$(findstring CYGWIN,$(shell uname)))
	@echo -n "  " && sh -c "type dos2unix"
	@echo -n "  " && sh -c "type unix2dos"
endif
	@echo "All Required Tools Present"



apply-configurations-if-needed:
	@test -e ConfigurationFiles/DefaultConfiguration.xml || make default-configuration --no-print-directory
	@test -e IntermediateFiles/APPLIED_CONFIGURATIONS || make apply-configurations --no-print-directory

apply-configurations:
	@#todo - must enahnce to support multiple configs
	@# must check here/fail if zero configs and direct user to call make default-configuration
	@#@perl ApplyConfiguration.pl --only-if-unconfigured
	@perl ScriptsLib/ApplyConfiguration.pl
	@touch IntermediateFiles/APPLIED_CONFIGURATIONS

default-configuration:
	@perl ScriptsLib/GenerateConfiguration.pl --default-for-platform $(DEFAULT_CONFIGURATION_ARGS)
