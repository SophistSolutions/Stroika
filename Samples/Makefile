ifneq ($(CONFIGURATION),)
ProjectPlatformSubdir	=	$(shell perl ../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)
qFeatureFlag_ATLMFC		=	$(shell perl ../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) qFeatureFlag_ATLMFC)
endif


.PHONY:	samples all check clobber skel

MAKE_INDENT_LEVEL?=$(MAKELEVEL)


SUBPROJECTS=

SUBPROJECTS+=	ArchiveUtility
ifeq ($(qFeatureFlag_ATLMFC), use-system)
SUBPROJECTS+=	ActiveLedIt
SUBPROJECTS+=	LedIt
SUBPROJECTS+=	LedLineIt
endif
# SOMEDAY SOON GET THIS FROM .PL FILE!!!
ifneq (,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
SUBPROJECTS+=	SimpleLedTest
endif
SUBPROJECTS+=	SSDPClient
SUBPROJECTS+=	SSDPServer
SUBPROJECTS+=	SimpleService
SUBPROJECTS+=	SystemPerformanceClient
SUBPROJECTS+=	WebServer




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
	@echo "    skel:                   -    INCOMPLETE-PRELIM - BUT IDEA IS MAKE CLONE OF SAMPLE TO NEW PROJECT"


all:		samples 


check:
	@../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && echo "Checking Stroika {$(CONFIGURATION)} Samples:";
	@for SUBPROJ in $(SUBPROJECTS) ; do \
		../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL) + 1)) && echo -n "Checking $$SUBPROJ... ";\
		(CONFIGURATION=$(CONFIGURATION) cd $$SUBPROJ && perl checkall.pl);\
	done	


clean:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory clean CONFIGURATION=$$i;\
	done
else
	@for SUBPROJ in $(SUBPROJECTS) ; do \
		(export CONFIGURATION=$(CONFIGURATION); export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL) + 1)); cd $$SUBPROJ && perl buildall.pl clean);\
	done	
endif


clobber:	clean
	@#skip cuz submnakefiles dont work
	@#for SUBPROJ in $(SUBPROJECTS) ; do 
	@#	(MAKEFLAGS= && cd $$SUBPROJ && perl buildall.pl clobber);
	@#done	


samples:
ifeq ($(CONFIGURATION),)
	@for i in `../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory samples CONFIGURATION=$(CONFIGURATION); MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL) + 1));\
	done
else
	@../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && echo "Building Stroika {$(CONFIGURATION)} Samples:"
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	@for SUBPROJ in $(SUBPROJECTS) ; do \
		(export CONFIGURATION=$(CONFIGURATION); export MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL) + 1)); export ECHO_BUILD_LINES=$(ECHO_BUILD_LINES); cd $$SUBPROJ && perl buildall.pl);\
    done
else
	@for SUBPROJ in $(SUBPROJECTS) ; do \
		$(MAKE) --directory ../IntermediateFiles/$(CONFIGURATION)/Samples_$$SUBPROJ --silent all;\
    done
endif
endif


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

	@mkdir -p $(SKEL_TRGPROJDIR)/$(SKEL_TRGPROJName)/Projects/Unix/
	@cp Skel/Makefile.skel $(SKEL_TRGPROJDIR)/$(SKEL_TRGPROJName)/Projects/Unix/Makefile;

	@mkdir -p $(SKEL_TRGPROJDIR)/IntermediateFiles/DefaultConfiguration;
	#ln -s ../../Projects/Unix/Makefile $(SKEL_TRGPROJName)/IntermediateFiles/DefaultConfiguration/Makefile;
	
	@mkdir -p $(SKEL_TRGPROJDIR)/$(SKEL_TRGPROJName)/Projects/VisualStudio.Net-2013/
	sed s/TEMPLATE/$(SKEL_TRGPROJName)/g < Skel/Projects/VisualStudio.Net-2013/TEMPLATE.sln > $(SKEL_TRGPROJDIR)/$(SKEL_TRGPROJName)/Projects/VisualStudio.Net-2013/$(SKEL_TRGPROJName).sln.tmp
	sed s/PATH2STROIKA/'$(SKEL_PATH2Stroika)'/g <$(SKEL_TRGPROJDIR)/$(SKEL_TRGPROJName)/Projects/VisualStudio.Net-2013/$(SKEL_TRGPROJName).sln.tmp > $(SKEL_TRGPROJDIR)/$(SKEL_TRGPROJName)/Projects/VisualStudio.Net-2013/$(SKEL_TRGPROJName).sln
	rm $(SKEL_TRGPROJDIR)/$(SKEL_TRGPROJName)/Projects/VisualStudio.Net-2013/$(SKEL_TRGPROJName).sln.tmp
	@sed s/TEMPLATE/$(SKEL_TRGPROJName)/g < Skel/Projects/VisualStudio.Net-2013/TEMPLATE.vcxproj > $(SKEL_TRGPROJDIR)/$(SKEL_TRGPROJName)/Projects/VisualStudio.Net-2013/$(SKEL_TRGPROJName).vcxproj
	@sed s/TEMPLATE/$(SKEL_TRGPROJName)/g < Skel/Projects/VisualStudio.Net-2013/TEMPLATE.vcxproj.filters > $(SKEL_TRGPROJDIR)/$(SKEL_TRGPROJName)/Projects/VisualStudio.Net-2013/$(SKEL_TRGPROJName).vcxproj.filters
