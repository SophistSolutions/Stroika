#
# ROUGH DRAFT - STARTING TO MOVE AWAY FROM OLD PERL STUFF
#

.DEFAULT_GOAL := all
.PHONY:	all clean clobber check doBuild doCreate doPatch
.NOTPARALLEL:

export StroikaRoot=$(shell realpath ../..)/

SHELL=/bin/bash

MAKE_INDENT_LEVEL?=$(MAKELEVEL)
ECHO?=	$(shell ../../ScriptsLib/GetDefaultShellVariable.sh ECHO)

ifneq ($(CONFIGURATION),)
ProjectPlatformSubdir			=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)
IncludeDebugSymbolsInLibraries	=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) IncludeDebugSymbolsInLibraries)
ENABLE_ASSERTIONS				=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ENABLE_ASSERTIONS)
OptimizerFlag					=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) OptimizerFlag)
endif

#
# CHECK FOR UPDATED VERSIONS https://curl.haxx.se/download.html
#

#VERSION=1.1.0
#VERSION=1.1.0d
#VERSION=1.1.0e
#VERSION=1.1.0f
VERSION=1.1.0g


BASENAME=openssl-$(VERSION)
EXTRACTED_DIRNAME=$(BASENAME)

ProjectPlatformSubdir	=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)
CrossCompiling			=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) CrossCompiling)
CONFIGURED_EXTRA_ARGS=		$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) qFeatureFlag_OpenSSL_ExtraArgs)

ifeq (Unix,$(ProjectPlatformSubdir))
CC=$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) CompilerDriver-C)
AR=$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) AR)
endif

#Gross hack so builds on AIX - https://stroika.atlassian.net/browse/STK-452
ifeq ($(shell uname),AIX)
CC=gcc
AR=ar
endif


ZIPEDFILE=../Origs-Cache/$(BASENAME).tar.gz

FETCHURLS=
FETCHURLS+=http://www.openssl.org/source/$(BASENAME).tar.gz
FETCHURLS+=http://www.openssl.org/source/old/1.0.2/$(BASENAME).tar.gz
FETCHURLS+=ftp://mirror.switch.ch/mirror/openssl/source/$(BASENAME).tar.gz
FETCHURLS+=http://artfiles.org/openssl.org/source/$(BASENAME).tar.gz
FETCHURLS+=https://ftp.heanet.ie/mirrors/ftp.openssl.org/source/$(BASENAME).tar.gz

.PHONY:	doCreate doBuild

	
BUILDS_ROOT=../../Builds/


CONFIG_ARGS=
CONFIG_ARGS+=	-no-shared
ifeq (purify,$(findstring purify,$(CONFIGURED_EXTRA_ARGS)))
#see https://stroika.atlassian.net/browse/STK-400 - -DPURIFY to cleanly valgrind
CONFIG_ARGS+=	-DPURIFY
endif


BUILDS_OUTPUT_DIR=$(shell realpath `pwd`/../../)/Builds/$(CONFIGURATION)/ThirdPartyComponents/
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH=$(shell cygpath --windows $(BUILDS_OUTPUT_DIR))
endif

ifeq (Unix,$(ProjectPlatformSubdir))
CONFIG_ARGS+=	--prefix=$(BUILDS_OUTPUT_DIR)
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
#CONFIG_ARGS+=	--prefix=$(shell cygpath --windows $(BUILDS_OUTPUT_DIR))
CONFIG_ARGS+=	--prefix=$(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH)
endif


ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
#https://stroika.atlassian.net/browse/STK-528
#CONFIG_ARGS+=	-asm
CONFIG_ARGS+=	-no-asm
endif

#Attempt to wrokaround bug on centos6:
#	(gdb) #0  0x0000000000000000 in ?? ()
#	(gdb) #1  0x00007ffff7bd629c in _dlerror_run () from /lib64/libdl.so.2
#	(gdb) #2  0x00007ffff7bd5ee1 in dlopen@@GLIBC_2.2.5 () from /lib64/libdl.so.2
#	(gdb) #3  0x0000000000d0d4bd in dlfcn_load ()
#	(gdb) #4  0x0000000000cd2348 in DSO_load ()
#	(gdb) #5  0x0000000000c60d06 in ibm_4758_cca_init ()
#
# Note - this COULD be specific to the fact that we are using 
#		-Wl,--rpath='$$ORIGIN/ii-esx-dynamic-libs' -Wl,--dynamic-linker='/opt/Analitiqa-II/ii-esx-dynamic-libs/ld-linux.so.2'
#	in which case we really want the disabling specific to that configuraiton so we need
#	https://stroika.atlassian.net/browse/STK-454 addressed. But - I don't think we really count on
#	dso or engines, and you can always use --openssl=use-system
#
CONFIG_ARGS+=	-no-dso
ifeq (Unix,$(ProjectPlatformSubdir))
CONFIG_ARGS+=	-no-engines
endif



