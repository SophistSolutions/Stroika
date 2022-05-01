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
#		@todo	Now that we pass in CFLAGS, CXXFLAGS etc, we probably can get away with fewer explicit settings in the build process (like _GLIBCXX_DEBUG)
#

StroikaRoot=$(abspath ../..)/

ifneq ($(CONFIGURATION),)
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)/ScriptsLib/SharedMakeVariables-Default.mk


.PHONY:	all clean clobber check bootstrap_ configureAndBuild_

PER_CONFIGURATION_BUILDS_DIR:=$(shell realpath --canonicalize-missing ../../Builds/$(CONFIGURATION)/ThirdPartyComponents/)/

#lose trailing / on PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_ because on macos make THATDIR doesn't work with trailing slash (old WEIRD_MACOS_MAKEFILE_DIR_BUG -  MacOS XCode 10 - GNU Make 3.81)
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_:=$(shell realpath --canonicalize-missing ../../IntermediateFiles/$(CONFIGURATION)/ThirdPartyComponents/boost)

#OUTPUT_WORKDIR_PRETTYNAME="{StroikaRoot}Inter.../Thir.../boost/"
OUTPUT_WORKDIR_PRETTYNAME="$(shell $(StroikaRoot)ScriptsLib/SubstituteBackVariables "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/")"


VERSION=1_79_0
#VERSION=1_78_0
#VERSION=1_78_0_b1
#VERSION=1_77_0
#VERSION=1_76_0
#VERSION=1_75_0
VERSION_WITH_DOTS=$(shell echo $(VERSION) | sed s/_/\./g)
VERSION_WITH_2DOTS=$(shell echo $(VERSION_WITH_DOTS) | sed 's/\.[ab][0-9]//')
ZIPPEDFILE=../Origs-Cache/boost_$(VERSION).tar.gz



all:	
ifeq ($(CONFIGURATION),)
	@for i in `../../ScriptsLib/GetConfigurations` ; do\
		$(MAKE) --no-print-directory all CONFIGURATION=$$i;\
	done
else
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents/boost Build $(VERSION) {$(CONFIGURATION)}:"
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@#make zippedfile even though a dependency on CURRENT and PRODUCED_ARTIFACTS for parallel-make so completed before either starts (else one sees file partly downloaded and proceeeds)
	@$(MAKE) --silent MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $(ZIPPEDFILE)
	@$(MAKE) --silent MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))  $(PRODUCED_OUTPUT_ARTIFACTS)
ifneq ($(QUICK_BUILD), 1)
	@$(MAKE) --silent MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) CURRENT
endif
	@$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) check
endif


### Haven't found any backup/mirrors and this only stores the last few versions. @todo - find a better backup! 
FETCHURLS=
FETCHURLS+=https://boostorg.jfrog.io/artifactory/main/release/$(VERSION_WITH_DOTS)/source/boost_$(VERSION).tar.gz
FETCHURLS+=https://sourceforge.net/projects/boost/files/boost/$(VERSION_WITH_DOTS)/boost_$(VERSION).tar.gz/download
FETCHURLS+=https://boostorg.jfrog.io/artifactory/main/beta/$(VERSION_WITH_2DOTS).beta1/source/boost_$(VERSION).tar.gz
#FETCHURLS+=ftp://ftp.sophists.com/Pub/Mirror/boost_$(VERSION).tar.gz
#(reasonable but breaks github 100mb limit)FETCHURLS+=file:///../Mirror/boost_$(VERSION).tar.gz
### the github.com/boostorg/boost/archive appear not fully compatible/real mirror of above - very different size and my build scripts dont work
#FETCHURLS+=https://github.com/boostorg/boost/archive/boost-$(VERSION_WITH_DOTS).tar.gz




## just a smatering of test files to see if it looks like copied
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(PER_CONFIGURATION_BUILDS_DIR)include/boost/config.hpp				\
	$(PER_CONFIGURATION_BUILDS_DIR)include/boost/process.hpp			\
	$(PER_CONFIGURATION_BUILDS_DIR)include/boost/type_traits.hpp
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/libboost_date_time$(LIB_SUFFIX)	\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/libboost_filesystem$(LIB_SUFFIX)	\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/libboost_system$(LIB_SUFFIX)


$(ZIPPEDFILE):
	@MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) ../../ScriptsLib/WebGet $(ZIPPEDFILE) $(FETCHURLS)
	

