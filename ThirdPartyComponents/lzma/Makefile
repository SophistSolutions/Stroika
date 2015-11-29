ifneq ($(CONFIGURATION),)
ProjectPlatformSubdir	=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)
endif



.DEFAULT_GOAL := all
.PHONY:	all clean clobber check doBuild doCreate doPatch

.NOTPARALLEL:


MAKE_INDENT_LEVEL?=$(MAKELEVEL)



DoCreateSymLink=0


# From:		http://www.7-zip.org/sdk.html



VERSION=1509
BASENAME=lzma$(VERSION)
ZIPEDFILE=../Origs-Cache/$(BASENAME).7z

FETCHURLS=
FETCHURLS+=http://www.7-zip.org/a/$(BASENAME).7z



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
LIBS=-lstdc++ -lc
endif

BUILDS_ROOT=../../Builds/


PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/lzma/7z.h			\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/lzma/7zCrc.h		\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/lzma/7zTypes.h

ifeq (Unix,$(ProjectPlatformSubdir))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/lzma.a
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/lzma.lib
endif





BUILDS_DIR=	CURRENT/Builds/


all:
	@../../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && echo "Building Stroika/ThirdPartyComponents/lzma $(VERSION):"
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
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo -n "..." 
	@../../ScriptsLib/WebGet.sh $(ZIPEDFILE) $(FETCHURLS)


check_prerequisites_:
	@type 7za > /dev/null || (../../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && echo "Checking tools......Missing 7za")


doCreate:	$(ZIPEDFILE)
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo -n "Extract $(ZIPEDFILE) to CURRENT..."
	@rm -rf CURRENT
	@7za x -oCURRENT $(ZIPEDFILE) > /dev/null
	@cp Subdir-Makefile CURRENT/Makefile
	@echo "done"


doBuilds:
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo -n "Buidling, and redirecting messages to CURRENT/BUILD.OUT ..."
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(MAKE) CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) doBuildInternal_ > CURRENT/BUILD.OUT 2>&1
	@echo "done"


doBuildInternal_:
ifeq (Unix,$(ProjectPlatformSubdir))
	$(MAKE) --directory CURRENT ProjectPlatformSubdir=$(ProjectPlatformSubdir) CC="$(CC)" LIBS="$(LIBS)"
	mkdir -p $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib && cp CURRENT/lzma.a  $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/
	mkdir -p $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/lzma && cp CURRENT/C/7z.h CURRENT/C/7zCrc.h CURRENT/C/7zTypes.h $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/lzma/
	cp CURRENT/lzma CURRENT/7zDec $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/
else
ifeq ($(CONFIGURATION),Debug-U-32)
	$(MAKE) --directory CURRENT \
		ProjectPlatformSubdir="$(ProjectPlatformSubdir)" \
		CC="$(WIN_CC32)" \
		CPlusPlus="$(WIN_CC32)" \
		CFLAGS="-nologo -W3 -MTd -Z7" \
		INCLUDE="$(INCLUDE)" \
		LIB="$(LIBDIR32)" \
		LIBS="oleaut32.lib ole32.lib user32.lib" \
		LD="$(WIN_LINK32)" \
		AR="$(WIN_LIB32)" \
		ARCRATEFLAGS="-nologo" \
		ARTARGETPREFIX=-out: \
		LINKTARGETPREFIX=-out: \
		LDFLAGS="-nologo -debug -incremental:no -opt:ref" \
		EXESUFFIX=.exe \
		LIBSUFFIX=lib \
		OBJSUFFIX=obj \
		all
	mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/lib && cp CURRENT/lzma.lib  $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/lib/
	mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/include/lzma && cp CURRENT/C/7z.h CURRENT/C/7zCrc.h CURRENT/C/7zTypes.h $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/include/lzma/
	cp CURRENT/lzma.exe CURRENT/7zDec.exe $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/
endif
ifeq (U-32,$(findstring U-32,$(CONFIGURATION)))
ifeq (Release,$(findstring Release,$(CONFIGURATION)))
	$(MAKE) --directory CURRENT \
		ProjectPlatformSubdir="$(ProjectPlatformSubdir)" \
		EXESUFFIX=.exe \
		LIBSUFFIX=lib \
		OBJSUFFIX=obj \
		clobber
	$(MAKE) --directory CURRENT \
		ProjectPlatformSubdir="$(ProjectPlatformSubdir)" \
		CC="$(WIN_CC32)" \
		CPlusPlus="$(WIN_CC32)" \
		CFLAGS="-nologo -W3 -MT -O2 -Oy- -Z7" \
		INCLUDE="$(INCLUDE)" \
		LIB="$(LIBDIR32)" \
		LIBS="oleaut32.lib ole32.lib user32.lib" \
		LD="$(WIN_LINK32)" \
		AR="$(WIN_LIB32)" \
		ARCRATEFLAGS="-nologo" \
		ARTARGETPREFIX=-out: \
		LINKTARGETPREFIX=-out: \
		LDFLAGS="-nologo -debug -incremental:no -opt:ref" \
		EXESUFFIX=.exe \
		LIBSUFFIX=lib \
		OBJSUFFIX=obj \
		all
ifeq ($(CONFIGURATION),Release-U-32)
	mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/lib && cp CURRENT/lzma.lib  $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/lib/
	mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/include/lzma && cp CURRENT/C/7z.h CURRENT/C/7zCrc.h CURRENT/C/7zTypes.h $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/include/lzma/
	cp CURRENT/lzma.exe CURRENT/7zDec.exe $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/
