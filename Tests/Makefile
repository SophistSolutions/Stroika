ProjectPlatformSubdir	=	$(shell perl ../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)

.PHONY:	tests all check clobber

TESTS=	        $(shell perl ScriptsLib/PrintTestsList.pl)

MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO_BUILD_LINES?=0



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
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory check CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done
else
	@export CONFIGURATION=$(CONFIGURATION);export MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL); perl ScriptsLib/_checkall.pl
endif


clean:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory clean CONFIGURATION=$$i;\
	done
else
	@CONFIGURATION=$(CONFIGURATION) perl ScriptsLib/_buildall.pl clean >/dev/null 2> /dev/null || true
endif


clobber:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory clobber CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done
else
	@(export CONFIGURATION=$(CONFIGURATION);export MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL); perl ScriptsLib/_buildall.pl clobber) > /dev/null 2> /dev/null || true
endif


project-files:
	@echo "Building VisualStudio.Net-2013 project files..."
	@cd Projects/VisualStudio.Net-2013 && perl BuildProjectsFiles.pl
	@echo "Building VisualStudio.Net-2015 project files..."
	@cd Projects/VisualStudio.Net-2015 && perl BuildProjectsFiles.pl


tests:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory tests CONFIGURATION=$$i ECHO_BUILD_LINES=$(ECHO_BUILD_LINES) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done
else
	@export CONFIGURATION=$(CONFIGURATION);export MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);export ECHO_BUILD_LINES=$(ECHO_BUILD_LINES); perl ScriptsLib/_buildall.pl build
endif


run-tests:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory run-tests CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done
else
	@../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && echo "Running Stroka {$(CONFIGURATION)} Tests:"
	@PREFIX=`../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL) + 1))`;\
	for i in $(TESTS);\
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
	        echo "$$PREFIX[$$EXERESULT]  ($$DIFFTIME seconds)  [$$i]  $$TESTNAME  ($(RUNTEST_EXE_RUN_PRINTLINE))";\
	done
endif
