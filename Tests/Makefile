.PHONY:	tests all check clobber


TEST_BUILD_DIR=	$(shell perl ScriptsLib/PrintTestBuildDir.pl)
TESTS=	        $(shell perl ScriptsLib/PrintTestsList.pl)


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
	@for i in $(TESTS);\
	do\
	    EXE=../Builds/DefaultConfiguration/Test$$i;\
	    TESTNAME=`perl ScriptsLib/PrintTestName.pl $$i`;\
	    TIMESTAMP=`date +%s`;\
	    EXERESULT=`$$EXE`;\
	    TIMESTAMP2=`date +%s`;\
	    DIFFTIME=`expr $$TIMESTAMP2 - $$TIMESTAMP`;\
	    echo "  [$$EXERESULT]  ($$DIFFTIME seconds)  [$$i]  $$TESTNAME  ($$EXE)";\
	done

