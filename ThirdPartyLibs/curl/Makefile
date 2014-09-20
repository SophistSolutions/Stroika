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

BUILDS_DIR=	`pwd`/Builds/


#TODO - fix to take into account DEBUG versus RELEASE! Must build all combos!
all:	$(TRGDIR)

$(ZIPEDFILE):
	wget --quiet --tries=10 --no-check-certificate -O $(ZIPEDFILE) $(FETCHURL) || (rm -f $(ZIPEDFILE) && false)

$(TRGDIR):	$(ZIPEDFILE)
	@echo Creating $(TRGDIR)...
	@tar xf $(ZIPEDFILE) --no-same-owner
	@mv $(EXTRACTED_DIRNAME) $(TRGDIR)

$(BUILDS_DIR):	$(TRGDIR)
	@echo Building $(TRGDIR) to $(BUILDS_DIR)...
ifneq (,$(findstring CYGWIN,$(UNAME)))
	cd CURRENT && cmd /C buildconf.bat > BUILD-CONFIG.OUT
else
	cd CURRENT && ./buildconf > BUILD-CONFIG.OUT
	#@TODO - fix to use my copy of ssl, if we build SSL...
	cd CURRENT && ./configure --prefix=$(BUILDS_DIR) > CONFIGURE.OUT
	make --directory CURRENT --no-print-directory install > MAKE.OUT
endif

all:	$(BUILDS_DIR)
	
check:
	@##THMPHACK
	@echo "ThirdPartyLibs/curl - [Succeeded]"

clean:
	@rm -rf $(TRGDIR)

clobber:
	@rm -rf $(BUILDS_DIR)
	@rm -rf $(TRGDIR)
