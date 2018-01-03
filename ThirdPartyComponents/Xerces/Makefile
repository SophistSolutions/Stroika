ifneq ($(CONFIGURATION),)
	ProjectPlatformSubdir	=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)
endif


.DEFAULT_GOAL := all
.PHONY:	all clean clobber check doBuilds_ doCreate_ doPatching_

SHELL=/bin/bash

.NOTPARALLEL:




VERSION=3.2.0
#VERSION=3.1.4
BASENAME=xerces-c-$(VERSION)
ZIPPEDFILE=../Origs-Cache/$(BASENAME).tar.gz
TARFILE=$(BASENAME).tar
EXTRACTED_DIRNAME=$(BASENAME)



ifneq ($(CONFIGURATION),)
ARCH							=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ARCH)
ProjectPlatformSubdir			=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)
IncludeDebugSymbolsInLibraries	=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) IncludeDebugSymbolsInLibraries)
ENABLE_ASSERTIONS				=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ENABLE_ASSERTIONS)
OptimizerFlag					=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) OptimizerFlag)
endif


ifeq (3.1,$(findstring 3.1,$(VERSION)))
BUILD_WITH_CMAKE=0
else
BUILD_WITH_CMAKE=1
endif


MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO?=	$(shell ../../ScriptsLib/GetDefaultShellVariable.sh ECHO)

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
ifeq (1,$(BUILD_WITH_CMAKE))
ifeq (Debug,$(findstring Debug, $(CONFIGURATION)))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/xerces-c_3D.lib
endif
ifeq (Release,$(findstring Release, $(CONFIGURATION)))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/xerces-c_3.lib
endif
else
ifeq (Debug,$(findstring Debug, $(CONFIGURATION)))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/xerces-c_static_3D.lib
endif
ifeq (Release,$(findstring Release, $(CONFIGURATION)))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/xerces-c_static_3.lib
endif
endif
endif

ifeq (1,$(BUILD_WITH_CMAKE))
CMAKE_PER_TARGET_BUILD_DIR:=../../Builds/$(CONFIGURATION)/ThirdPartyComponents/
CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR:=../../IntermediateFiles/$(CONFIGURATION)/ThirdPartyComponents/Xerces
CMAKE_CURRENT_SOURCE_DIR:=CURRENT
ifeq ($(UNAME),Cygwin)
CMAKE_PER_TARGET_BUILD_DIR:=$(shell cygpath -a -w $(CMAKE_PER_TARGET_BUILD_DIR) | sed s/\\\\/\\\\\\\\/g)
CMAKE_CURRENT_SOURCE_DIR:=$(shell cygpath -a -w $(CMAKE_CURRENT_SOURCE_DIR) | sed s/\\\\/\\\\\\\\/g)
CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR:=$(shell cygpath -a $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR))
else
CMAKE_PER_TARGET_BUILD_DIR:=$(shell realpath --canonicalize-missing $(CMAKE_PER_TARGET_BUILD_DIR) )
CMAKE_CURRENT_SOURCE_DIR:=$(shell realpath --canonicalize-missing $(CMAKE_CURRENT_SOURCE_DIR) )
CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR:=$(shell realpath --canonicalize-missing $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR))
endif
endif



ifeq (Unix,$(ProjectPlatformSubdir))
CC=$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) CompilerDriver-C)
CXX=$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) CompilerDriver-C++)
AR=$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) AR)
RANLIB=$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) RANLIB)
#CFLAGS=$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) EXTRA_COMPILER_ARGS)
endif


