export StroikaRoot?=$(abspath ../)/

ifneq ($(CONFIGURATION),)
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk



.PHONY:	tests all check clobber


TESTS=	        $(shell ScriptsLib/PrintTestsList)

BASEEXEPATTERN=Test$$i${EXE_SUFFIX}
EXEPATTERN=../Builds/$(CONFIGURATION)/Tests/${BASEEXEPATTERN}


# Only applies to run-tests
export TEST_FAILURES_CAUSE_FAILED_MAKE?=1

#SSH_REMOTE_WARNINGS_OFF=-o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no
SSH_REMOTE_WARNINGS_OFF= -o StrictHostKeyChecking=no


# Consider exposing this explicitly (as we once did) in the configuration file
# But for now this is fine...LGP 2020-04-27
ifeq (qAllowBlockAllocation=1,$(findstring qAllowBlockAllocation=1,$(CPPFLAGS)))
USING_BLOCK_ALLOCATION?=1
else ifeq (qAllowBlockAllocation=0,$(findstring qAllowBlockAllocation=0,$(CPPFLAGS)))
USING_BLOCK_ALLOCATION?=0
else ifeq (qDebug=1,$(findstring qDebug=1,$(CPPFLAGS)))
USING_BLOCK_ALLOCATION?=1
else
USING_BLOCK_ALLOCATION?=0
endif

EXTRA_VALGRIND_SUPPRESSIONS?=

ifeq (memcheck,$(VALGRIND))
DEFAULT_VALGRIND_SUPPRESSIONS_=Valgrind-MemCheck-Common.supp
#DEFAULT_VALGRIND_SUPPRESSIONS_+= BlockAllocation-Valgrind.supp
ifneq (,$(wildcard Valgrind-MemCheck-Common-$(ARCH).supp))
DEFAULT_VALGRIND_SUPPRESSIONS_+= Valgrind-MemCheck-Common-$(ARCH).supp
endif
ifeq (1,$(USING_BLOCK_ALLOCATION))
DEFAULT_VALGRIND_SUPPRESSIONS_+= Valgrind-MemCheck-BlockAllocation.supp
endif
VALGRIND_SUPPRESSIONS?= $(DEFAULT_VALGRIND_SUPPRESSIONS_)
else ifeq (helgrind,$(VALGRIND))
DEFAULT_VALGRIND_SUPPRESSIONS_=Valgrind-Helgrind-Common.supp
ifneq (,$(wildcard Valgrind-Helgrind-Common-$(ARCH).supp))
DEFAULT_VALGRIND_SUPPRESSIONS_+= Valgrind-Helgrind-Common-$(ARCH).supp
endif
VALGRIND_SUPPRESSIONS?= $(DEFAULT_VALGRIND_SUPPRESSIONS_) $(EXTRA_VALGRIND_SUPPRESSIONS)
endif

ifeq (memcheck,$(VALGRIND))
EXTRA_VALGRIND_OPTIONS=
VALGRIND_EXPANDED_SUPPRESSIONS:= $(foreach vs,$(VALGRIND_SUPPRESSIONS),--suppressions=$(vs))
#VALGRIND_OPTIONS=-q --tool=helgrind $(VALGRIND_EXPANDED_SUPPRESSIONS) $(EXTRA_VALGRIND_OPTIONS)
VALGRIND_OPTIONS=-q --track-origins=yes --tool=memcheck --leak-check=full $(VALGRIND_EXPANDED_SUPPRESSIONS) $(EXTRA_VALGRIND_OPTIONS)
else ifeq (helgrind,$(VALGRIND))
EXTRA_VALGRIND_OPTIONS=
VALGRIND_EXPANDED_SUPPRESSIONS:= $(foreach vs,$(VALGRIND_SUPPRESSIONS),--suppressions=$(vs))
VALGRIND_OPTIONS=-q --tool=helgrind $(VALGRIND_EXPANDED_SUPPRESSIONS) $(EXTRA_VALGRIND_OPTIONS)
endif