ORIGs CURRENT $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_):	$(ZIPPEDFILE)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "boost ${VERSION} - Extracting $(ZIPPEDFILE) to $@"` "... "
	@mkdir -p $@
ifeq ($(DETECTED_HOST_OS),Darwin)
	@tar xf $(ZIPPEDFILE) -C $@ --strip-components 1
else ifneq ($(findstring $(DETECTED_HOST_OS),MSYS-Cygwin),)
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
	@rm -rf $(B2FullPath)
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
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "boost ${VERSION} - Checking ... "
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

ScriptsLibPath=$(shell realpath ../../ScriptsLib)

ifeq ($(DETECTED_HOST_OS),Cygwin)
CXX:= $(shell cygpath --mixed "$(CXX)")
B2FullPath:= $(shell cygpath --mixed "$(B2FullPath)")
else ifeq ($(DETECTED_HOST_OS),MSYS)
CXX:= $(shell cygpath --mixed "$(CXX)")
B2FullPath:= $(shell cygpath --mixed "$(B2FullPath)")
endif



bootstrapIfNeeded_:	$(B2FullPath)

$(B2FullPath):
	$(MAKE) --no-print-directory bootstrap_

bootstrap_:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "boost ${VERSION} - Bootstrapping - see ${OUTPUT_WORKDIR_PRETTYNAME}BOOTSTRAP-OUT.txt"` "... "
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	@### From http://www.boost.org/doc/libs/1_66_0/more/getting_started/windows.html - section 5.2
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && (${ScriptsLibPath}/RunArgumentsWithCommonBuildVars $(CONFIGURATION) "cd tools\build && bootstrap.bat msvc" > BOOTSTRAP-OUT.txt)
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && (${ScriptsLibPath}/RunArgumentsWithCommonBuildVars $(CONFIGURATION) "cd tools\build && b2 install --prefix=$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(PER_CONFIGURATION_BUILDS_DIR))" >> BOOTSTRAP-OUT.txt 2>&1)
else
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && (echo "./bootstrap.sh --prefix=$(PER_CONFIGURATION_BUILDS_DIR)" > BOOTSTRAP-OUT.txt)
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && (./bootstrap.sh --prefix=$(PER_CONFIGURATION_BUILDS_DIR) >> BOOTSTRAP-OUT.txt 2>&1)
endif

ifeq (true,$(HasMakefileBugWorkaround_lto_skipping_undefined_incompatible))
CFLAGS:=$(filter-out -flto, $(CFLAGS))
CXXFLAGS:=$(filter-out -flto, $(CXXFLAGS))
endif


### Run b2.exe --help

# this selects a bunch of parameters, but the compiler itself can still be overriden by other parameters - just the
# darwin.jam file (basically) - and other defaults sort of related to that.
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
ifeq ($(ProjectPlatformSubdir),VisualStudio.Net-2022)
### This SHOULD be 14.3, but must use 14.2 for the time being since thats all thats supported in boost 1_77_0
TOOLSET_NAME=msvc-14.2
else ifeq ($(ProjectPlatformSubdir),VisualStudio.Net-2019)
TOOLSET_NAME=msvc-14.2
else
TOOLSET_NAME=msvc
endif
else ifeq ($(DETECTED_HOST_OS),Darwin)
#using the name darwin caused errors with XCode 11.4, but otherwise appears to make sense (and we used til 11.4) -- LGP 2020-04-12
#TOOLSET_NAME=darwin
TOOLSET_NAME=clang-darwin
else
TOOLSET_NAME=gcc
endif

B2_CONFIGURATION_ARGS=
B2_CONFIGURATION_ARGS +=	toolset=$(TOOLSET_NAME)
B2_CONFIGURATION_ARGS +=	link=static
B2_CONFIGURATION_ARGS +=	runtime-link=static
B2_CONFIGURATION_ARGS +=	threading=multi
B2_CONFIGURATION_ARGS +=	--layout=system

### omit some things unlikely to be intersted (mostly cuz already adopted into STL) in (due to build speed)
B2_CONFIGURATION_ARGS +=	--without-atomic
#B2_CONFIGURATION_ARGS +=	--without-chrono
B2_CONFIGURATION_ARGS +=	--without-python
B2_CONFIGURATION_ARGS +=	--without-regex
B2_CONFIGURATION_ARGS +=	--without-wave

### shorter paths avoids problems building on windows (abbrevpaths not enuf on windows when root dir is maybe 100 chars, so use --hash which works better)
#B2_CONFIGURATION_ARGS +=	--abbreviate-paths
B2_CONFIGURATION_ARGS +=	--hash

#output verbosity (default is -d1, but use -d0 cuz faster)
#B2_CONFIGURATION_ARGS +=	-d0
#B2_CONFIGURATION_ARGS +=	-d+2
B2_CONFIGURATION_ARGS +=	-d2
#SHOW VARIABLES - B2_CONFIGURATION_ARGS +=	-d7

B2_CONFIGURATION_ARGS +=	--prefix=$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(PER_CONFIGURATION_BUILDS_DIR))
ifeq (0,$(ENABLE_ASSERTIONS))
B2_CONFIGURATION_ARGS += variant=release
else
B2_CONFIGURATION_ARGS += variant=debug
endif


### @todo - redo this using new ADDRESS_MODEL field in CONFIGURE (JAM) file
### MAYBE can do soemthing like toolset=${TOOLSET_NAME}-$(ARCH) change; but didn't appear to work
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
ifeq ($(ARCH),x86)
B2_CONFIGURATION_ARGS +=	address-model=32
else ifeq ($(ARCH),x86_64)
B2_CONFIGURATION_ARGS +=	address-model=64
endif
endif


ifeq (Unix,$(ProjectPlatformSubdir))
B2_CONFIGURATION_ARGS +=	-sBOOST_BUILD_USER_CONFIG=${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}/user-config.jam
endif


#
# NB: Attempted to figure out how to pass along the -j JOBS flag to b2, and had some tricky code for a while (til Stroika 2.1b14)
# but that code never worked, was complex, and didn't improve performance, so KISS, and do nothing.
#



configureAndBuild_:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "boost ${VERSION} - Config+Build; see ${OUTPUT_WORKDIR_PRETTYNAME}ConfigureAndBuild-OUT.txt"` "..."
	@
	@rm -f $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/ConfigureAndBuild-OUT.txt
	@
	@#https://stroika.atlassian.net/browse/STK-633
ifeq ($(DETECTED_HOST_OS),Cygwin)
	@MAXCHAR=120 && PWD=`realpath --canonicalize-missing $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)` && if [[ $${#PWD} -gt $$MAXCHAR ]] ; then $(ECHO) && $(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "WARNING: STRLEN(boost-build-dir) == $${#PWD}: sometimes if its more than $$MAXCHAR characters on windows it may cause boost build problem (https://stroika.atlassian.net/browse/STK-633)" && $(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n; fi;
endif
	@
	@#@todo - document why we need to patch project-config.jam file - why cannot specifiy ARCH/COMPILER externally?
	@#Note would probably be better to not use quotes around CFLAGS etc, then the boost configure file fails to read the full set of options properly.
	@#I MAY need to break the <cxflags> etc lines into multple statements to avoid that. Docs not super clear.
	@#see https://www.boost.org/doc/libs/1_66_0/doc/html/bbv2/overview.html
	@#     If multiple of the same type of options are needed, they can be concatenated with quotes or have multiple instances of the option tag.
	@#but note how this doesnt make clear about nested quotes as in quotes in the options themselves.
	@rm -f $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/user-config.jam;
	@$(ECHO) 'using ${TOOLSET_NAME} : $(ARCH) : "${CXX}" : <cflags>"${CFLAGS}" <cxxflags>"${CXXFLAGS}" <linkflags>"${LDFLAGS}" ;' >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/user-config.jam;
	@
	@#
	@### NOTE - sometimes if this starts failing (e.g. *** [Makefile:288: configureAndBuild_] Error 1), from a DOS shell, type del %tmp%\b2_*.cmd
	@#(above comment maybe obsolete as of 2020-01-03)
	@
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && \
	    ($(ECHO) "${ScriptsLibPath}/RunArgumentsWithCommonBuildVars $(CONFIGURATION) $(B2FullPath) $(B2_CONFIGURATION_ARGS) install" >> ConfigureAndBuild-OUT.txt) &&\
	    (${ScriptsLibPath}/RunArgumentsWithCommonBuildVars $(CONFIGURATION) $(B2FullPath) $(B2_CONFIGURATION_ARGS) install >> ConfigureAndBuild-OUT.txt)
else
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && \
	    $(ECHO) "$(B2FullPath) $(B2_CONFIGURATION_ARGS) install" >> ConfigureAndBuild-OUT.txt &&\
	    ($(B2FullPath) $(B2_CONFIGURATION_ARGS) install >> ConfigureAndBuild-OUT.txt)
endif
	@
	@#
	@#NOT sure why this is needed: why boost doesnt do soemthing similar automatically?
	@#BUT without, we get link error (WINDOWS ONLY) cuz linking appears to assume ignoring how
	@#we built boost with --layout=system
	@#Include this logic on ALL platforms because on ALL platforms we explicitly set --layout=system
	@#
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "boost ${VERSION} - Patching $(PER_CONFIGURATION_BUILDS_DIR)include/boost/config/user.hpp"`
	@$(ECHO) Patching $(PER_CONFIGURATION_BUILDS_DIR)include/boost/config/user.hpp >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/ConfigureAndBuild-OUT.txt
	@$(ECHO) "// GENERATED BY STROIKA MAKEFILE" >> $(PER_CONFIGURATION_BUILDS_DIR)include/boost/config/user.hpp >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/ConfigureAndBuild-OUT.txt
	@$(ECHO) "#define BOOST_AUTO_LINK_SYSTEM 1" >> $(PER_CONFIGURATION_BUILDS_DIR)include/boost/config/user.hpp >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/ConfigureAndBuild-OUT.txt
	@$(ECHO) "// GENERATED BY STROIKA MAKEFILE" >> $(PER_CONFIGURATION_BUILDS_DIR)include/boost/config/user.hpp
	@$(ECHO) "#define BOOST_AUTO_LINK_SYSTEM 1" >> $(PER_CONFIGURATION_BUILDS_DIR)include/boost/config/user.hpp
