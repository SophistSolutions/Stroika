.PHONY:	tests
.FORCE:	check-tools
.FORCE:	apply-configurations


help:
	@echo "Help for making Stroika"
	@echo "Targets"
	@echo "    all:                    -    builds everything"
	@echo "    check:                  -    checks everything built properly"
	@echo "    clean:"
	@echo "    clobber:"
	@echo "    libraries:              -	builds Stroika foundation & frameworks, and any things it depends on (like thirdpartyproducts)"
	@echo "    project-files:          -	builds project files for things like visual studio.net etc (NYI)"
	@echo "    tests:"
	@echo "    run-tests:"
	@echo "    apply-configurations:   -    create implied files / links for any configurations in the Configuratoons folder (forces a rebuild of configs)"
	@echo "    default-configuration:  -    creates the default configuration in Configurations folder (target takes DEFAULT_CONFIGURATION_ARGS)"
	@echo "    check-tools:            -    check the tools needed to build stroika are installed."

all:		IntermediateFiles/TOOLS_CHECKED apply-configurations-if-needed
	@./buildall.pl build


check:
	@make --directory ThirdPartyLibs check
	./checkall.pl

clean:
	./buildall.pl clean

clobber:
	./buildall.pl clobber
	rm -f IntermediateFiles/TOOLS_CHECKED apply-configurations-if-needed

libraries:	IntermediateFiles/TOOLS_CHECKED apply-configurations-if-needed
	#NYI correctly - this also builds tests/demos etc
	@./buildall.pl build

project-files:
	@echo NYI

tests:	IntermediateFiles/TOOLS_CHECKED apply-configurations-if-needed
	@./buildall.pl build

run-tests:	IntermediateFiles/TOOLS_CHECKED apply-configurations-if-needed
	@./buildall.pl build+


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
	@perl ApplyConfiguration.pl
	@touch IntermediateFiles/APPLIED_CONFIGURATIONS

default-configuration:
	@perl GenerateConfiguration.pl --default-for-platform $(DEFAULT_CONFIGURATION_ARGS)