endif
ifeq ($(CONFIGURATION),Release-DbgMemLeaks-U-32)
	mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/lib && cp CURRENT/lzma.lib  $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/lib/
	mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/include/lzma && cp CURRENT/C/7z.h CURRENT/C/7zCrc.h CURRENT/C/7zTypes.h $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/include/lzma/
	cp CURRENT/lzma.exe CURRENT/7zDec.exe $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/
endif
ifeq ($(CONFIGURATION),Release-Logging-U-32)
	mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/lib && cp CURRENT/lzma.lib  $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/lib/
	mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/include/lzma && cp CURRENT/C/7z.h CURRENT/C/7zCrc.h CURRENT/C/7zTypes.h $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/include/lzma/
	cp CURRENT/lzma.exe CURRENT/7zDec.exe $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/
endif
endif
endif

ifeq ($(CONFIGURATION),Debug-U-64)
	$(MAKE) --directory CURRENT \
		ProjectPlatformSubdir="$(ProjectPlatformSubdir)" \
		EXESUFFIX=.exe \
		LIBSUFFIX=lib \
		OBJSUFFIX=obj \
		clobber
	$(MAKE) --directory CURRENT \
		ProjectPlatformSubdir="$(ProjectPlatformSubdir)" \
		CC="$(WIN_CC64)" \
		CPlusPlus="$(WIN_CC64)" \
		CFLAGS="-nologo -W3 -MTd -Oy- -Z7" \
		INCLUDE="$(INCLUDE)" \
		LIB="$(LIBDIR64)" \
		PATH="$(PATH)" \
		LIBS="oleaut32.lib ole32.lib user32.lib" \
		LD="$(WIN_LINK64)" \
		AR="$(WIN_LIB64)" \
		ARCRATEFLAGS="-nologo" \
		ARTARGETPREFIX=-out: \
		LINKTARGETPREFIX=-out: \
		LDFLAGS="-nologo -debug -incremental:no -opt:ref" \
		EXESUFFIX=.exe \
		LIBSUFFIX=lib \
		OBJSUFFIX=obj \
		all
	mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/lib && cp CURRENT/lzma.lib  $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/lib/
	mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/include/lzma && cp CURRENT/C/7z.h CURRENT/C/7zCrc.h CURRENT/C/7zTypes.h $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/include/lzma/
	cp CURRENT/lzma.exe CURRENT/7zDec.exe $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/
endif

ifeq (U-64,$(findstring U-64,$(CONFIGURATION)))
ifeq (Release,$(findstring Release,$(CONFIGURATION)))
	$(MAKE) --directory CURRENT \
		ProjectPlatformSubdir="$(ProjectPlatformSubdir)" \
		EXESUFFIX=.exe \
		LIBSUFFIX=lib \
		OBJSUFFIX=obj \
		clobber
	$(MAKE) --directory CURRENT \
		ProjectPlatformSubdir="$(ProjectPlatformSubdir)" \
		CC="$(WIN_CC64)" \
		CPlusPlus="$(WIN_CC64)" \
		CFLAGS="-nologo -W3 -MT -O2 -Oy- -Z7" \
		INCLUDE="$(INCLUDE)" \
		LIB="$(LIBDIR64)" \
		PATH="$(PATH)" \
		LIBS="oleaut32.lib ole32.lib user32.lib" \
		LD="$(WIN_LINK64)" \
		AR="$(WIN_LIB64)" \
		ARCRATEFLAGS="-nologo" \
		ARTARGETPREFIX=-out: \
		LINKTARGETPREFIX=-out: \
		LDFLAGS="-nologo -debug -incremental:no -opt:ref" \
		EXESUFFIX=.exe \
		LIBSUFFIX=lib \
		OBJSUFFIX=obj \
		all

ifeq ($(CONFIGURATION),Release-U-64)
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/lib && cp CURRENT/lzma.lib  $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/include/lzma && cp CURRENT/C/7z.h CURRENT/C/7zCrc.h CURRENT/C/7zTypes.h $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/include/lzma/
	@cp CURRENT/lzma.exe CURRENT/7zDec.exe $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/
endif
ifeq ($(CONFIGURATION),Release-Logging-U-64)
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/lib && cp CURRENT/lzma.lib  $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/include/lzma && cp CURRENT/C/7z.h CURRENT/C/7zCrc.h CURRENT/C/7zTypes.h $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/include/lzma/
	@cp CURRENT/lzma.exe CURRENT/7zDec.exe $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/
endif
endif
endif
endif


ORIGS:	$(ZIPEDFILE)
	@7z x -oORIGS $(ZIPEDFILE) > /dev/null;


check:
	@../../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && echo -n "Checking..."
	@for i in $(PRODUCED_OUTPUT_ARTIFACTS) ; do \
		if [ ! -e $$i ]; then\
			echo "   FAILED: missing $$i";\
			exit 1;\
		fi;\
	done
	@echo " Stroika/ThirdPartyComponents/lzma -    [Succeeded]";


clean:
	@rm -rf CURRENT


clobber:	clean
ifeq ($(CONFIGURATION),)
	@rm -rf $(BUILDS_ROOT)*/ThirdPartyComponents/lib/{lzma}* $(BUILDS_ROOT)*/ThirdPartyComponents/include/lzma
else
	@rm -f $(PRODUCED_OUTPUT_ARTIFACTS)
endif