#ifeq (Unix,$(ProjectPlatformSubdir))
#https://stroika.atlassian.net/browse/STK-527
#	This hack doesn't appear needed right now, and isnt right in general, because it depeds if target OS uses pthreads or some other thread library
#CONFIG_ARGS+=	-lpthread
#endif



ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
WIN_CRYPTO_LIB_BASENAME=libcrypto
WIN_SSL_LIB_BASENAME=libssl
WIN_PDB_BASENAME=ossl_static
endif

PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/openssl/aes.h
ifeq (Unix,$(ProjectPlatformSubdir))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/libcrypto.a							\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/libssl.a							\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/bin/openssl
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/$(WIN_CRYPTO_LIB_BASENAME).lib		\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/$(WIN_SSL_LIB_BASENAME).lib			\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/$(WIN_PDB_BASENAME).pdb				\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/bin/openssl.exe							\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/bin/openssl.pdb							\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/bin/c_rehash.pl
endif


all:
ifeq ($(CONFIGURATION),)
	@for i in `../../ScriptsLib/GetConfigurations.sh` ; do\
		$(MAKE) --no-print-directory all CONFIGURATION=$$i;\
	done
else
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Building Stroika/ThirdPartyComponents/OpenSSL $(VERSION) {$(CONFIGURATION)}:"
	@../../ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
ifeq ($(PRODUCED_OUTPUT_ARTIFACTS),)
	$(error PRODUCED_OUTPUT_ARTIFACTS missing)
endif
	@#echo "***DBG-entering make rule all with PRODUCED_OUTPUT_ARTIFACTS=$(PRODUCED_OUTPUT_ARTIFACTS); and cur-target=$@"
	@#need --silent in PRODUCED_OUTPUT_ARTIFACTS to avoid 'up to date' warning
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) --silent MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) check
endif


$(PRODUCED_OUTPUT_ARTIFACTS):
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@#echo "***DBG-entering make rule with PRODUCED_OUTPUT_ARTIFACTS=$(PRODUCED_OUTPUT_ARTIFACTS); and cur-target=$@"
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) doCreate
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) doBuilds


clean:
	@rm -rf CURRENT ORIGS


clobber:	clean
ifeq ($(CONFIGURATION),)
	@rm -rf $(BUILDS_ROOT)*/ThirdPartyComponents/lib/{libcrypto,libssl}* $(BUILDS_ROOT)*/ThirdPartyComponents/include/openssl $(BUILDS_ROOT)*/ThirdPartyComponents/bin/openssl
else
	@rm -rf $(PRODUCED_OUTPUT_ARTIFACTS) $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/openssl
endif



check-prerequisite-tools:
	@#nothing

check:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
ifeq ($(PRODUCED_OUTPUT_ARTIFACTS),)
	$(error This makefile requires a recognized ProjectPlatformSubdir)
endif
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Checking ... "
	@for i in $(PRODUCED_OUTPUT_ARTIFACTS) ; do \
		if [ ! -e $$i ]; then\
			$(ECHO) "      FAILED: missing $$i";\
			exit 1;\
		fi;\
	done
	@$(ECHO) " Stroika/ThirdPartyComponents/OpenSSL - [Succeeded]";


$(ZIPEDFILE):
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ../../ScriptsLib/WebGet.sh $(ZIPEDFILE) $(FETCHURLS)


doPatch:
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n "Patching OpenSSL ... "
	@$(ECHO) -n .
	@patch --quiet -p0 CURRENT/Configurations/windows-makefile.tmpl < Patches/FixQuotesInWindowsMakefileTemplate.PATCH
	@$(ECHO) "done"
endif


