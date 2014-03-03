.PHONY:	samples all check clobber skel

UNAME := $(shell uname)


# SOMEDAY SOON GET THIS FROM .PL FILE!!!
ifneq (,$(findstring CYGWIN,$(UNAME)))
CONFIGRUATIONS=		Debug-U-32 Release-U-32 Debug-U-64 Release-U-64
else
CONFIGRUATIONS=		DefaultConfiguration
endif


SKEL_SRCPROJDIR=WebServer
SKEL_TRGPROJName=Clone-Of-$(SKEL_SRCPROJDIR)


help:
	@echo "Help for making Stroika"
	@echo "Targets"
	@echo "    all:                    -    builds everything"
	@echo "    check:                  -    checks everything built properly"
	@echo "    clean:"
	@echo "    clobber:"
	@echo "    project-files:          -    builds project files for things like visual studio.net etc"
	@echo "    samples:"
	@echo "    skel:				-	INCOMPLETE-PRELIM - BUT IDEA IS MAKE CLONE OF SAMPLE TO NEW PROJECT"


all:		samples 

check:
	@perl checkall.pl

clean:
	@perl buildall.pl clean

clobber:
	@perl buildall.pl clobber

samples:
	@perl buildall.pl build

skel:
	@if [ -e "$(SKEL_TRGPROJName)" ]; then\
	  echo "make skel FAILED because directory $(SKEL_TRGPROJName) already exists";\
	  exit 1;\
	else\
	  cp -r $(SKEL_SRCPROJDIR) $(SKEL_TRGPROJName);\
	  mkdir -p $(SKEL_TRGPROJName)/IntermediateFiles/DefaultConfiguration;\
	  ln -s ../../Projects/Linux/Makefile $(SKEL_TRGPROJName)/IntermediateFiles/DefaultConfiguration/Makefile;\
	  cp Skel/Makefile.skel $(SKEL_TRGPROJName)/Projects/Linux/Makefile;\
	fi

