#
#	TODO:
###		@todo	use cmake 'Unix Makefiles' so that I can run make -jN (or so regular make inherits flags from $(MAKE)
###				then use that to lose horrible MAKEFLAGS= hack
###				***NOTE*** this is by far the slowest part of the UNIX build process (latency) - so this maybe a priority
###
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

include $(StroikaRoot)/ScriptsLib/SharedMakeVariables-Default.mk


ifneq ($(CONFIGURATION),)
include $(StroikaRoot)ScriptsLib/Makefile-CMake-Common.mk
endif

.PHONY:	all clean clobber check doBuilds_


VERSION=3.2.3
#VERSION=3.2.2
#VERSION=3.2.1
BASENAME=xerces-c-$(VERSION)
ZIPPEDFILE=../Origs-Cache/$(BASENAME).tar.gz
TARFILE=$(BASENAME).tar
EXTRACTED_DIRNAME=$(BASENAME)

PER_CONFIGURATION_BUILDS_DIR:=$(shell realpath --canonicalize-missing ../../Builds/$(CONFIGURATION)/ThirdPartyComponents/)/

#lose trailing / on PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_ because on macos make THATDIR doesn't work with trailing slash (old WEIRD_MACOS_MAKEFILE_DIR_BUG -  MacOS XCode 10 - GNU Make 3.81)
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_:=$(shell realpath --canonicalize-missing ../../IntermediateFiles/$(CONFIGURATION)/ThirdPartyComponents/Xerces)

OUTPUT_WORKDIR_PRETTYNAME="$(shell $(StroikaRoot)ScriptsLib/SubstituteBackVariables "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/")"
#OUTPUT_WORKDIR_PRETTYNAME="{StroikaRoot}Inter.../Thir.../Xerces/"


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





PRODUCED_OUTPUT_ARTIFACTS=	\
	$(PER_CONFIGURATION_BUILDS_DIR)include/xercesc/dom/DOM.hpp

ifeq (Unix,$(ProjectPlatformSubdir))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/libxerces-c.a
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
ifeq (Debug,$(CMAKE_CONFIGURATION))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/xerces-c_3D.lib
endif
ifeq (Release,$(CMAKE_CONFIGURATION))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/xerces-c_3.lib
endif
endif

ifneq ($(CONFIGURATION),)
CMAKE_CURRENT_SOURCE_DIR:=$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
CMAKE_CURRENT_SOURCE_DIR:=$(shell realpath --canonicalize-missing $(CMAKE_CURRENT_SOURCE_DIR) )
CMAKE_CURRENT_SOURCE_DIR:=$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(CMAKE_CURRENT_SOURCE_DIR))
endif

ifeq (true,$(HasMakefileBugWorkaround_lto_skipping_undefined_incompatible))
### not sure why???
#Building Stroika Tools {clang++-6-release-libstdc++}:
#         Compiling {StroikaRoot}Tools/Sources/Stroika/Frameworks/WebServer/HTMLViewCompiler/HTMLViewCompiler.cpp ...
#      Linking  $StroikaRoot/Builds/clang++-6-release-libstdc++/HTMLViewCompiler...
#/usr/bin/ld: skipping incompatible /Sandbox/Stroika-Dev/Builds/clang++-6-release-libstdc++/ThirdPartyComponents/lib//libxerces-c.so when searching for -lxerces-c
CFLAGS:=$(filter-out -flto, $(CFLAGS))
CXXFLAGS:=$(filter-out -flto, $(CXXFLAGS))
else ifeq (Unix,$(ProjectPlatformSubdir))
## similar bug with g++ - but just warnings here (all regtests pass without this change)
##/home/lewis/Sandbox/Stroika-Build-Dir-Ubuntu1804_x86_64/Builds/g++-optimized/ThirdPartyComponents/include/xercesc/util/BinInputStream.hpp:29:22: warning: type 'struct BinInputStream' violates the C++ One Definition Rule [-Wodr]
CFLAGS:=$(filter-out -flto, $(CFLAGS))
CXXFLAGS:=$(filter-out -flto, $(CXXFLAGS))
endif


ifeq (Unix,$(ProjectPlatformSubdir))
#@todo fixup - bug workaround - configure autodetects -posix and se we have to add it in here. Must find
# some way we can ADD these params to CFLAGS without REPLACING
# -- without this hack - we get link error building Xerces
CFLAGS+= -pthread
CXXFLAGS+= -pthread
endif


##
## It appears just /I"C:/Program Files (x86)/Windows Kits/10/include/10.0.18362.0/ucrt"
## causes build problems. Unsure why (didnt did much).
##
## But there is no need for any of the /I parameters, as none were supported before Stroika 2.1a2, so strip them all for now
##		-- LGP 2019-09-26
##
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
CFLAGS:=$(call STRIP_INCLUDE_COMPILER_FLAGS,$(CFLAGS))
CXXFLAGS:=$(call STRIP_INCLUDE_COMPILER_FLAGS,$(CXXFLAGS))
endif


