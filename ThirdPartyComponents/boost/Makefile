#
# For docs on building Boost try:
#    http://www.boost.org/doc/libs/1_62_0/more/getting_started/windows.html
#
# and for docs on b2 and its options, b2 --help or http://www.boost.org/build/doc/html/bbv2/overview/invocation.html
#
#	TODO:
#		@todo	NOTE - try for $(word 1,$(PRODUCED_OUTPUT_ARTIFACTS)) and $(wordlist 2, 9999, $(PRODUCED_OUTPUT_ARTIFACTS)):	$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS))
#				(repeated in most ThirdPartyComponents makefiles)
#				doesn't work if the first produced output artifact is fine, but the others got deleted. No BIGGIE, as you can just make clobber in that
#				exceedingly rare case; but certainly a defect of this approach.
#

StroikaRoot=$(abspath ../..)/

ifneq ($(CONFIGURATION),)
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk


.PHONY:	all clean clobber check bootstrap_ configureAndBuild_

PER_CONFIGURATION_BUILDS_DIR:=$(shell realpath --canonicalize-missing ../../Builds/$(CONFIGURATION)/ThirdPartyComponents/)/

#lose trailing / on PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_ because on macos make THATDIR doesn't work with trailing slash (old WEIRD_MACOS_MAKEFILE_DIR_BUG -  MacOS XCode 10 - GNU Make 3.81)
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_:=$(shell realpath --canonicalize-missing ../../IntermediateFiles/$(CONFIGURATION)/ThirdPartyComponents/boost)

#OUTPUT_WORKDIR_PRETTYNAME="{StroikaRoot}Inter.../Thir.../boost/"
OUTPUT_WORKDIR_PRETTYNAME="$(shell $(StroikaRoot)ScriptsLib/SubstituteBackVariables "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/")"


VERSION=1_69_0
#VERSION=1_68_0
#VERSION=1_67_0_rc2
#VERSION=1_66_0
VERSION_WITH_DOTS=$(shell echo $(VERSION) | sed s/_/\./g)
VERSION_WITH_2DOTS=$(VERSION_WITH_DOTS)
ZIPPEDFILE=../Origs-Cache/boost_$(VERSION).tar.gz
###tmphack for pre-release 1.67 release
#VERSION_WITH_2DOTS=1.67.0



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
	@for i in `../../ScriptsLib/GetConfigurations` ; do\
		$(MAKE) --no-print-directory all CONFIGURATION=$$i;\
	done
else
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents/boost Build $(VERSION) {$(CONFIGURATION)}:"
	@../../ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@#make zippedfile even though a dependency on CURRENT and PRODUCED_ARTIFACTS for parallel-make so completed before either starts (else one sees file partly downloaded and proceeeds)
	@$(MAKE) --silent MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $(ZIPPEDFILE)
	@$(MAKE) --silent MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) CURRENT $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) check
endif


### Haven't found any backup/mirrors and this only stores the last few versions. @todo - find a better backup! 
FETCHURLS=
FETCHURLS+=https://dl.bintray.com/boostorg/release/$(VERSION_WITH_DOTS)/source/boost_$(VERSION).tar.gz
FETCHURLS+=https://github.com/boostorg/boost/archive/boost-$(VERSION_WITH_DOTS).tar.gz
#FETCHURLS+=https://dl.bintray.com/boostorg/release/$(VERSION_WITH_2DOTS)/source/boost_$(VERSION).tar.gz
###FETCHURLS+=https://dl.bintray.com/boostorg/beta/1.67.0.beta.1/source/boost_$(VERSION).tar.gz




## just a smatering of test files to see if it looks like copied
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(PER_CONFIGURATION_BUILDS_DIR)include/boost/config.hpp				\
	$(PER_CONFIGURATION_BUILDS_DIR)include/boost/process.hpp			\
	$(PER_CONFIGURATION_BUILDS_DIR)include/boost/type_traits.hpp

## just a smatering of test files to see if it looks like copied
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/libboost_date_time$(LIB_SUFFIX)	\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/libboost_filesystem$(LIB_SUFFIX)	\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/libboost_system$(LIB_SUFFIX)


$(ZIPPEDFILE):
	@MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) ../../ScriptsLib/WebGet $(ZIPPEDFILE) $(FETCHURLS)
	

