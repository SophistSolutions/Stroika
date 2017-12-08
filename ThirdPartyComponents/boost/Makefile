#
# For docs on building Boost try:
#    http://www.boost.org/doc/libs/1_62_0/more/getting_started/windows.html
#
# and for docs on b2 and its options, b2 --help or http://www.boost.org/build/doc/html/bbv2/overview/invocation.html
#


SHELL=/bin/bash

.DEFAULT_GOAL := all
.PHONY:	all clean clobber check bootstrap_ configureAndBuild_

MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO?=	$(shell ../../ScriptsLib/GetDefaultShellVariable.sh ECHO)
PER_CONFIGURATION_BUILDS_DIR:=$(shell realpath --canonicalize-missing ../../Builds/$(CONFIGURATION)/ThirdPartyComponents/)
PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR:=../../IntermediateFiles/$(CONFIGURATION)/ThirdPartyComponents/boost/

-include $(shell pwd)/../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

VERSION=1_65_1
VERSION_WITH_DOTS=$(shell echo $(VERSION) | sed s/_/\./g)
ZIPPEDFILE=../Origs-Cache/boost_$(VERSION).tar.gz


ifneq ($(CONFIGURATION),)
ProjectPlatformSubdir			=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)
IncludeDebugSymbolsInLibraries	=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) IncludeDebugSymbolsInLibraries)
ENABLE_ASSERTIONS				=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ENABLE_ASSERTIONS)
ENABLE_GLIBCXX_DEBUG			=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ENABLE_GLIBCXX_DEBUG)
OptimizerFlag					=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) OptimizerFlag)
endif


ifneq (,$(findstring /cygdrive/,$(PATH)))
    UNAME := Cygwin
else
ifneq (,$(findstring WINDOWS,$(PATH)))
    UNAME := Windows
else
    UNAME := $(shell uname -s)
endif
endif


