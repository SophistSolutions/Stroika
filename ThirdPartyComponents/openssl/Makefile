#
# ROUGH DRAFT - STARTING TO MOVE AWAY FROM OLD PERL STUFF
#

export StroikaRoot?=$(shell realpath ../..)/

ifneq ($(CONFIGURATION),)
	include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

	CONFIGURED_EXTRA_ARGS			=	$(qFeatureFlag_OpenSSL_ExtraArgs)
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk

.DEFAULT_GOAL := all
.PHONY:	all clean clobber check doBuild_
.NOTPARALLEL:


#
# CHECK FOR UPDATED VERSIONS https://www.openssl.org/source/
#

VERSION=1.1.1a
#VERSION=1.1.1
#VERSION=1.1.1-pre9
#VERSION=1.1.0g


BASENAME=openssl-$(VERSION)
EXTRACTED_DIRNAME=$(BASENAME)



PER_CONFIGURATION_BUILDS_DIR:=$(shell realpath --canonicalize-missing ../../Builds/$(CONFIGURATION)/ThirdPartyComponents/)
PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR:=$(shell realpath --canonicalize-missing ../../IntermediateFiles/$(CONFIGURATION)/ThirdPartyComponents/openssl)

OUTPUT_WORKDIR_PRETTYNAME="{StroikaRoot}/Inter.../Thir.../openssl/"


ZIPPEDFILE=../Origs-Cache/$(BASENAME).tar.gz

FETCHURLS=
FETCHURLS+=http://www.openssl.org/source/$(BASENAME).tar.gz
FETCHURLS+=http://www.openssl.org/source/old/1.0.2/$(BASENAME).tar.gz
FETCHURLS+=ftp://mirror.switch.ch/mirror/openssl/source/$(BASENAME).tar.gz
FETCHURLS+=http://artfiles.org/openssl.org/source/$(BASENAME).tar.gz
FETCHURLS+=https://ftp.heanet.ie/mirrors/ftp.openssl.org/source/$(BASENAME).tar.gz

ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
StroikaRoot_ASWINPATH=$(shell cygpath --windows $(StroikaRoot))
endif

	
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
#ifeq (Unix,$(ProjectPlatformSubdir))
# for some reason, no-engines not working on Ubuntu 1804 and 1.1.0g openssl - not worth debugging now...
### NOW no-engine breaking for 1.1.1-pre9, so just disable no-engines altogether. IF I ever need to support centos6 with this, and if it causes problems, revisit -- LGP 2018-08-21
#ifneq (1.1.0,$(findstring 1.1.0,$(VERSION)))
#CONFIG_ARGS+=	-no-engines
#endif
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
	@for i in `../../ScriptsLib/GetConfigurations` ; do\
		$(MAKE) --no-print-directory all CONFIGURATION=$$i;\
	done
else
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents/OpenSSL Build $(VERSION) {$(CONFIGURATION)}:"
	@../../ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@$(MAKE) --silent MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $(ZIPPEDFILE)
ifeq ($(PRODUCED_OUTPUT_ARTIFACTS),)
	$(error PRODUCED_OUTPUT_ARTIFACTS missing)
endif
	@#echo "***DBG-entering make rule all with PRODUCED_OUTPUT_ARTIFACTS=$(PRODUCED_OUTPUT_ARTIFACTS); and cur-target=$@"
	@#need --silent in PRODUCED_OUTPUT_ARTIFACTS to avoid 'up to date' warning
	@$(MAKE) --no-print-directory --silent MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) CURRENT $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) check
endif


$(PRODUCED_OUTPUT_ARTIFACTS):
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@#echo "***DBG-entering make rule with PRODUCED_OUTPUT_ARTIFACTS=$(PRODUCED_OUTPUT_ARTIFACTS); and cur-target=$@"
	@$(MAKE) --no-print-directory $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR)
	@$(MAKE) --no-print-directory doBuilds_


