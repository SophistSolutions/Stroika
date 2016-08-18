#
# ROUGH DRAFT - STARTING TO MOVE AWAY FROM OLD PERL STUFF
#

.DEFAULT_GOAL := all
.PHONY:	all clean clobber check doBuild doCreate doPatch
.NOTPARALLEL:

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

#VERSION=1.0.2g
VERSION=1.0.2h

#https://www.openssl.org/source/openssl-1.1.0-pre5.tar.gz
#TRIED - but build on windows not working yet (wait for next release). Build works on UNIX but then I run into Stroika incompat with some
# changes in openssl (so will require work)
#VERSION=1.1.0-pre6

#appears needed for 1.0.2d
ifeq ($(VERSION),1.0.2d)
BAD_TARFILE_BUG_WORKAROUND_=i
endif

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


ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
ifeq (1.0.,$(findstring 1.0.,$(VERSION)))
WIN_CRYPTO_LIB_BASENAME=libeay32
WIN_SSL_LIB_BASENAME=ssleay32
WIN_PDB_BASENAME=lib
endif
ifneq  (1.0.,$(findstring 1.0.,$(VERSION)))
WIN_CRYPTO_LIB_BASENAME=libcrypto
WIN_SSL_LIB_BASENAME=libssl
WIN_PDB_BASENAME=ossl_static
endif
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

# COULD make this conditional
### DISABLE TEMPORARILY TO MAKE SURE NOT A SOURCE OF BUGS WITH upgrade to 1.1 (currently hard)
#STRIP_ANNOYING_MESSAGES=		1
STRIP_ANNOYING_MESSAGES=		0

all:
	@../../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) "Building Stroika/ThirdPartyComponents/OpenSSL $(VERSION):"
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
ifeq ($(PRODUCED_OUTPUT_ARTIFACTS),)
	$(error PRODUCED_OUTPUT_ARTIFACTS missing)
endif
	@#echo "***DBG-entering make rule all with PRODUCED_OUTPUT_ARTIFACTS=$(PRODUCED_OUTPUT_ARTIFACTS); and cur-target=$@"
	@#need --silent in PRODUCED_OUTPUT_ARTIFACTS to avoid 'up to date' warning
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) --silent MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) check


$(PRODUCED_OUTPUT_ARTIFACTS):
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@#echo "***DBG-entering make rule with PRODUCED_OUTPUT_ARTIFACTS=$(PRODUCED_OUTPUT_ARTIFACTS); and cur-target=$@"
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) doCreate
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) doBuilds
	@#echo done; sleep 10;


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
	@../../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && $(ECHO) -n "Checking..."
	@for i in $(PRODUCED_OUTPUT_ARTIFACTS) ; do \
		if [ ! -e $$i ]; then\
			$(ECHO) "      FAILED: missing $$i";\
			exit 1;\
		fi;\
	done
	@$(ECHO) " Stroika/ThirdPartyComponents/OpenSSL - [Succeeded]";


$(ZIPEDFILE):
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1))
	@../../ScriptsLib/WebGet.sh $(ZIPEDFILE) $(FETCHURLS)


doPatch:
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo -n "Patching OpenSSL..."
ifeq (1,$(STRIP_ANNOYING_MESSAGES))
	@$(ECHO) -n .
	@patch --quiet -p0 CURRENT/util/copy.pl < Patches/LosePRINTOfCopyingInUtils.PATCH
endif
ifeq  (1.1.,$(findstring 1.1.,$(VERSION)))
	@$(ECHO) -n .
	@patch --quiet -p0 CURRENT/Configurations/windows-makefile.tmpl < Patches/FixQuotesInWindowsMakefileTemplate.PATCH
endif
	@$(ECHO) "done"
endif


