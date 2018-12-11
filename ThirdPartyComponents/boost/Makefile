#
# For docs on building Boost try:
#    http://www.boost.org/doc/libs/1_62_0/more/getting_started/windows.html
#
# and for docs on b2 and its options, b2 --help or http://www.boost.org/build/doc/html/bbv2/overview/invocation.html
#

export StroikaRoot?=$(shell realpath ../..)/

ifneq ($(CONFIGURATION),)
	include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk


.DEFAULT_GOAL := all
.PHONY:	all clean clobber check bootstrap_ configureAndBuild_

PER_CONFIGURATION_BUILDS_DIR:=$(shell realpath --canonicalize-missing ../../Builds/$(CONFIGURATION)/ThirdPartyComponents/)/
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR:=$(shell realpath --canonicalize-missing ../../IntermediateFiles/$(CONFIGURATION)/ThirdPartyComponents/boost)/

OUTPUT_WORKDIR_PRETTYNAME="{StroikaRoot}/Inter.../Thir.../boost/"


VERSION=1_68_0
#VERSION=1_67_0_rc2
#VERSION=1_66_0
VERSION_WITH_DOTS=$(shell echo $(VERSION) | sed s/_/\./g)
VERSION_WITH_2DOTS=$(VERSION_WITH_DOTS)
ZIPPEDFILE=../Origs-Cache/boost_$(VERSION).tar.gz
###tmphack for pre-release 1.67 release
#VERSION_WITH_2DOTS=1.67.0


# Needed for $(PRODUCED_OUTPUT_ARTIFACTS): rule (is there a simple way to disable parallel for just that one rule)?
.NOTPARALLEL:



ifneq (,$(findstring /cygdrive/,$(PATH)))
    UNAME := Cygwin
else
ifneq (,$(findstring WINDOWS,$(PATH)))
    UNAME := Windows
else
    UNAME := $(shell uname -s)
endif
endif


# Found true in 2018-12-10 - MacOS XCode 10 - GNU Make 3.81
#   make CONFIGURATION=Debug /Users/lewis/Sandbox/StroikaDev/IntermediateFiles/Debug/ThirdPartyComponents/boost WORKS
#   but  make CONFIGURATION=Debug /Users/lewis/Sandbox/StroikaDev/IntermediateFiles/Debug/ThirdPartyComponents/boost/ FAILS
#	though the argument matches exactly the PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR
ifeq ($(UNAME),Darwin)
WEIRD_MACOS_MAKEFILE_DIR_BUG=1
endif



all:	
ifeq ($(CONFIGURATION),)
	@for i in `../../ScriptsLib/GetConfigurations` ; do\
		$(MAKE) --no-print-directory all CONFIGURATION=$$i;\
	done
else
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents/boost Build $(VERSION) {$(CONFIGURATION)}:"
	@../../ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@$(MAKE) --silent MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) CURRENT $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) check
endif


### Haven't found any backup/mirrors and this only stores the last few versions. @todo - find a better backup! 
FETCHURLS=
FETCHURLS+=https://dl.bintray.com/boostorg/release/$(VERSION_WITH_DOTS)/source/boost_$(VERSION).tar.gz
FETCHURLS+=https://github.com/boostorg/boost/archive/boost-$(VERSION_WITH_DOTS).tar.gz
#FETCHURLS+=https://dl.bintray.com/boostorg/release/$(VERSION_WITH_2DOTS)/source/boost_$(VERSION).tar.gz
###FETCHURLS+=https://dl.bintray.com/boostorg/beta/1.67.0.beta.1/source/boost_$(VERSION).tar.gz



BUILDS_ROOT=../../Builds/

## just a smatering of test files to see if it looks like copied

## @todo FIX so UNIX AND WINDOWS BOTH LOSED THE boost-$(VERSION) crap!
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/boost/config.hpp \
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/boost/process.hpp

## just a smatering of test files to see if it looks like copied
ifeq (Unix, $(ProjectPlatformSubdir))
LIBSUFFIX_=a
else
LIBSUFFIX_=lib
endif
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/libboost_date_time.$(LIBSUFFIX_)


$(ZIPPEDFILE):
	@MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) ../../ScriptsLib/WebGet.sh $(ZIPPEDFILE) $(FETCHURLS)
	