RUNTEST_REMOTECOPYSETUP=true
RUNTEST_REMOTECOPYSETUP2=true
ifneq (,$(REMOTE))
RUNTEST_REMOTECOPYSETUP=scp $(SSH_REMOTE_WARNINGS_OFF) -q $(EXEPATTERN) $(REMOTE):/tmp/$(BASEEXEPATTERN)
VALEXTRARUN_=
ifneq (,$(VALGRIND))
VALEXTRARUN_= cd /tmp && valgrind $(VALGRIND_OPTIONS)
RUNTEST_REMOTECOPYSETUP2=  scp $(SSH_REMOTE_WARNINGS_OFF) -q *.supp $(REMOTE):/tmp/
endif
RUNTEST_EXE_RUN=ssh $(SSH_REMOTE_WARNINGS_OFF) $(REMOTE) ${RUN_PREFIX} "$(VALEXTRARUN_) /tmp/$(BASEEXEPATTERN)"
RUNTEST_EXE_RUN_PRINTLINE=scp $(EXEPATTERN)...; ssh $(REMOTE) ${RUN_PREFIX} $(VALEXTRARUN_) /tmp/$(BASEEXEPATTERN)
else ifeq (memcheck,$(VALGRIND))
RUNTEST_EXE_RUN=valgrind $(VALGRIND_OPTIONS) $(EXEPATTERN)
RUNTEST_EXE_RUN_PRINTLINE=$(RUNTEST_EXE_RUN)
else ifeq (helgrind,$(VALGRIND))
RUNTEST_EXE_RUN=valgrind $(VALGRIND_OPTIONS) $(EXEPATTERN)
RUNTEST_EXE_RUN_PRINTLINE=$(RUNTEST_EXE_RUN)
else
ifeq (${RUN_PREFIX},)
RUNTEST_EXE_RUN=$(EXEPATTERN)
RUNTEST_EXE_RUN_PRINTLINE=$(EXEPATTERN)
else
RUNTEST_EXE_RUN=${RUN_PREFIX} $(EXEPATTERN)
RUNTEST_EXE_RUN_PRINTLINE=${RUN_PREFIX} $(EXEPATTERN)
endif
endif



help:
	@$(ECHO) "Help for making Stroika"
	@$(ECHO) "Targets"
	@$(ECHO) "    all:                    -    builds everything"
	@$(ECHO) "    check:                  -    checks everything built properly"
	@$(ECHO) "    clean:"
	@$(ECHO) "    clobber:"
	@$(ECHO) "    project-files:          -    builds project files for things like visual studio.net etc"
	@$(ECHO) "    tests:"
	@$(ECHO) "    run-tests:              -    [REMOTE=] - eg. REMOTE=lewis@localhost;"
	@$(ECHO) "                                 [VALGRIND=memcheck or helgrind, etc] to run with valgrind (EXTRA_VALGRIND_OPTIONS= can be used with valgrind)"
	@$(ECHO) "                                 OR VALGRIND_SUPPRESSIONS=\"Valgrind-MemCheck-Common.supp Valgrind-MemCheck-BlockAllocation.supp\""
	@$(ECHO) "                                 OR VALGRIND_SUPPRESSIONS=\"Valgrind-Helgrind-Common.supp\""
	@$(ECHO) "                                 EG: VALGRIND_SUPPRESSIONS=\"Valgrind-Helgrind-Common.supp\" make VALGRIND=helgrind run-tests"


all:		tests 


check:
ifeq ($(CONFIGURATION),)
	@for i in `$(StroikaRoot)ScriptsLib/GetConfigurations` ; do\
		$(MAKE) --no-print-directory check CONFIGURATION=$$i | exit $$?;\
	done
else
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Checking Stroika Tests {$(CONFIGURATION)}:"
	@for tst in $(TESTS) ; do\
		$(MAKE) --directory $$tst --no-print-directory check MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
	done
endif


clean:
ifeq ($(CONFIGURATION),)
	@for i in `$(StroikaRoot)ScriptsLib/GetConfigurations` ; do\
		$(MAKE) --no-print-directory clean CONFIGURATION=$$i;\
	done
else
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "Cleaning Stroika Tests {$(CONFIGURATION)} ... "
	@rm -rf $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Tests/
	@$(ECHO) done
