#
#	TODO:
#		@todo	NOTE - try for $(word 1,$(PRODUCED_OUTPUT_ARTIFACTS)) and $(wordlist 2, 9999, $(PRODUCED_OUTPUT_ARTIFACTS)):	$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS))
#				(repeated in most ThirdPartyComponents makefiles)
#				doesn't work if the first produced output artifact is fine, but the others got deleted. No BIGGIE, as you can just make clobber in that
#				exceedingly rare case; but certainly a defect of this approach.
#
#	@todo REDO using entire build system and cmake - full sqlite-src instead of amalagamation (that has .pc file)
#
StroikaRoot	:=	$(abspath ../..)/

include $(StroikaRoot)Build/ScriptsLib/SharedMakeVariables-Default.mk

include $(StroikaRoot)Build/ScriptsLib/SharedBuildRules-Default.mk


# because we list multiple targets in PRODUCED_OUTPUT_ARTIFACTS, all of which are built with the same script
.NOTPARALLEL:

# https://www.sqlite.org/download.html
VERSION:=3530300
# VERSION:=3530200
# VERSION:=3510200
# VERSION:=3510100
# VERSION:=3510000

#
# Download of SQLITE from www.sqlite.org mysteriously started failing on the macos machines only for travisci
# Do I looked for a mirror. I only found one mirroring the amalgamation files - and that used a differently
# formatted version#. Thus the trickery here to handle that alternate download source.
#
# @todo redo this more generally - use substr in first string and eval to lose leading zeros - I think
#
#	--LGP 2020-07-02
#
MAPPED_VERSION=$(shell (echo ${VERSION} | echo -n $$(cut -b 1)); echo -n '.'; (echo ${VERSION} | echo -n $$(cut -b 2-3)); echo -n '.'; (echo ${VERSION} | echo -n $$(cut -b 5)))

AMALGAMATION_WITH_VERSION:=sqlite-amalgamation-$(VERSION)

ZIPPEDFILE:=../Origs-Cache/$(AMALGAMATION_WITH_VERSION).zip

FETCHURLS:=
FETCHURLS+=https://www.sqlite.org/2026/$(AMALGAMATION_WITH_VERSION).zip
FETCHURLS+=https://www.sqlite.org/2025/$(AMALGAMATION_WITH_VERSION).zip
FETCHURLS+=https://www.sqlite.org/2024/$(AMALGAMATION_WITH_VERSION).zip
FETCHURLS+=https://www.sqlite.org/2023/$(AMALGAMATION_WITH_VERSION).zip

# apparently only old copies of sqlite here - up to 2022 apx --LGP 2025-04-05
# FETCHURLS+=https://github.com/azadkuh/sqlite-amalgamation/archive/${MAPPED_VERSION}.zip

## TRIED but they dont archive the amalgamation files, so I would need separate build step for that
##FETCHURLS+=https://github.com/sqlite/sqlite/archive/refs/tags/version-${MAPPED_VERSION}.zip



PER_CONFIGURATION_BUILDS_DIR_:=${StroikaPlatformTargetBuildDir}
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_:=$(shell realpath --canonicalize-missing ../../IntermediateFiles/$(CONFIGURATION)/ThirdPartyComponents/sqlite)/
OUTPUT_WORKDIR_PRETTYNAME:="$(shell $(StroikaRoot)Build/ScriptsLib/SubstituteBackVariables "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)")"

# Only needed on MacOS due to make 3.81, I believe --LGP 2024-07-01
ifeq (3.81,$(firstword $(sort $(MAKE_VERSION) 3.81)))
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_OLDMAKEBWA_:=$(shell realpath --canonicalize-missing ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_})
else
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_OLDMAKEBWA_:=$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)
endif


.PHONY:	all clean clobber check

ifeq (Unix,$(BuildPlatform))
SQLITE3LIB=libsqlite.a
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(BuildPlatform)))
SQLITE3LIB=sqlite.lib
endif
TARGETDIR			:=	$(PER_CONFIGURATION_BUILDS_DIR_)bin/
TARGETEXE			:=	$(TARGETDIR)sqlite${EXE_SUFFIX}

ObjDir:=$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)

LibObjs:=\
	$(ObjDir)sqlite3${OBJ_SUFFIX}\

Objs:=\
	$(LibObjs)\
	$(ObjDir)shell${OBJ_SUFFIX}\


PRODUCED_OUTPUT_ARTIFACTS=	\
	$(PER_CONFIGURATION_BUILDS_DIR_)include/sqlite/sqlite3.h	\
	$(PER_CONFIGURATION_BUILDS_DIR_)lib/$(SQLITE3LIB) 			\
	$(PER_CONFIGURATION_BUILDS_DIR_)lib/pkgconfig/sqlite.pc		\
	$(TARGETEXE)


$(ZIPPEDFILE):
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $(StroikaRoot)Build/ScriptsLib/WebGet $(ZIPPEDFILE) $(FETCHURLS)

SILENT_BUILD_FLAG=	-silent
ifeq ($(ECHO_BUILD_LINES),1)
SILENT_BUILD_FLAG=
endif




