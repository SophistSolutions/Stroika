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
	@buildall.pl clean

clobber:
	@buildall.pl clobber

project-files:
	@echo "Building VisualStudio.Net-2013 project files..."
	@(cd Projects/VisualStudio.Net-2013 && perl BuildProjectsFiles.pl)

tests:
	@perl buildall.pl build

run-tests:	tests
	#echo TESTBUIDLR = $(TEST_BUILD_DIR)
	#echo TESTS = $(TESTS)
	@perl Run.pl
