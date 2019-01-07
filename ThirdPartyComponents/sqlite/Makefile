export StroikaRoot?=$(shell realpath ../..)/

ifneq ($(CONFIGURATION),)
	include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
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

#lose trailing / on PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR because on macos make THATDIR doesn't work with trailing slash (old WEIRD_MACOS_MAKEFILE_DIR_BUG -  MacOS XCode 10 - GNU Make 3.81)
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
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ../../ScriptsLib/WebGet.sh $(ZIPPEDFILE) $(FETCHURLS)


SILENT_BUILD_FLAG=	-silent
ifeq ($(ECHO_BUILD_LINES),1)
SILENT_BUILD_FLAG=
endif


all:
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents/sqlite Build $(VERSION) {$(CONFIGURATION)}:"
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@../../ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@#make zippedfile even though a dependenncy on CURRENT and PRODUCED_ARTIFACTS for parallel-make so completed before either starts (else one sees file partly downloaded and proceeeds)
	@$(MAKE) --no-print-directory $(SILENT_BUILD_FLAG) $(ZIPPEDFILE)
	@$(MAKE) --no-print-directory $(SILENT_BUILD_FLAG) CURRENT $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) check


ORIGs CURRENT $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_):	$(ZIPPEDFILE)
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "sqlite ${VERSION} - Extracting $(ZIPPEDFILE) to $@"` "... "
	@mkdir -p $@
	@unzip -q $(ZIPPEDFILE) -d $@
	@mv $@/$(AMALGAMATION_WITH_VERSION)/* $@/
	@rm -rf $@/$(AMALGAMATION_WITH_VERSION)
	@touch $@



SQLITE3OBJ=$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/sqlite3${OBJSUFFIX}
OBJS=$(SQLITE3OBJ)


$(PRODUCED_OUTPUT_ARTIFACTS):	$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/PRODUCED_OUTPUT_ARTIFACTS_
$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/PRODUCED_OUTPUT_ARTIFACTS_:
	@$(MAKE) --silent CURRENT $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) "sqlite $(VERSION) - Build ..."
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




CFLAGS	+= -DSQLITE_OMIT_LOAD_EXTENSION

ifeq (1,$(ENABLE_ASSERTIONS))
CFLAGS	+= -DSQLITE_ENABLE_API_ARMOR
CFLAGS	+= -DSQLITE_DEBUG
endif


ifeq (Unix,$(ProjectPlatformSubdir))
ifeq ($(ENABLE_GLIBCXX_DEBUG), 1)
	CFLAGS +=  -D_GLIBCXX_DEBUG 
endif
ifeq ($(IncludeDebugSymbolsInLibraries), 1)
	CFLAGS += -g
endif
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
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

CFLAGS	+= $(OptimizerFlag)

$(SQLITE3OBJ):
ifeq (Unix,$(ProjectPlatformSubdir))
	cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && $(CC) -c -o $@ sqlite3.c $(CFLAGS)
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	export PATH="$(TOOLS_PATH_ADDITIONS):$(PATH)" INCLUDE="$(WIN_INCLUDES_PATH)";\
		cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_); \
		"$(CC)" $(CFLAGS) /c /Fo`cygpath -w $@` sqlite3.c > STUPID_FILENAME_OUTPUT_SUPRESS && rm -f STUPID_FILENAME_OUTPUT_SUPRESS || cat STUPID_FILENAME_OUTPUT_SUPRESS
endif


check-prerequisite-tools:
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Checking Prerequisite tools for sqlite ... "
	@{ type unzip > /dev/null; } || { ../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Checking required build tools ... " && ../../ScriptsLib/GetMessageForMissingTool.sh unzip && exit 1; }
	@$(ECHO) "done";


check:
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Checking ... "
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
