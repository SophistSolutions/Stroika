export StroikaRoot?=$(shell realpath ../..)/

ifneq ($(CONFIGURATION),)
	include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk

.PHONY:	all clean clobber check doBuilds_

###
### TODO:
###		@todo	use cmake –G”Unix Makefiles” so that I can run make -jN (or so regular make inherits flags from $(MAKE)
###				then use that to lose horrible MAKEFLAGS= hack
###				***NOTE*** this is the slowest part of the UNIX build process (latency) - so this maybe a priority
###


VERSION=3.2.2
#VERSION=3.2.1
BASENAME=xerces-c-$(VERSION)
ZIPPEDFILE=../Origs-Cache/$(BASENAME).tar.gz
TARFILE=$(BASENAME).tar
EXTRACTED_DIRNAME=$(BASENAME)

PER_CONFIGURATION_BUILDS_DIR:=$(shell realpath --canonicalize-missing ../../Builds/$(CONFIGURATION)/ThirdPartyComponents/)/

#lose trailing / on PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_ because on macos make THATDIR doesn't work with trailing slash (old WEIRD_MACOS_MAKEFILE_DIR_BUG -  MacOS XCode 10 - GNU Make 3.81)
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_:=$(shell realpath --canonicalize-missing ../../IntermediateFiles/$(CONFIGURATION)/ThirdPartyComponents/Xerces)

OUTPUT_WORKDIR_PRETTYNAME="{StroikaRoot}/Inter.../Thir.../Xerces/"


#
# CHECK FOR UPDATED VERSIONS http://xerces.apache.org/xerces-c/
#

FETCHURLS=
#
#ARCHIVE sites have old versions which matters when you are building an
#older verison of Stroika
#
FETCHURLS+=http://archive.apache.org/dist/xerces/c/3/sources/$(BASENAME).tar.gz

#
#Most Mirrors only have the current release so when xercesc updated, these start
#failing (bad when building old stroika)
#
FETCHURLS+=http://apache.cs.utah.edu//xerces/c/3/sources/$(BASENAME).tar.gz
FETCHURLS+=http://www.apache.org/dist/xerces/c/3/sources/$(BASENAME).tar.gz
FETCHURLS+=http://www-us.apache.org/dist/xerces/c/3/sources/$(BASENAME).tar.gz


ifneq (,$(findstring /cygdrive/,$(PATH)))
    UNAME := Cygwin
else
ifneq (,$(findstring WINDOWS,$(PATH)))
    UNAME := Windows
else
    UNAME := $(shell uname -s)
endif
endif



PRODUCED_OUTPUT_ARTIFACTS=	\
	$(PER_CONFIGURATION_BUILDS_DIR)include/xercesc/dom/DOM.hpp

ifeq (Unix,$(ProjectPlatformSubdir))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/libxerces-c.a
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
ifeq (Debug,$(findstring Debug, $(CONFIGURATION)))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/xerces-c_3D.lib
endif
ifeq (Release,$(findstring Release, $(CONFIGURATION)))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/xerces-c_3.lib
endif
endif


ifneq ($(CONFIGURATION),)
CMAKE_PER_TARGET_BUILD_DIR:=../../Builds/$(CONFIGURATION)/ThirdPartyComponents/
CMAKE_CURRENT_SOURCE_DIR:=$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
ifeq ($(UNAME),Cygwin)
CMAKE_PER_TARGET_BUILD_DIR:=$(shell cygpath -a -w $(CMAKE_PER_TARGET_BUILD_DIR) | sed s/\\\\/\\\\\\\\/g)
CMAKE_CURRENT_SOURCE_DIR:=$(shell cygpath -a -w $(CMAKE_CURRENT_SOURCE_DIR) | sed s/\\\\/\\\\\\\\/g)
else
CMAKE_PER_TARGET_BUILD_DIR:=$(shell realpath --canonicalize-missing $(CMAKE_PER_TARGET_BUILD_DIR) )
CMAKE_CURRENT_SOURCE_DIR:=$(shell realpath --canonicalize-missing $(CMAKE_CURRENT_SOURCE_DIR) )
endif
endif

