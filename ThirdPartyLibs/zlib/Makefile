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
all:
	@echo Building Stroika/ThirdPartyLibs/zlib $(VERSION)...
	@$(MAKE) --no-print-directory doCreate
	@$(MAKE) --no-print-directory doBuilds
	@$(MAKE) --no-print-directory check

$(ZIPEDFILE):
	@echo -n "   ..." 
	wget --quiet --tries=10 --no-check-certificate -O $(ZIPEDFILE) $(FETCHURL) || (rm -f $(ZIPEDFILE) && false)

$(TRGDIR):	$(ZIPPEDFILE)
	@$(MAKE) --no-print-directory doCreate

doPatch:
	@echo -n "   ...Patching"
ifneq (,$(findstring CYGWIN,$(UNAME)))
	@rm -f PATCHING.OUT
	@dos2unix -q CURRENT/win32/Makefile.msc > PATCHING.OUT
	@patch -p0 CURRENT/win32/Makefile.msc Patches/Makefile.msc.PATCH >> PATCHING.OUT
	@patch -p0 CURRENT/win32/Makefile.msc Patches/Makefile.msc.Z7.PATCH >> PATCHING.OUT
	@unix2dos -q CURRENT/win32/Makefile.msc >> PATCHING.OUT
endif
	@echo "   ...done"

doConfigure:
	@echo -n "   ...Configuring redirecting messages to CONFIGURE.OUT..."
ifeq (,$(findstring CYGWIN,$(UNAME)))
	@(cd CURRENT && ./configure) > CONFIGURE.OUT 2>&1
endif
	@echo "   ...done"

doCreate:	$(ZIPEDFILE)
	@if [ -e  $(TRGDIR) ]; then\
		echo "   ...CURRENT dir already exists";\
	else\
		echo "   ...Extract $(ZIPEDFILE) to $(TRGDIR)...";\
		tar xf $(ZIPEDFILE) --no-same-owner;\
		sleep 1;	# hack cuz sometimes it appears command not fully done writing - and we get sporadic failures on next stop on win7;\
		mv $(EXTRACTED_DIRNAME) $(TRGDIR);\
		$(MAKE) --no-print-directory doPatch;\
		$(MAKE) --no-print-directory doConfigure;\
	fi

doBuilds:
	@if [ -e  BUILD.OUT ]; then\
		echo "   ...BUILD.OUT already exists";\
	else\
		echo "   ...Buidling, and redirecting messages to BUILD.OUT ...";\
		make doBuildInternal_ > BUILD.OUT 2>&1;\
	fi
	
doBuildInternal_:
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
else
	@$(MAKE) --directory CURRENT --no-print-directory
endif


ORIGS:	$(ZIPEDFILE)
	@tar xf $(ZIPEDFILE) --no-same-owner
	@mv $(EXTRACTED_DIRNAME) ORIGS
	@unix2dos ORIGS/win32/Makefile.msc

check:
	@echo -n "   ...Checking..."
ifeq (,$(findstring CYGWIN,$(UNAME)))
	@if [ ! -e CURRENT/libz.a ]; then\
		echo "   FAILED: missing CURRENT/libz.a";\
	else\
		echo "   Stroika/ThirdPartyLibs/zlib - [Succeeded]";\
	fi
else
	@if [ ! -e CURRENT/Builds/Debug32/zlib.lib ]; then\
		echo "   FAILED: missing CURRENT/Builds/Debug32/zlib.lib";\
	elif [ ! -e CURRENT/Builds/Release32/zlib.lib ]; then\
		echo "   FAILED: missing CURRENT/Builds/Release32/zlib.lib";\
	else\
		echo "   Stroika/ThirdPartyLibs/zlib - [Succeeded]";\
	fi
endif

clean:
	@$(MAKE) --directory $(TRGDIR) --no-print-directory MAKEFLAGS=$(MAKEFLAGS) clean

clobber:
	@rm -rf $(TRGDIR)
	@rm -f BUILD.OUT
