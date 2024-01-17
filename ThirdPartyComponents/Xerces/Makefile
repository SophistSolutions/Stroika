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

include $(StroikaRoot)/ScriptsLib/SharedMakeVariables-Default.mk


ifneq ($(CONFIGURATION),)
include $(StroikaRoot)ScriptsLib/Makefile-CMake-Common.mk
endif

.PHONY:	all clean clobber check doBuilds_

QUICK_BUILD ?= 1

VERSION=3.2.5
#VERSION=3.2.4
#VERSION=3.2.3
#VERSION=3.2.2
ifeq (Unix,$(ProjectPlatformSubdir))
ifeq ($(VERSION), 3.2.5)
# This version of xerces requires more recent copy of cmake
ifeq ($(shell $(StroikaRoot)ScriptsLib/VersionCompare `cmake --version | awk '{print $$3}' | awk 'FNR == 1'` 3.12; echo $$?), 2)
VERSION=3.2.3
endif
endif
ifeq ($(VERSION), 3.2.4)
# This version of xerces requires more recent copy of cmake
ifeq ($(shell $(StroikaRoot)ScriptsLib/VersionCompare `cmake --version | awk '{print $$3}' | awk 'FNR == 1'` 3.12; echo $$?), 2)
VERSION=3.2.3
endif
endif
endif
BASENAME=xerces-c-$(VERSION)
ZIPPEDFILE=../Origs-Cache/$(BASENAME).tar.gz
TARFILE=$(BASENAME).tar
EXTRACTED_DIRNAME=$(BASENAME)

PER_CONFIGURATION_BUILDS_DIR:=$(shell realpath --canonicalize-missing ../../Builds/$(CONFIGURATION)/ThirdPartyComponents/)/

#lose trailing / on PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_ because on macos make THATDIR doesn't work with trailing slash (old WEIRD_MACOS_MAKEFILE_DIR_BUG -  MacOS XCode 10 - GNU Make 3.81)
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_:=$(shell realpath --canonicalize-missing ../../IntermediateFiles/$(CONFIGURATION)/ThirdPartyComponents/Xerces)

OUTPUT_WORKDIR_PRETTYNAME="$(shell $(StroikaRoot)ScriptsLib/SubstituteBackVariables "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/")"


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
# Used to use
#		CMAKE_CURRENT_SOURCE_DIR:=$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
#		which allows changes per platofrm, but goes away on make clean (so debugger doesnt see sources)
#		Using CURRENT folder maybe better
CMAKE_CURRENT_SOURCE_DIR:=${StroikaRoot}/ThirdPartyComponents/Xerces/CURRENT/
CMAKE_CURRENT_SOURCE_DIR:=$(shell realpath --canonicalize-missing $(CMAKE_CURRENT_SOURCE_DIR) )
CMAKE_CURRENT_SOURCE_DIR:=$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(CMAKE_CURRENT_SOURCE_DIR))
endif

ifeq (Unix,$(ProjectPlatformSubdir))
#@todo fixup - bug workaround - configure autodetects -posix and se we have to add it in here. Must find
# some way we can ADD these params to CFLAGS without REPLACING
# -- without this hack - we get link error building Xerces
CFLAGS+= -pthread
CXXFLAGS+= -pthread
endif

# Avoid having warnings leak from here to linking code when Xerces is used --LGP 2022-01-18
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
ifeq (-LTCG,$(findstring -LTCG,$(LinkerArgs_ExtraPrefix)))
CXXFLAGS+= -wd4702 
CXXFLAGS+= -wd4706
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
	@$(MAKE) --no-print-directory --silent MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) CURRENT
	@$(MAKE) --no-print-directory --silent MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1))  check
endif

#ALL produced artifacts produced by the same thing. But if you try the same rule to make all, if parallel makes invoked on this, you get
#multiple tries to run the same rule. So remap all to making the first.
$(wordlist 2, 9999, $(PRODUCED_OUTPUT_ARTIFACTS)):	$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS))

