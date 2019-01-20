export StroikaRoot?=$(shell realpath ../..)/

ifneq ($(CONFIGURATION),)
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk


VERSION=3260000
#VERSION=3250200
#VERSION=3240000

AMALGAMATION_WITH_VERSION=sqlite-amalgamation-$(VERSION)

ZIPPEDFILE=../Origs-Cache/$(AMALGAMATION_WITH_VERSION).zip

FETCHURLS=
FETCHURLS+=https://www.sqlite.org/2018/$(AMALGAMATION_WITH_VERSION).zip
FETCHURLS+=https://www.sqlite.org/2017/$(AMALGAMATION_WITH_VERSION).zip
FETCHURLS+=https://www.sqlite.org/2016/$(AMALGAMATION_WITH_VERSION).zip


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
	@$(MAKE) --no-print-directory $(SILENT_BUILD_FLAG) CURRENT $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) check


ORIGs CURRENT $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_):	$(ZIPPEDFILE)
	@../../ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "sqlite ${VERSION} - Extracting $(ZIPPEDFILE) to $@"` "... "
	@mkdir -p $@
	@unzip -q $(ZIPPEDFILE) -d $@
	@mv $@/$(AMALGAMATION_WITH_VERSION)/* $@/
	@rm -rf $@/$(AMALGAMATION_WITH_VERSION)
	@touch $@



SQLITE3OBJ=$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/sqlite3${OBJSUFFIX}
OBJS=$(SQLITE3OBJ)


#ALL produced artifacts produced by the same thing. But if you try the same rule to make all, if parallel makes invoked on this, you get
#multiple tries to run the same rule. So remap all to making the first.
$(wordlist 2, 9999, $(PRODUCED_OUTPUT_ARTIFACTS)):	$(StroikaRoot)ConfigurationFiles/$(CONFIGURATION).xml
	@$(MAKE) $(word 1,$(PRODUCED_OUTPUT_ARTIFACTS))

$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS)):
	@$(MAKE) --silent CURRENT $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
	@../../ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "sqlite $(VERSION) - Build ..."
	mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)include/sqlite
	cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/sqlite3.h $(PER_CONFIGURATION_BUILDS_DIR)include/sqlite/sqlite3.h
	mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)lib
	$(MAKE) --no-print-directory $(SILENT_BUILD_FLAG) $(OBJS)
ifeq (Unix,$(ProjectPlatformSubdir))
	$(AR) cr $(PER_CONFIGURATION_BUILDS_DIR)lib/$(SQLITE3LIB) $(OBJS)
	$(RANLIB) $(PER_CONFIGURATION_BUILDS_DIR)lib/$(SQLITE3LIB)
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	export PATH="$(TOOLS_PATH_ADDITIONS):$(PATH)";\
		"$(AR)" /OUT:`cygpath -w $(PER_CONFIGURATION_BUILDS_DIR)lib/$(SQLITE3LIB)` /nologo `cygpath -w $(OBJS)`
endif


ifeq (true,$(HasMakefileBugWorkaround_lto_skipping_undefined_incompatible))
#@todo and figure out why we need -flto in library archive?? (need to filter it out)? - only for clang -- LGP 2019-01-10
CFLAGS:=$(filter-out -flto, $(CFLAGS))
endif


CFLAGS	+= -DSQLITE_OMIT_LOAD_EXTENSION

ifeq (1,$(ENABLE_ASSERTIONS))
CFLAGS	+= -DSQLITE_ENABLE_API_ARMOR
CFLAGS	+= -DSQLITE_DEBUG
endif


ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
#/Z7 program databaes debug format
CFLAGS	+= /Z7
CFLAGS	+= /nologo
ifeq (1,$(ENABLE_ASSERTIONS))
CFLAGS	+= /MTd
endif
ifeq (0,$(ENABLE_ASSERTIONS))
CFLAGS	+= /MT
endif
endif


$(SQLITE3OBJ):
ifeq (Unix,$(ProjectPlatformSubdir))
	cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && $(CC) -c -o $@ sqlite3.c $(CFLAGS)
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	export PATH="$(TOOLS_PATH_ADDITIONS):$(PATH)" INCLUDE="$(WIN_INCLUDES_PATH)";\
		cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_); \
		"$(CC)" $(CFLAGS) /c /Fo`cygpath -w $@` sqlite3.c > STUPID_FILENAME_OUTPUT_SUPRESS && rm -f STUPID_FILENAME_OUTPUT_SUPRESS || cat STUPID_FILENAME_OUTPUT_SUPRESS
endif


check-prerequisite-tools:
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "Checking Prerequisite tools for sqlite ... "
	@{ type unzip > /dev/null; } || { ../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Checking required build tools ... " && ../../ScriptsLib/GetMessageForMissingTool unzip && exit 1; }
	@$(ECHO) "done";


check:
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "Checking ... "
	@$(StroikaRoot)ScriptsLib/CheckFileExists $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(ECHO) " Stroika/ThirdPartyComponents/sqlite -  [Succeeded]";


clobber:	clean
ifeq ($(CONFIGURATION),)
	@rm -rf CURRENT ORIGS
	@rm -rf  $(StroikaRoot)Builds/*/ThirdPartyComponents/include/sqlite $(StroikaRoot)Builds/*/ThirdPartyComponents/lib/*sqlite.*
else
	@rm -f $(PRODUCED_OUTPUT_ARTIFACTS)
endif

	
clean:
	@rm -rf $(AMALGAMATION_WITH_VERSION)
ifeq ($(CONFIGURATION),)
	@rm -rf  $(StroikaRoot)IntermediateFiles/*/ThirdPartyComponents/sqlite
else
	@rm -rf $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
endif
