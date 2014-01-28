.PHONY:	tests documentation all check clobber libraries
.FORCE:	check-tools
.FORCE:	apply-configurations


help:
	@echo "Help for making Stroika"
	@echo "Targets"
	@echo "    all:                    -    builds everything"
	@echo "    check:                  -    checks everything built properly"
	@echo "    clean:"
	@echo "    clobber:"
	@echo "    libraries:              -    builds Stroika foundation & frameworks, and any things it depends on (like thirdpartyproducts)"
	@echo "    project-files:          -    builds project files for things like visual studio.net etc (NYI)"
	@echo "    tests:"
	@echo "    format-code:            -    run astyle on source code, and update it to conform to Stroika code formatting standards"
	@echo "    samples:"
	@echo "    documentation:"
	@echo "    third-party-libs:"
	@echo "    run-tests:"
	@echo "    apply-configurations:   -    create implied files / links for any configurations in the Configuratoons folder (forces a rebuild of configs)"
	@echo "    default-configuration:  -    creates the default configuration in Configurations folder (target takes DEFAULT_CONFIGURATION_ARGS)"
	@echo "    check-tools:            -    check the tools needed to build stroika are installed."


all:		IntermediateFiles/TOOLS_CHECKED apply-configurations-if-needed libraries tools tests samples documentation
	@make check


check:
	@make --directory ThirdPartyLibs --no-print-directory MAKEFLAGS= check
	@(cd Library && perl checkall.pl)
	@(cd Tools && perl checkall.pl)
	@(cd Samples && perl checkall.pl)
	@make --directory Tests --no-print-directory MAKEFLAGS= check


clean:
	@make --directory ThirdPartyLibs --no-print-directory clean
	@(cd Library; perl buildall.pl clean)
	@(cd Tools; perl buildall.pl clean)
	@(cd Samples; perl buildall.pl clean)
	@make --directory Tests --no-print-directory MAKEFLAGS= clean


clobber:
	@make --directory ThirdPartyLibs --no-print-directory clobber
	@rm -f IntermediateFiles/TOOLS_CHECKED apply-configurations-if-needed
	@make --directory Tests --no-print-directory MAKEFLAGS= clobber


documentation:
	@cd Documentation; cd Doxygen; perl ./RunDoxygen.pl


libraries:	IntermediateFiles/TOOLS_CHECKED apply-configurations-if-needed third-party-libs
	@cd Library; perl buildall.pl build


third-party-libs:
	@make --directory ThirdPartyLibs --no-print-directory MAKEFLAGS= all


project-files:
	@make --directory Tests --no-print-directory MAKEFLAGS= project-files


tools:	libraries
	@cd Tools; perl buildall.pl build


tests:	tools libraries
	@make --directory Tests --no-print-directory MAKEFLAGS= tests


samples:	tools libraries
	@cd Samples; perl buildall.pl build

run-tests:	tests
	@make --directory Tests --no-print-directory MAKEFLAGS= run-tests

format-code:
	@echo Running Astyle...
	@perl ScriptsLib/RunAstyle.pl


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
