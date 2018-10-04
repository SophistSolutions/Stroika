ProjectPlatformSubdir	=	$(shell ../../ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" ProjectPlatformSubdir)

.PHONY:	all
.DEFAULT_GOAL := all

SHELL=/bin/bash

DoCreateSymLink=0

# last stable version
VERSION=1.2.8
# crashes in regtest for inflate on Win32 (but not Win64), 1.2.9 thru 1.2.11
#VERSION=1.2.11


BASENAME=zlib-$(VERSION)
ZIPEDFILE=../Origs-Cache/$(BASENAME).tar.gz
TARFILE=$(BASENAME).tar
EXTRACTED_DIRNAME=$(BASENAME)
SLINKDIRNAME=$(BASENAME)
USE_DIRNAME=CURRENT

#
# CHECK FOR UPDATED VERSIONS http://zlib.net/
#

FETCHURLS=
FETCHURLS+=https://github.com/madler/zlib/archive/v$(VERSION).tar.gz
FETCHURLS+=http://zlib.net/zlib-$(VERSION).tar.gz
FETCHURLS+=http://prdownloads.sourceforge.net/libpng/zlib-$(VERSION).tar.gz?download
#FETCHURLS+=http://sourceforge.net/projects/libpng/files/zlib/$(VERSION)/zlib-$(VERSION).tar.gz/download

ifneq (,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
WIN_CC	=	$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) CC)")
WIN_AS	=	$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) AS)")
WIN_AR	=	$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) AR)")
endif

ifeq (Unix,$(ProjectPlatformSubdir))
CC	=	$(shell ../../ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" CompilerDriver-C)
endif

BUILDS_ROOT=../../Builds/

MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO?=	$(shell ../../ScriptsLib/GetDefaultShellVariable.sh ECHO)


ifeq (Unix,$(ProjectPlatformSubdir))
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/libz.a
else
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/zlib.lib
endif
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/zlib.h		\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/zconf.h

.NOTPARALLEL:




BUILDS_DIR=	CURRENT/Builds/


all:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Building Stroika/ThirdPartyComponents/zlib $(VERSION) {$(CONFIGURATION)}:"
	@$(MAKE) --no-print-directory --silent CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) check


$(PRODUCED_OUTPUT_ARTIFACTS):
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)  doCreate
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)  doBuilds


$(ZIPEDFILE):
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ../../ScriptsLib/WebGet.sh $(ZIPEDFILE) $(FETCHURLS)


doPatch:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n "Patching ... "
	@dos2unix -q CURRENT/win32/Makefile.msc > CURRENT/PATCHING.OUT
	@patch -p0 CURRENT/win32/Makefile.msc Patches/Makefile.msc.PATCH >> CURRENT/PATCHING.OUT
	@patch -p0 CURRENT/win32/Makefile.msc Patches/Makefile.msc.Z7.PATCH >> CURRENT/PATCHING.OUT
	@unix2dos -q CURRENT/win32/Makefile.msc >> CURRENT/PATCHING.OUT
	@$(ECHO) "done"
endif


doConfigure:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
ifeq (Unix,$(ProjectPlatformSubdir))
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n "Configuring (messages to CURRENT/CONFIGURE.OUT) ... "
	@echo "export CC=$(CC) cd CURRENT && ./configure" > CURRENT/CONFIGURE.OUT
	@(export CC="$(CC)"; cd CURRENT && ./configure) >> CURRENT/CONFIGURE.OUT 2>&1
	@$(ECHO) "done"
endif


