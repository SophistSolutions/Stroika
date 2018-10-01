ifneq ($(CONFIGURATION),)
ProjectPlatformSubdir	=	$(shell ../../ScriptsLib/GetConfigurationParameter $(CONFIGURATION) ProjectPlatformSubdir)
endif



.DEFAULT_GOAL := all
.PHONY:	all clean clobber check doBuild doCreate doPatch

SHELL=/bin/bash

.NOTPARALLEL:


MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO?=	$(shell ../../ScriptsLib/GetDefaultShellVariable.sh ECHO)



DoCreateSymLink=0


#
# CHECK FOR UPDATED VERSIONS http://www.7-zip.org/sdk.html
#



#NO - BIG CHANGES - AND NOT DOCUMENTED AND NOT CLEAR BETTER - VERSION=1700
VERSION=1604
#VERSION=1602
BASENAME=lzma$(VERSION)
ZIPEDFILE=../Origs-Cache/$(BASENAME).7z

FETCHURLS=
FETCHURLS+=http://www.7-zip.org/a/$(BASENAME).7z



ifneq (,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
WIN_CC=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) CC)
WIN_AR=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) AR)
WIN_LD=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) LD)
INCLUDE=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) INCLUDE)
WIN_LIB_DIR=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) LIB)
TOOLPATH=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) PATH)
endif

ifeq (Unix,$(ProjectPlatformSubdir))
CC=$(shell ../../ScriptsLib/GetConfigurationParameter $(CONFIGURATION) CompilerDriver-C)
LIBS=-lstdc++ -lc
endif

BUILDS_ROOT=../../Builds/


PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/lzma/7z.h			\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/lzma/7zCrc.h		\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/lzma/7zTypes.h

ifeq (Unix,$(ProjectPlatformSubdir))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/liblzma.a
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/lzma.lib
endif





#BUILDS_DIR=	CURRENT/Builds/


all:
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Building Stroika/ThirdPartyComponents/lzma $(VERSION) {$(CONFIGURATION)}:"
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(MAKE) --no-print-directory --silent CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) check


$(PRODUCED_OUTPUT_ARTIFACTS):
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) check_prerequisites_
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) doCreate
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) doBuilds


$(ZIPEDFILE):
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ../../ScriptsLib/WebGet.sh $(ZIPEDFILE) $(FETCHURLS)


check-prerequisite-tools:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Checking Prerequisite tools for lzma ... "
	@$(MAKE) --no-print-directory --silent CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) check_prerequisites_
	@$(ECHO) "done";



check_prerequisites_:
	@{ type 7za > /dev/null 2>&1; } || { ../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) && ../../ScriptsLib/GetMessageForMissingTool.sh 7za && exit 1; }


doCreate:	$(ZIPEDFILE)
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n "Extract $(ZIPEDFILE) to CURRENT ... "
	@rm -rf CURRENT
	@7za x -oCURRENT $(ZIPEDFILE) > /dev/null
	@#DISABLE - OLD - NOT APPARNETLY NEEDEED ANYMORE - AND CHMOD -R doesn't work on macos --LGP 2017-01-03
	@#chmod a+r -R CURRENT  #older versions of 7za extract bad permissions -- LGP 2015-12-29
	@cp Subdir-Makefile CURRENT/Makefile
	@$(ECHO) "done"


doBuilds:
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n "Buidling, and redirecting messages to CURRENT/BUILD.OUT ..."
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(MAKE) CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) doBuildInternal_ > CURRENT/BUILD.OUT 2>&1
	@$(ECHO) "done"


doBuildInternal_:
ifeq (Unix,$(ProjectPlatformSubdir))
	$(MAKE) --directory CURRENT ProjectPlatformSubdir=$(ProjectPlatformSubdir) CC="$(CC)" LIBS="$(LIBS)"
	mkdir -p $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib && cp CURRENT/lzma.a  $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/liblzma.a
	mkdir -p $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/lzma && cp CURRENT/C/7z.h CURRENT/C/7zCrc.h CURRENT/C/7zTypes.h $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/lzma/
	cp CURRENT/lzma CURRENT/7zDec $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/
else
	#clean out CURRENT
	$(MAKE) --directory CURRENT \
		ProjectPlatformSubdir="$(ProjectPlatformSubdir)" \
		EXESUFFIX=.exe \
		LIBSUFFIX=lib \
		OBJSUFFIX=obj \
		clobber

	#Build to CURRENT - if its 32-bit debug/relase