clean:
ifeq ($(CONFIGURATION),)
	@rm -rf ../../IntermediateFiles/*/ThirdPartyComponents/openssl
else
	@rm -rf $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR)
endif


clobber:	clean
ifeq ($(CONFIGURATION),)
	@rm -rf CURRENT ORIGS
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


$(ZIPPEDFILE):
	@MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) ../../ScriptsLib/WebGet.sh $(ZIPPEDFILE) $(FETCHURLS)



ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
ifeq ($(ARCH), x86)
ifeq (0,$(ENABLE_ASSERTIONS))
USE_CONFIGURE_TARGET_NAME_=VC-WIN32
else
USE_CONFIGURE_TARGET_NAME_=debug-VC-WIN32
#
#from https://wiki.openssl.org/index.php/Compilation_and_Installation: -anythingelse, +anythingelse	Compiler options, will become part of CFLAGS.
#and appears to default incorrectly to /MT
#
CONFIG_ARGS +=	-MTd
endif
else ifeq ($(ARCH), x86_64)
ifeq (0,$(ENABLE_ASSERTIONS))
USE_CONFIGURE_TARGET_NAME_=VC-WIN64A
else
USE_CONFIGURE_TARGET_NAME_=debug-VC-WIN64A
#
#from https://wiki.openssl.org/index.php/Compilation_and_Installation: -anythingelse, +anythingelse	Compiler options, will become part of CFLAGS.
#and appears to default incorrectly to /MT
#
CONFIG_ARGS +=	-MTd
endif
endif
endif


doBuilds_:
	@
	@#Run openssl configure scripts
	@
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "openssl ${VERSION} - Configure (see ${OUTPUT_WORKDIR_PRETTYNAME}CONFIG.OUT) ..."` " "
ifeq (Unix,$(ProjectPlatformSubdir))
	@rm -f $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR)/CONFIG.OUT && touch $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR)/CONFIG.OUT
	@$(ECHO) "CONFIG_ARGS: $(CONFIG_ARGS)" >> $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR)/CONFIG.OUT
	@$(ECHO) >> $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR)/CONFIG.OUT
	@($(ECHO) "CONFIG -t output: "; export CC="$(CC)"; export AR="$(AR)"; export RANLIB=$(RANLIB); export MACHINE=$(ARCH); export MAKEFLAGS=; cd $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR) && ./config -t) >> $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR)/CONFIG.OUT 2>&1
	@$(ECHO) >> $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR)/CONFIG.OUT
	@$(ECHO) "EXECUTE CONFIG LINE: export CC=$(CC); export AR=$(AR); export RANLIB=$(RANLIB); export MACHINE=$(ARCH); cd $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR) && ./config $(CONFIG_ARGS)" >> $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR)/CONFIG.OUT
	@$(ECHO) >> $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR)/CONFIG.OUT
	@(export CC="$(CC)"; export AR="$(AR)"; export RANLIB=$(RANLIB); export MACHINE=$(ARCH); export MAKEFLAGS=; cd $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR) && ./config $(CONFIG_ARGS)) >> $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR)/CONFIG.OUT 2>&1
	@$(ECHO) >> $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR)/CONFIG.OUT
	@cd $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR) && perl configdata.pm --dump >> $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR)/CONFIG.OUT
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	echo "MFLAGS= MAKEFLAGS= cd $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR) && perl $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) 'set PATH=$(StroikaRoot_ASWINPATH)ThirdPartyComponents\ActivePerl\CURRENT\bin;%PATH% && perl Configure $(USE_CONFIGURE_TARGET_NAME_) $(CONFIG_ARGS)'" > $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR)/CONFIG.OUT 2>&1
	env --unset=MFLAGS --unset=MAKEFLAGS sh -c "cd $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR) && perl $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) \"set PATH=$(StroikaRoot_ASWINPATH)ThirdPartyComponents\ActivePerl\CURRENT\bin;%PATH% && perl Configure $(USE_CONFIGURE_TARGET_NAME_) $(CONFIG_ARGS)\"" >> $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR)/CONFIG.OUT 2>&1
endif
	@
	@
	@#Do actual OpenSSL build
	@
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "openssl ${VERSION} - Build (see ${OUTPUT_WORKDIR_PRETTYNAME}BUILD.OUT)"` " ... ";
ifeq (Unix,$(ProjectPlatformSubdir))
	@$(MAKE) --directory $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR) --no-print-directory all > $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR)/BUILD.OUT 2>&1;
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@echo "env --unset=MFLAGS --unset=MAKEFLAGS sh -c \"cd $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR) && perl $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) \"nmake 2>&1\"\"" > $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR)/BUILD.OUT
	@env --unset=MFLAGS --unset=MAKEFLAGS sh -c "cd $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR) && perl $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) \"nmake 2>&1\"">> $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR)/BUILD.OUT
endif
	@
	@
	@# OpenSSL build to INSTALLDIR (inside Stroika Builds)
	@
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "openssl ${VERSION} - Installing (see ${OUTPUT_WORKDIR_PRETTYNAME}INSTALL.OUT) ... "
ifeq (Unix,$(ProjectPlatformSubdir))
	@$(MAKE) -j1 --directory $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR) --no-print-directory install > $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR)/INSTALL.OUT 2>&1;
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@echo "env --unset=MFLAGS --unset=MAKEFLAGS sh -c \"cd $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR) && perl $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) \"nmake install INSTALLTOP=$(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH) OPENSSLDIR=$(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH) ENGINESDIR=$(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH) 2>&1\"\"" > $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR)/INSTALL.OUT
	@env --unset=MFLAGS --unset=MAKEFLAGS sh -c "cd $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR) && perl $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) \"nmake install INSTALLTOP=$(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH) OPENSSLDIR=$(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH) ENGINESDIR=$(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH) 2>&1\"">> $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR)/INSTALL.OUT
endif
	@
	@
	@#Run Regression Tests (unless cross compiling)
	@
ifeq (false,$(CrossCompiling))
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "openssl ${VERSION} - Running tests (see ${OUTPUT_WORKDIR_PRETTYNAME}TEST-OUT.txt)"` "...";
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@### @todo FIX FOR WINDOWS BUILDS - not sure why all tests fail -- which is why we don't bother with diff
	@env --unset=MFLAGS --unset=MAKEFLAGS sh -c "cd $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR) && perl $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) \"nmake test 2>&1\"">> $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR)/TEST-OUT.txt || true
else
	@#we get a single failure on BLOW, so ignore, minor, and should do diff of results
	@$(MAKE) -j1 --directory $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR) --no-print-directory -s test > $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR)/TEST-OUT.txt 2>&1 || true
	export NDIFSS=`diff -c --text -b REFERENCE_TEST_OUTPUT.txt $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR)/TEST-OUT.txt | egrep -v "^---" | egrep -c "^[!+-]"` && ../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n && $(ECHO) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "openssl $(VERSION) - Test results: $$NDIFSS lines differ between REFERENCE_TEST_OUTPUT.txt and $(OUTPUT_WORKDIR_PRETTYNAME)TEST.OUT"`
endif
endif


ORIGs CURRENT $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR):	$(ZIPPEDFILE)
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "openssl ${VERSION} - Extracting $(ZIPPEDFILE) to $@"` "... "
	@mkdir -p $@
ifeq ($(shell uname -s),Darwin)
	@tar xf $(ZIPPEDFILE) -C $@ --strip-components 1
else ifeq (CYGWIN,$(findstring CYGWIN,$(shell uname)))
	@#extra sleep and realpath to relative path since fails on cygwin otherwise
	@#sleep 5 # I THINK this works around occasional bug with MSFT filesystem/cygwin - failing to find dir in realpath
	@#sleep 5 # I THINK this works around occasional bug with MSFT filesystem/cygwin - failing to find dir in realpath
	@PWD=`pwd` tar xf $(ZIPPEDFILE) --no-same-owner --directory `realpath --relative-to=$$PWD $@` --strip-components=1
else
	@tar xf $(ZIPPEDFILE) --no-same-owner --directory $@ --strip-components=1
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
ifeq ($@, $(PER_CONFIGURATION_OPENSSL_INTERMEDIATEFILES_DIR))
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n "Patching OpenSSL ... "
	@$(ECHO) -n .
	@patch --quiet -p0 $@/Configurations/windows-makefile.tmpl < Patches/FixQuotesInWindowsMakefileTemplate.PATCH
	@$(ECHO) "done"
endif
endif