all:
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Building Stroika/ThirdPartyComponents/xerces $(VERSION) {$(CONFIGURATION)}:"
ifeq ($(CONFIGURATION),)
	@for i in `../../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory all CONFIGURATION=$$i;\
	done
else
ifeq ($(PRODUCED_OUTPUT_ARTIFACTS),)
	$(error This makefile requires a recognized ProjectPlatformSubdir)
endif
	@$(MAKE) --no-print-directory --silent CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))  check
endif


$(PRODUCED_OUTPUT_ARTIFACTS):
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@#with cmake we dont change CURRENT, but other makes do write files there...
ifeq (1,$(BUILD_WITH_CMAKE))
	@$(MAKE) --silent CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) CURRENT
else
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)  doCreate_
endif
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)  doBuilds_


clean:
ifeq (1,$(BUILD_WITH_CMAKE))
	@rm -rf $(EXTRACTED_DIRNAME) $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR)
else
	@rm -rf $(EXTRACTED_DIRNAME) CURRENT
endif


clobber:	clean
ifeq ($(CONFIGURATION),)
	@rm -rf $(BUILDS_ROOT)*/ThirdPartyComponents/lib/*xerces* $(BUILDS_ROOT)*/ThirdPartyComponents/include/*xerces*
else
	@rm -f $(PRODUCED_OUTPUT_ARTIFACTS)
endif
	@rm -rf CURRENT


check-prerequisite-tools:
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Checking Prerequisite tools for Xerces..."
ifeq (1,$(BUILD_WITH_CMAKE))
ifeq ($(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) qFeatureFlag_PrivateOverrideOfCMake), no)
	@{ type cmake > /dev/null 2>&1; } || { ../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) && ../../ScriptsLib/GetMessageForMissingTool.sh cmake && exit 1; }
endif
endif
	@$(ECHO) "done";


check:
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Checking..."
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
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ../../ScriptsLib/WebGet.sh $(ZIPPEDFILE) $(FETCHURLS)



CURRENT:	$(ZIPPEDFILE)
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)  doCreate_


doCreate_:	$(ZIPPEDFILE)
	@#### @todo FIX this code so this is not deleted/re-created each time we build!!!! -- not needed once we switch to cmake build -- LGP 2017-10-30
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n "Creating CURRENT...";
	@mkdir -p CURRENT && rm -rf CURRENT/*
ifeq ($(shell uname -s),Darwin)
	@tar xf $(ZIPPEDFILE) -C CURRENT --strip-components 1
else
	@tar xf $(ZIPPEDFILE) --no-same-owner --directory CURRENT --strip-components=1
endif
	@$(ECHO) "done"
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)  doPatching_


doPatching_:
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
ifeq (0,$(BUILD_WITH_CMAKE))
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n "Patching Xerces...";
	@(cd CURRENT; tar xf ../Patches/VC15Projects.tar.gz --no-same-owner);
	@$(ECHO) "done"
endif
endif


ifeq (0,$(BUILD_WITH_CMAKE))
ifeq (Unix,$(ProjectPlatformSubdir))
CONFIGURE_ARGS=
CONFIGURE_ARGS+=--enable-static --disable-shared --without-icu --without-curl
#on RedHat6, I had trouble with icu, so lose the dependency - not sure why helpful; I think issue
#also exists with gnuiconv - and has todo with fact that service script (init.d stuff) sets LANG=""
CONFIGURE_ARGS+=--enable-transcoder-iconv
CONFIGURE_ARGS+=--disable-transcoder-gnuiconv
CONFIGURE_ARGS+=--disable-transcoder-icu
ifeq ($(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) CrossCompiling),true)
CONFIGURE_ARGS+= --host $(shell $(CXX) -dumpmachine)
endif
endif
endif




ifeq (1,$(BUILD_WITH_CMAKE))

CMAKE_ARGS=
CMAKE_ARGS+= -Dnetwork:BOOL=OFF
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
CMAKE_ARGS+= -Dtranscoder=windows
else ifeq ($(UNAME),Darwin)
#AVOID Undefined symbols for architecture x86_64:  "_CFRelease", etc.... Avoid library dependency
CMAKE_ARGS+= -Dtranscoder=iconv
endif
CMAKE_ARGS+= -Dmessage-loader=inmemory
CMAKE_ARGS+= -DBUILD_SHARED_LIBS:BOOL=OFF
CMAKE_ARGS+= -DCMAKE_INSTALL_PREFIX=$(CMAKE_PER_TARGET_BUILD_DIR)


ifeq (0,$(ENABLE_ASSERTIONS))
CMAKE_CONFIGURATION=Release
else
CMAKE_CONFIGURATION=Debug
endif

#Default Xerces build generates depednency on DLL version of runtime
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
ifeq (Release,$(CMAKE_CONFIGURATION))
CMAKE_ARGS+= -DCMAKE_CXX_FLAGS_RELEASE="/MT"
else
CMAKE_ARGS+= -DCMAKE_CXX_FLAGS_DEBUG="/MTd"
endif
endif


#@todo fix config mechanism to have var saying 32 or 64 bits?? Or -arch?
CMAKE_USE_GENERATOR=Unix Makefiles
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
ifeq ($(ARCH), x86)
CMAKE_USE_GENERATOR=Visual Studio 15 2017
else ifeq ($(ARCH), x86_64)
CMAKE_USE_GENERATOR=Visual Studio 15 2017 Win64
endif
endif

endif


ALTERNATE_CMAKE_BINDIR=
ifeq ($(UNAME),Cygwin)
ALTERNATE_CMAKE_BINDIR=$(shell cygpath --absolute --unix ../cmake/CURRENT/bin/)
endif



doBuilds_:
ifeq (1,$(BUILD_WITH_CMAKE))
	@mkdir -p $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR)
	@touch $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR)/CONFIGURE.OUT
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "Configuring (Log to $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR)/CONFIGURE.OUT) ... "`
	@echo 'export CC=$(CC); export CXX=$(CXX); export AR=$(AR); export RANLIB=$(RANLIB); cd $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR) ; $(ALTERNATE_CMAKE_BINDIR)cmake -G "$(CMAKE_USE_GENERATOR)" $(CMAKE_ARGS) $(CMAKE_CURRENT_SOURCE_DIR)' >> $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR)/CONFIGURE.OUT 2>&1
	@export CC=$(CC); export CXX=$(CXX); export AR=$(AR); export RANLIB=$(RANLIB); cd $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR) ; $(ALTERNATE_CMAKE_BINDIR)cmake -G "$(CMAKE_USE_GENERATOR)" $(CMAKE_ARGS) $(CMAKE_CURRENT_SOURCE_DIR) >> $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR)/CONFIGURE.OUT 2>&1
	@echo done
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "Compiling (Log to $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR)/BUILD.OUT) ... "`
	@cd $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR) && $(ALTERNATE_CMAKE_BINDIR)cmake --build . --config $(CMAKE_CONFIGURATION) >> $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR)/BUILD.OUT 2>&1
else ifeq (Unix,$(ProjectPlatformSubdir))
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n "Compiling (REDIRECT to CURRENT/BUILD.OUT)..."
	@echo "export CC=\"$(CC)\"; export CXX=\"$(CXX)\"; ./configure $(CONFIGURE_ARGS)" > CURRENT/BUILD.OUT
	@cd CURRENT; export CC="$(CC)"; export CXX="$(CXX)"; ./configure $(CONFIGURE_ARGS) 2>&1 >> BUILD.OUT
	$(MAKE) --directory CURRENT --no-print-directory -s CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) all 2>&1 >> CURRENT/BUILD.OUT
else
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n "Compiling (Log to CURRENT/BUILD.OUT)..."
ifeq ($(CONFIGURATION),Debug-U-32)
	@perl ../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:Summary CURRENT/Projects/Win32/VC15/xerces-all/XercesLib/XercesLib.vcxproj /p:Configuration=\"Static Debug\",Platform=Win32 /target:Build" > CURRENT/BUILD.OUT 2>&1
endif
ifeq ($(CONFIGURATION),Release-U-32)
	@perl ../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:Summary CURRENT/Projects/Win32/VC15/xerces-all/XercesLib/XercesLib.vcxproj /p:Configuration=\"Static Release\",Platform=Win32 /target:Build" > CURRENT/BUILD.OUT 2>&1
endif
ifeq ($(CONFIGURATION),Release-DbgMemLeaks-U-32)
	@perl ../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:Summary CURRENT/Projects/Win32/VC15/xerces-all/XercesLib/XercesLib.vcxproj /p:Configuration=\"Static Release\",Platform=Win32 /target:Build" > CURRENT/BUILD.OUT 2>&1
endif
ifeq ($(CONFIGURATION),Release-Logging-U-32)
	@perl ../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:Summary CURRENT/Projects/Win32/VC15/xerces-all/XercesLib/XercesLib.vcxproj /p:Configuration=\"Static Release\",Platform=Win32 /target:Build" > CURRENT/BUILD.OUT 2>&1
endif
ifeq ($(CONFIGURATION),Debug-U-64)
	@perl ../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:Summary CURRENT/Projects/Win32/VC15/xerces-all/XercesLib/XercesLib.vcxproj /p:Configuration=\"Static Debug\",Platform=x64 /target:Build" > CURRENT/BUILD.OUT 2>&1
endif
ifeq ($(CONFIGURATION),Release-U-64)
	@perl ../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:Summary CURRENT/Projects/Win32/VC15/xerces-all/XercesLib/XercesLib.vcxproj /p:Configuration=\"Static Release\",Platform=x64 /target:Build" > CURRENT/BUILD.OUT 2>&1
endif
ifeq ($(CONFIGURATION),Release-Logging-U-64)
	@perl ../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) "MSBuild.exe /nologo /v:quiet /clp:Summary CURRENT/Projects/Win32/VC15/xerces-all/XercesLib/XercesLib.vcxproj /p:Configuration=\"Static Release\",Platform=x64 /target:Build" > CURRENT/BUILD.OUT 2>&1
endif
endif
	@$(ECHO) "done"
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "Copying builds to $(BUILDS_ROOT) (Log to $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR)/INSTALL.OUT) ... "`
ifeq (1,$(BUILD_WITH_CMAKE))
	@#NB: Set MAKEFLAGS= to silence warnings about -j jobserver when run with -j N (probaly bug with Xerces 3.2 cmake makefile but didnt dig cuz not important)
	@MAKEFLAGS= && cd $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR) && $(ALTERNATE_CMAKE_BINDIR)cmake --build . --config $(CMAKE_CONFIGURATION) --target install >> $(CMAKE_PER_TARGET_INTERMEDIATEFILES_DIR)/INSTALL.OUT