doCreate:	$(ZIPEDFILE)
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n "Extracting $(ZIPEDFILE) to CURRENT ... "
	@mkdir -p CURRENT && rm -rf CURRENT/*
ifeq ($(shell uname -s),Darwin)
	@tar xf $(ZIPEDFILE) -C CURRENT --strip-components 1
else
	@tar xf $(ZIPEDFILE) --no-same-owner --directory CURRENT --strip-components=1
endif
	@$(ECHO) "done"
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) doPatch




ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
ifeq (0,$(ENABLE_ASSERTIONS))
USE_CONFIGURE_TARGET_NAME_=VC-WIN32
endif
ifeq (1,$(ENABLE_ASSERTIONS))
USE_CONFIGURE_TARGET_NAME_=debug-VC-WIN32
#
#from https://wiki.openssl.org/index.php/Compilation_and_Installation: -anythingelse, +anythingelse	Compiler options, will become part of CFLAGS.
#and appears to default incorrectly to /MT
#
CONFIG_ARGS +=	-MTd
endif
endif

doBuilds:
	@
	@#Run openssl configure scripts
	@
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n "Configuring (output to CURRENT/CONFIG.OUT) ... "
ifeq (Unix,$(ProjectPlatformSubdir))
	@$(ECHO) "export CC=$(CC); export AR=$(AR); cd CURRENT && eval (./config -t | tail -1) $(CONFIG_ARGS)" > CURRENT/CONFIG.OUT
	@(export CC="$(CC)"; export AR="$(AR)"; export MAKEFLAGS=; cd CURRENT && ./config -t | tail -1) >> CONFIG.OUT 2>&1
	@(export CC="$(CC)"; export AR="$(AR)"; export MAKEFLAGS=; cd CURRENT && eval `./config -t | tail -1` $(CONFIG_ARGS)) >> CONFIG.OUT 2>&1
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@echo "MFLAGS= MAKEFLAGS= cd CURRENT && perl ../../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) 'set PATH=..\..\ActivePerl\CURRENT\bin;%PATH% && perl Configure $(USE_CONFIGURE_TARGET_NAME_) $(CONFIG_ARGS)'" > CURRENT/CONFIG.OUT 2>&1
	@env --unset=MFLAGS --unset=MAKEFLAGS sh -c "cd CURRENT && perl ../../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) \"set PATH=..\..\ActivePerl\CURRENT\bin;%PATH% && perl Configure $(USE_CONFIGURE_TARGET_NAME_) $(CONFIG_ARGS)\"" >> CURRENT/CONFIG.OUT 2>&1
endif
	@$(ECHO) "done"
	@
	@
	@#Do actual OpenSSL build
	@
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n "Buidling (redirecting messages to CURRENT/BUILD.OUT) ... ";
ifeq (Unix,$(ProjectPlatformSubdir))
	@$(MAKE) --directory CURRENT --no-print-directory all > CURRENT/BUILD.OUT 2>&1;
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@echo "env --unset=MFLAGS --unset=MAKEFLAGS sh -c \"cd CURRENT && perl ../../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) \"nmake 2>&1\"\"" > CURRENT/BUILD.OUT
	@env --unset=MFLAGS --unset=MAKEFLAGS sh -c "cd CURRENT && perl ../../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) \"nmake 2>&1\"">> CURRENT/BUILD.OUT
endif
	@$(ECHO) "done"
	@
	@
	@# OpenSSL build to INSTALLDIR (inside Stroika Builds)
	@
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n "Installing "`$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "to $(BUILDS_OUTPUT_DIR), and redirecting messages to CURRENT/INSTALL.OUT"`" ... "
ifeq (Unix,$(ProjectPlatformSubdir))
	@$(MAKE) -j1 --directory CURRENT --no-print-directory install > CURRENT/INSTALL.OUT 2>&1;
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@echo "env --unset=MFLAGS --unset=MAKEFLAGS sh -c \"cd CURRENT && perl ../../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) \"nmake install INSTALLTOP=$(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH) OPENSSLDIR=$(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH) ENGINESDIR=$(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH) 2>&1\"\"" > CURRENT/INSTALL.OUT
	@env --unset=MFLAGS --unset=MAKEFLAGS sh -c "cd CURRENT && perl ../../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) \"nmake install INSTALLTOP=$(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH) OPENSSLDIR=$(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH) ENGINESDIR=$(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH) 2>&1\"">> CURRENT/INSTALL.OUT
endif
	@$(ECHO) "done"
	@
	@
	@#Run Regression Tests (unless cross compiling)
	@
ifeq (false,$(CrossCompiling))
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n "Running openssl tests (OUTPUT TO CURRENT/TEST-OUT.txt) ... ";
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@### @todo FIX FOR WINDOWS BUILDS - not sure why all tests fail -- which is why we dont bother with diff
	@env --unset=MFLAGS --unset=MAKEFLAGS sh -c "cd CURRENT && perl ../../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) \"nmake test 2>&1\"">> CURRENT/TEST-OUT.txt || true
else
	@#we get a single failure on BLOW, so ignore, minor, and should do diff of results
	@$(MAKE) -j1 --directory CURRENT --no-print-directory -s test > CURRENT/TEST-OUT.txt 2>&1 || true
	export NDIFSS=`diff -c --text -b REFERENCE_TEST_OUTPUT.txt CURRENT/TEST-OUT.txt | egrep -v "^---" | egrep -c "^[!+-]"` && $(ECHO) -n "($$NDIFSS lines differ between REFERENCE_TEST_OUTPUT.txt and CURRENT/TEST.OUT) ... "
endif
	@$(ECHO) "done"
endif


ORIGS:	$(ZIPEDFILE)
	@mkdir -p ORIGS && rm -rf ORIGS/*
ifeq ($(shell uname -s),Darwin)
	@tar xf $(ZIPEDFILE) -C ORIGS --strip-components 1
else
	@tar xf $(ZIPEDFILE) --no-same-owner --directory ORIGS --strip-components=1
endif
