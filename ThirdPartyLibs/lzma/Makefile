CONFIGURATION 			?=	$(shell perl ../../ScriptsLib/GetDefaultConfiguration.pl)
ProjectPlatformSubdir	=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)


.PHONY:	all
.DEFAULT_GOAL := all

DoCreateSymLink=0


# From:		http://www.7-zip.org/sdk.html



VERSION=1506
BASENAME=lzma$(VERSION)
ZIPEDFILE=../Origs-Cache/$(BASENAME).7z

FETCHURL=http://www.7-zip.org/a/$(BASENAME).7z

ifneq (,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
WIN_CC32=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl CC_32)
WIN_CC64=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl CC_64)
#WIN_AS32=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl AS_32)")
#WIN_AS64=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl AS_64)")
WIN_LIB32=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl LIB_32)
WIN_LIB64=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl LIB_64)
WIN_LINK32=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl LINK_32)
WIN_LINK64=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl LINK_64)
INCLUDE=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl INCLUDE)
LIBDIR32=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl LIBDIR32)
LIBDIR64=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl LIBDIR64)
PATH=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl PATH)
endif


BUILDS_ROOT=../../Builds/


ifeq (Unix,$(ProjectPlatformSubdir))
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Libs/lzma.a				\
	$(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Includes/lzma/7z.h		\
	$(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Includes/lzma/7zCRC.h		\
	$(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Includes/lzma/7zTypes.h
else
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Libs/lzma.lib						\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Includes/lzma/7z.h					\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Includes/lzma/7zCRC.h				\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Includes/lzma/7zTypes.h				\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Libs/lzma.lib						\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Includes/lzma/7z.h				\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Includes/lzma/7zCRC.h				\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Includes/lzma/7zTypes.h			\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Libs/lzma.lib			\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Includes/lzma/7z.h	\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Includes/lzma/7zCRC.h	\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Includes/lzma/7zTypes.h\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Libs/lzma.lib				\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Includes/lzma/7z.h		\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Includes/lzma/7zCRC.h		\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Includes/lzma/7zTypes.h	\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Libs/lzma.lib						\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Includes/lzma/7z.h					\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Includes/lzma/7zCRC.h				\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Includes/lzma/7zTypes.h				\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Libs/lzma.lib						\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Includes/lzma/7z.h				\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Includes/lzma/7zCRC.h				\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Includes/lzma/7zTypes.h			\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Libs/lzma.lib				\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Includes/lzma/7z.h		\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Includes/lzma/7zCRC.h		\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Includes/lzma/7zTypes.h
endif





TRGDIR=CURRENT

BUILDS_DIR=	CURRENT/Builds/


all:		$(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory check


$(PRODUCED_OUTPUT_ARTIFACTS):
	@echo "   Building Stroika/ThirdPartyLibs/lzma $(VERSION)..."
	@echo "      ...checking tools..."
	@$(MAKE) --no-print-directory check_prerequisites_
	@$(MAKE) --no-print-directory doCreate
	@$(MAKE) --no-print-directory doBuilds


$(ZIPEDFILE):
	@echo -n "      ..." 
	wget --quiet --tries=10 -O $(ZIPEDFILE) $(FETCHURL) || (rm -f $(ZIPEDFILE) && false)

$(TRGDIR):	$(ZIPPEDFILE)
	@$(MAKE) --no-print-directory doCreate



check_prerequisites_:
	@type 7za > /dev/null || echo "......Missing 7za"



doCreate:	$(ZIPEDFILE)
	@if [ -e  $(TRGDIR) ]; then\
		echo "      ...$(TRGDIR) dir already exists";\
	else\
		echo "      ...Extract $(ZIPEDFILE) to $(TRGDIR)...";\
		7za x -o$(TRGDIR) $(ZIPEDFILE) > /dev/null;\
		cp Subdir-Makefile $(TRGDIR)/Makefile;\
	fi



doBuilds:
	@echo "      ...Buidling, and redirecting messages to BUILD.OUT ..."
	@make doBuildInternal_ > BUILD.OUT 2>&1



doBuildInternal_:
ifeq (Unix,$(ProjectPlatformSubdir))
	make --directory $(TRGDIR) ProjectPlatformSubdir=$(ProjectPlatformSubdir)
	@mkdir -p $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Libs && cp CURRENT/lzma.a  $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Includes/lzma && cp CURRENT/C/7z.h CURRENT/C/7zCRC.h CURRENT/C/7zTypes.h $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Includes/lzma/
	@cp CURRENT/lzma CURRENT/7zDec $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/
else
	make --directory $(TRGDIR) \
		ProjectPlatformSubdir="$(ProjectPlatformSubdir)" \
		CC="$(WIN_CC32)" \
		CPlusPlus="$(WIN_CC32)" \
		CFLAGS="-nologo -W3 -O2 -Oy- -Z7" \
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
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Libs && cp CURRENT/lzma.lib  $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Includes/lzma && cp CURRENT/C/7z.h CURRENT/C/7zCRC.h CURRENT/C/7zTypes.h $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Includes/lzma/
	@cp CURRENT/lzma.exe CURRENT/7zDec.exe $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Libs && cp CURRENT/lzma.lib  $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Includes/lzma && cp CURRENT/C/7z.h CURRENT/C/7zCRC.h CURRENT/C/7zTypes.h $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Includes/lzma/
	@cp CURRENT/lzma.exe CURRENT/7zDec.exe $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Libs && cp CURRENT/lzma.lib  $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Includes/lzma && cp CURRENT/C/7z.h CURRENT/C/7zCRC.h CURRENT/C/7zTypes.h $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Includes/lzma/
	@cp CURRENT/lzma.exe CURRENT/7zDec.exe $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Libs && cp CURRENT/lzma.lib  $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Includes/lzma && cp CURRENT/C/7z.h CURRENT/C/7zCRC.h CURRENT/C/7zTypes.h $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Includes/lzma/
	@cp CURRENT/lzma.exe CURRENT/7zDec.exe $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/
	make --directory $(TRGDIR) \
		ProjectPlatformSubdir="$(ProjectPlatformSubdir)" \
		EXESUFFIX=.exe \
		LIBSUFFIX=lib \
		OBJSUFFIX=obj \
		clobber
	make --directory $(TRGDIR) \
		ProjectPlatformSubdir="$(ProjectPlatformSubdir)" \
		CC="$(WIN_CC64)" \
		CPlusPlus="$(WIN_CC64)" \
		CFLAGS="-nologo -W3 -O2 -Oy- -Z7" \
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
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Libs && cp CURRENT/lzma.lib  $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Includes/lzma && cp CURRENT/C/7z.h CURRENT/C/7zCRC.h CURRENT/C/7zTypes.h $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Includes/lzma/
	@cp CURRENT/lzma.exe CURRENT/7zDec.exe $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Libs && cp CURRENT/lzma.lib  $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Includes/lzma && cp CURRENT/C/7z.h CURRENT/C/7zCRC.h CURRENT/C/7zTypes.h $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Includes/lzma/
	@cp CURRENT/lzma.exe CURRENT/7zDec.exe $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Libs && cp CURRENT/lzma.lib  $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Includes/lzma && cp CURRENT/C/7z.h CURRENT/C/7zCRC.h CURRENT/C/7zTypes.h $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Includes/lzma/
	@cp CURRENT/lzma.exe CURRENT/7zDec.exe $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/
endif


ORIGS:	$(ZIPEDFILE)
	@7z x -oORIGS $(ZIPEDFILE) > /dev/null;

check:
	@echo -n "      ...Checking..."
	@for i in $(PRODUCED_OUTPUT_ARTIFACTS) ; do \
		if [ ! -e $$i ]; then\
			echo "   FAILED: missing $$i";\
			exit 1;\
		fi;\
	done
	@echo "   Stroika/ThirdPartyLibs/lzma - [Succeeded]";

clean:
	@-$(MAKE) --directory $(TRGDIR) --no-print-directory MAKEFLAGS=$(MAKEFLAGS) clean

clobber:
	@rm -rf $(TRGDIR) BUILD.OUT $(PRODUCED_OUTPUT_ARTIFACTS)