$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS)):
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
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
	@MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) $(StroikaRoot)ScriptsLib/WebGet $(ZIPPEDFILE) $(FETCHURLS)


ORIGs CURRENT:	$(ZIPPEDFILE)
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
		patch -p0 $@/src/xercesc/validators/schema/NamespaceScope.cpp Patches/SilenceSanitizerWarningAboutMemcpyNullPtr2.PATCH >> $@/PATCHING.OUT;\
		patch -p0 $@/src/xercesc/util/XMLString.hpp Patches/AvoidMemMoveOfZero.patch >> $@/PATCHING.OUT;\
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
CFLAGS += -DWIN32
CXXFLAGS += -DWIN32
endif

# DIDN'T HELP - cuz then when we do the install, we generate all the samples anyhow...
# ifeq ($(QUICK_BUILD), 1)
# CMAKE_BUILD_TARGET_SPEC_ = --target xerces-c
# endif


doBuilds_:
	@mkdir -p $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
	@rm -f $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIGURE-OUT.txt && touch  $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIGURE-OUT.txt
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "Xerces ${VERSION} - Configuring (See $(OUTPUT_WORKDIR_PRETTYNAME)CONFIGURE-OUT.txt)"` "... "
	@echo CXX='$(CXX)' >>  $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIGURE-OUT.txt
	@echo CFLAGS='$(CFLAGS)' >>  $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIGURE-OUT.txt
	@echo CXXFLAGS='$(CXXFLAGS)' >>  $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIGURE-OUT.txt
	@echo 'cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) ; cmake -G "$(CMAKE_USE_GENERATOR)" $(CMAKE_ARGS) $(CMAKE_CURRENT_SOURCE_DIR)' >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIGURE-OUT.txt 2>&1
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) ; cmake -G "$(CMAKE_USE_GENERATOR)" $(CMAKE_ARGS) $(CMAKE_CURRENT_SOURCE_DIR) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIGURE-OUT.txt 2>&1
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "Xerces ${VERSION} - Compiling (See $(OUTPUT_WORKDIR_PRETTYNAME)BUILD-OUT.txt)"` "... "
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && $(ECHO) "cmake --build . $(CMAKE_BUILD_TARGET_SPEC_) --config $(CMAKE_CONFIGURATION)" > $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD-OUT.txt 2>&1
	@+cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && cmake --build . $(CMAKE_BUILD_TARGET_SPEC_) --config $(CMAKE_CONFIGURATION) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD-OUT.txt 2>&1
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "Xerces ${VERSION} - Copying builds (See $(OUTPUT_WORKDIR_PRETTYNAME)INSTALL.OUT)"` "... "
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && $(ECHO) "cmake --build . --config $(CMAKE_CONFIGURATION) --target install" > $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/INSTALL.OUT
	@+cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && cmake --build . --config $(CMAKE_CONFIGURATION) --target install >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/INSTALL.OUT 2>&1
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@#We get warnings about a missing pdb file when linking otherwise...(not sure why since symbol info sb in the .lib)
	@$(ECHO) "cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/src/xerces-c.dir/$(CMAKE_CONFIGURATION)/xerces*.pdb ${PER_CONFIGURATION_BUILDS_DIR}lib/ 2>/dev/null || :" >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/INSTALL.OUT
	@cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/src/xerces-c.dir/$(CMAKE_CONFIGURATION)/xerces*.pdb ${PER_CONFIGURATION_BUILDS_DIR}lib/ 2>/dev/null || :
endif


check-prerequisite-tools:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "Xerces ${VERSION} - Checking Prerequisite tools ... "
	@{ type cmake > /dev/null 2>&1; } || { $(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) && $(StroikaRoot)ScriptsLib/GetMessageForMissingTool cmake && exit 1; }
	@$(ECHO) "done";