ORIGs CURRENT $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_):	$(ZIPPEDFILE)
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "boost ${VERSION} - Extracting $(ZIPPEDFILE) to $@"` "... "
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
	@rm -rf $(StroikaRoot)Builds/*/ThirdPartyComponents/include/boost
	@rm -f $(StroikaRoot)Builds/*/ThirdPartyComponents/lib/libboost_*
	@rm -rf $(StroikaRoot)Builds/*/ThirdPartyComponents/share/boost-build
	@rm -rf CURRENT
else
	@rm -rf $(PER_CONFIGURATION_BUILDS_DIR)include/boost
	@rm -f $(PER_CONFIGURATION_BUILDS_DIR)lib/libboost_*
	@rm -rf $(PER_CONFIGURATION_BUILDS_DIR)share/boost-build
endif


clean:
ifeq ($(CONFIGURATION),)
	@rm -rf =$(StroikaRoot)IntermediateFiles/*/ThirdPartyComponents/boost/
else
	@rm -rf $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
endif


check:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
ifeq ($(PRODUCED_OUTPUT_ARTIFACTS),)
	$(error This makefile requires a recognized ProjectPlatformSubdir)
endif
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "Checking ... "
	@$(StroikaRoot)ScriptsLib/CheckFileExists $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(ECHO) " Stroika/ThirdPartyComponents/boost -   [Succeeded]";


check-prerequisite-tools:
	@#none specific to boost


#ALL produced artifacts produced by the same thing. But if you try the same rule to make all, if parallel makes invoked on this, you get
#multiple tries to run the same rule. So remap all to making the first.
$(wordlist 2, 9999, $(PRODUCED_OUTPUT_ARTIFACTS)):	$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS))

$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS)):
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(MAKE) --silent $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
	@$(MAKE) --silent bootstrapIfNeeded_
	@$(MAKE) --silent configureAndBuild_


#unsure why windows and unix builds go to such different places, but they do. Probably not too hard to unify them, but not a priority
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
B2FullPath=$(PER_CONFIGURATION_BUILDS_DIR)bin/b2.exe
else
B2FullPath=$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/b2
endif

bootstrapIfNeeded_:	$(B2FullPath)

$(B2FullPath):
	$(MAKE) --no-print-directory bootstrap_

bootstrap_:
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "boost ${VERSION} - Bootstrapping - see ${OUTPUT_WORKDIR_PRETTYNAME}BOOTSTRAP.OUT"` "... "
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	@### From http://www.boost.org/doc/libs/1_66_0/more/getting_started/windows.html - section 5.2
	@SL=`realpath ../../ScriptsLib` && cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && ($$SL/RunArgumentsWithCommonBuildVars $(CONFIGURATION) "cd tools\build && bootstrap.bat" > BOOTSTRAP.OUT)
	@SL=`realpath ../../ScriptsLib` && cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && ($$SL/RunArgumentsWithCommonBuildVars $(CONFIGURATION) "cd tools\build && b2 install --prefix=$(shell cygpath --mixed $(PER_CONFIGURATION_BUILDS_DIR))" >> BOOTSTRAP.OUT)
else
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && (echo "./bootstrap.sh --prefix=$(PER_CONFIGURATION_BUILDS_DIR)" > BOOTSTRAP.OUT)
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && (./bootstrap.sh --prefix=$(PER_CONFIGURATION_BUILDS_DIR) >> BOOTSTRAP.OUT)
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
#B2_CONFIGURATION_ARGS +=	--without-system
B2_CONFIGURATION_ARGS +=	--without-wave

### shorter paths avoids problems building on windows (abbrevpaths not enuf on windows when root dir is maybe 100 chars, so use --hash which works better)
#B2_CONFIGURATION_ARGS +=	--abbreviate-paths
B2_CONFIGURATION_ARGS +=	--hash

#output verbosity (default is -d1, but use -d0 cuz faster)
#B2_CONFIGURATION_ARGS +=	-d0
B2_CONFIGURATION_ARGS +=	-d+2
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
TOOLSET_NAME=darwin
else
TOOLSET_NAME=gcc
endif
B2_CONFIGURATION_ARGS +=	toolset=${TOOLSET_NAME}-$(ARCH)
endif

ifeq (Unix,$(ProjectPlatformSubdir))
B2_CONFIGURATION_ARGS +=	-sBOOST_BUILD_USER_CONFIG=${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}/user-config.jam
endif


