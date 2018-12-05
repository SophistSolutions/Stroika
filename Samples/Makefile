export StroikaRoot=$(shell realpath ../)/

ifneq ($(CONFIGURATION),)
	include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk
	ProjectPlatformSubdir	=	$(shell perl $(StroikaRoot)ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)
	qFeatureFlag_ATLMFC		=	$(shell perl $(StroikaRoot)ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) qFeatureFlag_ATLMFC)
endif

SHELL=/bin/bash

.PHONY:	samples all check clobber skel

MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO?=$(shell $(StroikaRoot)ScriptsLib/GetDefaultShellVariable.sh ECHO)
ECHO_BUILD_LINES?=0


SUBPROJECTS=

ifeq ($(qFeatureFlag_ATLMFC), use-system)
SUBPROJECTS+=	ActiveLedIt
endif
SUBPROJECTS+=	ArchiveUtility
SUBPROJECTS+=	Containers
ifeq ($(qFeatureFlag_ATLMFC), use-system)
SUBPROJECTS+=	LedIt
SUBPROJECTS+=	LedLineIt
endif
SUBPROJECTS+=	Serialization
# SOMEDAY SOON GET IF BUILT FROM CONFIGURATION FILE!!!
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
SUBPROJECTS+=	SimpleLedTest
endif
SUBPROJECTS+=	Service
SUBPROJECTS+=	SSDPClient
SUBPROJECTS+=	SSDPServer
SUBPROJECTS+=	SystemPerformanceClient
SUBPROJECTS+=	Traceroute
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
ifeq ($(CONFIGURATION),)
	@for i in `$(StroikaRoot)ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory check CONFIGURATION=$$i || exit $$?;\
	done
else
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Checking Stroika Samples {$(CONFIGURATION)}:"
	@for SUBPROJ in $(SUBPROJECTS) ; do \
		$(MAKE) --directory $$SUBPROJ --no-print-directory check CONFIGURATION=$(CONFIGURATION) || exit $$?;\
    done
endif


clean:
ifeq ($(CONFIGURATION),)
	@for i in `$(StroikaRoot)ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory clean CONFIGURATION=$$i;\
	done
else
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Cleaning Stroika Samples {$(CONFIGURATION)} ... ";
	@for SUBPROJ in $(SUBPROJECTS) ; do \
		$(MAKE) --directory $$SUBPROJ --silent clean CONFIGURATION=$(CONFIGURATION) 2> /dev/null || true;\
    done
	@$(ECHO) done
endif


clobber:
ifeq ($(CONFIGURATION),)
	@for i in `$(StroikaRoot)ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory clobber CONFIGURATION=$$i;\
	done
else
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Clobbering Stroika Samples {$(CONFIGURATION)} ... ";
	@for SUBPROJ in $(SUBPROJECTS) ; do \
		$(MAKE) --directory $$SUBPROJ --silent clobber CONFIGURATION=$(CONFIGURATION) 2> /dev/null || true;\
    done
	@$(ECHO) done
endif


samples:
ifeq ($(CONFIGURATION),)
	@for i in `$(StroikaRoot)ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory samples CONFIGURATION=$$i MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL) + 1)) || exit $$?;\
	done
else
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Building Stroika Samples {$(CONFIGURATION)}:"
	@$(MAKE) --no-print-directory $(SUBPROJECTS) CONFIGURATION=$(CONFIGURATION) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL) + 1))
endif


.PHONY: $(SUBPROJECTS)
$(SUBPROJECTS):
ifeq ($(CONFIGURATION),)
	$(error CONFIGURATION variable required for this target)
endif
	@$(MAKE) --directory $@ --silent all CONFIGURATION=$(CONFIGURATION) ECHO_BUILD_LINES=$(ECHO_BUILD_LINES) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)


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