CFLAGS=$(EXTRA_COMPILER_ARGS)

ifeq (Unix,$(ProjectPlatformSubdir))
#clang (not clang++) barfs if given --std=c++17 argument
#And we later get linker errors from clang if we use -flto on object files stored here
CFLAGS:=$(filter-out --std%, $(CFLAGS))
CFLAGS:=$(filter-out -std%, $(CFLAGS))
CFLAGS:=$(filter-out -flto, $(CFLAGS))
CXXFLAGS=$(EXTRA_COMPILER_ARGS)
CXXFLAGS:=$(filter-out -flto, $(CXXFLAGS))
#@todo fixup - bug workaround - configure autodetects -posix and se we have to add it in here. Must find
# some way we can ADD these params to CFLAGS without REPLACING
# -- without this hack - we get link error building Xerces
CFLAGS+= -pthread
CXXFLAGS+= -pthread
endif


all:
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents/xerces $(VERSION) Build {$(CONFIGURATION)}:"
ifeq ($(CONFIGURATION),)
	@for i in `../../ScriptsLib/GetConfigurations` ; do\
		$(MAKE) --no-print-directory all CONFIGURATION=$$i;\
	done
else
	@#make zippedfile even though a dependency on CURRENT and PRODUCED_ARTIFACTS for parallel-make so completed before either starts (else one sees file partly downloaded and proceeeds)
	@$(MAKE) --silent MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $(ZIPPEDFILE)
	@../../ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
ifeq ($(PRODUCED_OUTPUT_ARTIFACTS),)
	$(error This makefile requires a recognized ProjectPlatformSubdir)
endif
	@$(MAKE) --no-print-directory --silent MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) CURRENT $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))  check
endif


#dependency on intermediate target allows make parallel without firing PRODUCED_OUTPUT_ARTIFACTS_ for each $(PRODUCED_OUTPUT_ARTIFACTS) target
$(PRODUCED_OUTPUT_ARTIFACTS):	$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/PRODUCED_OUTPUT_ARTIFACTS_
$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/PRODUCED_OUTPUT_ARTIFACTS_:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(MAKE) --silent $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
	@$(MAKE) --no-print-directory   doBuilds_
	@touch $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/PRODUCED_OUTPUT_ARTIFACTS_


clean:
	@rm -rf $(EXTRACTED_DIRNAME) 
