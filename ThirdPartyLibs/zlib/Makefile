CONFIGURATION 			?=	$(shell perl ../../ScriptsLib/GetDefaultConfiguration.pl)
ProjectPlatformSubdir	=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)


.PHONY:	all
.DEFAULT_GOAL := all

DoCreateSymLink=0

VERSION=1.2.8
BASENAME=zlib-$(VERSION)
ZIPEDFILE=../Origs-Cache/$(BASENAME).tar.gz
TARFILE=$(BASENAME).tar
EXTRACTED_DIRNAME=$(BASENAME)
SLINKDIRNAME=$(BASENAME)
USE_DIRNAME=CURRENT

FETCHURL=http://zlib.net/$(BASENAME).tar.gz
#FETCHURL=http://sourceforge.net/projects/libpng/files/zlib/$(VERSION)/zlib-$(VERSION).tar.gz/download

ifneq (,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
WIN_CC32=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl CC_32)")
WIN_CC64=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl CC_64)")
WIN_AS32=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl AS_32)")
WIN_AS64=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl AS_64)")
WIN_LIB32=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl LIB_32)")
WIN_LIB64=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl LIB_64)")
endif


BUILDS_ROOT=../../Builds/


ifeq (Linux,$(ProjectPlatformSubdir))
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Libs/libz.a			\
	$(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Includes/zlib.h		\
	$(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Includes/zconf.h
else
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Libs/zlib.lib					\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Includes/zlib.h				\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Includes/zconf.h				\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Libs/zlib.lib				\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Includes/zlib.h				\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Includes/zconf.h				\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Libs/zlib.lib		\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Includes/zlib.h		\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Includes/zconf.h	\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Libs/zlib.lib			\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Includes/zlib.h		\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Includes/zconf.h		\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Libs/zlib.lib					\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Includes/zlib.h				\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Includes/zconf.h				\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Libs/zlib.lib				\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Includes/zlib.h				\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Includes/zconf.h				\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Libs/zlib.lib			\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Includes/zlib.h		\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Includes/zconf.h
endif





TRGDIR=CURRENT

BUILDS_DIR=	CURRENT/Builds/


all:		$(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory check


$(PRODUCED_OUTPUT_ARTIFACTS):	CURRENT/Builds
	
CURRENT/Builds:
	@echo "   Building Stroika/ThirdPartyLibs/zlib $(VERSION)..."
	@$(MAKE) --no-print-directory doCreate
	@$(MAKE) --no-print-directory doBuilds


$(ZIPEDFILE):
	@echo -n "      ..." 
	wget --quiet --tries=10 -O $(ZIPEDFILE) $(FETCHURL) || (rm -f $(ZIPEDFILE) && false)

$(TRGDIR):	$(ZIPPEDFILE)
	@$(MAKE) --no-print-directory doCreate

doPatch:
	@echo -n "      ...Patching"
ifneq (,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	@rm -f PATCHING.OUT
	@dos2unix -q CURRENT/win32/Makefile.msc > PATCHING.OUT
	@patch -p0 CURRENT/win32/Makefile.msc Patches/Makefile.msc.PATCH >> PATCHING.OUT
	@patch -p0 CURRENT/win32/Makefile.msc Patches/Makefile.msc.Z7.PATCH >> PATCHING.OUT
	@unix2dos -q CURRENT/win32/Makefile.msc >> PATCHING.OUT
endif
	@echo "   ...done"

doConfigure:
	@echo -n "      ...Configuring redirecting messages to CONFIGURE.OUT..."
ifeq (Linux,$(ProjectPlatformSubdir))
	@(cd CURRENT && ./configure) > CONFIGURE.OUT 2>&1
endif
	@echo "   ...done"


doCreate:	$(ZIPEDFILE)
	@if [ -e  $(TRGDIR) ]; then\
		echo "      ...CURRENT dir already exists";\
	else\
		echo "      ...Extract $(ZIPEDFILE) to $(TRGDIR)...";\
		tar xf $(ZIPEDFILE) --no-same-owner;\
		sleep 1;	# hack cuz sometimes it appears command not fully done writing - and we get sporadic failures on next stop on win7;\
		mv $(EXTRACTED_DIRNAME) $(TRGDIR);\
		$(MAKE) --no-print-directory doPatch;\
		$(MAKE) --no-print-directory doConfigure;\
	fi


doBuilds:
	@if [ -e  BUILD.OUT ]; then\
		echo "      ...BUILD.OUT already exists";\
	else\
		echo "      ...Buidling, and redirecting messages to BUILD.OUT ...";\
		make doBuildInternal_ > BUILD.OUT 2>&1;\
	fi
	@echo "      ...Copying builds to ... $(BUILDS_ROOT)"
ifeq (Linux,$(ProjectPlatformSubdir))
	@mkdir -p $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Libs && cp CURRENT/libz.a  $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Includes && cp CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Includes/
else
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Libs && cp CURRENT/Builds/Debug32/zlib.lib $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Includes && cp  CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Includes/
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Libs && cp CURRENT/Builds/Debug32/zlib.lib $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Includes && cp CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Includes/
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Libs && cp CURRENT/Builds/Debug32/zlib.lib $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Includes && cp CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Includes/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Libs && cp CURRENT/Builds/Debug32/zlib.lib $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Includes && cp CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Includes/
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Libs && cp CURRENT/Builds/Debug32/zlib.lib $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Includes && cp CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Includes/
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Libs && cp CURRENT/Builds/Debug32/zlib.lib $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Includes && cp CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Includes/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Libs && cp CURRENT/Builds/Debug32/zlib.lib $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Includes && cp CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Includes/
endif

	
doBuildInternal_:
ifneq (,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	@rm -rf $(BUILDS_DIR)
	@cd $(TRGDIR) && rm -f *.obj *.pdb *.lib
	@perl ../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl "cd $(TRGDIR) && MFLAGS= && MAKEFLAGS= && nmake MAKEFLAGS= MFLAGS= -f win32/Makefile.msc LOC='/MTd -DASMV -DASMINF' OBJA='inffas32.obj match686.obj' AS='$(WIN_AS32)' CC='$(WIN_CC32)' AR='$(WIN_LIB32)' zlib.lib"
	@mkdir -p $(BUILDS_DIR)Debug32
	@mv $(TRGDIR)/zlib.lib $(BUILDS_DIR)Debug32/

	@cd $(TRGDIR) && rm -f *.obj *.pdb *.lib
	@perl ../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl "cd $(TRGDIR) && MFLAGS= && MAKEFLAGS= && nmake MAKEFLAGS= MFLAGS= -f win32/Makefile.msc LOC='/MT -DASMV -DASMINF' OBJA='inffas32.obj match686.obj' AS='$(WIN_AS32)' 'CC=$(WIN_CC32)' 'AR=$(WIN_LIB32)' zlib.lib"
	@mkdir -p $(BUILDS_DIR)Release32
	@mv $(TRGDIR)/zlib.lib $(BUILDS_DIR)Release32/

	@cd $(TRGDIR) && rm -f *.obj *.pdb *.lib
	@perl ../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl "cd $(TRGDIR) && MFLAGS= && MAKEFLAGS= && nmake MAKEFLAGS= MFLAGS= -f win32/Makefile.msc LOC='-DASMV -DASMINF -I.' OBJA='inffasx64.obj gvmat64.obj inffas8664.obj' AS='$(WIN_AS64)' CC='$(WIN_CC64)' AR='$(WIN_LIB64)' zlib.lib"
	@mkdir -p $(BUILDS_DIR)Debug64
	@mv $(TRGDIR)/zlib.lib $(BUILDS_DIR)Debug64/

	@cd $(TRGDIR) && rm -f *.obj *.pdb *.lib
	@perl ../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl "cd $(TRGDIR) && MFLAGS= && MAKEFLAGS= && nmake MAKEFLAGS= MFLAGS= -f win32/Makefile.msc LOC='-DASMV -DASMINF -I.' OBJA='inffasx64.obj gvmat64.obj inffas8664.obj' AS='$(WIN_AS64)' 'CC=$(WIN_CC64)' 'AR=$(WIN_LIB64)' zlib.lib"
	@mkdir -p $(BUILDS_DIR)Release64
	@mv $(TRGDIR)/zlib.lib $(BUILDS_DIR)Release64/
else
	@$(MAKE) --directory CURRENT --no-print-directory
endif


ORIGS:	$(ZIPEDFILE)
	@tar xf $(ZIPEDFILE) --no-same-owner
	@mv $(EXTRACTED_DIRNAME) ORIGS
	@unix2dos ORIGS/win32/Makefile.msc

check:
	@echo -n "      ...Checking..."
	@for i in $(PRODUCED_OUTPUT_ARTIFACTS) ; do \
		if [ ! -e $$i ]; then\
			echo "   FAILED: missing $$i";\
			exit 1;\
		fi;\
	done
	@echo "   Stroika/ThirdPartyLibs/zlib - [Succeeded]";

clean:
	@$(MAKE) --directory $(TRGDIR) --no-print-directory MAKEFLAGS=$(MAKEFLAGS) clean

clobber:
	@rm -rf $(TRGDIR)
	@rm -f BUILD.OUT PATCHING.OUT
