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
#WIN_CC32=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl CC_32)")
WIN_CC32=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl CC_32)
WIN_CC64=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl CC_64)")
WIN_AS32=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl AS_32)")
WIN_AS64=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl AS_64)")
WIN_LIB32=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl LIB_32)
WIN_LIB64=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl LIB_64)")
WIN_LIB64=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl LIB_64)")
WIN_LINK32=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl LINK_32)
WIN_LINK64=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl LINK_64)")
INCLUDE=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl INCLUDE)
LIB=$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl LIB)
endif


BUILDS_ROOT=../../Builds/


ifeq (Unix,$(ProjectPlatformSubdir))
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Libs/lzma$.a			\
	$(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Includes/zlib.h		\
	$(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Includes/zconf.h
else
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Libs/lzma.lib					\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Includes/zlib.h				\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Includes/zconf.h				\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Libs/lzma.lib				\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Includes/zlib.h				\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Includes/zconf.h				\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Libs/lzma.lib		\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Includes/zlib.h		\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Includes/zconf.h	\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Libs/lzma.lib			\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Includes/zlib.h		\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Includes/zconf.h		\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Libs/lzma.lib					\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Includes/zlib.h				\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Includes/zconf.h				\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Libs/lzma.lib				\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Includes/zlib.h				\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Includes/zconf.h				\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Libs/lzma.lib			\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Includes/zlib.h		\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Includes/zconf.h
endif





TRGDIR=CURRENT

BUILDS_DIR=	CURRENT/Builds/


all:		$(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory check


$(PRODUCED_OUTPUT_ARTIFACTS):	$(TRGDIR)/Builds
	
$(TRGDIR)/Builds:
	@echo "   Building Stroika/ThirdPartyLibs/lzma $(VERSION)..."
	@$(MAKE) --no-print-directory doCreate
	@$(MAKE) --no-print-directory doBuilds


$(ZIPEDFILE):
	@echo -n "      ..." 
	wget --quiet --tries=10 -O $(ZIPEDFILE) $(FETCHURL) || (rm -f $(ZIPEDFILE) && false)

$(TRGDIR):	$(ZIPPEDFILE)
	@$(MAKE) --no-print-directory doCreate


doCreate:	$(ZIPEDFILE)
	@if [ -e  $(TRGDIR) ]; then\
		echo "      ...$(TRGDIR) dir already exists";\
	else\
		echo "      ...Extract $(ZIPEDFILE) to $(TRGDIR)...";\
		7z x -o$(TRGDIR) $(ZIPEDFILE) > /dev/null;\
		cp Subdir-Makefile $(TRGDIR)/Makefile;\
	fi


doBuilds:
	@if [ -e  BUILD.OUT ]; then\
		echo "      ...BUILD.OUT already exists";\
	else\
		echo "      ...Buidling, and redirecting messages to BUILD.OUT ...";\
		make doBuildInternal_ > BUILD.OUT 2>&1;\
	fi
ifeq (0,1)
	@echo "      ...Copying builds to ... $(BUILDS_ROOT)"
ifeq (Unix,$(ProjectPlatformSubdir))
	@mkdir -p $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Libs && cp CURRENT/libz.a  $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Includes && cp CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Includes/
else
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Libs && cp CURRENT/Builds/Debug32/lzma.lib $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Includes && cp  CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Includes/
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Libs && cp CURRENT/Builds/Debug32/lzma.lib $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Includes && cp CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Includes/
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Libs && cp CURRENT/Builds/Debug32/lzma.lib $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Includes && cp CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Includes/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Libs && cp CURRENT/Builds/Debug32/lzma.lib $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Includes && cp CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Includes/
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Libs && cp CURRENT/Builds/Debug32/lzma.lib $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Includes && cp CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Includes/
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Libs && cp CURRENT/Builds/Debug32/lzma.lib $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Includes && cp CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Includes/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Libs && cp CURRENT/Builds/Debug32/lzma.lib $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Includes && cp CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Includes/
endif
endif



doBuildInternal_:
ifeq (Unix,$(ProjectPlatformSubdir))
	make --directory $(TRGDIR)
else
	make --directory $(TRGDIR) CC="$(WIN_CC32)" CPlusPlus="$(WIN_CC32)" CFLAGS="-nologo -W3 -O2 -Oy- -Z7" INCLUDE="$(INCLUDE)" LIB="$(LIB)" AR="$(WIN_LIB32)" ARCRATEFLAGS=-nologo ARTARGETPREFIX=-out: LINKTARGETPREFIX=-out: LD="$(WIN_LINK32)" LDFLAGS="-nologo -debug -incremental:no -opt:ref" LIBSUFFIX=lib OBJSUFFIX=obj EXESUFFIX=.exe
endif


ORIGS:	$(ZIPEDFILE)
	@7z x -oORIGS $(ZIPEDFILE) > /dev/null;

check:
	@echo -n "      ...Checking..."
	@#tmphack disable check
	@for i in $(_HACK2DISABLE_PRODUCED_OUTPUT_ARTIFACTS) ; do \
		if [ ! -e $$i ]; then\
			echo "   FAILED: missing $$i";\
			exit 1;\
		fi;\
	done
	@echo "   Stroika/ThirdPartyLibs/lzma - [Succeeded]";

clean:
	@-$(MAKE) --directory $(TRGDIR) --no-print-directory MAKEFLAGS=$(MAKEFLAGS) clean

clobber:
	@rm -rf $(TRGDIR) BUILD.OUT
