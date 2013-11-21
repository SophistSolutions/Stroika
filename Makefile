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
	@echo "    tests:"
	@echo "    run-tests:"
	@echo "    apply-configurations:   -    create implied files / links for any configurations in the Configuraitons folder"
	@echo "    default-configuration:  -    creates the default configuration in Configurations folder"
	@echo "    check-tools:            -    check the tools needed to build stroika are installed."

all:		IntermediateFiles/TOOLS_CHECKED apply-configurations-if-needed
	@./buildall.pl build


check:
	./checkall.pl

clean:
	./buildall.pl clean

clobber:
	./buildall.pl clobber
	rm -f IntermediateFiles/TOOLS_CHECKED apply-configurations-if-needed

libraries:	IntermediateFiles/TOOLS_CHECKED apply-configurations-if-needed
	#NYI correctly - this also builds tests/demos etc
	@./buildall.pl build

tests:	IntermediateFiles/TOOLS_CHECKED apply-configurations-if-needed
	@./buildall.pl build

run-tests:	IntermediateFiles/TOOLS_CHECKED apply-configurations-if-needed
	@./buildall.pl build+


# useful internal check to make sure users dont run/build while missing key components that will
# just fail later
# but dont check if already checked
IntermediateFiles/TOOLS_CHECKED:
	@# no point in checking make ;-)
	@hash wget &> /dev/null || if [ $$? -eq 1 ]; then (echo >&2 "ToolCheck: wget not found" && exit 1); fi
	@hash perl &> /dev/null || if [ $$? -eq 1 ]; then (echo >&2 "ToolCheck: perl not found" && exit 1); fi
	@hash tar &> /dev/null || if [ $$? -eq 1 ]; then (echo >&2 "ToolCheck: tar not found" && exit 1); fi
	@hash curl &> /dev/null || if [ $$? -eq 1 ]; then (echo >&2 "ToolCheck: curl not found" && exit 1); fi
	@hash patch &> /dev/null || if [ $$? -eq 1 ]; then (echo >&2 "ToolCheck: patch not found" && exit 1); fi
ifneq (,$(findstring CYGWIN,$(shell uname)))
	@hash dos2unix &> /dev/null || if [ $$? -eq 1 ]; then (echo >&2 "ToolCheck: dos2unix not found" && exit 1); fi
	@hash unix2dos &> /dev/null || if [ $$? -eq 1 ]; then (echo >&2 "ToolCheck: unix2dos not found" && exit 1); fi
endif
	@touch IntermediateFiles/TOOLS_CHECKED

# Force TOOLS_CHECKED test
check-tools:	IntermediateFiles/TOOLS_CHECKED
	@echo "Tools present"



apply-configurations-if-needed:
	@test -e IntermediateFiles/APPLIED_CONFIGURATIONS || make apply-configurations

apply-configurations:
	@#todo - must enahnce to support multiple configs
	@# must check here/fail if zero configs and direct user to call make default-configuration
	@perl ApplyConfiguration.pl --only-if-unconfigured
	@touch IntermediateFiles/APPLIED_CONFIGURATIONS

default-configuration:
	@perl GenerateConfiguration.pl --default-for-platform
