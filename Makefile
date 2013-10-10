.PHONY:	tests
.FORCE:	check-tools

all:		TOOLS_CHECKED
	./buildall.pl build

clean:
	./buildall.pl clean

clobber:
	./buildall.pl clobber
	rm -f TOOLS_CHECKED

tests:	TOOLS_CHECKED
	./buildall.pl build+

run-tests:
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
