ProjectPlatformSubdir	=	$(shell perl ../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)

.PHONY:	tests all check clobber

TEST_BUILD_DIR=	$(shell perl ScriptsLib/PrintTestBuildDir.pl)
TESTS=	        $(shell perl ScriptsLib/PrintTestsList.pl)

INDENT_LEVEL=	0


ifeq (Unix,$(ProjectPlatformSubdir))
BASEEXEPATTERN=Test$$i
EXEPATTERN=../Builds/$(CONFIGURATION)/Test$$i
else
BASEEXEPATTERN=Test$$i.exe
EXEPATTERN=../Builds/$(CONFIGURATION)/Test$$i/Test$$i.exe
endif


ifneq (,$(REMOTE))
RUNTEST_REMOTECOPYSETUP=scp -q $(EXEPATTERN) $(REMOTE):/tmp/$(BASEEXEPATTERN)
RUNTEST_EXE_RUN=ssh $(REMOTE) /tmp/$(BASEEXEPATTERN)
RUNTEST_EXE_RUN_PRINTLINE=scp $(EXEPATTERN)...; ssh $(REMOTE) /tmp/$(BASEEXEPATTERN)
else ifneq (,$(VALGRIND))
EXTRA_VALGRIND_OPTIONS=
VALGRIND_SUPPRESSIONS?= Common-Valgrind.supp
#VALGRIND_SUPPRESSIONS?= Common-Valgrind.supp BlockAllocation-Valgrind.supp
VALGRIND_EXPANDED_SUPPRESSIONS:= $(foreach vs,$(VALGRIND_SUPPRESSIONS),--suppressions=$(vs))
#VALGRIND_OPTIONS=-q --tool=helgrind $(VALGRIND_EXPANDED_SUPPRESSIONS) $(EXTRA_VALGRIND_OPTIONS)
VALGRIND_OPTIONS=-q --track-origins=yes --tool=memcheck --leak-check=full $(VALGRIND_EXPANDED_SUPPRESSIONS) $(EXTRA_VALGRIND_OPTIONS)
RUNTEST_REMOTECOPYSETUP=:
RUNTEST_EXE_RUN=valgrind $(VALGRIND_OPTIONS) $(EXEPATTERN)
RUNTEST_EXE_RUN_PRINTLINE=$(RUNTEST_EXE_RUN)
else
RUNTEST_REMOTECOPYSETUP=:
RUNTEST_EXE_RUN=$(EXEPATTERN)
RUNTEST_EXE_RUN_PRINTLINE=$(EXEPATTERN)
endif



help:
	@echo "Help for making Stroika"
	@echo "Targets"
	@echo "    all:                    -    builds everything"
	@echo "    check:                  -    checks everything built properly"
	@echo "    clean:"
	@echo "    clobber:"
	@echo "    project-files:          -    builds project files for things like visual studio.net etc"
	@echo "    tests:"
	@echo "    run-tests:              -    [REMOTE=] - eg. REMOTE=lewis@localhost; [VALGRIND=1] to run with valgrind (EXTRA_VALGRIND_OPTIONS= can be used with valgrind, as can VALGRIND_SUPPRESSIONS=)"


all:		tests 

check:
	@perl ScriptsLib/_checkall.pl

clean:
	@perl ScriptsLib/_buildall.pl clean

clobber:
	@perl ScriptsLib/_buildall.pl clobber

project-files:
	@echo "Building VisualStudio.Net-2013 project files..."
	@(cd Projects/VisualStudio.Net-2013 && perl BuildProjectsFiles.pl)
	@echo "Building VisualStudio.Net-2015 project files..."
	@(cd Projects/VisualStudio.Net-2015 && perl BuildProjectsFiles.pl)

tests:
	@#NOTE - MAKEFLAGS trick to avoid warning about -j builds til we get rid of perl stuff
	@MAKEFLAGS= perl ScriptsLib/_buildall.pl build

run-tests:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) run-tests CONFIGURATION=$$i;\
	done
else
	@../ScriptsLib/PrintLevelLeader.sh $(INDENT_LEVEL) && echo "Running Tests {$(CONFIGURATION)}..."
	@for i in $(TESTS);\
	do\
	     TESTNAME=`perl ScriptsLib/PrintTestName.pl $$i`;\
		$(RUNTEST_REMOTECOPYSETUP);\
	        TIMESTAMP=`date +%s`;\
	        EXERESULT=`$(RUNTEST_EXE_RUN)`;\
		   if [ -e PerformanceDump.txt ]; then\
	          mv PerformanceDump.txt ../IntermediateFiles/PerformanceDump.$$ci.txt;\
		   fi;\
	        TIMESTAMP2=`date +%s`;\
		   EXERESULT=`echo $$EXERESULT | sed 's/ *$$//g'`;\
	        DIFFTIME=`expr $$TIMESTAMP2 - $$TIMESTAMP`;\
			if [ "$$DIFFTIME" -lt "10" ]; then\
	            DIFFTIME=$$DIFFTIME" ";\
			fi;\
	        echo "   [$$EXERESULT]  ($$DIFFTIME seconds)  [$$i]  $$TESTNAME  ($(RUNTEST_EXE_RUN_PRINTLINE))";\
	done
	@../ScriptsLib/PrintLevelLeader.sh $(INDENT_LEVEL) && echo "Running Tests...done"
endif