all:	
ifeq ($(CONFIGURATION),)
	@for i in `../../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory all CONFIGURATION=$$i;\
	done
else
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Building Stroika/ThirdPartyComponents/boost $(VERSION) {$(CONFIGURATION)}:"
	@$(MAKE) --silent CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR)
	@$(MAKE) --silent CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) --silent bootstrapIfNeeded_
	@$(MAKE) --silent CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) configureAndBuildIfNeeded_
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) check
endif


### Haven't found any backup/mirrors and this only stores the last few versions. @todo - find a better backup! 
FETCHURLS=
FETCHURLS+=https://dl.bintray.com/boostorg/release/$(VERSION_WITH_DOTS)/source/boost_$(VERSION).tar.gz


BUILDS_ROOT=../../Builds/

## just a smatering of test files to see if it looks like copied

## @todo FIX so UNIX AND WINDOWS BOTH LOSED THE boost-$(VERSION) crap!
ifeq (Unix,$(ProjectPlatformSubdir))
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/boost/config.hpp \
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/boost/process.hpp
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/boost-$(VERSION)/boost/config.hpp \
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/boost-$(VERSION)/boost/process.hpp
endif

## just a smatering of test files to see if it looks like copied
ifeq (Unix,$(ProjectPlatformSubdir))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/libboost_date_time.a
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
###	@todo - for 'vc141-mt-sgd' part - will need to dynamically compute this - at least the vc141 part (not sure what sgd is) --LGP 2017-12-07
TARGET_LIB_VCTOOLS_=vc141
ifeq (Unix,$(ENABLE_ASSERTIONS))
TARGET_LIB_STAG_=sgd
else
TARGET_LIB_STAG_=s
endif
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/libboost_date_time-$(TARGET_LIB_VCTOOLS_)-mt-$(TARGET_LIB_STAG_)-$(VERSION).lib
endif


$(ZIPPEDFILE):
	@MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) ../../ScriptsLib/WebGet.sh $(ZIPPEDFILE) $(FETCHURLS)
	

ORIGs $(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR):	$(ZIPPEDFILE)
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Extracting $(ZIPPEDFILE) to $@ ..."
	@mkdir -p $@
ifeq ($(shell uname -s),Darwin)
	@tar xf $(ZIPPEDFILE) -C $@ --strip-components 1
else
	@tar xf $(ZIPPEDFILE) --no-same-owner --directory $@ --strip-components=1
endif
	@echo done


clobber:	clean
ifeq ($(CONFIGURATION),)
	@rm -rf $(BUILDS_ROOT)*/ThirdPartyComponents/include/boost-$(VERSION)
	@rm -f $(BUILDS_ROOT)*/ThirdPartyComponents/lib/libboost_*-$(VERSION).*
else
	@rm -rf $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/boost-$(VERSION)
	@rm -f $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/libboost_*-$(VERSION).*
endif


clean:
ifeq ($(CONFIGURATION),)
	rm -rf =../../IntermediateFiles/*/ThirdPartyComponents/boost/
else
	@rm -rf $(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR)
endif


check:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
ifeq ($(PRODUCED_OUTPUT_ARTIFACTS),)
	$(error This makefile requires a recognized ProjectPlatformSubdir)
endif
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Checking..."
	@for i in $(PRODUCED_OUTPUT_ARTIFACTS) ; do \
		if [ ! -e $$i ]; then\
			$(ECHO) "      FAILED: missing $$i";\
			exit 1;\
		fi;\
	done
	@$(ECHO) " Stroika/ThirdPartyComponents/boost - [Succeeded]";


check-prerequisite-tools:
	@#none specific to boost


ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
B2=b2.exe
B2FullPath=$(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR)b2.exe
else
B2=b2
B2FullPath=$(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR)b2
endif

bootstrapIfNeeded_:	$(B2FullPath)

$(B2FullPath):
	$(MAKE) --no-print-directory bootstrap_ MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)

bootstrap_:
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Building Bootstap (b2) in $(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR); log2=BOOTSTRAP.OUT..."
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	@SL=`realpath ../../ScriptsLib` && cd $(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR) && (perl $$SL/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) "./bootstrap.bat" > BOOTSTRAP.OUT)
else
	@cd $(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR) && (./bootstrap.sh > BOOTSTRAP.OUT)
endif
	@echo done


### Run b2.exe --help
CONFIGURE_ARGS_=
CONFIGURE_ARGS_ +=	link=static
CONFIGURE_ARGS_ +=	runtime-link=static
CONFIGURE_ARGS_ +=	threading=multi
### omit some things unlikely to be intersted (mostly cuz already adopted into STL) in (due to build speed)
CONFIGURE_ARGS_ +=	--without-atomic
CONFIGURE_ARGS_ +=	--without-chrono
CONFIGURE_ARGS_ +=	--without-filesystem
CONFIGURE_ARGS_ +=	--without-python
CONFIGURE_ARGS_ +=	--without-regex
CONFIGURE_ARGS_ +=	--without-signals
CONFIGURE_ARGS_ +=	--without-system
CONFIGURE_ARGS_ +=	--without-wave

#output verbosity
#CONFIGURE_ARGS_ +=	--d0

ifeq (,$(findstring CYGWIN,$(shell uname)))
CONFIGURE_ARGS_ +=	--prefix=$(PER_CONFIGURATION_BUILDS_DIR)
else
CONFIGURE_ARGS_ +=	--prefix=$(shell cygpath --mixed $(PER_CONFIGURATION_BUILDS_DIR))
endif
ifeq (0,$(ENABLE_ASSERTIONS))
CONFIGURE_ARGS_ += variant=release
else
CONFIGURE_ARGS_ += variant=debug
endif
ifeq (1,$(ENABLE_GLIBCXX_DEBUG))
CONFIGURE_ARGS_ += define=_GLIBCXX_DEBUG
endif
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
CONFIGURE_ARGS_ += --toolset=msvc
### @todo - redo this using new ADDRESS_MODEL field in CONFIGURE file
ifneq (,$(findstring 32,$(CONFIGURATION)))
CONFIGURE_ARGS_ +=	address-model=32
endif
ifneq (,$(findstring 64,$(CONFIGURATION)))
CONFIGURE_ARGS_ +=	address-model=64
endif
endif
CONFIGURE_ARGS_ +=	install

ifndef IF_PARALLEL_MAKE_USE_NJOBS
#no easy way to know right number...
IF_PARALLEL_MAKE_USE_NJOBS=	-j10
endif

#on windows, we want parallel b2 make, but -j doesn't work with makefile
ifndef FORCE_PARALLEL_b2_MAKE
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
FORCE_PARALLEL_b2_MAKE=1
else
FORCE_PARALLEL_b2_MAKE=0
endif
endif

ifeq ($(FORCE_PARALLEL_b2_MAKE),1)
CONFIGURE_ARGS_ +=	$(IF_PARALLEL_MAKE_USE_NJOBS)
endif

configureAndBuildIfNeeded_:
	@$(MAKE) --silent $(PRODUCED_OUTPUT_ARTIFACTS)

$(PRODUCED_OUTPUT_ARTIFACTS):
	@$(MAKE) --silent configureAndBuild_

configureAndBuild_:
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Config+Build to $(PER_CONFIGURATION_BUILDS_DIR); log2=$(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR)/ConfigureAndBuild.OUT..."
	@
	@#Cannot use ifeq() macro stuff cuz thats done in first phase of make eval, and MAKEFLAGs is only defined later
	@export CFA="$(CONFIGURE_ARGS_)" && \
	cd  $(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR) && \
	if [ "-j" = "$(findstring -j,$(MAKEFLAGS))" ] ; then CFA="$$CFA $(IF_PARALLEL_MAKE_USE_NJOBS)"; fi && \
	(echo ./$(B2) $$CFA > ConfigureAndBuild.OUT) && \
	(sh -c "./$(B2) $$CFA >> ConfigureAndBuild.OUT")
	@echo done
