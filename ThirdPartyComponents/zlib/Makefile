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
WIN_CC32=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) CC_32)")
WIN_CC64=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) CC_64)")
WIN_AS32=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) AS_32)")
WIN_AS64=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) AS_64)")
WIN_LIB32=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) LIB_32)")
WIN_LIB64=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) LIB_64)")
endif

ifeq (Unix,$(ProjectPlatformSubdir))
CC=$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) CompilerDriver-C)
endif

BUILDS_ROOT=../../Builds/

MAKE_INDENT_LEVEL?=$(MAKELEVEL)


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
	@../../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && echo "Building Stroika/ThirdPartyComponents/zlib $(VERSION):"
	@$(MAKE) --no-print-directory --silent CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) check


$(PRODUCED_OUTPUT_ARTIFACTS):
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)  doCreate
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)  doBuilds


$(ZIPEDFILE):
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo -n "..." 
	@../../ScriptsLib/WebGet.sh $(ZIPEDFILE) $(FETCHURLS)


doPatch:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo -n "...Patching"
ifneq (,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	@rm -f PATCHING.OUT
	@dos2unix -q CURRENT/win32/Makefile.msc > PATCHING.OUT
	@patch -p0 CURRENT/win32/Makefile.msc Patches/Makefile.msc.PATCH >> PATCHING.OUT
	@patch -p0 CURRENT/win32/Makefile.msc Patches/Makefile.msc.Z7.PATCH >> PATCHING.OUT
	@unix2dos -q CURRENT/win32/Makefile.msc >> PATCHING.OUT
endif
	@echo "   ...done"


doConfigure:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo -n "...Configuring redirecting messages to CONFIGURE.OUT..."
ifeq (Unix,$(ProjectPlatformSubdir))
	@(CC="$(CC)" cd CURRENT && ./configure) > CONFIGURE.OUT 2>&1
endif
	@echo "   ...done"


doCreate:	$(ZIPEDFILE)
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo "...Extract $(ZIPEDFILE) to CURRENT..."
	@rm -rf CURRENT
	@tar xf $(ZIPEDFILE) --no-same-owner
	@mv $(EXTRACTED_DIRNAME) CURRENT
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)  doPatch
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)  doConfigure


doBuilds:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo "...Buidling, and redirecting messages to BUILD.OUT ..."
	@make doBuildInternal_ > BUILD.OUT 2>&1
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo "...Copying builds to ... $(BUILDS_ROOT)"
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

	
doBuildInternal_:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
ifeq (U-32,$(findstring U-32,$(CONFIGURATION)))
ifeq (Debug,$(findstring Debug,$(CONFIGURATION)))
	@rm -rf $(BUILDS_DIR)
	@cd CURRENT && rm -f *.obj *.pdb *.lib
	@perl ../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) "cd CURRENT && MFLAGS= && MAKEFLAGS= && nmake MAKEFLAGS= MFLAGS= -f win32/Makefile.msc LOC='/safeseh /MTd -DASMV -DASMINF' OBJA='inffas32.obj match686.obj' AS='$(WIN_AS32)' CC='$(WIN_CC32)' AR='$(WIN_LIB32)' zlib.lib"
	@mkdir -p $(BUILDS_DIR)Debug32
	@mv CURRENT/zlib.lib $(BUILDS_DIR)Debug32/
endif
ifeq (Release,$(findstring Release,$(CONFIGURATION)))
	@cd CURRENT && rm -f *.obj *.pdb *.lib
	@perl ../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) "cd CURRENT && MFLAGS= && MAKEFLAGS= && nmake MAKEFLAGS= MFLAGS= -f win32/Makefile.msc LOC='/safeseh /MT -DASMV -DASMINF' OBJA='inffas32.obj match686.obj' AS='$(WIN_AS32)' 'CC=$(WIN_CC32)' 'AR=$(WIN_LIB32)' zlib.lib"
	@mkdir -p $(BUILDS_DIR)Release32
	@mv CURRENT/zlib.lib $(BUILDS_DIR)Release32/
endif
endif
ifeq (U-64,$(findstring U-64,$(CONFIGURATION)))
ifeq (Debug,$(findstring Debug,$(CONFIGURATION)))
	@cd CURRENT && rm -f *.obj *.pdb *.lib
	@perl ../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) "cd CURRENT && MFLAGS= && MAKEFLAGS= && nmake MAKEFLAGS= MFLAGS= -f win32/Makefile.msc LOC='/MTd -DASMV -DASMINF -I.' OBJA='inffasx64.obj gvmat64.obj inffas8664.obj' AS='$(WIN_AS64)' CC='$(WIN_CC64)' AR='$(WIN_LIB64)' zlib.lib"
	@mkdir -p $(BUILDS_DIR)Debug64
	@mv CURRENT/zlib.lib $(BUILDS_DIR)Debug64/
endif
ifeq (Release,$(findstring Release,$(CONFIGURATION)))
	@cd CURRENT && rm -f *.obj *.pdb *.lib
	@perl ../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) "cd CURRENT && MFLAGS= && MAKEFLAGS= && nmake MAKEFLAGS= MFLAGS= -f win32/Makefile.msc LOC='/MT -DASMV -DASMINF -I.' OBJA='inffasx64.obj gvmat64.obj inffas8664.obj' AS='$(WIN_AS64)' 'CC=$(WIN_CC64)' 'AR=$(WIN_LIB64)' zlib.lib"
	@mkdir -p $(BUILDS_DIR)Release64
	@mv CURRENT/zlib.lib $(BUILDS_DIR)Release64/
endif
endif
else
	@$(MAKE) --directory CURRENT --no-print-directory CC="$(CC)"
endif


ORIGS:	$(ZIPEDFILE)
	@tar xf $(ZIPEDFILE) --no-same-owner
	@mv $(EXTRACTED_DIRNAME) ORIGS
	@unix2dos ORIGS/win32/Makefile.msc


check:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@../../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && echo -n "...Checking..."
	@for i in $(PRODUCED_OUTPUT_ARTIFACTS) ; do \
		if [ ! -e $$i ]; then\
			echo "   FAILED: missing $$i";\
			exit 1;\
		fi;\
	done
	@echo "Stroika/ThirdPartyComponents/zlib -    [Succeeded]";


clean:
	@rm -rf CURRENT


clobber:	clean
	@rm -f BUILD.OUT PATCHING.OUT
ifeq ($(CONFIGURATION),)
	@rm -rf $(BUILDS_ROOT)*/ThirdPartyComponents/lib/libz.a $(BUILDS_ROOT)*/ThirdPartyComponents/include/{zlib,zconf}.h
else
	@rm -f $(PRODUCED_OUTPUT_ARTIFACTS)
endif
