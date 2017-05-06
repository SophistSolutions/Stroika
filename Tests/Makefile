ProjectPlatformSubdir	=	$(shell perl ../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)
ifeq (Unix,$(ProjectPlatformSubdir))
StroikaRoot=$(realpath ../)
endif

.PHONY:	tests all check clobber

SHELL=/bin/bash

TESTS=	        $(shell perl ScriptsLib/PrintTestsList.pl)

ECHO?=	$(shell ../ScriptsLib/GetDefaultShellVariable.sh ECHO)
MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO_BUILD_LINES?=0


REMOTE_RUN_PREFIX=

ifeq (Unix,$(ProjectPlatformSubdir))
BASEEXEPATTERN=Test$$i
EXEPATTERN=../Builds/$(CONFIGURATION)/Test$$i
else
BASEEXEPATTERN=Test$$i.exe
EXEPATTERN=../Builds/$(CONFIGURATION)/Test$$i/Test$$i.exe
endif


ifneq (,$(REMOTE))
RUNTEST_REMOTECOPYSETUP=scp -q $(EXEPATTERN) $(REMOTE):/tmp/$(BASEEXEPATTERN)
RUNTEST_EXE_RUN=ssh $(REMOTE) $(REMOTE_RUN_PREFIX) /tmp/$(BASEEXEPATTERN)
RUNTEST_EXE_RUN_PRINTLINE=scp $(EXEPATTERN)...; ssh $(REMOTE) $(REMOTE_RUN_PREFIX) /tmp/$(BASEEXEPATTERN)
else ifeq (memcheck,$(VALGRIND))
EXTRA_VALGRIND_OPTIONS=
VALGRIND_SUPPRESSIONS?= Valgrind-MemCheck-Common.supp
#VALGRIND_SUPPRESSIONS?= Common-Valgrind.supp BlockAllocation-Valgrind.supp
VALGRIND_EXPANDED_SUPPRESSIONS:= $(foreach vs,$(VALGRIND_SUPPRESSIONS),--suppressions=$(vs))
#VALGRIND_OPTIONS=-q --tool=helgrind $(VALGRIND_EXPANDED_SUPPRESSIONS) $(EXTRA_VALGRIND_OPTIONS)
VALGRIND_OPTIONS=-q --track-origins=yes --tool=memcheck --leak-check=full $(VALGRIND_EXPANDED_SUPPRESSIONS) $(EXTRA_VALGRIND_OPTIONS)
RUNTEST_REMOTECOPYSETUP=:
RUNTEST_EXE_RUN=valgrind $(VALGRIND_OPTIONS) $(EXEPATTERN)
RUNTEST_EXE_RUN_PRINTLINE=$(RUNTEST_EXE_RUN)
else ifeq (helgrind,$(VALGRIND))
EXTRA_VALGRIND_OPTIONS=
VALGRIND_SUPPRESSIONS?= Valgrind-Helgrind-Common.supp
VALGRIND_EXPANDED_SUPPRESSIONS:= $(foreach vs,$(VALGRIND_SUPPRESSIONS),--suppressions=$(vs))
VALGRIND_OPTIONS=-q --tool=helgrind $(VALGRIND_EXPANDED_SUPPRESSIONS) $(EXTRA_VALGRIND_OPTIONS)
RUNTEST_REMOTECOPYSETUP=:
RUNTEST_EXE_RUN=valgrind $(VALGRIND_OPTIONS) $(EXEPATTERN)
RUNTEST_EXE_RUN_PRINTLINE=$(RUNTEST_EXE_RUN)
else
RUNTEST_REMOTECOPYSETUP=:
RUNTEST_EXE_RUN=$(EXEPATTERN)
RUNTEST_EXE_RUN_PRINTLINE=$(EXEPATTERN)
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
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory check CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done
else
	@../ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@export CONFIGURATION=$(CONFIGURATION);export MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL); perl ScriptsLib/_checkall.pl
endif


clean:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory clean CONFIGURATION=$$i;\
	done
else
	@../ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) -n "Cleaning Stroika {$(CONFIGURATION)} Tests..."
	@rm -rf ../IntermediateFiles/$(CONFIGURATION)/Tests/
	@$(ECHO) done
endif


clobber:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory clobber CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done
else
	@../ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@rm -rf ../IntermediateFiles/$(CONFIGURATION)/Tests/
	@rm -rf ../Builds/$(CONFIGURATION)/Test??
endif


project-files:
	@echo -n "Building VisualStudio.Net-2017 project files..."
	@cd Projects/VisualStudio.Net-2017 && perl BuildProjectsFiles.pl
	@echo done


tests:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory tests CONFIGURATION=$$i ECHO_BUILD_LINES=$(ECHO_BUILD_LINES) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done
else
	@../ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
ifeq (Unix,$(ProjectPlatformSubdir))
	@PREFIX=`../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL) + 1))`;\
	(cd Projects/Unix && ./BuildProjectsFiles.pl);\
	for i in $(TESTS); do\
		tstName=`$(StroikaRoot)/Tests/ScriptsLib/PrintTestName.pl $$i`;\
		$(ECHO) "$$PREFIX" "Test $$i: $$tstName:";\
		$(MAKE) --directory $$i -s all CONFIGURATION=$(CONFIGURATION) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL) + 2));\
	done
else
	@export CONFIGURATION=$(CONFIGURATION);export MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);export ECHO_BUILD_LINES=$(ECHO_BUILD_LINES); perl ScriptsLib/_buildall.pl build
endif
endif


run-tests:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory run-tests CONFIGURATION=$$i MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL);\
	done
else
	@../ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && echo "Running Stroka {$(CONFIGURATION)} Tests:"
	@PREFIX=`../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL) + 1))`;\
        if [ "`perl ../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) CrossCompiling`" = "true" ] && [ "$(REMOTE)" = "" ] ; then\
           echo "$$PREFIX" "Skiping cuz Cross-Compiling and REMOTE= not specified...";\
	else\
		for i in $(TESTS); do\
			TESTNAME=`perl ScriptsLib/PrintTestName.pl $$i`;\
			$(RUNTEST_REMOTECOPYSETUP);\
			TIMESTAMP=`date +%s`;\
			EXERESULT=`$(RUNTEST_EXE_RUN)`;\
			if [ -e PerformanceDump.txt ]; then\
			  mv PerformanceDump.txt ../Builds/$(CONFIGURATION)/PerformanceDump.txt 2>/dev/null || true;\
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
