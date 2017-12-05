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
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Building Stroika/ThirdPartyComponents/boost $(VERSION):"
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR)
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) --silent bootstrapIfNeeded_
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) configureAndBuild_


### Haven't found any backup/mirrors and this only stores the last few versions. @todo - find a better backup! 
FETCHURLS=
FETCHURLS+=https://dl.bintray.com/boostorg/release/$(VERSION_WITH_DOTS)/source/boost_$(VERSION).tar.gz


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
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@echo todo - delete just right stuff

clean:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@rm -rf $(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR)

check:
	@echo NYI


ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
B2=$(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR)b2.exe
else
B2=$(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR)b2
endif

bootstrapIfNeeded_:	$(B2)

$(B2):
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

configureAndBuild_:
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Configuring and Building and installing to $(PER_CONFIGURATION_BUILDS_DIR); log2=$(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR)/ConfigureAndBuild.OUT..."
	@cd $(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR) && (echo b2.exe $(CONFIGURE_ARGS_) > ConfigureAndBuild.OUT)
	@cd $(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR) && (sh -c "./b2.exe $(CONFIGURE_ARGS_) >> ConfigureAndBuild.OUT")
	@echo done