doCreate:	$(ZIPEDFILE)
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n "Extract $(ZIPEDFILE) to CURRENT ... "
	@mkdir -p CURRENT && rm -rf CURRENT/*
ifeq ($(shell uname -s),Darwin)
	@tar xf $(ZIPEDFILE) -C CURRENT --strip-components 1
else
	@tar xf $(ZIPEDFILE) --no-same-owner --directory CURRENT --strip-components=1
endif
	@$(ECHO) "done"
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) doPatch
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) doConfigure


doBuilds:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n "Buidling (redirecting messages to CURRENT/BUILD.OUT) ... "
	@make doBuildInternal_ > CURRENT/BUILD.OUT 2>&1
	@$(ECHO) "done"
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n "Copying builds to $(BUILDS_ROOT) ..."
ifeq (Unix,$(ProjectPlatformSubdir))
	@mkdir -p $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib && cp CURRENT/libz.a  $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include && cp CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/
else
ifeq ($(CONFIGURATION),Debug-U-32)
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/lib && cp CURRENT/Builds/Debug32/zlib.lib $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/include && cp  CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Release-U-32)
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/lib && cp CURRENT/Builds/Release32/zlib.lib $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/include && cp CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Release-DbgMemLeaks-U-32)
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/lib && cp CURRENT/Builds/Release32/zlib.lib $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/include && cp CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Release-Logging-U-32)
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/lib && cp CURRENT/Builds/Release32/zlib.lib $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/include && cp CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Debug-U-64)
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/lib && cp CURRENT/Builds/Debug64/zlib.lib $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/include && cp CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Release-U-64)
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/lib && cp CURRENT/Builds/Release64/zlib.lib $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/include && cp CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Release-Logging-U-64)
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/lib && cp CURRENT/Builds/Release64/zlib.lib $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/include && cp CURRENT/zlib.h CURRENT/zconf.h $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/include/
endif
endif
	@$(ECHO) "done"

	
doBuildInternal_:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	rm -rf $(BUILDS_DIR)
	cd CURRENT && rm -f *.obj *.pdb *.lib
ifeq (U-32,$(findstring U-32,$(CONFIGURATION)))
ifeq (Debug,$(findstring Debug,$(CONFIGURATION)))
	cd CURRENT && env --unset=MAKEFLAGS --unset=MFLAGS sh -c "perl ../../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) \"nmake -f win32/Makefile.msc LOC=\\\"/safeseh /MTd /DZLIB_DEBUG /DASMV /DASMINF\\\" OBJA=\\\"inffas32.obj match686.obj\\\" AS=\\\"$(WIN_AS)\\\" CC=\\\"$(WIN_CC)\\\" AR=\\\"$(WIN_AR)\\\" zlib.lib\""
	mkdir -p $(BUILDS_DIR)Debug32
	mv CURRENT/zlib.lib $(BUILDS_DIR)Debug32/
endif
ifeq (Release,$(findstring Release,$(CONFIGURATION)))
	cd CURRENT && env --unset=MAKEFLAGS --unset=MFLAGS sh -c "perl ../../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) \"nmake -f win32/Makefile.msc LOC=\\\"/safeseh /MT /DASMV /DASMINF\\\" OBJA=\\\"inffas32.obj match686.obj\\\" AS=\\\"$(WIN_AS)\\\" CC=\\\"$(WIN_CC)\\\" AR=\\\"$(WIN_AR)\\\" zlib.lib\""
	mkdir -p $(BUILDS_DIR)Release32
	mv CURRENT/zlib.lib $(BUILDS_DIR)Release32/
endif
endif
ifeq (U-64,$(findstring U-64,$(CONFIGURATION)))
ifeq (Debug,$(findstring Debug,$(CONFIGURATION)))
	cd CURRENT && env --unset=MAKEFLAGS --unset=MFLAGS sh -c "perl ../../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) \"nmake -f win32/Makefile.msc LOC=\\\"/safeseh /MTd /DZLIB_DEBUG  /DASMV /DASMINF /I.\\\" OBJA=\\\"inffasx64.obj gvmat64.obj inffas8664.obj\\\" AS=\\\"$(WIN_AS)\\\" CC=\\\"$(WIN_CC)\\\" AR=\\\"$(WIN_AR)\\\" zlib.lib\""
	mkdir -p $(BUILDS_DIR)Debug64
	mv CURRENT/zlib.lib $(BUILDS_DIR)Debug64/
endif
ifeq (Release,$(findstring Release,$(CONFIGURATION)))
	cd CURRENT && env --unset=MAKEFLAGS --unset=MFLAGS sh -c "perl ../../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) \"nmake -f win32/Makefile.msc LOC=\\\"/safeseh /MT /DASMV /DASMINF /I.\\\" OBJA=\\\"inffasx64.obj gvmat64.obj inffas8664.obj\\\" AS=\\\"$(WIN_AS)\\\" CC=\\\"$(WIN_CC)\\\" AR=\\\"$(WIN_AR)\\\" zlib.lib\""
	mkdir -p $(BUILDS_DIR)Release64
	mv CURRENT/zlib.lib $(BUILDS_DIR)Release64/
endif
endif
else
	@$(MAKE) --directory CURRENT --no-print-directory CC="$(CC)"
endif


ORIGS:	$(ZIPEDFILE)
	@mkdir -p ORIGS && rm -rf ORIGS/*
ifeq ($(shell uname -s),Darwin)
	@tar xf $(ZIPEDFILE) -C ORIGS --strip-components 1
else
	@tar xf $(ZIPEDFILE) --no-same-owner --directory ORIGS --strip-components=1
endif
	@unix2dos ORIGS/win32/Makefile.msc


check-prerequisite-tools:
	@#nothing

check:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Checking ... "
	@for i in $(PRODUCED_OUTPUT_ARTIFACTS) ; do \
		if [ ! -e $$i ]; then\
			$(ECHO) "   FAILED: missing $$i";\
			exit 1;\
		fi;\
	done
	@$(ECHO) " Stroika/ThirdPartyComponents/zlib -    [Succeeded]";


clean:
	@rm -rf CURRENT


clobber:	clean
ifeq ($(CONFIGURATION),)
	@rm -rf $(BUILDS_ROOT)*/ThirdPartyComponents/lib/libz.a $(BUILDS_ROOT)*/ThirdPartyComponents/include/{zlib,zconf}.h
else
	@rm -f $(PRODUCED_OUTPUT_ARTIFACTS)
endif