ifeq ($(CONFIGURATION),)
	@rm -rf $(StroikaRoot)IntermediateFiles/*/ThirdPartyComponents/Xerces
else
	@rm -rf $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
endif


clobber:	clean
ifeq ($(CONFIGURATION),)
	@rm -rf $(StroikaRoot)Builds/*/ThirdPartyComponents/lib/*xerces* $(StroikaRoot)Builds/*/ThirdPartyComponents/include/*xerces*
	@rm -rf CURRENT
else
	@rm -f $(PRODUCED_OUTPUT_ARTIFACTS)
endif


check:
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "Checking ... "
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
ifeq ($(PRODUCED_OUTPUT_ARTIFACTS),)
	$(error This makefile requires a recognized ProjectPlatformSubdir)
endif
	@$(StroikaRoot)ScriptsLib/CheckFileExists $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(ECHO) " Stroika/ThirdPartyComponents/Xerces -  [Succeeded]";


$(ZIPPEDFILE):
	@MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) ../../ScriptsLib/WebGet $(ZIPPEDFILE) $(FETCHURLS)


ORIGs CURRENT $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_):	$(ZIPPEDFILE)
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "Xerces ${VERSION} - Extracting $(ZIPPEDFILE) to $@"` "... "
	@mkdir -p $@
ifeq ($(shell uname -s),Darwin)
	@tar xf $(ZIPPEDFILE) -C $@ --strip-components 1
else ifeq (CYGWIN,$(findstring CYGWIN,$(shell uname)))
	@PWD=`pwd` tar xf $(ZIPPEDFILE) --no-same-owner --directory `realpath --relative-to=$$PWD $@` --strip-components=1
else
	@tar xf $(ZIPPEDFILE) --no-same-owner --directory $@ --strip-components=1
endif
	@if [ "$@" != "ORIGs" ] ; then \
		../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Xerces ${VERSION} - Patching ... ";\
		patch -p0 $@/src/xercesc/internal/ElemStack.cpp Patches/SilenceSanitizerWarningAboutMemcpyNullPtr.PATCH >> $@/PATCHING.OUT;\
	fi





###NOTE - because we set network:BOOL:OFF - Xerces doesn't depend on CURL (but we could make it so if we wanted, but then update dependencies
### in ThirdPartyLibs/Makefile
CMAKE_ARGS=
CMAKE_ARGS+= -Dnetwork:BOOL=OFF
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
CMAKE_ARGS+= -Dtranscoder=windows
else ifeq ($(UNAME),Darwin)
#AVOID Undefined symbols for architecture x86_64:  "_CFRelease", etc.... Avoid library dependency
CMAKE_ARGS+= -Dtranscoder=iconv
else
CMAKE_ARGS+= -Dtranscoder=gnuiconv
endif
CMAKE_ARGS+= -Dmessage-loader=inmemory
CMAKE_ARGS+= -DBUILD_SHARED_LIBS:BOOL=OFF
CMAKE_ARGS+= -DCMAKE_INSTALL_PREFIX=$(CMAKE_PER_TARGET_BUILD_DIR)
ifeq (Unix,$(ProjectPlatformSubdir))
#for reasons I didn't investigate, CMAKE appears to require this to be a full path (CMAKE_C_COMPILER etc...) -- LGP 2019-01-04
CMAKE_ARGS+= -DCMAKE_C_COMPILER="$(shell which $(CC))"
CMAKE_ARGS+= -DCMAKE_CXX_COMPILER="$(shell which $(CXX))"
CMAKE_ARGS+= -DCMAKE_AR="$(shell which $(AR))"
CMAKE_ARGS+= -DCMAKE_RANLIB="$(shell which $(RANLIB))"
CMAKE_ARGS+= -DCMAKE_C_FLAGS="$(CFLAGS)"
CMAKE_ARGS+= -DCMAKE_CXX_FLAGS="$(CXXFLAGS)"
endif

ifeq (0,$(ENABLE_ASSERTIONS))
CMAKE_CONFIGURATION:=Release
else
CMAKE_CONFIGURATION:=Debug
endif


#Default Xerces build generates dependency on DLL version of runtime
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
#DEFAULT values from CMakeCache.txt
#NB: We must REPLACE the values (copy defaults and munge) cuz otherwise need to edit CMakeCache.txt
#or one of the other config files (maybe patch or script). This seems the lesser evil...
#	--LGP 2018-10-31
ifeq (Release,$(CMAKE_CONFIGURATION))
#		CMAKE_CXX_FLAGS_RELEASE:STRING=/MD /O2 /Ob2 /DNDEBUG
#		CMAKE_C_FLAGS_RELEASE:STRING=/MD /O2 /Ob2 /DNDEBUG
CMAKE_ARGS+= -DCMAKE_CXX_FLAGS_RELEASE="/MT /O2 /Ob2 /DNDEBUG"
CMAKE_ARGS+= -DCMAKE_C_FLAGS_RELEASE="/MT /O2 /Ob2 /DNDEBUG"
else
#		CMAKE_CXX_FLAGS_DEBUG:STRING=/MDd /Zi /Ob0 /Od /RTC1
#		CMAKE_C_FLAGS_DEBUG:STRING=/MDd /Zi /Ob0 /Od /RTC1
CMAKE_ARGS+= -DCMAKE_CXX_FLAGS_DEBUG="/MTd /Zi /Ob0 /Od /RTC1"
CMAKE_ARGS+= -DCMAKE_C_FLAGS_DEBUG="/MTd /Zi /Ob0 /Od /RTC1"
endif
endif


CMAKE_USE_GENERATOR=Unix Makefiles
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
ifeq ($(ARCH), x86)
CMAKE_USE_GENERATOR=Visual Studio 15 2017
else ifeq ($(ARCH), x86_64)
CMAKE_USE_GENERATOR=Visual Studio 15 2017 Win64
endif
endif


# For reasons not totally understood - probably just that the cmake in cygwin is quite old - it doesn't work right here.
# so use the copy from visual studio which appears to be recent enough to know about the verison of visual studio it comes with ;-)
#	--LGP 2018-12-15
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
PATH_ADJUSTMENT_PREFIX= PATH="$(TOOLS_PATH_ADDITIONS):$(PATH)" 
else
endif

doBuilds_:
	@mkdir -p $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
	@touch $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIGURE.OUT
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "Xerces ${VERSION} - Configuring (See $(OUTPUT_WORKDIR_PRETTYNAME)/CONFIGURE.OUT)"` "... "
	@echo 'cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) ; $(PATH_ADJUSTMENT_PREFIX)cmake -G "$(CMAKE_USE_GENERATOR)" $(CMAKE_ARGS) $(CMAKE_CURRENT_SOURCE_DIR)' > $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIGURE.OUT 2>&1
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) ; $(PATH_ADJUSTMENT_PREFIX)cmake -G "$(CMAKE_USE_GENERATOR)" $(CMAKE_ARGS) $(CMAKE_CURRENT_SOURCE_DIR) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIGURE.OUT 2>&1
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "Xerces ${VERSION} - Compiling (See $(OUTPUT_WORKDIR_PRETTYNAME)/BUILD.OUT)"` "... "
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && $(ECHO) $(PATH_ADJUSTMENT_PREFIX)"cmake --build . --config $(CMAKE_CONFIGURATION)" > $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD.OUT 2>&1
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && $(PATH_ADJUSTMENT_PREFIX)cmake --build . --config $(CMAKE_CONFIGURATION) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD.OUT 2>&1
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "Xerces ${VERSION} - Copying builds (See $(OUTPUT_WORKDIR_PRETTYNAME)/INSTALL.OUT)"` "... "
	@#NB: Set MAKEFLAGS= to silence warnings about -j jobserver when run with -j N (probaly bug with Xerces 3.2 cmake makefile but didnt dig cuz not important)
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && $(ECHO) $(PATH_ADJUSTMENT_PREFIX)"cmake --build . --config $(CMAKE_CONFIGURATION) --target install" > $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/INSTALL.OUT
	@MAKEFLAGS= && cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && $(PATH_ADJUSTMENT_PREFIX)cmake --build . --config $(CMAKE_CONFIGURATION) --target install >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/INSTALL.OUT
ifeq (Unix,$(ProjectPlatformSubdir))
	@cd ${PER_CONFIGURATION_BUILDS_DIR}lib && rm -f libxerces-c.a && ln -s libxerces-c-3.2.a libxerces-c.a
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@#We get warnings about a missing pdb file when linking otherwise...(not sure why since symbol info sb in the .lib)
	@$(ECHO) "cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/src/xerces-c.dir/$(CMAKE_CONFIGURATION)/xerces*.pdb ${PER_CONFIGURATION_BUILDS_DIR}lib/ 2>/dev/null || :" >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/INSTALL.OUT
	@cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/src/xerces-c.dir/$(CMAKE_CONFIGURATION)/xerces*.pdb ${PER_CONFIGURATION_BUILDS_DIR}lib/ 2>/dev/null || :
endif



check-prerequisite-tools:
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "Checking Prerequisite tools for Xerces ... "
ifeq ($(PATH_ADJUSTMENT_PREFIX),)
	@{ type cmake > /dev/null 2>&1; } || { ../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) && $(PATH_ADJUSTMENT_PREFIX) ../../ScriptsLib/GetMessageForMissingTool cmake && exit 1; }
else
	@$(PATH_ADJUSTMENT_PREFIX) ; { type cmake > /dev/null 2>&1; } || { ../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) && $(PATH_ADJUSTMENT_PREFIX) ../../ScriptsLib/GetMessageForMissingTool cmake && exit 1; }
endif
	@$(ECHO) "done";


