.PHONY:	tests all check clobber

UNAME := $(shell uname)

TEST_BUILD_DIR=	$(shell perl ScriptsLib/PrintTestBuildDir.pl)
TESTS=	        $(shell perl ScriptsLib/PrintTestsList.pl)

ifneq (,$(findstring CYGWIN,$(UNAME)))
EXEPATTERN=../Builds/Windows/$$ci/Test$$i/Test$$i.exe
else
EXEPATTERN=../Builds/$$ci/Test$$i
endif

# SOMEDAY SOON GET THIS FROM .PL FILE!!!
ifneq (,$(findstring CYGWIN,$(UNAME)))
CONFIGRUATIONS=		Debug-U-32 Release-U-32 Debug-U-64 Release-U-64
else
CONFIGRUATIONS=		DefaultConfiguration
endif

help:
	@echo "Help for making Stroika"
	@echo "Targets"
	@echo "    all:                    -    builds everything"
	@echo "    check:                  -    checks everything built properly"
	@echo "    clean:"
	@echo "    clobber:"
	@echo "    project-files:          -    builds project files for things like visual studio.net etc (NYI)"
	@echo "    tests:"
	@echo "    run-tests:"


all:		tests 

check:
	@perl checkall.pl

clean:
	@perl buildall.pl clean

clobber:
	@perl buildall.pl clobber

project-files:
	@echo "Building VisualStudio.Net-2013 project files..."
	@(cd Projects/VisualStudio.Net-2013 && perl BuildProjectsFiles.pl)

tests:
	@perl buildall.pl build

run-tests:
	@echo "Running Tests..."
	@for i in $(TESTS);\
	do\
	    TESTNAME=`perl ScriptsLib/PrintTestName.pl $$i`;\
		for ci in $(CONFIGRUATIONS);\
		do\
	        EXE=$(EXEPATTERN);\
	        TIMESTAMP=`date +%s`;\
	        EXERESULT=`$$EXE`;\
	        TIMESTAMP2=`date +%s`;\
		    EXERESULT=`echo $$EXERESULT | sed 's/ *$$//g'`;\
	        DIFFTIME=`expr $$TIMESTAMP2 - $$TIMESTAMP`;\
			if [ "$$DIFFTIME" -lt "10" ]; then\
	            DIFFTIME="0"$$DIFFTIME;\
			fi;\
	        echo "  [$$EXERESULT]  ($$DIFFTIME seconds)  [$$i]  $$TESTNAME  ($$EXE)";\
		done;\
	done

