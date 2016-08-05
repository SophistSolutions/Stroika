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
WIN_CC32=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) CC_32)
WIN_CC64=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) CC_64)
#WIN_AS32=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) AS_32)")
#WIN_AS64=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) AS_64)")
WIN_LIB32=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) LIB_32)
WIN_LIB64=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) LIB_64)
WIN_LINK32=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) LINK_32)
WIN_LINK64=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) LINK_64)
INCLUDE=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) INCLUDE)
LIBDIR32=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) LIBDIR32)
LIBDIR64=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) LIBDIR64)
PATH=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) PATH)
endif

ifeq (Unix,$(ProjectPlatformSubdir))
CC=$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) CompilerDriver-C)
endif

BUILDS_ROOT=../../Builds/


PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/sqlite/sqlite3.h	\

ifeq (Unix,$(ProjectPlatformSubdir))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/sqlite/sqlite.a
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
	@rm -rf sqlite-$(AMALGAMATION_WITH_VERSION) CURRENT
	
clean:
	@rm -rf $(AMALGAMATION_WITH_VERSION)

CURRENT:	$(ZIPEDFILE)
	@unzip -q $(ZIPEDFILE)
	@mv $(AMALGAMATION_WITH_VERSION) CURRENT
	@touch CURRENT

OBJS=

$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/sqlite/sqlite3.h:	CURRENT
	mkdir -p $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/sqlite
	cp CURRENT/sqlite3.h $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/sqlite/sqlite3.h
	
ifeq (Unix,$(ProjectPlatformSubdir))
$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/sqlite/sqlite.a:
	mkdir -p $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib
	$(AR) cr $@ $(OBJS)
	$(RANLIB) $@
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/sqlite.lib:
	mkdir -p $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib
	lib /OUT:$@ $(OBJS)
endif

	