#
##
### NOTE - all this nonsense is for a few reasons:
###		o	make builds a bit faster
###		o	GNU make makes it very hard to figure out what -j parameter was passed to make
###		o	boost uses b2, which has a parallel make feature, just one thats incompatiple with the gnu make version
##
#
export JOBS
ifndef JOBS
#no easy way to know right number; but hints from https://stackoverflow.com/questions/5303553/gnu-make-extracting-argument-to-j-within-makefile
#not reasonable - https://blog.jgc.org/2015/03/gnu-make-insanity-finding-value-of-j.html
MAKE_PID := $(shell echo $$PPID)
JOB_FLAG_ := $(filter -j%, $(subst -j ,-j,$(shell ps T | grep "^\s*$(MAKE_PID).*$(MAKE)")))
JOBS     ?= $(subst -j,,$(JOB_FLAG_))
endif

ifndef JOBS_FLAG
ifeq ($(JOBS),)
# if we cannot detect, safest thing is none, but on windows/mac this never works, so there default to 5
ifeq (CYGWIN,$(findstring CYGWIN,$(shell uname)))
JOBS_FLAG:=	-j5
else ifeq ($(shell uname -s),Darwin)
JOBS_FLAG:=	-j5
else
# probably no -j on parent make, so none on b2; but - alas - very frequently the above doesn't work, so use '5' instead
#JOBS_FLAG:=
JOBS_FLAG:=	-j5
endif
endif
endif


B2_CONFIGURATION_ARGS +=	$(JOBS_FLAG)


configureAndBuild_:
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "boost ${VERSION} - Config+Build; see ${OUTPUT_WORKDIR_PRETTYNAME}ConfigureAndBuild.OUT"` "..."
	@
	@#https://stroika.atlassian.net/browse/STK-633
ifeq (CYGWIN,$(findstring CYGWIN,$(shell uname)))
	@MAXCHAR=120 && PWD=`realpath --canonicalize-missing $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)` && if [[ $${#PWD} -gt $$MAXCHAR ]] ; then $(ECHO) && ../../ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "WARNING: STRLEN(boost-build-dir) == $${#PWD}: sometimes if its more than $$MAXCHAR characters on windows it may cause boost build problem (https://stroika.atlassian.net/browse/STK-633)" && ../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n; fi;
endif
	@##NB: using ; separator cuz / appears in file names (sed call)
	@#@todo - document why we need to patch project-config.jam file - why cannot specifiy ARCH/COMPILER externally?
ifneq (CYGWIN,$(findstring CYGWIN,$(shell uname)))
	rm -f $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/user-config.jam; \
	echo 'using ${TOOLSET_NAME} : $(ARCH) : ${CXX} : <cflags>"${CFLAGS}" <cxxflags>"${CXXFLAGS}" <linkflags>"${LDFLAGS}" ;' >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/user-config.jam;
endif
	@#
	@### NOTE - sometimes if this starts failing (e.g. *** [Makefile:288: configureAndBuild_] Error 1), from a DOS shell, type del %tmp%\b2_*.cmd
	@#
	@#Cannot use ifeq() macro stuff cuz thats done in first phase of make eval, and MAKEFLAGs is only defined later
	@rm -f $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/ConfigureAndBuild.OUT
	@export CFA="$(B2_CONFIGURATION_ARGS)" && \
		cd  $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && \
		(echo $(B2FullPath) $$CFA install >> ConfigureAndBuild.OUT) && \
		(sh -c "$(B2FullPath) $$CFA install >> ConfigureAndBuild.OUT")
	@#NOT SURE WHY THIS IS NEEDED - WHY BOOST DOESNT DO SOMETHING SIMILAR AUTOMATICALLY?
	@#BUT without, we get link error cuz linking appears to assume ignoring how we built boost with --layout=system
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "Patching $(PER_CONFIGURATION_BUILDS_DIR)include/boost/config/user.hpp"`
	@$(ECHO) Patching $(PER_CONFIGURATION_BUILDS_DIR)include/boost/config/user.hpp >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/ConfigureAndBuild.OUT
	@$(ECHO) "// GENERATED BY STROIKA MAKEFILE" >> $(PER_CONFIGURATION_BUILDS_DIR)include/boost/config/user.hpp >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/ConfigureAndBuild.OUT
	@$(ECHO) "#define BOOST_AUTO_LINK_SYSTEM 1" >> $(PER_CONFIGURATION_BUILDS_DIR)include/boost/config/user.hpp >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/ConfigureAndBuild.OUT
	@$(ECHO) "// GENERATED BY STROIKA MAKEFILE" >> $(PER_CONFIGURATION_BUILDS_DIR)include/boost/config/user.hpp
	@$(ECHO) "#define BOOST_AUTO_LINK_SYSTEM 1" >> $(PER_CONFIGURATION_BUILDS_DIR)include/boost/config/user.hpp