# Avoid having warnings leak from here to linking code when Xerces is used --LGP 2022-01-18
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
ifneq ($(findstring -GL,$(CXXFLAGS)),)
CXXFLAGS+= -wd4702 
endif
endif



all:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents/xerces $(VERSION) Build {$(CONFIGURATION)}:"
ifeq ($(CONFIGURATION),)
	@for i in `$(StroikaRoot)GetConfigurations` ; do\
		$(MAKE) --no-print-directory all CONFIGURATION=$$i;\
	done
else
	@#make zippedfile even though a dependency on CURRENT and PRODUCED_ARTIFACTS for parallel-make so completed before either starts (else one sees file partly downloaded and proceeeds)
	@$(MAKE) --silent MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $(ZIPPEDFILE)
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
ifeq ($(PRODUCED_OUTPUT_ARTIFACTS),)
	$(error This makefile requires a recognized ProjectPlatformSubdir)
endif
	@$(MAKE) --no-print-directory --silent MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $(PRODUCED_OUTPUT_ARTIFACTS)
ifneq ($(QUICK_BUILD), 1)
	@$(MAKE) --no-print-directory --silent MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) CURRENT
endif
	@$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))  check
endif

#ALL produced artifacts produced by the same thing. But if you try the same rule to make all, if parallel makes invoked on this, you get
#multiple tries to run the same rule. So remap all to making the first.
$(wordlist 2, 9999, $(PRODUCED_OUTPUT_ARTIFACTS)):	$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS))

$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS)):
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(MAKE) --silent $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
	@$(MAKE) --no-print-directory doBuilds_


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
	@rm -fr $(PRODUCED_OUTPUT_ARTIFACTS) $(PER_CONFIGURATION_BUILDS_DIR)include/xercesc
endif


check:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "Xerces ${VERSION} - Checking ... "
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
ifeq ($(PRODUCED_OUTPUT_ARTIFACTS),)
	$(error This makefile requires a recognized ProjectPlatformSubdir)
endif
	@$(StroikaRoot)ScriptsLib/CheckFileExists $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(ECHO) " Stroika/ThirdPartyComponents/Xerces -  [Succeeded]";


$(ZIPPEDFILE):
	@MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) $(StroikaRoot)WebGet $(ZIPPEDFILE) $(FETCHURLS)