ORIGs CURRENT $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR):	$(ZIPPEDFILE)
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "boost ${VERSION} - Extracting $(ZIPPEDFILE) to $@"` "... "
	@mkdir -p $@
ifeq ($(shell uname -s),Darwin)
	@tar xf $(ZIPPEDFILE) -C $@ --strip-components 1
else ifeq (CYGWIN,$(findstring CYGWIN,$(shell uname)))
	@PWD=`pwd` tar xf $(ZIPPEDFILE) --no-same-owner --directory `realpath --relative-to=$$PWD $@` --strip-components=1
else
	@tar xf $(ZIPPEDFILE) --no-same-owner --directory $@ --strip-components=1
endif


clobber:	clean
ifeq ($(CONFIGURATION),)
	@rm -rf $(BUILDS_ROOT)*/ThirdPartyComponents/include/boost
	@rm -f $(BUILDS_ROOT)*/ThirdPartyComponents/lib/libboost_*
	@rm -rf $(BUILDS_ROOT)*/ThirdPartyComponents/share/boost-build
	@rm -rf CURRENT
else
	@rm -rf $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/boost
	@rm -f $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/libboost_*
	@rm -rf $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/share/boost-build
endif


clean:
ifeq ($(CONFIGURATION),)
	@rm -rf =../../IntermediateFiles/*/ThirdPartyComponents/boost/
else
	@rm -rf $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)
endif


check:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
ifeq ($(PRODUCED_OUTPUT_ARTIFACTS),)
	$(error This makefile requires a recognized ProjectPlatformSubdir)
endif
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Checking ... "
	@for i in $(PRODUCED_OUTPUT_ARTIFACTS) ; do \
		if [ ! -e $$i ]; then\
			$(ECHO) "      FAILED: missing $$i";\
			exit 1;\
		fi;\
	done
	@$(ECHO) " Stroika/ThirdPartyComponents/boost -   [Succeeded]";


check-prerequisite-tools:
	@#none specific to boost



$(PRODUCED_OUTPUT_ARTIFACTS):
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
ifeq ($(WEIRD_MACOS_MAKEFILE_DIR_BUG),1)
	@$(MAKE) -- $(shell realpath --canonicalize-missing ../../IntermediateFiles/$(CONFIGURATION)/ThirdPartyComponents/boost)

else
	@$(MAKE) --silent $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)
endif
	@$(MAKE) --silent bootstrapIfNeeded_
	@$(MAKE) --silent configureAndBuild_



#unsure why windows and unix builds go to such different places, but they do. Probably not too hard to unify them, but not a priority
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
B2FullPath=$(PER_CONFIGURATION_BUILDS_DIR)bin/b2.exe
else
B2FullPath=$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)b2
endif

bootstrapIfNeeded_:	$(B2FullPath)

$(B2FullPath):
	$(MAKE) --no-print-directory bootstrap_

bootstrap_:
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "boost ${VERSION} - Bootstrapping - see ${OUTPUT_WORKDIR_PRETTYNAME}BOOTSTRAP.OUT"` "... "
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	@### From http://www.boost.org/doc/libs/1_66_0/more/getting_started/windows.html - section 5.2
	@SL=`realpath ../../ScriptsLib` && cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR) && (perl $$SL/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) "cd tools\build && bootstrap.bat" > BOOTSTRAP.OUT)
	@SL=`realpath ../../ScriptsLib` && cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR) && (perl $$SL/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) "cd tools\build && b2 install --prefix=$(shell cygpath --mixed $(PER_CONFIGURATION_BUILDS_DIR))" >> BOOTSTRAP.OUT)
else
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR) && (echo "./bootstrap.sh --prefix=$(PER_CONFIGURATION_BUILDS_DIR)" > BOOTSTRAP.OUT)
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR) && (./bootstrap.sh --prefix=$(PER_CONFIGURATION_BUILDS_DIR) >> BOOTSTRAP.OUT)
endif


### Run b2.exe --help
B2_CONFIGURATION_ARGS=
B2_CONFIGURATION_ARGS +=	link=static
B2_CONFIGURATION_ARGS +=	runtime-link=static
B2_CONFIGURATION_ARGS +=	threading=multi
B2_CONFIGURATION_ARGS +=	--layout=system

### omit some things unlikely to be intersted (mostly cuz already adopted into STL) in (due to build speed)
B2_CONFIGURATION_ARGS +=	--without-atomic
B2_CONFIGURATION_ARGS +=	--without-chrono
B2_CONFIGURATION_ARGS +=	--without-filesystem
B2_CONFIGURATION_ARGS +=	--without-python
B2_CONFIGURATION_ARGS +=	--without-regex
B2_CONFIGURATION_ARGS +=	--without-signals
#B2_CONFIGURATION_ARGS +=	--without-system
B2_CONFIGURATION_ARGS +=	--without-wave

