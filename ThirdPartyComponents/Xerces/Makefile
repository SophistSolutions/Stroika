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


.PHONY:	all clean clobber check doBuilds_


VERSION=3.2.2
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


ifneq (,$(findstring /cygdrive/,$(PATH)))
    UNAME := Cygwin
else
ifneq (,$(findstring WINDOWS,$(PATH)))
    UNAME := Windows
else
    UNAME := $(shell uname -s)
endif
endif


ifeq (0,$(ENABLE_ASSERTIONS))
CMAKE_CONFIGURATION:=Release
else
CMAKE_CONFIGURATION:=Debug
endif



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
CMAKE_PER_TARGET_BUILD_DIR:=../../Builds/$(CONFIGURATION)/ThirdPartyComponents/
CMAKE_PER_TARGET_BUILD_DIR:=$(shell realpath --canonicalize-missing $(CMAKE_PER_TARGET_BUILD_DIR) )
CMAKE_PER_TARGET_BUILD_DIR:=$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(CMAKE_PER_TARGET_BUILD_DIR))
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


### makefile forces /Zi (at least during configure) and that's not compile with /GL (whole program optimization) and so compiler/config process
### errors out
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
CFLAGS:=$(filter-out /GL, $(CFLAGS))
CXXFLAGS:=$(filter-out /GL, $(CXXFLAGS))
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
	@rm -f $(PRODUCED_OUTPUT_ARTIFACTS)
endif


check:
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "Xerces ${VERSION} - Checking ... "
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





#### SEE IF OTHER GENERATORS WORK WELL/BETTER??? https://www.mankier.com/7/cmake-generators
CMAKE_USE_GENERATOR=Unix Makefiles
ifeq ($(ProjectPlatformSubdir),VisualStudio.Net-2019)
CMAKE_USE_GENERATOR=Visual Studio 16 2019
else ifeq ($(ProjectPlatformSubdir),VisualStudio.Net-2017)
CMAKE_USE_GENERATOR=Visual Studio 15 2017
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
$(error "unsupported version of visual studio.net")
endif


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
else
CMAKE_ARGS+= -DCMAKE_C_COMPILER="$(CC)"
CMAKE_ARGS+= -DCMAKE_CXX_COMPILER="$(CXX)"
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
ifeq ($(ProjectPlatformSubdir),VisualStudio.Net-2019)
ifeq (x86_64,$(ARCH))
CMAKE_ARGS+= -A x64
else
CMAKE_ARGS+= -A Win32
endif
else ifeq ($(ProjectPlatformSubdir),VisualStudio.Net-2017)
ifeq (x86_64,$(ARCH))
CMAKE_USE_GENERATOR = Visual Studio 15 2017 Win64
endif
else
$(error "unsupported version of visual studio.net")
endif
endif

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
# A bit confusing how cmake uses the _DEBUG and _RELEASE flags on windows but not unix
# No matter - set them all to the same thing
#
CMAKE_ARGS+= -DCMAKE_C_FLAGS='$(CFLAGS)'
CMAKE_ARGS+= -DCMAKE_C_FLAGS_DEBUG='$(CFLAGS)'
CMAKE_ARGS+= -DCMAKE_C_FLAGS_RELEASE='$(CFLAGS)'
CMAKE_ARGS+= -DCMAKE_CXX_FLAGS='$(CXXFLAGS)'
CMAKE_ARGS+= -DCMAKE_CXX_FLAGS_DEBUG='$(CXXFLAGS)'
CMAKE_ARGS+= -DCMAKE_CXX_FLAGS_RELEASE='$(CXXFLAGS)'


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






# For reasons not totally understood - probably just that the cmake in cygwin is quite old - it doesn't work right here.
# so use the copy from visual studio which appears to be recent enough to know about the verison of visual studio it comes with ;-)
#	--LGP 2018-12-15
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
PATH_ADJUSTMENT_PREFIX= PATH="$(TOOLS_PATH_ADDITIONS):$(PATH)" 
endif


doBuilds_:
	@mkdir -p $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
	@rm -f $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIGURE.OUT && touch  $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIGURE.OUT
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "Xerces ${VERSION} - Configuring (See $(OUTPUT_WORKDIR_PRETTYNAME)CONFIGURE.OUT)"` "... "
	@echo CXX='$(CXX)' >>  $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIGURE.OUT
	@echo CFLAGS='$(CFLAGS)' >>  $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIGURE.OUT
	@echo CXXFLAGS='$(CXXFLAGS)' >>  $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIGURE.OUT
	@echo 'cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) ; $(PATH_ADJUSTMENT_PREFIX)cmake -G "$(CMAKE_USE_GENERATOR)" $(CMAKE_ARGS) $(CMAKE_CURRENT_SOURCE_DIR)' >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIGURE.OUT 2>&1
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) ; $(PATH_ADJUSTMENT_PREFIX)cmake -G "$(CMAKE_USE_GENERATOR)" $(CMAKE_ARGS) $(CMAKE_CURRENT_SOURCE_DIR) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIGURE.OUT 2>&1
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "Xerces ${VERSION} - Compiling (See $(OUTPUT_WORKDIR_PRETTYNAME)BUILD.OUT)"` "... "
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && $(ECHO) $(PATH_ADJUSTMENT_PREFIX)"cmake --build . --config $(CMAKE_CONFIGURATION)" > $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD.OUT 2>&1
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && $(PATH_ADJUSTMENT_PREFIX)cmake --build . --config $(CMAKE_CONFIGURATION) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD.OUT 2>&1
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "Xerces ${VERSION} - Copying builds (See $(OUTPUT_WORKDIR_PRETTYNAME)INSTALL.OUT)"` "... "
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
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "Xerces ${VERSION} - Checking Prerequisite tools ... "
ifeq ($(PATH_ADJUSTMENT_PREFIX),)
	@{ type cmake > /dev/null 2>&1; } || { ../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) && $(PATH_ADJUSTMENT_PREFIX) ../../ScriptsLib/GetMessageForMissingTool cmake && exit 1; }
else
	@$(PATH_ADJUSTMENT_PREFIX) ; { type cmake > /dev/null 2>&1; } || { ../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) && $(PATH_ADJUSTMENT_PREFIX) ../../ScriptsLib/GetMessageForMissingTool cmake && exit 1; }
endif
	@$(ECHO) "done";
