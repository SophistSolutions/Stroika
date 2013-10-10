.PHONY:	tests
.FORCE:	check-tools
.FORCE:	apply-configurations

all:		TOOLS_CHECKED APPLIED_CONFIGURATIONS
	./buildall.pl build

clean:
	./buildall.pl clean

clobber:
	./buildall.pl clobber
	rm -f TOOLS_CHECKED APPLIED_CONFIGURATIONS

tests:	TOOLS_CHECKED APPLIED_CONFIGURATIONS
	./buildall.pl build+

run-tests:	TOOLS_CHECKED APPLIED_CONFIGURATIONS
	./buildall.pl build+


# useful internal check to make sure users dont run/build while missing key components that will
# just fail later
# but dont check if already checked
TOOLS_CHECKED:
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
	@touch TOOLS_CHECKED

# Force TOOLS_CHECKED test
check-tools:	TOOLS_CHECKED
	@echo "Tools present"


APPLIED_CONFIGURATIONS:
	#todo - must enahnce to support multiple configs
	# must check here/fail if zero configs and direct user to call make default-configuration
	@perl ApplyConfiguration.pl --only-if-unconfigured
	@touch APPLIED_CONFIGURATIONS

apply-configurations:	APPLIED_CONFIGURATIONS

default-configuration:
	@perl GenerateConfiguration.pl --default-for-platform
