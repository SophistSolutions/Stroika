#
#	TODO:
#		@todo	NOTE - try for $(word 1,$(PRODUCED_OUTPUT_ARTIFACTS)) and $(wordlist 2, 9999, $(PRODUCED_OUTPUT_ARTIFACTS)):	$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS))
#				(repeated in most ThirdPartyComponents makefiles)
#				doesn't work if the first produced output artifact is fine, but the others got deleted. No BIGGIE, as you can just make clobber in that
#				exceedingly rare case; but certainly a defect of this approach.
StroikaRoot:=$(abspath ../..)/

ifneq ($(CONFIGURATION),)
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk
ifneq (${CONFIGURATION},)
include ${StroikaRoot}ScriptsLib/Makefile-CMake-Common.mk		# for things like CMAKE_ARGS
endif

# because we list multiple targets in PRODUCED_OUTPUT_ARTIFACTS, all of which are built with the same script
.NOTPARALLEL:

# https://github.com/fmtlib/fmt/releases
VERSION:=10.2.1


ZIPPEDFILE:=../Origs-Cache/fmtlib-$(VERSION).zip

FETCHURLS:=
FETCHURLS+=https://github.com/fmtlib/fmt/archive/refs/tags/${VERSION}.zip


PER_CONFIGURATION_BUILDS_DIR:=$(shell realpath --canonicalize-missing ../../Builds/$(CONFIGURATION)/ThirdPartyComponents/)/

#lose trailing / on PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_ because on macos make THATDIR doesn't work with trailing slash (old WEIRD_MACOS_MAKEFILE_DIR_BUG -  MacOS XCode 10 - GNU Make 3.81)
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_:=$(shell realpath --canonicalize-missing ../../IntermediateFiles/$(CONFIGURATION)/ThirdPartyComponents/fmtlib)

OUTPUT_WORKDIR_PRETTYNAME:="$(shell $(StroikaRoot)ScriptsLib/SubstituteBackVariables "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/")"



ifeq (${AssertionsEnabled},1)
CMAKE_BUILD_TYPE_=Debug
MAYBED_=d
else
CMAKE_BUILD_TYPE_=Release
MAYBED_=
endif

ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
OUTPUT_LIB_=gtest${LIB_SUFFIX}
else
OUTPUT_LIB_=libgtest${LIB_SUFFIX}
endif






.PHONY:	all clean clobber check

ifeq (Unix,$(ProjectPlatformSubdir))
SQLITE3LIB=libfmtlib.a
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
SQLITE3LIB=fmtlib.lib
endif
TARGETDIR			:=	$(PER_CONFIGURATION_BUILDS_DIR)bin/
TARGETEXE			:=	$(TARGETDIR)fmtlib${EXE_SUFFIX}

ObjDir:=$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/

LibObjs:=\
	$(ObjDir)fmtlib3${OBJ_SUFFIX}\

Objs:=\
	$(LibObjs)\
	$(ObjDir)shell${OBJ_SUFFIX}\

# Use the stroika link macro (DEFAULT_LINK_LINE), but omit the stroika lib itself, and other injected depepdencies
# for sqlite executable
LinkerArgs_LibDependencies:=
StroikaLibs:=
ifeq (Unix,$(ProjectPlatformSubdir))
LinkerArgs_LibDependencies:=-lpthread
endif

PRODUCED_OUTPUT_ARTIFACTS=	\
	$(PER_CONFIGURATION_BUILDS_DIR)include/fmtlib/fmtlib3.h	\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/$(SQLITE3LIB) \
	$(TARGETEXE)


$(ZIPPEDFILE):
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $(StroikaRoot)ScriptsLib/WebGet $(ZIPPEDFILE) $(FETCHURLS)

SILENT_BUILD_FLAG=	-silent
ifeq ($(ECHO_BUILD_LINES),1)
SILENT_BUILD_FLAG=
endif


all:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents/fmtlib Build $(VERSION) {$(CONFIGURATION)}:"
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@#make zippedfile even though a dependency on CURRENT and PRODUCED_ARTIFACTS for parallel-make so completed before either starts (else one sees file partly downloaded and proceeeds)
	@$(MAKE) --no-print-directory $(SILENT_BUILD_FLAG) $(ZIPPEDFILE)
ifneq ($(QUICK_BUILD), 1)
	@$(MAKE) --no-print-directory $(SILENT_BUILD_FLAG) CURRENT