ifeq (Unix,$(ProjectPlatformSubdir))
	@cd $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib && ln -s libxerces-c-3.2.a libxerces-c.a
endif
else ifeq (Unix,$(ProjectPlatformSubdir))
	@mkdir -p $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib && cp CURRENT/src/.libs/libxerces-c.a $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/
else
ifeq ($(CONFIGURATION),Debug-U-32)
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/lib && cp "CURRENT/Build/Win32/VC12/Static Debug"/xerces-c_static_3* $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Release-U-32)
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/lib && cp "CURRENT/Build/Win32/VC12/Static Release"/xerces-c_static_3* $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Release-DbgMemLeaks-U-32)
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/lib && cp "CURRENT/Build/Win32/VC12/Static Release"/xerces-c_static_3* $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Release-Logging-U-32)
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/lib && cp "CURRENT/Build/Win32/VC12/Static Release"/xerces-c_static_3* $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Debug-U-64)
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/lib && cp "CURRENT/Build/Win64/VC12/Static Debug"/xerces-c_static_3* $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Release-U-64)
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/lib && cp "CURRENT/Build/Win64/VC12/Static Release"/xerces-c_static_3* $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Release-Logging-U-64)
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/lib && cp "CURRENT/Build/Win64/VC12/Static Release"/xerces-c_static_3* $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/include && cp -r CURRENT/src/xercesc $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/include/
endif
endif
	@$(ECHO) "done"
