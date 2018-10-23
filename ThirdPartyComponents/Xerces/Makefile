ifneq ($(CONFIGURATION),)
	ProjectPlatformSubdir	=	$(shell ../../ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" ProjectPlatformSubdir)
endif


.DEFAULT_GOAL := all
.PHONY:	all clean clobber check doBuilds_

SHELL=/bin/bash

.NOTPARALLEL:

export StroikaRoot=$(shell realpath ../..)/


### link error with clang++ on ubuntu - 3.2.2 - still must debug
VERSION=3.2.2
#VERSION=3.2.1
#VERSION=3.2.0
BASENAME=xerces-c-$(VERSION)
ZIPPEDFILE=../Origs-Cache/$(BASENAME).tar.gz
TARFILE=$(BASENAME).tar
EXTRACTED_DIRNAME=$(BASENAME)



ifneq ($(CONFIGURATION),)
ARCH							=	$(shell ../../ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" ARCH)
ProjectPlatformSubdir			=	$(shell ../../ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" ProjectPlatformSubdir)
IncludeDebugSymbolsInLibraries	=	$(shell ../../ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" IncludeDebugSymbolsInLibraries)
ENABLE_ASSERTIONS				=	$(shell ../../ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" ENABLE_ASSERTIONS)
OptimizerFlag					=	$(shell ../../ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" OptimizerFlag)
endif




MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO?=	$(shell ../../ScriptsLib/GetDefaultShellVariable.sh ECHO)

PER_CONFIGURATION_BUILDS_DIR:=$(shell realpath --canonicalize-missing ../../Builds/$(CONFIGURATION)/ThirdPartyComponents/)
PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR:=$(shell realpath --canonicalize-missing ../../IntermediateFiles/$(CONFIGURATION)/ThirdPartyComponents/Xerces)


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

BUILDS_ROOT=../../Builds/

PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/xercesc/dom/DOM.hpp

ifeq (Unix,$(ProjectPlatformSubdir))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/libxerces-c.a
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
ifeq (Debug,$(findstring Debug, $(CONFIGURATION)))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/xerces-c_3D.lib
endif
ifeq (Release,$(findstring Release, $(CONFIGURATION)))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/xerces-c_3.lib
endif
endif

CMAKE_PER_TARGET_BUILD_DIR:=../../Builds/$(CONFIGURATION)/ThirdPartyComponents/
CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR:=../../IntermediateFiles/$(CONFIGURATION)/ThirdPartyComponents/Xerces
CMAKE_CURRENT_SOURCE_DIR:=$(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR)
ifeq ($(UNAME),Cygwin)
CMAKE_PER_TARGET_BUILD_DIR:=$(shell cygpath -a -w $(CMAKE_PER_TARGET_BUILD_DIR) | sed s/\\\\/\\\\\\\\/g)
CMAKE_CURRENT_SOURCE_DIR:=$(shell cygpath -a -w $(CMAKE_CURRENT_SOURCE_DIR) | sed s/\\\\/\\\\\\\\/g)
CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR:=$(shell cygpath -a $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR))
else
CMAKE_PER_TARGET_BUILD_DIR:=$(shell realpath --canonicalize-missing $(CMAKE_PER_TARGET_BUILD_DIR) )
CMAKE_CURRENT_SOURCE_DIR:=$(shell realpath --canonicalize-missing $(CMAKE_CURRENT_SOURCE_DIR) )
CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR:=$(shell realpath --canonicalize-missing $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR))
endif



ifeq (Unix,$(ProjectPlatformSubdir))
CC:=$(shell ../../ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" CompilerDriver-C)
CXX:=$(shell ../../ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" CompilerDriver-C++)
AR:=$(shell ../../ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" AR)
RANLIB:=$(shell ../../ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" RANLIB)
CFLAGS=$(shell ../../ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" EXTRA_COMPILER_ARGS)
#clang (not clang++) barfs if given --std=c++17 argument
#And we later get linker errors from clang if we use -flto on object files stored here
CFLAGS:=$(filter-out --std%, $(CFLAGS))
CFLAGS:=$(filter-out -std%, $(CFLAGS))
CFLAGS:=$(filter-out -flto, $(CFLAGS))
CXXFLAGS=$(shell ../../ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" EXTRA_COMPILER_ARGS)
CXXFLAGS:=$(filter-out -flto, $(CXXFLAGS))
#@todo fixup - bug workaround - configure autodetects -posix and se we have to add it in here. Must find
# some way we can ADD these params to CFLAGS without REPLACING
# -- without this hack - we get link error building Xerces
CFLAGS+= -pthread
CXXFLAGS+= -pthread
endif


all:
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Building Stroika/ThirdPartyComponents/xerces $(VERSION) {$(CONFIGURATION)}:"
ifeq ($(CONFIGURATION),)
	@for i in `../../ScriptsLib/GetConfigurations` ; do\
		$(MAKE) --no-print-directory all CONFIGURATION=$$i;\
	done
else
	@$(MAKE) --silent MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) CURRENT
	@../../ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
ifeq ($(PRODUCED_OUTPUT_ARTIFACTS),)
	$(error This makefile requires a recognized ProjectPlatformSubdir)
endif
	@$(MAKE) --no-print-directory --silent CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))  check
endif


$(PRODUCED_OUTPUT_ARTIFACTS):
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(MAKE) --silent CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) CURRENT $(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR)
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)  doBuilds_


clean:
	@rm -rf $(EXTRACTED_DIRNAME) $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR)


clobber:	clean
ifeq ($(CONFIGURATION),)
	@rm -rf $(BUILDS_ROOT)*/ThirdPartyComponents/lib/*xerces* $(BUILDS_ROOT)*/ThirdPartyComponents/include/*xerces*
	@rm -rf CURRENT
else
	@rm -f $(PRODUCED_OUTPUT_ARTIFACTS)

endif


check-prerequisite-tools:
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Checking Prerequisite tools for Xerces ... "
ifeq ($(shell ../../ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" qFeatureFlag_PrivateOverrideOfCMake), no)
	@{ type cmake > /dev/null 2>&1; } || { ../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) && ../../ScriptsLib/GetMessageForMissingTool.sh cmake && exit 1; }
endif
	@$(ECHO) "done";


check:
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Checking ... "
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
ifeq ($(PRODUCED_OUTPUT_ARTIFACTS),)
	$(error This makefile requires a recognized ProjectPlatformSubdir)
endif
	@for i in $(PRODUCED_OUTPUT_ARTIFACTS) ; do \
		if [ ! -e $$i ]; then\
			$(ECHO) "   FAILED: missing $$i";\
			exit 1;\
		fi;\
	done
	@$(ECHO) " Stroika/ThirdPartyComponents/Xerces -  [Succeeded]";


$(ZIPPEDFILE):
	@MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) ../../ScriptsLib/WebGet.sh $(ZIPPEDFILE) $(FETCHURLS)



ORIGs CURRENT $(PER_CONFIGURATION_BOOST_INTERMEDIATEFILES_DIR):	$(ZIPPEDFILE)
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "Extracting $(ZIPPEDFILE) to $@"` "... "
	@mkdir -p $@
ifeq ($(shell uname -s),Darwin)
	@tar xf $(ZIPPEDFILE) -C $@ --strip-components 1
else ifeq (CYGWIN,$(findstring CYGWIN,$(shell uname)))
	@#extra sleep and realpath to relative path since fails on cygwin otherwise
	@sleep 5 # I THINK this works around occasional bug with MSFT filesystem/cygwin - failing to find dir in realpath
	@tar xf $(ZIPPEDFILE) --no-same-owner --directory $(shell realpath --relative-to=`pwd` $@) --strip-components=1 || tar xf $(ZIPPEDFILE) --no-same-owner --directory $(shell realpath --relative-to=`pwd` $@) --strip-components=1
else
	@tar xf $(ZIPPEDFILE) --no-same-owner --directory $@ --strip-components=1
endif
	@echo done







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
CMAKE_ARGS+= -DCMAKE_C_COMPILER="$(shell which $(CC))"
CMAKE_ARGS+= -DCMAKE_CXX_COMPILER="$(shell which $(CXX))"
CMAKE_ARGS+= -DCMAKE_AR="$(shell which $(AR))"
CMAKE_ARGS+= -DCMAKE_RANLIB="$(shell which $(RANLIB))"
CMAKE_ARGS+= -DCMAKE_C_FLAGS="$(CFLAGS)"
CMAKE_ARGS+= -DCMAKE_CXX_FLAGS="$(CXXFLAGS)"

ifeq (0,$(ENABLE_ASSERTIONS))
CMAKE_CONFIGURATION=Release
else
CMAKE_CONFIGURATION=Debug
endif

#Default Xerces build generates depednency on DLL version of runtime
#TRIED To ADD (or replace /MDd with /MTd) from cmdline for cmake but could find no way.
#So used sed script to post-process CMakeCache.txt
#ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
#ifeq (Release,$(CMAKE_CONFIGURATION))
#CMAKE_ARGS+= -DCMAKE_CXX_FLAGS_RELEASE="/MT"
#else
#CMAKE_ARGS+= -DCMAKE_CXX_FLAGS_DEBUG="/MTd"
#endif
#endif


#@todo fix config mechanism to have var saying 32 or 64 bits?? Or -arch?
CMAKE_USE_GENERATOR=Unix Makefiles
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
ifeq ($(ARCH), x86)
CMAKE_USE_GENERATOR=Visual Studio 15 2017
else ifeq ($(ARCH), x86_64)
CMAKE_USE_GENERATOR=Visual Studio 15 2017 Win64
endif
endif



ALTERNATE_CMAKE_BINDIR=
ifeq ($(UNAME),Cygwin)
ALTERNATE_CMAKE_BINDIR=$(shell cygpath --absolute --unix ../cmake/CURRENT/bin/)
endif



doBuilds_:
	@mkdir -p $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR)
	@touch $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR)/CONFIGURE.OUT
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "Configuring (Log to $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR)/CONFIGURE.OUT)"` "... "
	@echo 'cd $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR) ; $(ALTERNATE_CMAKE_BINDIR)cmake -G "$(CMAKE_USE_GENERATOR)" $(CMAKE_ARGS) $(CMAKE_CURRENT_SOURCE_DIR)' > $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR)/CONFIGURE.OUT 2>&1
	@cd $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR) ; $(ALTERNATE_CMAKE_BINDIR)cmake -G "$(CMAKE_USE_GENERATOR)" $(CMAKE_ARGS) $(CMAKE_CURRENT_SOURCE_DIR) >> $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR)/CONFIGURE.OUT 2>&1
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@###I could find no param to set going into cmake to use the static link version of the C++ runtime, but this does it (I hope not too aggressively)
	@echo "sed -i CMakeCache.txt -e 's/MD/MT/g'" >> $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR)/CONFIGURE.OUT
	@cd $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR) && sed -i CMakeCache.txt -e 's/MD/MT/g'
	@###Then though not documented, you have to re-run cmake SRCDIR to get it to pay attention to the changes
	@echo 'cd $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR) ; $(ALTERNATE_CMAKE_BINDIR)cmake $(CMAKE_CURRENT_SOURCE_DIR)' >> $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR)/CONFIGURE.OUT 2>&1
	@cd $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR) ; $(ALTERNATE_CMAKE_BINDIR)cmake $(CMAKE_CURRENT_SOURCE_DIR) >> $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR)/CONFIGURE.OUT 2>&1
endif	
	@echo done
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "Compiling (Log to $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR)/BUILD.OUT)"` "... "
	@cd $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR) && $(ECHO) "$(ALTERNATE_CMAKE_BINDIR)cmake --build . --config $(CMAKE_CONFIGURATION)" > $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR)/BUILD.OUT 2>&1
	@cd $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR) && $(ALTERNATE_CMAKE_BINDIR)cmake --build . --config $(CMAKE_CONFIGURATION) >> $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR)/BUILD.OUT 2>&1
	@$(ECHO) "done"
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "Copying builds to $(BUILDS_ROOT) (Log to $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR)/INSTALL.OUT)"` "... "
	@#NB: Set MAKEFLAGS= to silence warnings about -j jobserver when run with -j N (probaly bug with Xerces 3.2 cmake makefile but didnt dig cuz not important)
	@cd $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR) && $(ECHO) "$(ALTERNATE_CMAKE_BINDIR)cmake --build . --config $(CMAKE_CONFIGURATION) --target install" > $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR)/INSTALL.OUT
	@MAKEFLAGS= && cd $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR) && $(ALTERNATE_CMAKE_BINDIR)cmake --build . --config $(CMAKE_CONFIGURATION) --target install >> $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR)/INSTALL.OUT
ifeq (Unix,$(ProjectPlatformSubdir))
	@cd $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib && ln -s libxerces-c-3.2.a libxerces-c.a
endif
	@$(ECHO) "done"