ifeq (U-32,$(findstring U-32,$(CONFIGURATION)))
ifeq (Debug,$(findstring Debug,$(CONFIGURATION)))
	$(MAKE) --directory CURRENT \
		ProjectPlatformSubdir="$(ProjectPlatformSubdir)" \
		CC="$(WIN_CC)" \
		CPlusPlus="$(WIN_CC)" \
		CFLAGS="-nologo -W3 -MTd -Z7" \
		INCLUDE="$(INCLUDE)" \
		LIB="$(WIN_LIB_DIR)" \
		LIBS="oleaut32.lib ole32.lib user32.lib" \
		LD="$(WIN_LD)" \
		AR="$(WIN_AR)" \
		ARCRATEFLAGS="-nologo" \
		ARTARGETPREFIX=-out: \
		LINKTARGETPREFIX=-out: \
		LDFLAGS="-nologo -debug -incremental:no -opt:ref" \
		EXESUFFIX=.exe \
		LIBSUFFIX=lib \
		OBJSUFFIX=obj \
		all
endif
ifeq (Release,$(findstring Release,$(CONFIGURATION)))
	$(MAKE) --directory CURRENT \
		ProjectPlatformSubdir="$(ProjectPlatformSubdir)" \
		CC="$(WIN_CC)" \
		CPlusPlus="$(WIN_CC)" \
		CFLAGS="-nologo -W3 -MT -O2 -Oy- -Z7" \
		INCLUDE="$(INCLUDE)" \
		LIB="$(WIN_LIB_DIR)" \
		LIBS="oleaut32.lib ole32.lib user32.lib" \
		LD="$(WIN_LD)" \
		AR="$(WIN_AR)" \
		ARCRATEFLAGS="-nologo" \
		ARTARGETPREFIX=-out: \
		LINKTARGETPREFIX=-out: \
		LDFLAGS="-nologo -debug -incremental:no -opt:ref" \
		EXESUFFIX=.exe \
		LIBSUFFIX=lib \
		OBJSUFFIX=obj \
		all
endif
endif


	#Build to CURRENT - if its 64-bit debug/relase
ifeq (U-64,$(findstring U-64,$(CONFIGURATION)))
ifeq (Debug,$(findstring Debug,$(CONFIGURATION)))
	$(MAKE) --directory CURRENT \
		ProjectPlatformSubdir="$(ProjectPlatformSubdir)" \
		CC="$(WIN_CC)" \
		CPlusPlus="$(WIN_CC)" \
		CFLAGS="-nologo -W3 -MTd -Oy- -Z7" \
		INCLUDE="$(INCLUDE)" \
		LIB="$(WIN_LIB_DIR)" \
		PATH="$(TOOLPATH)" \
		LIBS="oleaut32.lib ole32.lib user32.lib" \
		LD="$(WIN_LD)" \
		AR="$(WIN_AR)" \
		ARCRATEFLAGS="-nologo" \
		ARTARGETPREFIX=-out: \
		LINKTARGETPREFIX=-out: \
		LDFLAGS="-nologo -debug -incremental:no -opt:ref" \
		EXESUFFIX=.exe \
		LIBSUFFIX=lib \
		OBJSUFFIX=obj \
		all
endif
ifeq (Release,$(findstring Release,$(CONFIGURATION)))
	$(MAKE) --directory CURRENT \
		ProjectPlatformSubdir="$(ProjectPlatformSubdir)" \
		CC="$(WIN_CC)" \
		CPlusPlus="$(WIN_CC)" \
		CFLAGS="-nologo -W3 -MT -O2 -Oy- -Z7" \
		INCLUDE="$(INCLUDE)" \
		LIB="$(WIN_LIB_DIR)" \
		PATH="$(TOOLPATH)" \
		LIBS="oleaut32.lib ole32.lib user32.lib" \
		LD="$(WIN_LD)" \
		AR="$(WIN_AR)" \
		ARCRATEFLAGS="-nologo" \
		ARTARGETPREFIX=-out: \
		LINKTARGETPREFIX=-out: \
		LDFLAGS="-nologo -debug -incremental:no -opt:ref" \
		EXESUFFIX=.exe \
		LIBSUFFIX=lib \
		OBJSUFFIX=obj \
		all
endif
endif

	# Now copy the right output
	mkdir -p $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib && cp CURRENT/lzma.lib  $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/
	mkdir -p $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/lzma && cp CURRENT/C/7z.h CURRENT/C/7zCrc.h CURRENT/C/7zTypes.h $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/lzma/
	cp CURRENT/lzma.exe CURRENT/7zDec.exe $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/

endif


ORIGS:	$(ZIPEDFILE)
	@7z x -oORIGS $(ZIPEDFILE) > /dev/null;


check:
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Checking ... "
	@for i in $(PRODUCED_OUTPUT_ARTIFACTS) ; do \
		if [ ! -e $$i ]; then\
			$(ECHO) "   FAILED: missing $$i";\
			exit 1;\
		fi;\
	done
	@$(ECHO) " Stroika/ThirdPartyComponents/lzma -    [Succeeded]";


clean:
	@rm -rf CURRENT


clobber:	clean
ifeq ($(CONFIGURATION),)
	@rm -rf $(BUILDS_ROOT)*/ThirdPartyComponents/lib/{lzma}* $(BUILDS_ROOT)*/ThirdPartyComponents/include/lzma
else
	@rm -f $(PRODUCED_OUTPUT_ARTIFACTS)
endif
