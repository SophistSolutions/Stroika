.PHONY:	samples all check clobber skel

UNAME := $(shell uname)


SUBPROJECTS=\
	ActiveLedIt\
	LedIt\
	LedLineIt\
	SimpleLedTest\
	SSDPClient\
	SSDPServer\
	SimpleService\
	SystemPerformanceClient\
	WebServer	


# SOMEDAY SOON GET THIS FROM .PL FILE!!!
ifneq (,$(findstring CYGWIN,$(UNAME)))
CONFIGRUATIONS=		Debug-U-32 Release-U-32 Debug-U-64 Release-U-64
else
CONFIGRUATIONS=		DefaultConfiguration
endif


SKEL_SRCPROJDIR?=
SKEL_TRGPROJDir?=
SKEL_TRGPROJName?=
SKEL_PATH2Stroika?=

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
	@for SUBPROJ in $(SUBPROJECTS) ; do \
		(MAKEFLAGS= && cd $$SUBPROJ && perl buildall.pl clean);\
	done	

clobber:	clean
	@#skip cuz submnakefiles dont work
	@#for SUBPROJ in $(SUBPROJECTS) ; do 
	@#	(MAKEFLAGS= && cd $$SUBPROJ && perl buildall.pl clobber);
	@#done	

samples:
	@#NOTE - MAKEFLAGS trick to avoid warning about -j builds til we get rid of perl stuff
	@for SUBPROJ in $(SUBPROJECTS) ; do \
		(MAKEFLAGS= && cd $$SUBPROJ && perl buildall.pl);\
    	done	

skel-usage_:
	@echo "\tUsage: make skel SKEL_SRCPROJDIR=XX SKEL_TRGPROJDIR=XX SKEL_TRGPROJName=XX SKEL_PATH2Stroika=XX"
	@echo "\t\tSKEL_SRCPROJDIR is the sample you wisht to clone"
	@echo "\t\tEG: make skel SKEL_SRCPROJDIR=WebServer SKEL_TRGPROJDIR=./SAMPLEOUT SKEL_TRGPROJName=CLONED_WS SKEL_PATH2Stroika=..\/..\/..\/..\/.."

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
	@if [ "$(SKEL_PATH2Stroika)" = "" ]; then\
		echo "make skel FAILED: need SKEL_PATH2Stroika";\
	 	$(MAKE) --no-print-directory skel-usage_;\
	 	exit 1;\
	fi
	@if [ -e "$(SKEL_TRGPROJDIR)" ]; then\
		echo "make skel FAILED because directory $(SKEL_TRGPROJDIR) already exists";\
	 	exit 1;\
	fi
	@echo "Building Project $(SKEL_TRGPROJName) in $(SKEL_TRGPROJDIR) from sample $(SKEL_SRCPROJDIR)"
	
	@mkdir -p $(SKEL_TRGPROJDIR)/$(SKEL_TRGPROJName)
	@mkdir -p $(SKEL_TRGPROJDIR)/$(SKEL_TRGPROJName)/Sources
	@cp -r $(SKEL_SRCPROJDIR)/Sources $(SKEL_TRGPROJDIR)/$(SKEL_TRGPROJName)/

	@mkdir -p $(SKEL_TRGPROJDIR)/$(SKEL_TRGPROJName)/Projects/Linux/
	@cp Skel/Makefile.skel $(SKEL_TRGPROJDIR)/$(SKEL_TRGPROJName)/Projects/Linux/Makefile;

	@mkdir -p $(SKEL_TRGPROJDIR)/IntermediateFiles/DefaultConfiguration;
	#ln -s ../../Projects/Linux/Makefile $(SKEL_TRGPROJName)/IntermediateFiles/DefaultConfiguration/Makefile;
	
	@mkdir -p $(SKEL_TRGPROJDIR)/$(SKEL_TRGPROJName)/Projects/VisualStudio.Net-2013/
	sed s/TEMPLATE/$(SKEL_TRGPROJName)/g < Skel/Projects/VisualStudio.Net-2013/TEMPLATE.sln > $(SKEL_TRGPROJDIR)/$(SKEL_TRGPROJName)/Projects/VisualStudio.Net-2013/$(SKEL_TRGPROJName).sln.tmp
	sed s/PATH2STROIKA/'$(SKEL_PATH2Stroika)'/g <$(SKEL_TRGPROJDIR)/$(SKEL_TRGPROJName)/Projects/VisualStudio.Net-2013/$(SKEL_TRGPROJName).sln.tmp > $(SKEL_TRGPROJDIR)/$(SKEL_TRGPROJName)/Projects/VisualStudio.Net-2013/$(SKEL_TRGPROJName).sln
	rm $(SKEL_TRGPROJDIR)/$(SKEL_TRGPROJName)/Projects/VisualStudio.Net-2013/$(SKEL_TRGPROJName).sln.tmp
	@sed s/TEMPLATE/$(SKEL_TRGPROJName)/g < Skel/Projects/VisualStudio.Net-2013/TEMPLATE.vcxproj > $(SKEL_TRGPROJDIR)/$(SKEL_TRGPROJName)/Projects/VisualStudio.Net-2013/$(SKEL_TRGPROJName).vcxproj
	@sed s/TEMPLATE/$(SKEL_TRGPROJName)/g < Skel/Projects/VisualStudio.Net-2013/TEMPLATE.vcxproj.filters > $(SKEL_TRGPROJDIR)/$(SKEL_TRGPROJName)/Projects/VisualStudio.Net-2013/$(SKEL_TRGPROJName).vcxproj.filters

