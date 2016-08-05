ifneq ($(CONFIGURATION),)
ProjectPlatformSubdir	=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)
endif

.DIRECTORY: CURRENT

VERSION=3130000
AMALGAMATION_WITH_VERSION=sqlite-amalgamation-$(VERSION)

ZIPEDFILE=../Origs-Cache/$(AMALGAMATION_WITH_VERSION).zip

FETCHURLS=
FETCHURLS+=https://www.sqlite.org/2016/$(AMALGAMATION_WITH_VERSION).zip



.DEFAULT_GOAL := all
.PHONY:	all clean clobber check doBuild doCreate doPatch

.NOTPARALLEL:


MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO?=	$(shell ../../ScriptsLib/GetDefaultShellVariable.sh ECHO)




ifneq (,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
ifeq (32,$(findstring 32,$(CONFIGURATION)))
CC=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) CC_32)
LIB=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) LIB_32)
endif
ifeq (64,$(findstring 64,$(CONFIGURATION)))
CC=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) CC_64)
LIB=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) LIB_64)
endif
PATH=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) PATH)
INCLUDE=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) INCLUDE)
endif



ifeq (Unix,$(ProjectPlatformSubdir))
CC=$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) CompilerDriver-C)
endif

BUILDS_ROOT=../../Builds/
INTERMEDIATES_ROOT=../../IntermediateFiles/


PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/sqlite/sqlite3.h	\

ifeq (Unix,$(ProjectPlatformSubdir))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/sqlite.a
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/sqlite.lib
endif







$(ZIPEDFILE):
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1))
	@../../ScriptsLib/WebGet.sh $(ZIPEDFILE) $(FETCHURLS)


all:
	@../../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && echo "Building Stroika/ThirdPartyComponents/sqlite $(VERSION):"
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(MAKE) --no-print-directory --silent CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) $(PRODUCED_OUTPUT_ARTIFACTS)
	@#$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) check

clobber:
	@rm -rf sqlite-$(AMALGAMATION_WITH_VERSION) CURRENT $(PRODUCED_OUTPUT_ARTIFACTS)
	
clean:
	@rm -rf $(AMALGAMATION_WITH_VERSION) CURRENT

CURRENT:	$(ZIPEDFILE)
	@unzip -q $(ZIPEDFILE)
	@mv $(AMALGAMATION_WITH_VERSION) CURRENT
	@touch CURRENT


ifeq (Unix,$(ProjectPlatformSubdir))
SQLITE3OBJ=$(INTERMEDIATES_ROOT)ThirdPartyComponents/lib/sqlite/sqlite3.o
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
SQLITE3OBJ=$(INTERMEDIATES_ROOT)ThirdPartyComponents/lib/sqlite/sqlite3.obj
endif
OBJS=$(SQLITE3OBJ)


$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/sqlite/sqlite3.h:	CURRENT
	mkdir -p $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/sqlite
	cp CURRENT/sqlite3.h $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/sqlite/sqlite3.h
	
ifeq (Unix,$(ProjectPlatformSubdir))
$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/sqlite.a:	$(OBJS)
	@mkdir -p $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib
	@$(AR) cr $@ $(OBJS)
	#$(RANLIB) $@
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/sqlite.lib:	$(OBJS)
	@mkdir -p $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib
	@"$(LIB)" /OUT:$@ $(OBJS)
endif


CFLAGS	+= -DSQLITE_OMIT_LOAD_EXTENSION

$(SQLITE3OBJ):
	@mkdir -p $(INTERMEDIATES_ROOT)ThirdPartyComponents/lib/sqlite/
ifeq (Unix,$(ProjectPlatformSubdir))
	@"$(CC)" -c -o $@ CURRENT/sqlite3.c $(CFLAGS)
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@INCLUDE="$(INCLUDE)" "$(CC)" $(CFLAGS) /c /Fo$@ CURRENT/sqlite3.c
endif
