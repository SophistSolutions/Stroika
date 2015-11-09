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

FETCHURLS=
FETCHURLS+=http://sourceforge.net/projects/libpng/files/zlib/$(VERSION)/zlib-$(VERSION).tar.gz/download

ifneq (,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
WIN_CC32=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl CC_32)")
WIN_CC64=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl CC_64)")
WIN_AS32=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl AS_32)")
WIN_AS64=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl AS_64)")
WIN_LIB32=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl LIB_32)")
WIN_LIB64=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl LIB_64)")
endif

ifeq (Unix,$(ProjectPlatformSubdir))
CC=$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) CompilerDriver-C)
endif

BUILDS_ROOT=../../Builds/


ifeq (Unix,$(ProjectPlatformSubdir))
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/lib/libz.a			\
	$(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/include/zlib.h		\
	$(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/include/zconf.h
else
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/lib/zlib.lib					\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/include/zlib.h				\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/include/zconf.h				\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/lib/zlib.lib				\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/include/zlib.h				\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/include/zconf.h				\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/lib/zlib.lib		\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/include/zlib.h		\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/include/zconf.h	\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/lib/zlib.lib			\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/include/zlib.h		\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/include/zconf.h		\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/lib/zlib.lib					\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/include/zlib.h				\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/include/zconf.h				\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/lib/zlib.lib				\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/include/zlib.h				\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/include/zconf.h				\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/lib/zlib.lib			\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/include/zlib.h		\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/include/zconf.h
endif

.NOTPARALLEL:	$(PRODUCED_OUTPUT_ARTIFACTS)




BUILDS_DIR=	CURRENT/Builds/


all:
	@echo "   Building Stroika/ThirdPartyLibs/zlib $(VERSION)..."
	@$(MAKE) --no-print-directory --silent $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory check


$(PRODUCED_OUTPUT_ARTIFACTS):
	@$(MAKE) --no-print-directory doCreate
	@$(MAKE) --no-print-directory doBuilds


$(ZIPEDFILE):
	@echo -n "      ..." 
	@../../ScriptsLib/WebGet.sh $(ZIPEDFILE) $(FETCHURLS)


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
ifeq (Unix,$(ProjectPlatformSubdir))
	@(CC="$(CC)" cd CURRENT && ./configure) > CONFIGURE.OUT 2>&1
endif
	@echo "   ...done"


doCreate:	$(ZIPEDFILE)
	@echo "      ...Extract $(ZIPEDFILE) to CURRENT..."
	@tar xf $(ZIPEDFILE) --no-same-owner
	@mv $(EXTRACTED_DIRNAME) CURRENT
	@$(MAKE) --no-print-directory doPatch
	@$(MAKE) --no-print-directory doConfigure

doBuilds:
	@echo "      ...Buidling, and redirecting messages to BUILD.OUT ..."
	@make doBuildInternal_ > BUILD.OUT 2>&1
	@echo "      ...Copying builds to ... $(BUILDS_ROOT)"
ifeq (Unix,$(ProjectPlatformSubdir))
	@mkdir -p $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/lib && cp CURRENT/libz.a  $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/lib/
	@mkdir -p $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/include && cp CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/include/
else
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/lib && cp CURRENT/Builds/Debug32/zlib.lib $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/lib/
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/include && cp  CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/include/
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/lib && cp CURRENT/Builds/Release32/zlib.lib $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/lib/
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/include && cp CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/include/
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/lib && cp CURRENT/Builds/Release32/zlib.lib $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/lib/
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/include && cp CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/include/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/lib && cp CURRENT/Builds/Release32/zlib.lib $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/lib/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/include && cp CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/include/
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/lib && cp CURRENT/Builds/Debug64/zlib.lib $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/lib/
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/include && cp CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/include/
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/lib && cp CURRENT/Builds/Release64/zlib.lib $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/lib/
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/include && cp CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/include/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/lib && cp CURRENT/Builds/Release64/zlib.lib $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/lib/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/include && cp CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/include/
endif

	
doBuildInternal_:
ifneq (,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	@rm -rf $(BUILDS_DIR)
	@cd CURRENT && rm -f *.obj *.pdb *.lib
	@perl ../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl "cd CURRENT && MFLAGS= && MAKEFLAGS= && nmake MAKEFLAGS= MFLAGS= -f win32/Makefile.msc LOC='/safeseh /MTd -DASMV -DASMINF' OBJA='inffas32.obj match686.obj' AS='$(WIN_AS32)' CC='$(WIN_CC32)' AR='$(WIN_LIB32)' zlib.lib"
	@mkdir -p $(BUILDS_DIR)Debug32
	@mv CURRENT/zlib.lib $(BUILDS_DIR)Debug32/

	@cd CURRENT && rm -f *.obj *.pdb *.lib
	@perl ../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl "cd CURRENT && MFLAGS= && MAKEFLAGS= && nmake MAKEFLAGS= MFLAGS= -f win32/Makefile.msc LOC='/safeseh /MT -DASMV -DASMINF' OBJA='inffas32.obj match686.obj' AS='$(WIN_AS32)' 'CC=$(WIN_CC32)' 'AR=$(WIN_LIB32)' zlib.lib"
	@mkdir -p $(BUILDS_DIR)Release32
	@mv CURRENT/zlib.lib $(BUILDS_DIR)Release32/

	@cd CURRENT && rm -f *.obj *.pdb *.lib
	@perl ../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl "cd CURRENT && MFLAGS= && MAKEFLAGS= && nmake MAKEFLAGS= MFLAGS= -f win32/Makefile.msc LOC='/MTd -DASMV -DASMINF -I.' OBJA='inffasx64.obj gvmat64.obj inffas8664.obj' AS='$(WIN_AS64)' CC='$(WIN_CC64)' AR='$(WIN_LIB64)' zlib.lib"
	@mkdir -p $(BUILDS_DIR)Debug64
	@mv CURRENT/zlib.lib $(BUILDS_DIR)Debug64/

	@cd CURRENT && rm -f *.obj *.pdb *.lib
	@perl ../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl "cd CURRENT && MFLAGS= && MAKEFLAGS= && nmake MAKEFLAGS= MFLAGS= -f win32/Makefile.msc LOC='/MT -DASMV -DASMINF -I.' OBJA='inffasx64.obj gvmat64.obj inffas8664.obj' AS='$(WIN_AS64)' 'CC=$(WIN_CC64)' 'AR=$(WIN_LIB64)' zlib.lib"
	@mkdir -p $(BUILDS_DIR)Release64
	@mv CURRENT/zlib.lib $(BUILDS_DIR)Release64/
else
	@$(MAKE) --directory CURRENT --no-print-directory CC="$(CC)"
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
	@rm -rf CURRENT

clobber:	clean
	@rm -f BUILD.OUT PATCHING.OUT
	@rm -f $(PRODUCED_OUTPUT_ARTIFACTS)
