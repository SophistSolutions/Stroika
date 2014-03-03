.PHONY:	samples all check clobber

UNAME := $(shell uname)


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
	@echo "    project-files:          -    builds project files for things like visual studio.net etc"
	@echo "    samples:"


all:		samples 

check:
	@perl checkall.pl

clean:
	@perl buildall.pl clean

clobber:
	@perl buildall.pl clobber

samples:
	@perl buildall.pl build
