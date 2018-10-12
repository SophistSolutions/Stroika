ProjectPlatformSubdir	=	$(shell ../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)
export StroikaRoot=$(realpath ../)/

.PHONY:	tests all check clobber

SHELL=/bin/bash

TESTS=	        $(shell ScriptsLib/PrintTestsList.pl)

ECHO?=	$(shell ../ScriptsLib/GetDefaultShellVariable.sh ECHO)
MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO_BUILD_LINES?=0

RUN_PREFIX	=	$(shell ../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) RUN_PREFIX)
ifneq (,$(RUN_PREFIX))
RUN_PREFIX:=$(RUN_PREFIX) 
endif

ifeq (Unix,$(ProjectPlatformSubdir))
BASEEXEPATTERN=Test$$i
EXEPATTERN=../Builds/$(CONFIGURATION)/Test$$i
else
BASEEXEPATTERN=Test$$i.exe
EXEPATTERN=../Builds/$(CONFIGURATION)/Test$$i/Test$$i.exe
endif


SSH_REMOTE_WARNINGS_OFF=-o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no


### @todo - rewrite the below mess so valgrind etc CONSTRUCT a RUN_PREFIX, and then unify the code a bit.
### and change place where RUN_PREFIX set (RegressionTests.sh) and use the RUN_PREFIX mechanism (configuration)


ifeq (memcheck,$(VALGRIND))
EXTRA_VALGRIND_OPTIONS=
VALGRIND_SUPPRESSIONS?= Valgrind-MemCheck-Common.supp
#VALGRIND_SUPPRESSIONS?= Common-Valgrind.supp BlockAllocation-Valgrind.supp
VALGRIND_EXPANDED_SUPPRESSIONS:= $(foreach vs,$(VALGRIND_SUPPRESSIONS),--suppressions=$(vs))
#VALGRIND_OPTIONS=-q --tool=helgrind $(VALGRIND_EXPANDED_SUPPRESSIONS) $(EXTRA_VALGRIND_OPTIONS)
VALGRIND_OPTIONS=-q --track-origins=yes --tool=memcheck --leak-check=full $(VALGRIND_EXPANDED_SUPPRESSIONS) $(EXTRA_VALGRIND_OPTIONS)
else ifeq (helgrind,$(VALGRIND))
EXTRA_VALGRIND_OPTIONS=
VALGRIND_SUPPRESSIONS?= Valgrind-Helgrind-Common.supp
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
RUNTEST_EXE_RUN=ssh $(SSH_REMOTE_WARNINGS_OFF) $(REMOTE) $(RUN_PREFIX) "$(VALEXTRARUN_) /tmp/$(BASEEXEPATTERN)"
RUNTEST_EXE_RUN_PRINTLINE=scp $(EXEPATTERN)...; ssh $(REMOTE) $(RUN_PREFIX) $(VALEXTRARUN_) /tmp/$(BASEEXEPATTERN)
else ifeq (memcheck,$(VALGRIND))
RUNTEST_EXE_RUN=valgrind $(VALGRIND_OPTIONS) $(EXEPATTERN)
RUNTEST_EXE_RUN_PRINTLINE=$(RUNTEST_EXE_RUN)
else ifeq (helgrind,$(VALGRIND))
RUNTEST_EXE_RUN=valgrind $(VALGRIND_OPTIONS) $(EXEPATTERN)
RUNTEST_EXE_RUN_PRINTLINE=$(RUNTEST_EXE_RUN)
else
RUNTEST_EXE_RUN=$(RUN_PREFIX)$(EXEPATTERN)
RUNTEST_EXE_RUN_PRINTLINE=$(RUN_PREFIX)$(EXEPATTERN)
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
	@for i in `$(StroikaRoot)ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory check CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) || exit $$?;\
	done
else
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Checking Stroika Tests {$(CONFIGURATION)}:"
	@for tst in $(TESTS) ; do\
		$(MAKE) --directory $$tst --no-print-directory check CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) || exit $$?;\
	done
endif


clean:
ifeq ($(CONFIGURATION),)
	@for i in `$(StroikaRoot)ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory clean CONFIGURATION=$$i;\
	done
