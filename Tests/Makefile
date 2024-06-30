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


GTEST_ARGS_=
GTEST_ARGS_+= --gtest_brief


# Only applies to run-tests
export TEST_FAILURES_CAUSE_FAILED_MAKE?=1

#SSH_REMOTE_WARNINGS_OFF=-o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no
SSH_REMOTE_WARNINGS_OFF= -o StrictHostKeyChecking=no


# Consider exposing this explicitly (as we once did) in the configuration file
# But for now this is fine...LGP 2020-04-27
ifeq (qStroika_Foundation_Memory_PreferBlockAllocation=1,$(findstring qStroika_Foundation_Memory_PreferBlockAllocation=1,$(CPPFLAGS)))
USING_BLOCK_ALLOCATION?=1
else ifeq (qStroika_Foundation_Memory_PreferBlockAllocation=0,$(findstring qStroika_Foundation_Memory_PreferBlockAllocation=0,$(CPPFLAGS)))
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
endif

ifeq (memcheck,$(VALGRIND))
EXTRA_VALGRIND_OPTIONS=
VALGRIND_EXPANDED_SUPPRESSIONS:= $(foreach vs,$(VALGRIND_SUPPRESSIONS),--suppressions=$(vs))
VALGRIND_OPTIONS=-q --track-origins=yes --tool=memcheck --gen-suppressions=all --leak-check=full $(VALGRIND_EXPANDED_SUPPRESSIONS) $(EXTRA_VALGRIND_OPTIONS)
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
RUNTEST_EXE_RUN=ssh $(SSH_REMOTE_WARNINGS_OFF) $(REMOTE) ${RunPrefix} "$(VALEXTRARUN_) /tmp/$(BASEEXEPATTERN) ${GTEST_ARGS_}"
RUNTEST_EXE_RUN_PRINTLINE=scp $(EXEPATTERN)...; ssh $(REMOTE) ${RunPrefix} $(VALEXTRARUN_) /tmp/$(BASEEXEPATTERN) ${GTEST_ARGS_}
else ifeq (memcheck,$(VALGRIND))
RUNTEST_EXE_RUN=valgrind $(VALGRIND_OPTIONS) $(EXEPATTERN) ${GTEST_ARGS_}
RUNTEST_EXE_RUN_PRINTLINE=$(RUNTEST_EXE_RUN) ${GTEST_ARGS_}
else
ifeq (${RunPrefix},)
RUNTEST_EXE_RUN=$(EXEPATTERN) ${GTEST_ARGS_}
RUNTEST_EXE_RUN_PRINTLINE=$(EXEPATTERN) ${GTEST_ARGS_}
else
RUNTEST_EXE_RUN=${RunPrefix} $(EXEPATTERN) ${GTEST_ARGS_}
RUNTEST_EXE_RUN_PRINTLINE=${RunPrefix} $(EXEPATTERN) ${GTEST_ARGS_}
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
	@$(ECHO) "                                 [VALGRIND=memcheck, etc] to run with valgrind (EXTRA_VALGRIND_OPTIONS= can be used with valgrind)"
	@$(ECHO) "                                 OR VALGRIND_SUPPRESSIONS=\"Valgrind-MemCheck-Common.supp Valgrind-MemCheck-BlockAllocation.supp\""
	@$(ECHO) "                                 EG: VALGRIND_SUPPRESSIONS=\"Valgrind-MemCheck-Common.supp\" make VALGRIND=memcheck run-tests"


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
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Creating default visual-studio Tests project files:"
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1))  -n "Building VisualStudio.Net-2022 project files..."
	@cd Projects/VisualStudio.Net-2022 && ./BuildProjectsFiles
	@$(ECHO) done


tests:
ifeq ($(CONFIGURATION),)
	@for i in `$(StroikaRoot)ScriptsLib/GetConfigurations` ; do\
		$(MAKE) --no-print-directory tests CONFIGURATION=$$i || exit $$?;\
	done
else
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Building Stroika Tests {$(CONFIGURATION)}:"
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(BuildPlatform)))
	@(cd Projects/$(BuildPlatform) && ./BuildProjectsFiles)
endif
	@$(MAKE) --no-print-directory $(TESTS) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL) + 1))
endif

PREFIX_:=$(shell ../ScriptsLib/PrintLevelLeader $$(($(MAKE_INDENT_LEVEL) + 1)))


#### @todo make targets final EXE and don't need PHONY and can be even quicker running
.PHONY: $(TESTS)
$(TESTS):
ifeq ($(CONFIGURATION),)
	$(error CONFIGURATION variable required for this target)
endif
	@$(MAKE) --directory $@ -s all

run-tests:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations` ; do\
		$(MAKE) --no-print-directory run-tests CONFIGURATION=$$i;\
	done
else
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Running Stroika Tests {$(CONFIGURATION)}:"
	@#NOTE - not obvious we want the 'tr -d but makes it a bit more terse'
	@if [ "$(CrossCompiling)" = "true" ] && [ "$(REMOTE)" = "" ] ; then\
           $(ECHO) "${PREFIX_}Skiping cuz Cross-Compiling and REMOTE= not specified...";\
	else\
		FAILED=0;\
		for i in $(TESTS); do\
			TESTNAME=`ScriptsLib/PrintTestName $$i`;\
			$(RUNTEST_REMOTECOPYSETUP);\
			$(RUNTEST_REMOTECOPYSETUP2);\
			$(ECHO) "${PREFIX_} [$$i] $$TESTNAME - $(RUNTEST_EXE_RUN_PRINTLINE)";\
			EXEOUTPUT=`$(RUNTEST_EXE_RUN)`;\
			EXERESULT=$$?; \
			$(ECHO) "$$EXEOUTPUT" | sed -e 's/^/${PREFIX_}      /g' | tr -d "\r\n"; $(ECHO); \
			if [ "$$EXERESULT" -ne 0 ]; then\
			    FAILED=1;\
			fi;\
			if [ -e PerformanceDump.txt ]; then\
			  mv PerformanceDump.txt ../Builds/$(CONFIGURATION)/PerformanceDump.txt 2>/dev/null;\
			fi;\
		done;\
		if [ "$$FAILED" -ne 0 ]; then\
			if [ "$$TEST_FAILURES_CAUSE_FAILED_MAKE" -ne 0 ]; then\
				$(ECHO) "${PREFIX_}   ***SOME TESTS FAILED***";\
				exit 1;\
			else\
				$(ECHO) "${PREFIX_}   ***SOME TESTS FAILED*** (exit suppressed by TEST_FAILURES_CAUSE_FAILED_MAKE)";\
			fi;\
		fi\
    fi
endif