### shorter paths avoids problems building on windows (abbrevpaths not enuf on windows when root dir is maybe 100 chars, so use --hash which works better)
#B2_CONFIGURATION_ARGS +=	--abbreviate-paths
B2_CONFIGURATION_ARGS +=	--hash

#output verbosity (default is -d1, but use -d0 cuz faster)
B2_CONFIGURATION_ARGS +=	-d0
#B2_CONFIGURATION_ARGS +=	-d+2
#SHOW VARIABLES - B2_CONFIGURATION_ARGS +=	-d7

ifeq (,$(findstring CYGWIN,$(shell uname)))
B2_CONFIGURATION_ARGS +=	--prefix=$(PER_CONFIGURATION_BUILDS_DIR)
else
B2_CONFIGURATION_ARGS +=	--prefix=$(shell cygpath --mixed $(PER_CONFIGURATION_BUILDS_DIR))
endif
ifeq (0,$(ENABLE_ASSERTIONS))
B2_CONFIGURATION_ARGS += variant=release
else
B2_CONFIGURATION_ARGS += variant=debug
endif
ifeq (1,$(ENABLE_GLIBCXX_DEBUG))
B2_CONFIGURATION_ARGS += define=_GLIBCXX_DEBUG
endif
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
B2_CONFIGURATION_ARGS += toolset=msvc
### @todo - redo this using new ADDRESS_MODEL field in CONFIGURE file
ifeq ($(ARCH),x86)
B2_CONFIGURATION_ARGS +=	address-model=32
else ifeq ($(ARCH),x86_64)
B2_CONFIGURATION_ARGS +=	address-model=64
endif
else
ifeq ($(shell uname -s),Darwin)
B2_CONFIGURATION_ARGS +=	toolset=darwin-$(ARCH)
else
B2_CONFIGURATION_ARGS +=	toolset=gcc-$(ARCH)
endif
endif

ifndef IF_PARALLEL_MAKE_USE_NJOBS
#no easy way to know right number...
IF_PARALLEL_MAKE_USE_NJOBS=	-j5
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
B2_CONFIGURATION_ARGS +=	$(IF_PARALLEL_MAKE_USE_NJOBS)
endif


configureAndBuild_:
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "boost ${VERSION} - Config+Build; see ${OUTPUT_WORKDIR_PRETTYNAME}ConfigureAndBuild.OUT"` "..."
	@
	@#https://stroika.atlassian.net/browse/STK-633
ifeq (CYGWIN,$(findstring CYGWIN,$(shell uname)))
	@MAXCHAR=120 && PWD=`realpath --canonicalize-missing $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)` && if [[ $${#PWD} -gt $$MAXCHAR ]] ; then $(ECHO) && ../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) "WARNING: STRLEN(boost-build-dir) == $${#PWD}: sometimes if its more than $$MAXCHAR characters on windows it may cause boost build problem (https://stroika.atlassian.net/browse/STK-633)" && ../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n; fi;
endif
	@##NB: using ; separator cuz / appears in file names (sed call)
	@#@todo - document why we need to patch project-config.jam file - why cannot specifiy ARCH/COMPILER externally?
ifneq (CYGWIN,$(findstring CYGWIN,$(shell uname)))
ifeq ($(shell uname -s),Darwin)
		cd  $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR);\
		cp project-config.jam project-config.jam.ORIG;\
		gsed -i 's;using darwin; using darwin : $(ARCH) : $(CC);' project-config.jam;
else
		cd  $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR);\
		cp project-config.jam project-config.jam.ORIG;\
		sed -i 's;using gcc; using gcc : $(ARCH) : $(CC);' project-config.jam;
endif
endif
	@#
	@### NOTE - sometimes if this starts failing (e.g. *** [Makefile:288: configureAndBuild_] Error 1), from a DOS shell, type del %tmp%\b2_*.cmd
	@#
	@#Cannot use ifeq() macro stuff cuz thats done in first phase of make eval, and MAKEFLAGs is only defined later
	@rm -f $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)ConfigureAndBuild.OUT
	@export CFA="$(B2_CONFIGURATION_ARGS)" && \
		cd  $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR) && \
		(echo $(B2FullPath) $$CFA install >> ConfigureAndBuild.OUT) && \
		(sh -c "$(B2FullPath) $$CFA install >> ConfigureAndBuild.OUT")
