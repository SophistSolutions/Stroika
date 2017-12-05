### SUPER ROUGH EXPERIMENTAL DRAFT OF BOOST SUPPORT
ifneq ($(CONFIGURATION),)
ProjectPlatformSubdir	=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)
endif

SHELL=/bin/bash

.DEFAULT_GOAL := all
.PHONY:	all clean clobber check bootstrap_ configure_ build_

MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO?=	$(shell ../../ScriptsLib/GetDefaultShellVariable.sh ECHO)
PER_CONFIGURATION_BUILDS_DIR:=../../Builds/$(CONFIGURATION)/ThirdPartyComponents/
PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR:=../../IntermediateFiles/$(CONFIGURATION)/ThirdPartyComponents/boost/

-include $(shell pwd)/../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

VERSION=1_65_1
VERSION_WITH_DOTS=$(shell echo $(VERSION) | sed s/_/\./g)
ZIPPEDFILE=../Origs-Cache/boost_$(VERSION).tar.gz


all:	
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	$(MAKE) --no-print-directory $(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR)
	$(MAKE) --no-print-directory --silent bootstrapIfNeeded_
	$(MAKE) --no-print-directory configure_
	$(MAKE) --no-print-directory build_


FETCHURLS=
FETCHURLS+=https://dl.bintray.com/boostorg/release/$(VERSION_WITH_DOTS)/source/boost_$(VERSION).tar.gz


$(ZIPPEDFILE):
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ../../ScriptsLib/WebGet.sh $(ZIPPEDFILE) $(FETCHURLS)
	

$(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR):	$(ZIPPEDFILE)
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n "Extracting $(ZIPPEDFILE) to $(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR)..."
	@mkdir -p $(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR)
ifeq ($(shell uname -s),Darwin)
	@tar xf $(ZIPPEDFILE) -C $(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR) --strip-components 1
else
	@tar xf $(ZIPPEDFILE) --no-same-owner --directory $(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR) --strip-components=1
endif
	@echo done


	

clobber:	clean
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@echo todo - delete just right stuff

clean:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	rm -rf $(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR)



ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
B2=$(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR)b2.exe
else
B2=$(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR)b2
endif

bootstrapIfNeeded_:	$(B2)

$(B2):
	$(MAKE) --no-print-directory bootstrap_

bootstrap_:
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n "Building Bootstap (b2) in $(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR); log2=BOOTSTRAP.OUT..."
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	@SL=`realpath ../../ScriptsLib` && cd $(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR) && (perl $$SL/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) "./bootstrap.bat" > BOOTSTRAP.OUT)
else
	@cd $(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR) && (./bootstrap.sh > BOOTSTRAP.OUT)
endif
	@echo done


configure_:


build_:
