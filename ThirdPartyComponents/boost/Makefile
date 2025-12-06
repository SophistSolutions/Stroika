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

StroikaRoot	:=	$(abspath ../..)/

include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

SHELL=/bin/bash

.PHONY:	all clean clobber check bootstrap_ configureAndBuild_

PER_CONFIGURATION_BUILDS_DIR_:=${StroikaPlatformTargetBuildDir}
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_:=$(shell realpath --canonicalize-missing ../../IntermediateFiles/$(CONFIGURATION)/ThirdPartyComponents/boost)
OUTPUT_WORKDIR_PRETTYNAME="$(shell $(StroikaRoot)ScriptsLib/SubstituteBackVariables "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)")/"

##
## Check latest release: https://www.boost.org/users/download/
##

VERSION:=1_89_0
#VERSION:=1_88_0
#VERSION:=1_87_0
#VERSION:=1_86_0
VERSION_WITH_DOTS:=$(shell echo $(VERSION) | sed s/_/\./g)
ZIPPEDFILE:=../Origs-Cache/boost_$(VERSION).tar.gz



#not parallel because submakefiles use parallelism, but generally best to sequence these top level requests. Like if you say
# make clobber all you don't want those to happen at the same time. And make libraries samples wouldn't really work since all the libraries
# have to be built before the samples etc...
#		(ALSO THIS COULD BE REASON BEHIND FLAKY FAIL ON HearHE Windows building boost) --LGP 2025-02-01
.NOTPARALLEL:



CPPFLAGS 	:= 	${PLATFORM_CPPFLAGS}
CFLAGS 		:= 	${PLATFORM_CFLAGS}
CXXFLAGS 	:= 	${PLATFORM_CXXFLAGS}


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


### https://archives.boost.io location new (just found 2025-11-18) 
FETCHURLS=
FETCHURLS+=https://archives.boost.io/release/$(VERSION_WITH_DOTS)/source/boost_$(VERSION).tar.gz
FETCHURLS+=https://sourceforge.net/projects/boost/files/boost/$(VERSION_WITH_DOTS)/boost_$(VERSION).tar.gz/download


## just a smattering of test files to see if it looks like copied
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(PER_CONFIGURATION_BUILDS_DIR_)include/boost/config.hpp				\
	$(PER_CONFIGURATION_BUILDS_DIR_)include/boost/process.hpp				\
	$(PER_CONFIGURATION_BUILDS_DIR_)include/boost/type_traits.hpp 			\
	$(PER_CONFIGURATION_BUILDS_DIR_)lib/pkgconfig/boost.pc
# NB: Keep this list in sync with those in boost*.pc files
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(PER_CONFIGURATION_BUILDS_DIR_)lib/libboost_chrono$(LIB_SUFFIX)		\
	$(PER_CONFIGURATION_BUILDS_DIR_)lib/libboost_filesystem$(LIB_SUFFIX)	\
	$(PER_CONFIGURATION_BUILDS_DIR_)lib/libboost_json$(LIB_SUFFIX)			\
	$(PER_CONFIGURATION_BUILDS_DIR_)lib/libboost_thread$(LIB_SUFFIX)		\



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
	@if [[ "$@" != "ORIGs" ]] ; then \
		if [[ "${VERSION}" == "1_85_0" ]] ; then \
			$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "boost ${VERSION} - Patching $@ (see ${OUTPUT_WORKDIR_PRETTYNAME}PATCHING.OUT) ... "`;\
			patch -p0 $@/tools/build/src/tools/msvc.jam Patches/MSVC144-Jam.Patch >> PATCHING.OUT;\
		fi;\
	fi


clobber:	clean
ifeq ($(CONFIGURATION),)
	@rm -rf $(StroikaRoot)Builds/*/include/boost
	@rm -f $(StroikaRoot)Builds/*/lib/libboost_*
	@rm -rf $(StroikaRoot)Builds/*/share/boost-build
	@rm -rf CURRENT
else
	@rm -rf $(PER_CONFIGURATION_BUILDS_DIR_)include/boost
	@rm -f $(PER_CONFIGURATION_BUILDS_DIR_)lib/libboost_*
	@rm -rf $(PER_CONFIGURATION_BUILDS_DIR_)share/boost-build
	@rm -rf $(B2FullPath)
endif


clean:
ifeq ($(CONFIGURATION),)
	@rm -rf $(StroikaRoot)IntermediateFiles/*/ThirdPartyComponents/boost/
else
	@rm -rf $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
endif


check:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
ifeq ($(PRODUCED_OUTPUT_ARTIFACTS),)
	$(error This makefile requires a recognized BuildPlatform)
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


B2FullPath=$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/b2${EXE_SUFFIX}

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

# BOOTSTRAP_TOOLSET?=msvc
BOOTSTRAP_TOOLSET?=

bootstrap_:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "boost ${VERSION} - Bootstrapping - see ${OUTPUT_WORKDIR_PRETTYNAME}BOOTSTRAP-OUT.txt"` "... "
ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
	@### From https://www.boost.org/doc/libs/1_87_0/more/getting_started/windows.html#id37 - section 5.2
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/tools/build && (${ScriptsLibPath}/RunArgumentsWithCommonBuildVars $(CONFIGURATION) "bootstrap.bat ${BOOTSTRAP_TOOLSET}" > ../../BOOTSTRAP-OUT.txt)
	@#cuz seems this is place UNIX puts it, so keep consistent
	@#NOTE as of 2025-02-06 - seeing this not always work for HearHE app on Windoze... Will need to debug further - but not reliably failing there either
	@#					      failures seem worse the more -jN flag given, and if vscode running/watching, and maybe if I left app running when rebuilding/clobber???
	@if [ ! -e "${B2FullPath}" ]; then cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/tools/build/src/engine/b2.exe ${B2FullPath}; fi
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/tools/build && (${ScriptsLibPath}/RunArgumentsWithCommonBuildVars $(CONFIGURATION) ".\b2 install --prefix=$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(PER_CONFIGURATION_BUILDS_DIR_))" >> ../../BOOTSTRAP-OUT.txt 2>&1)
else
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && (echo "./bootstrap.sh --prefix=$(PER_CONFIGURATION_BUILDS_DIR_)" > BOOTSTRAP-OUT.txt)
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && (./bootstrap.sh --prefix=$(PER_CONFIGURATION_BUILDS_DIR_) >> BOOTSTRAP-OUT.txt 2>&1)
endif
	@if [ ! -e "${B2FullPath}" ]; then $(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "${B2FullPath} not found - so bootstrap failed"; exit 1; fi

ifeq (true,$(HasMakefileBugWorkaround_lto_skipping_undefined_incompatible))
CFLAGS:=$(filter-out -flto, $(CFLAGS))
CXXFLAGS:=$(filter-out -flto, $(CXXFLAGS))
endif



### Run b2.exe --help

# this selects a bunch of parameters, but the compiler itself can still be overriden by other parameters - just the
# darwin.jam file (basically) - and other defaults sort of related to that.
ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
TOOLSET_NAME=msvc
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


# At least fails on build-n-test-Linux (Linux, ubuntu-22.04-clang++-15-debug-libc++-c++23, clang++-15, ubuntu-latest,... 
# and build-n-test-MacOS (MacOS, MacOS-13-XCode-14.3-Debug, macos-13, 14.3, Debug, --apply-default-debu...
#		--LGP 2023-12-15
ifeq (clang,$(findstring clang,$(CXX)))
# only seems to affect libc++ (not libstdc++) -- 
#	./boost/cobalt/config.hpp:51:10: fatal error: 'memory_resource' file not found
#	#include <memory_resource>
#             ^~~~~~~~~~~~~~~~~
ifeq (-stdlib=libc++,$(findstring -stdlib=libc++,$(CXXFLAGS)))
B2_CONFIGURATION_ARGS +=	--without-cobalt
endif
endif

### shorter paths avoids problems building on windows (abbrevpaths not enuf on windows when root dir is maybe 100 chars, so use --hash which works better)
#B2_CONFIGURATION_ARGS +=	--abbreviate-paths
B2_CONFIGURATION_ARGS +=	--hash

#output verbosity (default is -d1, but use -d0 cuz faster)
#B2_CONFIGURATION_ARGS +=	-d0
#B2_CONFIGURATION_ARGS +=	-d+2
B2_CONFIGURATION_ARGS +=	-d2
#SHOW VARIABLES - B2_CONFIGURATION_ARGS +=	-d7

B2_CONFIGURATION_ARGS +=	--prefix=$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(PER_CONFIGURATION_BUILDS_DIR_))
ifeq (0,$(AssertionsEnabled))
B2_CONFIGURATION_ARGS += variant=release
else
B2_CONFIGURATION_ARGS += variant=debug
endif


### @todo - redo this using new ADDRESS_MODEL field in CONFIGURE (JAM) file
### MAYBE can do soemthing like toolset=${TOOLSET_NAME}-$(ARCH) change; but didn't appear to work
ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
ifeq ($(ARCH),x86)
B2_CONFIGURATION_ARGS +=	address-model=32
else ifeq ($(ARCH),x86_64)
B2_CONFIGURATION_ARGS +=	address-model=64
endif
endif

ifeq (Unix,$(BuildPlatform))
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
ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && \
	    ($(ECHO) "${ScriptsLibPath}/RunArgumentsWithCommonBuildVars $(CONFIGURATION) $(B2FullPath) $(B2_CONFIGURATION_ARGS) install" >> ConfigureAndBuild-OUT.txt) &&\
	    (${ScriptsLibPath}/RunArgumentsWithCommonBuildVars $(CONFIGURATION) $(B2FullPath) $(B2_CONFIGURATION_ARGS) install >> ConfigureAndBuild-OUT.txt)
else
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && \
	    $(ECHO) "$(B2FullPath) $(B2_CONFIGURATION_ARGS) install" >> ConfigureAndBuild-OUT.txt &&\
	    ($(B2FullPath) $(B2_CONFIGURATION_ARGS) install >> ConfigureAndBuild-OUT.txt)
endif
ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
	@cp boost-msvc.pc "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/boost.pc"
else
	@cp boost.pc "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/"
endif
	@
	@#
	@#NOT sure why this is needed: why boost doesnt do soemthing similar automatically?
	@#BUT without, we get link error (WINDOWS ONLY) cuz linking appears to assume ignoring how
	@#we built boost with --layout=system
	@#Include this logic on ALL platforms because on ALL platforms we explicitly set --layout=system
	@#
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "boost ${VERSION} - Patching $(PER_CONFIGURATION_BUILDS_DIR_)include/boost/config/user.hpp"`
	@$(ECHO) Patching $(PER_CONFIGURATION_BUILDS_DIR_)include/boost/config/user.hpp >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/ConfigureAndBuild-OUT.txt
	@$(ECHO) "// GENERATED BY STROIKA MAKEFILE" >> $(PER_CONFIGURATION_BUILDS_DIR_)include/boost/config/user.hpp >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/ConfigureAndBuild-OUT.txt
	@$(ECHO) "#define BOOST_AUTO_LINK_SYSTEM 1" >> $(PER_CONFIGURATION_BUILDS_DIR_)include/boost/config/user.hpp >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/ConfigureAndBuild-OUT.txt
	@$(ECHO) "// GENERATED BY STROIKA MAKEFILE" >> $(PER_CONFIGURATION_BUILDS_DIR_)include/boost/config/user.hpp
	@$(ECHO) "#define BOOST_AUTO_LINK_SYSTEM 1" >> $(PER_CONFIGURATION_BUILDS_DIR_)include/boost/config/user.hpp