endif


clobber:
ifeq ($(CONFIGURATION),)
	@for i in `$(StroikaRoot)ScriptsLib/GetConfigurations` ; do\
		$(MAKE) --no-print-directory clobber CONFIGURATION=$$i;\
	done
else
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@rm -rf $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Tests/
	@rm -rf $(StroikaRoot)Builds/$(CONFIGURATION)/Tests/
endif


project-files:
	@echo -n "Building VisualStudio.Net-2017 project files..."
	@cd Projects/VisualStudio.Net-2017 && ./BuildProjectsFiles
	@echo done
	@echo -n "Building VisualStudio.Net-2019 project files..."
	@cd Projects/VisualStudio.Net-2019 && ./BuildProjectsFiles
	@echo done


tests:
ifeq ($(CONFIGURATION),)
	@for i in `$(StroikaRoot)ScriptsLib/GetConfigurations` ; do\
		$(MAKE) --no-print-directory tests CONFIGURATION=$$i || exit $$?;\
	done
else
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Stroika Tests {$(CONFIGURATION)}:"
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@(cd Projects/$(ProjectPlatformSubdir) && ./BuildProjectsFiles)
endif
	@$(MAKE) --no-print-directory $(TESTS) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL) + 1))
endif


#### @todo make targets final EXE and don't need PHONY and can be even quicker running
.PHONY: $(TESTS)
$(TESTS):
ifeq ($(CONFIGURATION),)
	$(error CONFIGURATION variable required for this target)
endif
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Test $@: `$(StroikaRoot)Tests/ScriptsLib/PrintTestName $@`:"
	@$(MAKE) --directory $@ -s all MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL) + 1))


run-tests:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations` ; do\
		$(MAKE) --no-print-directory run-tests CONFIGURATION=$$i;\
	done
else
	@../ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Running Stroika Tests {$(CONFIGURATION)}:"
	@PREFIX=`../ScriptsLib/PrintLevelLeader $$(($(MAKE_INDENT_LEVEL) + 1))`;\
    if [ "$(CrossCompiling)" = "true" ] && [ "$(REMOTE)" = "" ] ; then\
           echo "$$PREFIX" "Skiping cuz Cross-Compiling and REMOTE= not specified...";\
	else\
		FAILED=0;\
		for i in $(TESTS); do\
			TESTNAME=`ScriptsLib/PrintTestName $$i`;\
			$(RUNTEST_REMOTECOPYSETUP);\
			$(RUNTEST_REMOTECOPYSETUP2);\
			TIMESTAMP=`date +%s`;\
			EXERESULT=`$(RUNTEST_EXE_RUN)`;\
			EXERESULT=`echo $$EXERESULT | tr -d '\n' | tr -d '\r'`;\
			if [ "$$EXERESULT" != "Succeeded" ]; then\
			    FAILED=1;\
			fi;\
			if [ -e PerformanceDump.txt ]; then\
			  mv PerformanceDump.txt ../Builds/$(CONFIGURATION)/PerformanceDump.txt 2>/dev/null;\
			fi;\
			TIMESTAMP2=`date +%s`;\
			DIFFTIME=`expr $$TIMESTAMP2 - $$TIMESTAMP`;\
			if [ "$$DIFFTIME" -lt "10" ]; then\
			    DIFFTIME=$$DIFFTIME" ";\
			fi;\
			echo "$$PREFIX[$$EXERESULT]  ($$DIFFTIME seconds)  [$$i]  $$TESTNAME  ($(RUNTEST_EXE_RUN_PRINTLINE))";\
		done;\
		if [ "$$FAILED" -ne 0 ]; then\
			if [ "$$TEST_FAILURES_CAUSE_FAILED_MAKE" -ne 0 ]; then\
				echo "$$PREFIX" "   ***SOME TESTS FAILED***";\
				exit 1;\
			else\
				echo "$$PREFIX" "   ***SOME TESTS FAILED*** (exit suppressed by TEST_FAILURES_CAUSE_FAILED_MAKE)";\
			fi;\
		fi\
    fi
endif
