#
#	TODO:
#		@todo	NOTE - try for $(word 1,$(PRODUCED_OUTPUT_ARTIFACTS)) and $(wordlist 2, 9999, $(PRODUCED_OUTPUT_ARTIFACTS)):	$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS))
#				(repeated in most ThirdPartyComponents makefiles)
#				doesn't work if the first produced output artifact is fine, but the others got deleted. No BIGGIE, as you can just make clobber in that
#				exceedingly rare case; but certainly a defect of this approach.
StroikaRoot=$(abspath ../..)/

ifneq ($(CONFIGURATION),)
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk

# because we list multiple targets in PRODUCED_OUTPUT_ARTIFACTS, all of which are built with the same script
.NOTPARALLEL:

VERSION=3380500
#VERSION=3380300
#VERSION=3370200
#VERSION=3370100

#
# Download of SQLITE from www.sqlite.org mysteriously started failing on the macos machines only for travisci
# Do I looked for a mirror. I only found one mirroring the amalgamation files - and that used a differently
# formatted version#. Thus the trickery here to handle that alternate download source.
#
# @todo redo this more generally - use substr in first string and eval to lose leading zeros - I think
#
#	--LGP 2020-07-02
#
ifeq (${VERSION},3320300)
MAPPED_VERSION=3.32.3
endif

AMALGAMATION_WITH_VERSION=sqlite-amalgamation-$(VERSION)

ZIPPEDFILE=../Origs-Cache/$(AMALGAMATION_WITH_VERSION).zip

FETCHURLS=
FETCHURLS+=https://www.sqlite.org/2022/$(AMALGAMATION_WITH_VERSION).zip
FETCHURLS+=https://www.sqlite.org/2021/$(AMALGAMATION_WITH_VERSION).zip
FETCHURLS+=https://www.sqlite.org/2020/$(AMALGAMATION_WITH_VERSION).zip
FETCHURLS+=https://www.sqlite.org/2019/$(AMALGAMATION_WITH_VERSION).zip
FETCHURLS+=https://github.com/azadkuh/sqlite-amalgamation/archive/${MAPPED_VERSION}.zip


PER_CONFIGURATION_BUILDS_DIR:=$(shell realpath --canonicalize-missing ../../Builds/$(CONFIGURATION)/ThirdPartyComponents/)/

#lose trailing / on PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_ because on macos make THATDIR doesn't work with trailing slash (old WEIRD_MACOS_MAKEFILE_DIR_BUG -  MacOS XCode 10 - GNU Make 3.81)
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_:=$(shell realpath --canonicalize-missing ../../IntermediateFiles/$(CONFIGURATION)/ThirdPartyComponents/sqlite)

OUTPUT_WORKDIR_PRETTYNAME="$(shell $(StroikaRoot)ScriptsLib/SubstituteBackVariables "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/")"


.PHONY:	all clean clobber check

ifeq (Unix,$(ProjectPlatformSubdir))
SQLITE3LIB=libsqlite.a
SQLITE3EXE=sqlite
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
SQLITE3LIB=sqlite.lib
SQLITE3EXE=sqlite.exe
endif
TARGETDIR				=	$(PER_CONFIGURATION_BUILDS_DIR)bin/
TARGETEXE				=	$(TARGETDIR)sqlite${EXE_SUFFIX}

LibObjs=\
	$(ObjDir)sqlite3${OBJ_SUFFIX}\

Objs=\
	$(LibObjs)\
	$(ObjDir)shell${OBJ_SUFFIX}\

# Use the stroika link macro (DEFAULT_LINK_LINE), but omit the stroika lib itself, and other injected depepdencies
# for sqlite executable
LIB_DEPENDENCIES=
StroikaLibs=
ifeq (Unix,$(ProjectPlatformSubdir))
LIB_DEPENDENCIES=-lpthread
endif

PRODUCED_OUTPUT_ARTIFACTS=	\
	$(PER_CONFIGURATION_BUILDS_DIR)include/sqlite/sqlite3.h	\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/$(SQLITE3LIB) \
	$(TARGETEXE)


$(ZIPPEDFILE):
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $(StroikaRoot)ScriptsLib/WebGet $(ZIPPEDFILE) $(FETCHURLS)

SILENT_BUILD_FLAG=	-silent
ifeq ($(ECHO_BUILD_LINES),1)
SILENT_BUILD_FLAG=
endif


all:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents/sqlite Build $(VERSION) {$(CONFIGURATION)}:"
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
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "sqlite ${VERSION} - Extracting $(ZIPPEDFILE) to $@"` "... "
	@mkdir -p $@
	@unzip -q $(ZIPPEDFILE) -d $@
	@#intentionally make mv/rm code version agnostic due to issue with differently formatted zip files in different mirrors/repositories
	@mv $@/sqlite-amalgamation-*/* $@/
	@rm -rf $@/sqlite-amalgamation-*
	@touch $@


# trick of using word 1 is to get this triggered just once
$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS)):
	@$(MAKE) --silent $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
	@rm -f $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD-OUT.txt && touch $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD-OUT.txt
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "sqlite $(VERSION) - Build (See $(OUTPUT_WORKDIR_PRETTYNAME)BUILD-OUT.txt) ..."
	@mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)include/sqlite >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD-OUT.txt
	@cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/sqlite3.h $(PER_CONFIGURATION_BUILDS_DIR)include/sqlite/sqlite3.h >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD-OUT.txt
	@mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)lib >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD-OUT.txt 2>&1
	@$(MAKE) --no-print-directory $(Objs) ECHO_BUILD_LINES=1 >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD-OUT.txt 2>&1
	@$(call DEFAULT_LIBRARY_GEN_LINE, $(PER_CONFIGURATION_BUILDS_DIR)lib/$(SQLITE3LIB), $(LibObjs)) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD-OUT.txt 2>&1
	@mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)bin >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD-OUT.txt 2>&1
	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
	    $(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1))  '$(call DEFAULT_LINK_LINE,$(TARGETEXE))';\
	fi
	@$(call DEFAULT_LINK_LINE,$(TARGETEXE)) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD-OUT.txt 2>&1


ifeq (true,$(HasMakefileBugWorkaround_lto_skipping_undefined_incompatible))
#@todo and figure out why we need -flto in library archive?? (need to filter it out)? - only for clang -- LGP 2019-01-10
CFLAGS:=$(filter-out -flto, $(CFLAGS))
endif

#
# NOTE: It appears to be OK to define these CFLAGS here, and not in the CONFIGURATION because these appear to only 
# be used in the .c file, not in the .h file for sqlite
#	@todo	Find docs/reference assuring this will always be so -- LGP 2021-04-28
#

CFLAGS	+= -DSQLITE_OMIT_LOAD_EXTENSION
# Important - soon to be used in Stroika (at least experimentally) - https://www.sqlite.org/json1.html
CFLAGS	+= -DSQLITE_ENABLE_JSON1

# See https://stroika.atlassian.net/browse/STK-753 and https://sqlite.org/forum/forumpost/cc6668e7b3 - avoid TSAN/Valgrind issues 
ifeq (1,$(ENABLE_ASSERTIONS))
ifeq (,$(findstring qStroika_FeatureSupported_Valgrind,$(CFLAGS)))
ifeq (,$(findstring thread,$(SanitizerFlags)))
CFLAGS	+= -DSQLITE_ENABLE_API_ARMOR
CFLAGS	+= -DSQLITE_DEBUG
endif
endif
endif

ifeq (1,$(qCompiler_HasNoMisleadingIndentation_Flag))
CFLAGS	+= -Wno-misleading-indentation
endif

ObjDir=$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/

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