else
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Cleaning Stroika Tests {$(CONFIGURATION)} ... "
	@rm -rf $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Tests/
	@$(ECHO) done
endif


clobber:
ifeq ($(CONFIGURATION),)
	@for i in `$(StroikaRoot)ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory clobber CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done
else
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@rm -rf $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Tests/
	@rm -rf $(StroikaRoot)Builds/$(CONFIGURATION)/Test??
endif


project-files:
	@echo -n "Building VisualStudio.Net-2017 project files..."
	@cd Projects/VisualStudio.Net-2017 && ./BuildProjectsFiles.pl
	@echo done


tests:
ifeq ($(CONFIGURATION),)
	@for i in `$(StroikaRoot)ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory tests CONFIGURATION=$$i ECHO_BUILD_LINES=$(ECHO_BUILD_LINES) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) || exit $$?;\
	done
else
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Building Stroika Tests {$(CONFIGURATION)}:"
	@(cd Projects/$(ProjectPlatformSubdir) && ./BuildProjectsFiles.pl)
	@$(MAKE) --no-print-directory $(TESTS) CONFIGURATION=$(CONFIGURATION) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL) + 1))
endif


#### @todo make targets final EXE and don't need PHONY and can be even quicker running
.PHONY: $(TESTS)
$(TESTS):
ifeq ($(CONFIGURATION),)
	$(error CONFIGURATION variable required for this target)
endif
	@$(StroikaRoot)ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Test $@: `$(StroikaRoot)Tests/ScriptsLib/PrintTestName.pl $@`:"
ifeq (Unix,$(ProjectPlatformSubdir))
	@$(MAKE) --directory $@ -s all CONFIGURATION=$(CONFIGURATION) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL) + 1))
else
	@$(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:NoSummary Projects/$(ProjectPlatformSubdir)/$@/Test$@.vcxproj /p:`$(StroikaRoot)ScriptsLib/GetVisualStudioConfigLine.pl $(CONFIGURATION)` /target:Build"
endif

run-tests:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory run-tests CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done
else
	@../ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Running Stroka Tests {$(CONFIGURATION)}:"
	@PREFIX=`../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL) + 1))`;\
    if [ "`../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) CrossCompiling`" = "true" ] && [ "$(REMOTE)" = "" ] ; then\
           echo "$$PREFIX" "Skiping cuz Cross-Compiling and REMOTE= not specified...";\
	else\
		for i in $(TESTS); do\
			TESTNAME=`ScriptsLib/PrintTestName.pl $$i`;\
			$(RUNTEST_REMOTECOPYSETUP);\
			$(RUNTEST_REMOTECOPYSETUP2);\
			TIMESTAMP=`date +%s`;\
			EXERESULT="CRASHED";\
			EXERESULT=`$(RUNTEST_EXE_RUN)`;\
			if [ -e PerformanceDump.txt ]; then\
			  mv PerformanceDump.txt ../Builds/$(CONFIGURATION)/PerformanceDump.txt 2>/dev/null;\
			fi;\
			TIMESTAMP2=`date +%s`;\
			EXERESULT=`echo $$EXERESULT | tr -d '\n' | tr -d '\r'`;\
			DIFFTIME=`expr $$TIMESTAMP2 - $$TIMESTAMP`;\
			if [ "$$DIFFTIME" -lt "10" ]; then\
			    DIFFTIME=$$DIFFTIME" ";\
			fi;\
			echo "$$PREFIX[$$EXERESULT]  ($$DIFFTIME seconds)  [$$i]  $$TESTNAME  ($(RUNTEST_EXE_RUN_PRINTLINE))";\
		done\
        fi
endif