doCreate:	$(ZIPEDFILE)
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && $(ECHO) -n "Extracting $(ZIPEDFILE) to CURRENT..."
	@rm -rf CURRENT
	@tar $(BAD_TARFILE_BUG_WORKAROUND_)xf $(ZIPEDFILE) --no-same-owner
	@
	@#windows quirks-- Sigh... --LGP 2016-08-17
	@sleep 1 && rm -rf CURRENT
	@
	@mv $(EXTRACTED_DIRNAME) CURRENT;
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
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && $(ECHO) -n "Configuring (output to CURRENT/CONFIG.OUT)..."
ifeq (Unix,$(ProjectPlatformSubdir))
	@$(ECHO) "export CC=$(CC); export AR=$(AR); cd CURRENT && eval (./config -t | tail -1) $(CONFIG_ARGS)" > CURRENT/CONFIG.OUT
	@export CC="$(CC)"; export AR="$(AR)"; export MAKEFLAGS=; cd CURRENT && ./config -t | tail -1 >> CONFIG.OUT
	@export CC="$(CC)"; export AR="$(AR)"; export MAKEFLAGS=; cd CURRENT && eval `./config -t | tail -1` $(CONFIG_ARGS) >> CONFIG.OUT;
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@echo "MFLAGS= MAKEFLAGS= cd CURRENT && perl ../../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) 'perl Configure $(USE_CONFIGURE_TARGET_NAME_) $(CONFIG_ARGS)'" > CURRENT/CONFIG.OUT
	@env --unset=MFLAGS --unset=MAKEFLAGS sh -c "cd CURRENT && perl ../../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) \"perl Configure $(USE_CONFIGURE_TARGET_NAME_) $(CONFIG_ARGS)\"" >> CURRENT/CONFIG.OUT
	@#MUST FIX THIS - ESP BIT ABOUT CDRIVE DEPENDENCY
	@echo "cat makefile | sed 's/\/usr\/bin\/perl/perl/g' > m1 && mv m1 makefile" >> CURRENT/CONFIG.OUT
	@cat CURRENT/makefile | sed 's/\/usr\/bin\/perl/perl/g' > m1 && mv m1 CURRENT/makefile >> CURRENT/CONFIG.OUT
	@echo "cat makefile | sed 's/\/cygdrive\/c/c:\//g' > m1 && mv m1 makefile" >> CURRENT/CONFIG.OUT
	@cat CURRENT/makefile | sed 's/\/cygdrive\/c/c:\//g' > m1 && mv m1 CURRENT/makefile >> CURRENT/CONFIG.OUT
endif
	@$(ECHO) "done"
	@
	@
	@#Do actual OpenSSL build
	@
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && $(ECHO) -n "Buidling (redirecting messages to CURRENT/BUILD.OUT)...";
ifeq (Unix,$(ProjectPlatformSubdir))
	@$(MAKE) -j1 --directory CURRENT --no-print-directory all > CURRENT/BUILD.OUT 2>&1;
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
ifneq  (1.0.,$(findstring 1.0.,$(VERSION)))
	@echo "env --unset=MFLAGS --unset=MAKEFLAGS sh -c \"cd CURRENT && perl ../../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) \"nmake 2>&1\"\"" > CURRENT/BUILD.OUT
	@env --unset=MFLAGS --unset=MAKEFLAGS sh -c "cd CURRENT && perl ../../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) \"nmake 2>&1\"">> CURRENT/BUILD.OUT
endif
endif
	@$(ECHO) "done"
	@
	@
	@# OpenSSL build to INSTALLDIR (inside Stroika Builds)
	@
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && $(ECHO) -n "Installing (to $(BUILDS_OUTPUT_DIR)), and redirecting messages to CURRENT/INSTALL.OUT ...";
ifeq (Unix,$(ProjectPlatformSubdir))
	@$(MAKE) -j1 --directory CURRENT --no-print-directory install > CURRENT/INSTALL.OUT 2>&1;
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
ifneq  (1.0.,$(findstring 1.0.,$(VERSION)))
	@### use install_dev cuz thats all that works - install fails (still to debug)
	@echo "env --unset=MFLAGS --unset=MAKEFLAGS sh -c \"cd CURRENT && perl ../../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) \"nmake install INSTALLTOP=$(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH) 2>&1\"\"" > CURRENT/INSTALL.OUT
	@env --unset=MFLAGS --unset=MAKEFLAGS sh -c "cd CURRENT && perl ../../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) \"nmake install INSTALLTOP=$(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH) 2>&1\"">> CURRENT/INSTALL.OUT
	@###HACK -workaround install bug??? --LGP 2016-08-15
	@rm -rf $(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH)/lib/engines-1_1 
	@rm -f $(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH)/ThirdPartyComponents/bin/app
endif
endif
	@$(ECHO) "done"
	@
	@#OpenSSL 1.0 VS support (using buildall_windows_.pl)  - LEGACY
	@
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
ifeq  (1.0.,$(findstring 1.0.,$(VERSION)))
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && $(ECHO) -n "Buidling and testing (redirecting messages to CURRENT/BUILD.OUT)...";
	@CONFIGURATION=$(CONFIGURATION) ./buildall_windows_.pl > CURRENT/BUILD.OUT 2>&1;
	@$(ECHO) "done"
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && $(ECHO) -n "Copying builds to $(BUILDS_ROOT) ..."
ifeq ($(CONFIGURATION),Debug-U-32)
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/bin && cp CURRENT/Builds/Debug32/openssl.* $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/bin/
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/lib && cp -r CURRENT/Builds/Debug32/libeay32.* CURRENT/Builds/Debug32/ssleay32.* CURRENT/Builds/Debug32/lib.pdb $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/include && cp -r --dereference CURRENT/Builds/Debug32/Includes/openssl $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Release-U-32)
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/bin && cp CURRENT/Builds/Release32/openssl.* $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/bin/
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/lib && cp -r CURRENT/Builds/Release32/libeay32.* CURRENT/Builds/Release32/ssleay32.* CURRENT/Builds/Release32/lib.pdb $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/include && cp -r --dereference CURRENT/Builds/Release32/Includes/openssl $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Release-DbgMemLeaks-U-32)
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/bin && cp CURRENT/Builds/Release32/openssl.* $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/bin/
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/lib && cp -r CURRENT/Builds/Release32/libeay32.* CURRENT/Builds/Release32/ssleay32.* CURRENT/Builds/Release32/lib.pdb $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/include && cp -r --dereference CURRENT/Builds/Release32/Includes/openssl $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Release-Logging-U-32)
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/bin && cp CURRENT/Builds/Release32/openssl.* $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/bin/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/lib && cp -r CURRENT/Builds/Release32/libeay32.* CURRENT/Builds/Release32/ssleay32.* CURRENT/Builds/Release32/lib.pdb $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/include && cp -r --dereference CURRENT/Builds/Release32/Includes/openssl $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Debug-U-64)
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/bin && cp CURRENT/Builds/Debug64/openssl.* $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/bin/
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/lib && cp -r CURRENT/Builds/Debug64/libeay32.* CURRENT/Builds/Debug64/ssleay32.* CURRENT/Builds/Debug64/lib.pdb $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/include && cp -r --dereference CURRENT/Builds/Debug64/Includes/openssl $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Release-U-64)
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/bin && cp CURRENT/Builds/Release64/openssl.* $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/bin/
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/lib && cp -r CURRENT/Builds/Release64/libeay32.* CURRENT/Builds/Release64/ssleay32.* CURRENT/Builds/Release64/lib.pdb $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/include && cp -r --dereference CURRENT/Builds/Release64/Includes/openssl $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Release-Logging-U-64)
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/bin && cp CURRENT/Builds/Release64/openssl.* $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/bin/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/lib && cp -r CURRENT/Builds/Release64/libeay32.* CURRENT/Builds/Release64/ssleay32.* CURRENT/Builds/Release64/lib.pdb $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/include && cp -r --dereference CURRENT/Builds/Release64/Includes/openssl $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/include/
endif
	@#irrelevant tmp hack for 1.0 openssl compat
	@touch $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/bin/c_rehash.pl
endif
endif
	@
	@#Run Regression Tests (unless cross compiling)
	@
ifeq (false,$(CrossCompiling))
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && $(ECHO) -n "Running openssl tests (OUTPUT TO CURRENT/TEST-OUT.txt)...";
ifneq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@### FIX FOR WINDOWS BUILDS
	@### DONT THINK WE NEED REHASH ANYMORE??? --LGP 2016-08-14
	@($(MAKE) --directory CURRENT --no-print-directory -s rehash 2>&1) > /dev/null || true;
	@($(MAKE) -j1 --directory CURRENT --no-print-directory -s test > CURRENT/TEST-OUT.txt 2>&1);
endif
	@$(ECHO) "done"
endif


ORIGS:	$(ZIPEDFILE)
	@tar $(BAD_TARFILE_BUG_WORKAROUND_)xf $(ZIPEDFILE) --no-same-owner
	@mv $(EXTRACTED_DIRNAME) ORIGS