CPPFLAGS 	:= 	${PLATFORM_CPPFLAGS}
CFLAGS 		:= 	${PLATFORM_CFLAGS}
CXXFLAGS 	:= 	${PLATFORM_CXXFLAGS}



all:
	@$(StroikaRoot)Build/ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents/sqlite Build $(VERSION) {$(CONFIGURATION)}:"
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(StroikaRoot)Build/ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@#make zippedfile even though a dependency on CURRENT and PRODUCED_ARTIFACTS for parallel-make so completed before either starts (else one sees file partly downloaded and proceeeds)
	@$(MAKE) --no-print-directory $(SILENT_BUILD_FLAG) $(ZIPPEDFILE)
ifneq ($(QUICK_BUILD), 1)
	@$(MAKE) --no-print-directory $(SILENT_BUILD_FLAG) CURRENT
endif
	@$(MAKE) --silent $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_OLDMAKEBWA_)
	@$(StroikaRoot)Build/ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "sqlite $(VERSION) - Build (See $(OUTPUT_WORKDIR_PRETTYNAME)BUILD-OUT.txt) ..."
	@$(MAKE) --no-print-directory $(PRODUCED_OUTPUT_ARTIFACTS)  >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)BUILD-OUT.txt  2>&1
	@$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) check


ORIGs CURRENT $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_OLDMAKEBWA_):	$(ZIPPEDFILE)
	@$(StroikaRoot)Build/ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "sqlite ${VERSION} - Extracting $(ZIPPEDFILE) to" `$(StroikaRoot)Build/ScriptsLib/SubstituteBackVariables $@` "... "
	@mkdir -p $@
	@unzip -q $(ZIPPEDFILE) -d $@
	@#intentionally make mv/rm code version agnostic due to issue with differently formatted zip files in different mirrors/repositories
	@mv $@/sqlite-amalgamation-*/* $@/
	@rm -rf $@/sqlite-amalgamation-*
	@touch $@


#ALL produced artifacts produced by the same thing. But if you try the same rule to make all, if parallel makes invoked on this, you get
#multiple tries to run the same rule. So remap all to making the first.
$(wordlist 2, 9999, $(PRODUCED_OUTPUT_ARTIFACTS)):	$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS))

# @todo not sure why this is needed on WSL/Ubuntu?
${TARGETEXE}:	$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS))


# trick of using word 1 is to get this triggered just once
$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS)):
	@$(MAKE) --silent $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_OLDMAKEBWA_)
	mkdir -p $(PER_CONFIGURATION_BUILDS_DIR_)include/sqlite
	cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)sqlite3.h $(PER_CONFIGURATION_BUILDS_DIR_)include/sqlite/sqlite3.h
	mkdir -p $(PER_CONFIGURATION_BUILDS_DIR_)lib
	@$(MAKE) --no-print-directory $(Objs) ECHO_BUILD_LINES=1
	$(call DEFAULT_LIBRARY_GEN_LINE,$(PER_CONFIGURATION_BUILDS_DIR_)lib/$(SQLITE3LIB),$(LibObjs))
	mkdir -p $(PER_CONFIGURATION_BUILDS_DIR_)bin 
	mkdir -p "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/"
	cp sqlite.pc "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/"
	$(call DEFAULT_LINK_LINE, $(TARGETEXE), stroika-platform) 


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


vpath %c $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)


$(ObjDir)%${OBJ_SUFFIX} : %.c
	mkdir -p `dirname $@`
	$(call DEFAULT_CC_LINE,$<,$@)


check-prerequisite-tools:
	@$(StroikaRoot)Build/ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "sqlite $(VERSION) - Checking Prerequisite tools ... "
	@{ type unzip > /dev/null; } || { $(StroikaRoot)Build/ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Checking required build tools ... " && $(StroikaRoot)Build/ScriptsLib/GetMessageForMissingTool unzip && exit 1; }
	@$(ECHO) "done";


check:
	@$(StroikaRoot)Build/ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "sqlite $(VERSION) - Checking ... "
	@$(StroikaRoot)Build/ScriptsLib/CheckFileExists $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(ECHO) " Stroika/ThirdPartyComponents/sqlite -  [Succeeded]";


clobber::	clean
ifeq ($(CONFIGURATION),)
	@rm -rf CURRENT ORIGS
	@rm -rf  $(StroikaRoot)Builds/*/include/sqlite $(StroikaRoot)Builds/*/lib/*sqlite.*  $(StroikaRoot)Builds/*/lib/pkgconfig/sqlite.pc
else
	@rm -rf $(PRODUCED_OUTPUT_ARTIFACTS) $(PER_CONFIGURATION_BUILDS_DIR_)include/sqlite
endif


clean::
	@rm -rf $(AMALGAMATION_WITH_VERSION)
ifeq ($(CONFIGURATION),)
	@rm -rf  $(StroikaRoot)IntermediateFiles/*/ThirdPartyComponents/sqlite
else
	@rm -rf $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)
endif

