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

VERSION=3340100
#VERSION=3340000
#VERSION=3330000
#VERSION=3320300

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
FETCHURLS+=https://www.sqlite.org/2020/$(AMALGAMATION_WITH_VERSION).zip
FETCHURLS+=https://www.sqlite.org/2019/$(AMALGAMATION_WITH_VERSION).zip
FETCHURLS+=https://www.sqlite.org/2018/$(AMALGAMATION_WITH_VERSION).zip
FETCHURLS+=https://github.com/azadkuh/sqlite-amalgamation/archive/${MAPPED_VERSION}.zip


PER_CONFIGURATION_BUILDS_DIR:=$(shell realpath --canonicalize-missing ../../Builds/$(CONFIGURATION)/ThirdPartyComponents/)/

#lose trailing / on PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_ because on macos make THATDIR doesn't work with trailing slash (old WEIRD_MACOS_MAKEFILE_DIR_BUG -  MacOS XCode 10 - GNU Make 3.81)
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_:=$(shell realpath --canonicalize-missing ../../IntermediateFiles/$(CONFIGURATION)/ThirdPartyComponents/sqlite)




.PHONY:	all clean clobber check




ifeq (Unix,$(ProjectPlatformSubdir))
SQLITE3LIB=libsqlite.a
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
SQLITE3LIB=sqlite.lib
endif


PRODUCED_OUTPUT_ARTIFACTS=	\
	$(PER_CONFIGURATION_BUILDS_DIR)include/sqlite/sqlite3.h	\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/$(SQLITE3LIB)




$(ZIPPEDFILE):
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ../../ScriptsLib/WebGet $(ZIPPEDFILE) $(FETCHURLS)

SILENT_BUILD_FLAG=	-silent
ifeq ($(ECHO_BUILD_LINES),1)
SILENT_BUILD_FLAG=
endif


all:
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents/sqlite Build $(VERSION) {$(CONFIGURATION)}:"
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@../../ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@#make zippedfile even though a dependency on CURRENT and PRODUCED_ARTIFACTS for parallel-make so completed before either starts (else one sees file partly downloaded and proceeeds)
	@$(MAKE) --no-print-directory $(SILENT_BUILD_FLAG) $(ZIPPEDFILE)
	@$(MAKE) --no-print-directory $(SILENT_BUILD_FLAG) $(PRODUCED_OUTPUT_ARTIFACTS)
ifneq ($(QUICK_BUILD), 1)
	@$(MAKE) --no-print-directory $(SILENT_BUILD_FLAG) CURRENT
endif
	@$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) check

ORIGs CURRENT $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_):	$(ZIPPEDFILE)
	@../../ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "sqlite ${VERSION} - Extracting $(ZIPPEDFILE) to $@"` "... "
	@mkdir -p $@
	@unzip -q $(ZIPPEDFILE) -d $@
	@#intentionally make mv/rm code version agnostic due to issue with differently formatted zip files in different mirrors/repositories
	@mv $@/sqlite-amalgamation-*/* $@/
	@rm -rf $@/sqlite-amalgamation-*
	@touch $@


SQLITE3OBJ=$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/sqlite3${OBJ_SUFFIX}
OBJS=$(SQLITE3OBJ)


#ALL produced artifacts produced by the same thing. But if you try the same rule to make all, if parallel makes invoked on this, you get
#multiple tries to run the same rule. So remap all to making the first.
$(wordlist 2, 9999, $(PRODUCED_OUTPUT_ARTIFACTS)):	$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS))

$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS)):
	@$(MAKE) --silent $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
	@../../ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "sqlite $(VERSION) - Build ..."
	mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)include/sqlite
	cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/sqlite3.h $(PER_CONFIGURATION_BUILDS_DIR)include/sqlite/sqlite3.h
	mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)lib
	$(MAKE) --no-print-directory $(SILENT_BUILD_FLAG) $(OBJS)
	$(call DEFAULT_LIBRARY_GEN_LINE, $(PER_CONFIGURATION_BUILDS_DIR)lib/$(SQLITE3LIB), $(OBJS))


ifeq (true,$(HasMakefileBugWorkaround_lto_skipping_undefined_incompatible))
#@todo and figure out why we need -flto in library archive?? (need to filter it out)? - only for clang -- LGP 2019-01-10
CFLAGS:=$(filter-out -flto, $(CFLAGS))
endif


CFLAGS	+= -DSQLITE_OMIT_LOAD_EXTENSION

ifeq (1,$(ENABLE_ASSERTIONS))
CFLAGS	+= -DSQLITE_ENABLE_API_ARMOR
CFLAGS	+= -DSQLITE_DEBUG
endif


$(SQLITE3OBJ):
ifeq (Unix,$(ProjectPlatformSubdir))
	cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && $(call DEFAULT_CC_LINE, sqlite3.c,$@)
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	export PATH="$(TOOLS_PATH_ADDITIONS):$(PATH)";\
		cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_); \
		$(call DEFAULT_CC_LINE, sqlite3.c,$@)
endif


check-prerequisite-tools:
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "sqlite $(VERSION) - Checking Prerequisite tools ... "
	@{ type unzip > /dev/null; } || { ../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Checking required build tools ... " && ../../ScriptsLib/GetMessageForMissingTool unzip && exit 1; }
	@$(ECHO) "done";


check:
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "sqlite $(VERSION) - Checking ... "
	@$(StroikaRoot)ScriptsLib/CheckFileExists $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(ECHO) " Stroika/ThirdPartyComponents/sqlite -  [Succeeded]";


clobber:	clean
ifeq ($(CONFIGURATION),)
	@rm -rf CURRENT ORIGS
	@rm -rf  $(StroikaRoot)Builds/*/ThirdPartyComponents/include/sqlite $(StroikaRoot)Builds/*/ThirdPartyComponents/lib/*sqlite.*
else
	@rm -rf $(PRODUCED_OUTPUT_ARTIFACTS) $(PER_CONFIGURATION_BUILDS_DIR)include/sqlite
endif

	
clean:
	@rm -rf $(AMALGAMATION_WITH_VERSION)
ifeq ($(CONFIGURATION),)
	@rm -rf  $(StroikaRoot)IntermediateFiles/*/ThirdPartyComponents/sqlite
else
	@rm -rf $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
endif