ORIGs CURRENT $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_):	$(ZIPPEDFILE)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "Xerces ${VERSION} - Extracting $(ZIPPEDFILE) to $@"` "... "
	@mkdir -p $@
ifeq ($(DETECTED_HOST_OS),Darwin)
	@tar xf $(ZIPPEDFILE) -C $@ --strip-components 1
else ifneq ($(findstring $(DETECTED_HOST_OS),MSYS-Cygwin),)
	@PWD=`pwd` tar xf $(ZIPPEDFILE) --no-same-owner --directory `realpath --relative-to=$$PWD $@` --strip-components=1
else
	@tar xf $(ZIPPEDFILE) --no-same-owner --directory $@ --strip-components=1
endif
	# Char16Test_IOSTREAMCHANGE for compatability with Visual Studio .Net 2022 17.1.0 compiler --LGP 2022-02-16
	@if [ "$@" != "ORIGs" ] ; then \
		$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Xerces ${VERSION} - Patching ... ";\
		patch -p0 $@/src/xercesc/internal/ElemStack.cpp Patches/SilenceSanitizerWarningAboutMemcpyNullPtr.PATCH >> $@/PATCHING.OUT;\
		patch -p0 $@/tests/src/Char16Test/Char16Test.cpp Patches/Char16Test_IOSTREAMCHANGE.PATCH >> $@/PATCHING.OUT;\
	fi






###NOTE - because we set network:BOOL:OFF - Xerces doesn't depend on CURL (but we could make it so if we wanted, but then update dependencies
### in ThirdPartyLibs/Makefile
CMAKE_ARGS+= -Dnetwork:BOOL=OFF
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
CMAKE_ARGS+= -Dtranscoder=windows
else ifeq ($(DETECTED_HOST_OS),Darwin)
#AVOID Undefined symbols for architecture x86_64:  "_CFRelease", etc.... Avoid library dependency
CMAKE_ARGS+= -Dtranscoder=iconv
else
CMAKE_ARGS+= -Dtranscoder=gnuiconv
endif
CMAKE_ARGS+= -Dmessage-loader=inmemory
CMAKE_ARGS+= -DBUILD_SHARED_LIBS:BOOL=OFF

##
### Avoid compile error in time_t XMLDateTime::getEpoch(bool duration) const
### not sure why this is needed?
###  -- LGP 2019-09-26
##
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
CFLAGS += /DWIN32
CXXFLAGS += /DWIN32
endif



#
#PARALLEL BUILD TRICKERY
#	We CANNOT find out the argument to -j (-jN). But even if we could, we would
#	really need to shared the same job-server in any other tasks which is captured in $(JOBSERVER_INFO)
#	Really - for now - just use if {JOBSERVER_INFO} is empty or not as a flag for whether or not to do any
#	parallel build magic (windoze and unix)
#		-- LGP 2019-01-10
#

# JOBSERVER_INFO is the data to be assigned to MFLAGS in calling sub-make.
# MAYBE it should come from MAKEFLAGS? and maybe 
# NOTE: SUBTLE - YOU CANNOT read the MFLAGS variable from THIS MAKE, but must do it from a SUBSHELL
JOBSERVER_INFO=$(filter -j --jobserver-fds=% --jobserver-auth=%, $(shell echo $${MFLAGS}))

ifneq ($(JOBSERVER_INFO),)
ifeq (Unix,$(ProjectPlatformSubdir))
# Trick to make gnu make JOBSERVER work with cmake - pass args to trampoline with environment variables; doesn't work on darwin(debug? lowpri)
export MAKE
CMAKE_ARGS+= -DCMAKE_MAKE_PROGRAM="$(shell realpath MAKE-Trampoline)"
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
# closest we can easily get to approximating support for job-server
export CL+=/MP
endif
endif

ifeq ($(DETECTED_HOST_OS),MSYS)
# Only needed to prevent failure with -j flag
TOOLSET_CMD_ENV_SETS_+= MAKEFLAGS= MFLAGS= CL= 
endif

doBuilds_:
	@mkdir -p $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
	@rm -f $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIGURE-OUT.txt && touch  $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIGURE-OUT.txt
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "Xerces ${VERSION} - Configuring (See $(OUTPUT_WORKDIR_PRETTYNAME)CONFIGURE-OUT.txt)"` "... "
	@echo CXX='$(CXX)' >>  $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIGURE-OUT.txt
	@echo CFLAGS='$(CFLAGS)' >>  $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIGURE-OUT.txt
	@echo CXXFLAGS='$(CXXFLAGS)' >>  $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIGURE-OUT.txt
	@echo 'cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) ; $(TOOLSET_CMD_ENV_SETS_)cmake -G "$(CMAKE_USE_GENERATOR)" $(CMAKE_ARGS) $(CMAKE_CURRENT_SOURCE_DIR)' >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIGURE-OUT.txt 2>&1
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) ; $(TOOLSET_CMD_ENV_SETS_)cmake -G "$(CMAKE_USE_GENERATOR)" $(CMAKE_ARGS) $(CMAKE_CURRENT_SOURCE_DIR) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIGURE-OUT.txt 2>&1
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "Xerces ${VERSION} - Compiling (See $(OUTPUT_WORKDIR_PRETTYNAME)BUILD-OUT.txt)"` "... "
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && $(ECHO) $(TOOLSET_CMD_ENV_SETS_)"cmake --build . --config $(CMAKE_CONFIGURATION)" > $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD-OUT.txt 2>&1
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && $(TOOLSET_CMD_ENV_SETS_)cmake --build . --config $(CMAKE_CONFIGURATION) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD-OUT.txt 2>&1
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "Xerces ${VERSION} - Copying builds (See $(OUTPUT_WORKDIR_PRETTYNAME)INSTALL.OUT)"` "... "
	@#NB: Set MAKEFLAGS= to silence warnings about -j jobserver when run with -j N (probaly bug with Xerces 3.2 cmake makefile but didnt dig cuz not important)
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && $(ECHO) $(TOOLSET_CMD_ENV_SETS_)"cmake --build . --config $(CMAKE_CONFIGURATION) --target install" > $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/INSTALL.OUT
	@MAKEFLAGS= && cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && $(TOOLSET_CMD_ENV_SETS_)cmake --build . --config $(CMAKE_CONFIGURATION) --target install >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/INSTALL.OUT
ifeq (Unix,$(ProjectPlatformSubdir))
ifeq (3.2.2,$(VERSION))
	@cd ${PER_CONFIGURATION_BUILDS_DIR}lib && rm -f libxerces-c.a && ln -s libxerces-c-3.2.a libxerces-c.a
endif
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@#We get warnings about a missing pdb file when linking otherwise...(not sure why since symbol info sb in the .lib)
	@$(ECHO) "cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/src/xerces-c.dir/$(CMAKE_CONFIGURATION)/xerces*.pdb ${PER_CONFIGURATION_BUILDS_DIR}lib/ 2>/dev/null || :" >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/INSTALL.OUT
	@cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/src/xerces-c.dir/$(CMAKE_CONFIGURATION)/xerces*.pdb ${PER_CONFIGURATION_BUILDS_DIR}lib/ 2>/dev/null || :
endif



check-prerequisite-tools:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "Xerces ${VERSION} - Checking Prerequisite tools ... "
	@{ type cmake > /dev/null 2>&1; } || { $(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) && $(StroikaRoot)ScriptsLib/GetMessageForMissingTool cmake && exit 1; }
	@$(ECHO) "done";