endif
	@$(MAKE) --no-print-directory $(SILENT_BUILD_FLAG) $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) check


ORIGs CURRENT $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_):	$(ZIPPEDFILE)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "fmtlib ${VERSION} - Extracting $(ZIPPEDFILE) to $@"` "... "
	@unzip -q $(ZIPPEDFILE) 
	@mv fmt-${VERSION}/ $@


# trick of using word 1 is to get this triggered just once
$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS)):
	@$(MAKE) --silent $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
	@rm -f $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD-OUT.txt && touch $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD-OUT.txt
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "sqlite $(VERSION) - Build (See $(OUTPUT_WORKDIR_PRETTYNAME)BUILD-OUT.txt) ..."
	@mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)include/sqlite >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD-OUT.txt
	@cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/sqlite3.h $(PER_CONFIGURATION_BUILDS_DIR)include/sqlite/sqlite3.h >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD-OUT.txt
	@mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)lib >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD-OUT.txt 2>&1
	@$(MAKE) --no-print-directory $(Objs) ECHO_BUILD_LINES=1 >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD-OUT.txt 2>&1
	@$(call DEFAULT_LIBRARY_GEN_LINE,$(PER_CONFIGURATION_BUILDS_DIR)lib/$(SQLITE3LIB),$(LibObjs)) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD-OUT.txt 2>&1
	@mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)bin >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD-OUT.txt 2>&1
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1))  '$(call DEFAULT_LINK_LINE,$(TARGETEXE))';\
	fi
	@$(call DEFAULT_LINK_LINE,$(TARGETEXE)) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD-OUT.txt 2>&1


#
# NOTE: It appears to be OK to define these CFLAGS here, and not in the CONFIGURATION because these appear to only 
# be used in the .c file, not in the .h file for sqlite
#	@todo	Find docs/reference assuring this will always be so -- LGP 2021-04-28
#

CFLAGS	+= -DSQLITE_OMIT_LOAD_EXTENSION
# Important - soon to be used in Stroika (at least experimentally) - https://www.sqlite.org/json1.html
CFLAGS	+= -DSQLITE_ENABLE_JSON1

# See https://stroika.atlassian.net/browse/STK-753 and https://sqlite.org/forum/forumpost/cc6668e7b3 - avoid TSAN/Valgrind issues 
ifeq (1,$(AssertionsEnabled))
ifeq (,$(findstring qStroika_FeatureSupported_Valgrind,$(CPPFLAGS)))
ifeq (,$(findstring thread,$(SanitizerFlags)))
CFLAGS	+= -DSQLITE_ENABLE_API_ARMOR
CFLAGS	+= -DSQLITE_DEBUG
endif
endif
endif

ifeq (1,$(qCompiler_HasNoMisleadingIndentation_Flag))
CFLAGS	+= -Wno-misleading-indentation
endif


vpath %c $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)


$(ObjDir)%${OBJ_SUFFIX} : %.c
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_CC_LINE,$<,$@)';\
	fi
	@mkdir -p `dirname $@`
	@$(call DEFAULT_CC_LINE,$<,$@)


check-prerequisite-tools:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "sqlite $(VERSION) - Checking Prerequisite tools ... "
	@{ type unzip > /dev/null; } || { $(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Checking required build tools ... " && $(StroikaRoot)ScriptsLib/GetMessageForMissingTool unzip && exit 1; }
	@$(ECHO) "done";


check:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "sqlite $(VERSION) - Checking ... "
	@$(StroikaRoot)ScriptsLib/CheckFileExists $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(ECHO) " Stroika/ThirdPartyComponents/sqlite -  [Succeeded]";


clobber::	clean
ifeq ($(CONFIGURATION),)
	@rm -rf CURRENT ORIGS
	@rm -rf  $(StroikaRoot)Builds/*/ThirdPartyComponents/include/sqlite $(StroikaRoot)Builds/*/ThirdPartyComponents/lib/*sqlite.*
else
	@rm -rf $(PRODUCED_OUTPUT_ARTIFACTS) $(PER_CONFIGURATION_BUILDS_DIR)include/sqlite
endif


clean::
	@rm -rf $(AMALGAMATION_WITH_VERSION)
ifeq ($(CONFIGURATION),)
	@rm -rf  $(StroikaRoot)IntermediateFiles/*/ThirdPartyComponents/sqlite
else
	@rm -rf $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
endif

