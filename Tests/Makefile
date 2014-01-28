.PHONY:	tests all check clobber


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
	@perl Run.pl
