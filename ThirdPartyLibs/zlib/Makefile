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

UNAME := $(shell uname)

ifneq (,$(findstring CYGWIN,$(UNAME)))
WIN_CC32=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl CC_32)")
WIN_CC64=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl CC_64)")
WIN_AS32=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl AS_32)")
WIN_AS64=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl AS_64)")
WIN_LIB32=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl LIB_32)")
WIN_LIB64=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl LIB_64)")
endif

TRGDIR=CURRENT

BUILDS_DIR=	CURRENT/Builds/


#TODO - fix to take into account DEBUG versus RELEASE! Must build all combos!
all:	$(TRGDIR)

$(ZIPEDFILE):
	wget --quiet --tries=10 --no-check-certificate -O $(ZIPEDFILE) $(FETCHURL) || (rm -f $(ZIPEDFILE) && false)

$(TRGDIR):	$(ZIPEDFILE)
	@echo Building $(TRGDIR)...
	@tar xf $(ZIPEDFILE) --no-same-owner
	@mv $(EXTRACTED_DIRNAME) $(TRGDIR)
ifneq (,$(findstring CYGWIN,$(UNAME)))
	@dos2unix -q CURRENT/win32/Makefile.msc
	@patch -p0 CURRENT/win32/Makefile.msc Patches/Makefile.msc.PATCH
	@patch -p0 CURRENT/win32/Makefile.msc Patches/Makefile.msc.Z7.PATCH
	@unix2dos -q CURRENT/win32/Makefile.msc
else
	@(cd CURRENT && ./configure)
	@$(MAKE) --directory CURRENT --no-print-directory
endif

ORIGS:	$(ZIPEDFILE)
	@tar xf $(ZIPEDFILE) --no-same-owner
	@mv $(EXTRACTED_DIRNAME) ORIGS
	@unix2dos ORIGS/win32/Makefile.msc

all:	$(TRGDIR)
ifneq (,$(findstring CYGWIN,$(UNAME)))
	@rm -rf $(BUILDS_DIR)
	@cd $(TRGDIR) && rm -f *.obj *.pdb *.lib
	@perl ../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl "cd $(TRGDIR) && MFLAGS= && MAKEFLAGS= && nmake MAKEFLAGS= MFLAGS= -f win32/Makefile.msc LOC='/MTd -DASMV -DASMINF' OBJA='inffas32.obj match686.obj' AS='$(WIN_AS32)' CC='$(WIN_CC32)' AR='$(WIN_LIB32)' zlib.lib"
	@mkdir -p $(BUILDS_DIR)Debug32
	@mv $(TRGDIR)/zlib.lib $(BUILDS_DIR)Debug32/

	@cd $(TRGDIR) && rm -f *.obj *.pdb *.lib
	@perl ../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl "cd $(TRGDIR) && MFLAGS= && MAKEFLAGS= && nmake MAKEFLAGS= MFLAGS= -f win32/Makefile.msc LOC='/MT -DASMV -DASMINF' OBJA='inffas32.obj match686.obj' AS='$(WIN_AS32)' 'CC=$(WIN_CC32)' 'AR=$(WIN_LIB32)' zlib.lib"
	@mkdir -p $(BUILDS_DIR)Release32
	@mv $(TRGDIR)/zlib.lib $(BUILDS_DIR)Release32
endif
	
check:
	@##THMPHACK
	@echo "ThirdPartyLibs/zlib - [Succeeded]"

clean:
	@$(MAKE) --directory $(TRGDIR) --no-print-directory MAKEFLAGS=$(MAKEFLAGS) clean

clobber:
	@rm -rf $(TRGDIR)
