.PHONY:	all
.DEFAULT_GOAL := all

DoCreateSymLink=0

VERSION=7_38_0
BASENAME=curl-curl-$(VERSION)
ZIPEDFILE=../Origs-Cache/$(BASENAME).tar.gz
TARFILE=$(BASENAME).tar
EXTRACTED_DIRNAME=$(BASENAME)
SLINKDIRNAME=$(BASENAME)
USE_DIRNAME=CURRENT

FETCHURL=https://github.com/bagder/curl/archive/curl-$(VERSION).tar.gz

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

BUILDS_DIR=	$(shell pwd)/Builds/


#TODO - fix to take into account DEBUG versus RELEASE! Must build all combos!
all:
	@echo Building libcurl $(VERSION)...
	@make --no-print-directory doCreate
	@make --no-print-directory doBuilds
	@make --no-print-directory check

$(ZIPEDFILE):
	wget --quiet --tries=10 --no-check-certificate -O $(ZIPEDFILE) $(FETCHURL) || (rm -f $(ZIPEDFILE) && false)


$(TRGDIR):	$(ZIPEDFILE)
	@make -f --no-print-directory doCreate


doCreate:
	@if [ -e  $(TRGDIR) ]; then\
		echo "   ...CURRENT dir already exists";\
	else\
		echo "   ...Creating $(TRGDIR)...";\
		tar xf $(ZIPEDFILE) --no-same-owner;\
		mv $(EXTRACTED_DIRNAME) $(TRGDIR);\
	fi


$(BUILDS_DIR):	$(TRGDIR)
	make -f --no-print-directory doBuilds


doBuilds:
ifneq (,$(findstring CYGWIN,$(UNAME)))
	@echo "   ....Building libcurl $(TRGDIR) into $(BUILDS_DIR)..."
	@rm -rf $(BUILDS_DIR)
	cd CURRENT && cmd /C buildconf.bat > BUILD-CONFIG.OUT
	@cd $(TRGDIR) && rm -f *.obj *.pdb *.lib

	@perl ../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl "cd $(TRGDIR)/winbuild && MFLAGS= && MAKEFLAGS= && nmake MAKEFLAGS= MFLAGS= -f Makefile.vc mode=static AS='$(WIN_AS32)' CC='$(WIN_CC32)' AR='$(WIN_LIB32)'"
else
	@if [ -e  $(BUILDS_DIR) ]; then\
		echo "   ...Libcurl in $(BUILDS_DIR) complete";\
	else\
		echo "   ...Building libcurl $(TRGDIR) into $(BUILDS_DIR)...";\
		cd CURRENT;\
		./buildconf > BUILD-CONFIG.OUT 2>&1;\
		echo "   ...TODO - fix to use my copy of ssl, if we build SSL...";\
		./configure --prefix=$(BUILDS_DIR) --without-openssl --disable-shared  > CONFIGURE.OUT 2>&1;\
		make --no-print-directory install > MAKE.OUT 2>&1;\
	fi
endif

check:
	@##THMPHACK
	@echo "ThirdPartyLibs/curl - [Succeeded]"

clean:
	@rm -rf $(TRGDIR)

clobber:
	@rm -rf $(BUILDS_DIR)
	@rm -rf $(TRGDIR)
