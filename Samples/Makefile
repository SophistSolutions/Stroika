.PHONY:	samples all check clobber skel

UNAME := $(shell uname)


# SOMEDAY SOON GET THIS FROM .PL FILE!!!
ifneq (,$(findstring CYGWIN,$(UNAME)))
CONFIGRUATIONS=		Debug-U-32 Release-U-32 Debug-U-64 Release-U-64
else
CONFIGRUATIONS=		DefaultConfiguration
endif


SKEL_SRCPROJDIR?=
SKEL_TRGPROJDir?=
SKEL_TRGPROJName?=

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


skel-usage_:
	@echo "\tUsage: make skel SKEL_SRCPROJDIR=XX SKEL_TRGPROJDIR=XX SKEL_TRGPROJName=XX"
	@echo "\t\tSKEL_SRCPROJDIR is the sample you wisht to clone"
	@echo "\t\tEG: make skel SKEL_SRCPROJDIR=WebServer SKEL_TRGPROJDIR=./SAMPLEOUT SKEL_TRGPROJName=CLONED_WS"

skel:
	@if [ "$(SKEL_SRCPROJDIR)" = "" ]; then\
		echo "make skel FAILED: need SKEL_SRCPROJDIR";\
	 	$(MAKE) --no-print-directory skel-usage_;\
	 	exit 1;\
	fi
	@if [ "$(SKEL_TRGPROJDIR)" = "" ]; then\
		echo "make skel FAILED: need SKEL_TRGPROJDIR";\
	 	$(MAKE) --no-print-directory skel-usage_;\
	 	exit 1;\
	fi
	@if [ "$(SKEL_TRGPROJName)" = "" ]; then\
		echo "make skel FAILED: need SKEL_TRGPROJName";\
	 	$(MAKE) --no-print-directory skel-usage_;\
	 	exit 1;\
	fi
	@if [ -e "$(SKEL_TRGPROJDIR)" ]; then\
		echo "make skel FAILED because directory $(SKEL_TRGPROJDIR) already exists";\
	 	exit 1;\
	fi
	@echo "Building Project $(SKEL_TRGPROJName) in $(SKEL_TRGPROJDIR) from sample $(SKEL_SRCPROJDIR)"
	@mkdir $(SKEL_TRGPROJDIR)
	@cp -r $(SKEL_SRCPROJDIR) $(SKEL_TRGPROJDIR)/$(SKEL_TRGPROJName);
	@mkdir -p $(SKEL_TRGPROJDIR)/IntermediateFiles/DefaultConfiguration;
	#ln -s ../../Projects/Linux/Makefile $(SKEL_TRGPROJName)/IntermediateFiles/DefaultConfiguration/Makefile;
	@cp Skel/Makefile.skel $(SKEL_TRGPROJDIR)/$(SKEL_TRGPROJName)/Projects/Linux/Makefile;

