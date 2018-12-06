export StroikaRoot?=$(shell realpath ../..)/

ifneq ($(CONFIGURATION),)
	include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

	IncludeDebugSymbolsInLibraries	=	$(shell $(StroikaRoot)ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" IncludeDebugSymbolsInLibraries)
	OptimizerFlag					=	$(shell $(StroikaRoot)ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" OptimizerFlag)
	CC								=	$(shell $(StroikaRoot)ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" CompilerDriver-C)
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk


SHELL=/bin/bash

.DIRECTORY: CURRENT

VERSION=3260000
#VERSION=3250200
#VERSION=3240000

AMALGAMATION_WITH_VERSION=sqlite-amalgamation-$(VERSION)

ZIPEDFILE=../Origs-Cache/$(AMALGAMATION_WITH_VERSION).zip

FETCHURLS=
FETCHURLS+=https://www.sqlite.org/2018/$(AMALGAMATION_WITH_VERSION).zip
FETCHURLS+=https://www.sqlite.org/2017/$(AMALGAMATION_WITH_VERSION).zip
FETCHURLS+=https://www.sqlite.org/2016/$(AMALGAMATION_WITH_VERSION).zip



.DEFAULT_GOAL := all
.PHONY:	all clean clobber check doBuild doCreate doPatch

.NOTPARALLEL:


MAKE_INDENT_LEVEL?=$(MAKELEVEL)


ifneq (,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
PATH	=	$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl "$(CONFIGURATION)" PATH)
INCLUDE	=	$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl "$(CONFIGURATION)" INCLUDE)
endif

ifeq (Unix,$(ProjectPlatformSubdir))
ENABLE_GLIBCXX_DEBUG	=	$(shell ../../ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" ENABLE_GLIBCXX_DEBUG)
endif



BUILDS_ROOT=../../Builds/
INTERMEDIATES_ROOT=../../IntermediateFiles/


PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/sqlite/sqlite3.h	\

ifeq (Unix,$(ProjectPlatformSubdir))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/libsqlite.a
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/sqlite.lib
endif







$(ZIPEDFILE):
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ../../ScriptsLib/WebGet.sh $(ZIPEDFILE) $(FETCHURLS)


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
	@$(MAKE) --no-print-directory $(SILENT_BUILD_FLAG) CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) check


CURRENT:	$(ZIPEDFILE)
	@unzip -q $(ZIPEDFILE)
	@mv $(AMALGAMATION_WITH_VERSION) CURRENT
	@touch CURRENT


ifeq (Unix,$(ProjectPlatformSubdir))
SQLITE3OBJ=$(INTERMEDIATES_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/sqlite/sqlite3.o
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
SQLITE3OBJ=$(INTERMEDIATES_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/sqlite/sqlite3.obj
endif
OBJS=$(SQLITE3OBJ)


$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/sqlite/sqlite3.h:	CURRENT
	mkdir -p $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/sqlite
	cp CURRENT/sqlite3.h $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/sqlite/sqlite3.h
	
ifeq (Unix,$(ProjectPlatformSubdir))
$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/libsqlite.a:
	$(MAKE) --no-print-directory $(SILENT_BUILD_FLAG) CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)	$(OBJS)
	mkdir -p $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib
	$(AR) cr $@ $(OBJS)
	$(RANLIB) $@
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/sqlite.lib:
	$(MAKE) --no-print-directory $(SILENT_BUILD_FLAG) CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)	$(OBJS)
	mkdir -p $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib
	"$(AR)" /OUT:$@ /nologo $(OBJS)
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

CFLAGS	+= $(OptimizerFlag)



$(SQLITE3OBJ):
	mkdir -p $(INTERMEDIATES_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/sqlite/
ifeq (Unix,$(ProjectPlatformSubdir))
	$(CC) -c -o $@ CURRENT/sqlite3.c $(CFLAGS)
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	INCLUDE="$(INCLUDE)" "$(CC)" $(CFLAGS) /c /Fo$@ CURRENT/sqlite3.c > STUPID_FILENAME_OUTPUT_SUPRESS && rm -f STUPID_FILENAME_OUTPUT_SUPRESS || cat STUPID_FILENAME_OUTPUT_SUPRESS
endif


check-prerequisite-tools:
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Checking Prerequisite tools for sqlite ... "
	@$(MAKE) --no-print-directory --silent MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) check_prerequisites_
	@$(ECHO) "done";



check_prerequisites_:
	@{ type unzip > /dev/null; } || { ../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Checking required build tools ... " && ../../ScriptsLib/GetMessageForMissingTool.sh unzip && exit 1; }


check:
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Checking ... "
	@for i in $(PRODUCED_OUTPUT_ARTIFACTS) ; do \
		if [ ! -e $$i ]; then\
			$(ECHO) "   FAILED: missing $$i";\
			exit 1;\
		fi;\
	done
	@$(ECHO) " Stroika/ThirdPartyComponents/sqlite -  [Succeeded]";


clobber:	clean
ifeq ($(CONFIGURATION),)
	@rm -rf  $(BUILDS_ROOT)*/ThirdPartyComponents/include/sqlite $(BUILDS_ROOT)*/ThirdPartyComponents/lib/sqlite.*
else
	@rm -f $(PRODUCED_OUTPUT_ARTIFACTS)
endif

	
clean:
	@rm -rf $(AMALGAMATION_WITH_VERSION) CURRENT
ifeq ($(CONFIGURATION),)
	@rm -rf  $(INTERMEDIATES_ROOT)*/ThirdPartyComponents/lib/sqlite
else
	@rm -f $(OBJS)
endif
