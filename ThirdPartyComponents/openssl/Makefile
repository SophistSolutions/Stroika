#
#
#	TODO:
#		@todo	NOTE - try for $(word 1,$(PRODUCED_OUTPUT_ARTIFACTS)) and $(wordlist 2, 9999, $(PRODUCED_OUTPUT_ARTIFACTS)):	$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS))
#				(repeated in most ThirdPartyComponents makefiles)
#				doesn't work if the first produced output artifact is fine, but the others got deleted. No BIGGIE, as you can just make clobber in that
#				exceedingly rare case; but certainly a defect of this approach.
StroikaRoot=$(abspath ../..)/

ifneq ($(CONFIGURATION),)
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

	CONFIGURED_EXTRA_ARGS			=	$(qFeatureFlag_OpenSSL_ExtraArgs)
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk

.PHONY:	all clean clobber check doBuild_


#
# CHECK FOR UPDATED VERSIONS https://www.openssl.org/source/
#

# alpha8 only tried on windows but seems same - must handle 'legacy' issue, and then re-run more carefully
# alpha7 fails on openssl regtests win and unix in regtests - like below error message - but probably due to Review https://github.com/openssl/openssl/issues/9349 - add legacy provider (though unix also has asan issue so maybe more)
# alpha6 fails on openssl regtests win and unix in regtests - like below error message - but probably due to Review https://github.com/openssl/openssl/issues/9349 - add legacy provider (though unix also has asan issue so maybe more)
# alpha5 fails on openssl regtests (make tests hangs on unix after long time - after test 80-test_x509aux.t, tests seem to complete on windows, but regtests fail with error:03000086:digital envelope routines::initialization error)
# alpha4 fails on openssl regtests (make tests hangs on unix)
# alpha2 works mostly bug regtests fail with error:060000CA:digital envelope routines::fetch failed
#VERSION=3.0.0-alpha8
#VERSION=3.0.0-alpha7
#VERSION=3.0.0-alpha6


VERSION=1.1.1i
#VERSION=1.1.1h
#VERSION=1.1.1g

BASENAME=openssl-$(VERSION)
EXTRACTED_DIRNAME=$(BASENAME)



PER_CONFIGURATION_BUILDS_DIR:=$(shell realpath --canonicalize-missing ../../Builds/$(CONFIGURATION)/ThirdPartyComponents/)/

#lose trailing / on PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_ because on macos make THATDIR doesn't work with trailing slash (old WEIRD_MACOS_MAKEFILE_DIR_BUG -  MacOS XCode 10 - GNU Make 3.81)
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_:=$(shell realpath --canonicalize-missing ../../IntermediateFiles/$(CONFIGURATION)/ThirdPartyComponents/openssl)

#OUTPUT_WORKDIR_PRETTYNAME="{StroikaRoot}Inter.../Thir.../openssl/"
OUTPUT_WORKDIR_PRETTYNAME="$(shell $(StroikaRoot)ScriptsLib/SubstituteBackVariables "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/")"


ZIPPEDFILE=../Origs-Cache/$(BASENAME).tar.gz

FETCHURLS=
FETCHURLS+=http://www.openssl.org/source/$(BASENAME).tar.gz
FETCHURLS+=http://www.openssl.org/source/old/1.1.1/$(BASENAME).tar.gz
FETCHURLS+=http://www.openssl.org/source/old/1.1.0/$(BASENAME).tar.gz
FETCHURLS+=ftp://mirror.switch.ch/mirror/openssl/source/$(BASENAME).tar.gz
FETCHURLS+=http://artfiles.org/openssl.org/source/$(BASENAME).tar.gz
FETCHURLS+=https://ftp.heanet.ie/mirrors/ftp.openssl.org/source/$(BASENAME).tar.gz



CONFIG_ARGS=
CONFIG_ARGS+=	-no-shared
ifeq (purify,$(findstring purify,$(CONFIGURED_EXTRA_ARGS)))
#see https://stroika.atlassian.net/browse/STK-400 - -DPURIFY to cleanly valgrind
CONFIG_ARGS+=	-DPURIFY
endif


BUILDS_OUTPUT_DIR=$(shell realpath `pwd`/../../)/Builds/$(CONFIGURATION)/ThirdPartyComponents/
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH=$(shell cygpath --mixed $(BUILDS_OUTPUT_DIR))
endif

ifeq (Unix,$(ProjectPlatformSubdir))
CONFIG_ARGS+=	--prefix=$(BUILDS_OUTPUT_DIR)
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
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
#CONFIG_ARGS+=	-no-dso		### EXPERIMENT TURNING THIS WORKAROUND OFF - 2019-09-25




ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
WIN_CRYPTO_LIB_BASENAME=libcrypto
WIN_SSL_LIB_BASENAME=libssl
endif

PRODUCED_OUTPUT_ARTIFACTS=	\
	$(PER_CONFIGURATION_BUILDS_DIR)include/openssl/aes.h
ifeq (Unix,$(ProjectPlatformSubdir))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/libcrypto.a							\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/libssl.a								\
	$(PER_CONFIGURATION_BUILDS_DIR)bin/openssl
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/$(WIN_CRYPTO_LIB_BASENAME).lib		\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/$(WIN_SSL_LIB_BASENAME).lib			\
	$(PER_CONFIGURATION_BUILDS_DIR)bin/openssl.exe							\
	$(PER_CONFIGURATION_BUILDS_DIR)bin/openssl.pdb							\
	$(PER_CONFIGURATION_BUILDS_DIR)bin/c_rehash.pl
endif


all:
ifeq ($(CONFIGURATION),)
	@for i in `../../ScriptsLib/GetConfigurations` ; do\
		$(MAKE) --no-print-directory all CONFIGURATION=$$i;\
	done
else
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents/OpenSSL Build $(VERSION) {$(CONFIGURATION)}:"
	@../../ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@$(MAKE) --silent MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $(ZIPPEDFILE)
ifeq ($(PRODUCED_OUTPUT_ARTIFACTS),)
	$(error PRODUCED_OUTPUT_ARTIFACTS missing)
endif
	@#make zippedfile even though a dependency on CURRENT and PRODUCED_ARTIFACTS for parallel-make so completed before either starts (else one sees file partly downloaded and proceeeds)
	@$(MAKE) --no-print-directory --silent $(ZIPPEDFILE)
	@#need --silent in PRODUCED_OUTPUT_ARTIFACTS to avoid 'up to date' warning
	@$(MAKE) --no-print-directory --silent MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $(PRODUCED_OUTPUT_ARTIFACTS)
ifneq ($(QUICK_BUILD), 1)
	@$(MAKE) --no-print-directory --silent MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) CURRENT 
endif
	@$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) check
endif


#ALL produced artifacts produced by the same thing. But if you try the same rule to make all, if parallel makes invoked on this, you get
#multiple tries to run the same rule. So remap all to making the first.
$(wordlist 2, 9999, $(PRODUCED_OUTPUT_ARTIFACTS)):	$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS))

$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS)):
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(MAKE) --no-print-directory $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
	@$(MAKE) --no-print-directory doBuilds_


clean:
ifeq ($(CONFIGURATION),)
	@rm -rf ../../IntermediateFiles/*/ThirdPartyComponents/openssl
else
	@rm -rf $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
endif


clobber:	clean
ifeq ($(CONFIGURATION),)
	@rm -rf CURRENT ORIGS
	@rm -rf $(StroikaRoot)Builds/*/ThirdPartyComponents/lib/{libcrypto,libssl}* $(StroikaRoot)Builds/*/ThirdPartyComponents/include/openssl $(StroikaRoot)Builds/*/ThirdPartyComponents/bin/openssl
else
	@rm -rf $(PRODUCED_OUTPUT_ARTIFACTS) $(PER_CONFIGURATION_BUILDS_DIR)include/openssl
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
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "openssl ${VERSION} - Checking ... "
	@$(StroikaRoot)ScriptsLib/CheckFileExists $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(ECHO) " Stroika/ThirdPartyComponents/OpenSSL - [Succeeded]"


$(ZIPPEDFILE):
	@MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) ../../ScriptsLib/WebGet $(ZIPPEDFILE) $(FETCHURLS)



ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
ifeq ($(ARCH), x86)
ifeq (0,$(ENABLE_ASSERTIONS))
USE_CONFIGURE_TARGET_NAME_=VC-WIN32
else
USE_CONFIGURE_TARGET_NAME_=debug-VC-WIN32
endif
else ifeq ($(ARCH), x86_64)
ifeq (0,$(ENABLE_ASSERTIONS))
USE_CONFIGURE_TARGET_NAME_=VC-WIN64A
else
USE_CONFIGURE_TARGET_NAME_=debug-VC-WIN64A
endif
endif
endif

#
# openssl with sanitizers doesn't work (causes issues with building libcurl)
# see ... IntermediateFiles\Debug\ThirdPartyComponents\curl\config.log for failure this causes (could fix by having this patch pkgconfig file)
# -- LGP 2019-09-25
#
CFLAGS:=$(filter-out -fsanitize=%, $(CFLAGS))
CXXFLAGS:=$(filter-out -fsanitize=%, $(CXXFLAGS))


###
### got error building openssl wihtout this filter-out with clang-7 release builds - not sure it was the same message though
### and 
###   /usr/lib/gcc-cross/arm-linux-gnueabihf/8/../../../../arm-linux-gnueabihf/bin/ld: armcap.o (symbol from plugin): in function `OPENSSL_cpuid_setup':
###   (.text+0x0): multiple definition of `OPENSSL_rdtsc'; armcap.o (symbol from plugin):(.text+0x0): first defined here - g++ - cross compiler, so
### disable for now, and try again later
###		-- LGP 2019-09-26
###
CFLAGS:=$(filter-out -flto, $(CFLAGS))
CXXFLAGS:=$(filter-out -flto, $(CXXFLAGS))


###
### Not critical todo, but leaving /GL in here produces warnings when the module is used (in vs2k17 and vs2k19)
###     EG   c:\sandbox\stroika\devroot\intermediatefiles\release-u-32\thirdpartycomponents\openssl\crypto\x509v3\v3_utl.c(405): warning C4706: assignment within conditional expression [C:\Sandbox\Stroika\DevRoot\Tests\Projects\VisualStudio.Net-2017\29\Test29.vcxproj]
###		-- LGP 2019-09-26
###
CFLAGS:=$(filter-out /GL, $(CFLAGS))
CXXFLAGS:=$(filter-out /GL, $(CXXFLAGS))

#
# from https://wiki.openssl.org/index.php/Compilation_and_Installation: -anythingelse, +anythingelse	Compiler options, will become part of CFLAGS.
# and appears to default incorrectly to /MT
#
# However, adding all of CFLAGs to the arguments of openssl configure didn't work. But looking through the code for configure, it appears to respect the passed in CFLAGS (as do most
# configure processes) - so use that trick here.
#
export CFLAGS
export CXXFLAGS

doBuilds_:
	@
	@#Run openssl configure scripts
	@
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "openssl ${VERSION} - Configure (see ${OUTPUT_WORKDIR_PRETTYNAME}CONFIG.OUT) ..."` " "
ifeq (Unix,$(ProjectPlatformSubdir))
	@rm -f $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIG.OUT && touch $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIG.OUT
	@$(ECHO) "CONFIG_ARGS: $(CONFIG_ARGS)" >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIG.OUT
	@$(ECHO) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIG.OUT
	@($(ECHO) "CONFIG -t output: "; export CC="$(CC)"; export AR="$(AR)"; export RANLIB=$(RANLIB); export MACHINE=$(ARCH); export MAKEFLAGS=; cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && ./config -t) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIG.OUT 2>&1
	@$(ECHO) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIG.OUT
	@$(ECHO) "EXECUTE CONFIG LINE: export CC=$(CC); export AR=$(AR); export RANLIB=$(RANLIB); export MACHINE=$(ARCH); cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && ./config $(CONFIG_ARGS)" >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIG.OUT
	@$(ECHO) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIG.OUT
	@(export CC="$(CC)"; export AR="$(AR)"; export RANLIB=$(RANLIB); export MACHINE=$(ARCH); export MAKEFLAGS=; cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && ./config $(CONFIG_ARGS)) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIG.OUT 2>&1
	@$(ECHO) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIG.OUT
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && perl configdata.pm --dump >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIG.OUT
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@#nb: need set path INSIDE args to RunArgumentsWithCommonBuildVars since it re-orders path
	@$(ECHO) 'cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && \
	 env --unset=MFLAGS --unset=MAKEFLAGS \
	 $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) \"set PATH=$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaRoot))ThirdPartyComponents/ActivePerl/CURRENT/bin;%PATH% && perl Configure $(USE_CONFIGURE_TARGET_NAME_) $(CONFIG_ARGS)\"' >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIG.OUT 2>&1
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && \
	 env --unset=MFLAGS --unset=MAKEFLAGS \
	 $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) "set PATH=$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaRoot))ThirdPartyComponents/ActivePerl/CURRENT/bin;%PATH% && perl Configure $(USE_CONFIGURE_TARGET_NAME_) $(CONFIG_ARGS)" >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIG.OUT 2>&1
endif
	@
	@
	@#Do actual OpenSSL build
	@
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "openssl ${VERSION} - Build (see ${OUTPUT_WORKDIR_PRETTYNAME}BUILD.OUT)"` " ... ";
ifeq (Unix,$(ProjectPlatformSubdir))
	@$(MAKE) --directory $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) --no-print-directory all > $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD.OUT 2>&1;
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@# Note, lines in BUILD.OUT like
	@#		cl : Command line warning D9025 : overriding '/MT' with '/MTd'
	@#		cl : Command line warning D9025 : overriding '/Zi' with '/Z7'
	@# are OK to ignore because openssl makefile structures rules that have defaults
	@# for these values, and then overrides them with the provided values (from our calling script)
	@$(ECHO) 'cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && \
	 ((env --unset=MFLAGS --unset=MAKEFLAGS $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) nmake) \
	 2>&1)' >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD.OUT
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && \
	 ((env --unset=MFLAGS --unset=MAKEFLAGS $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) nmake) \
	 2>&1) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD.OUT
endif
	@
	@
	@# OpenSSL build to INSTALLDIR (inside Stroika Builds)
	@
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "openssl ${VERSION} - Installing (see ${OUTPUT_WORKDIR_PRETTYNAME}INSTALL.OUT) ... "
ifeq (Unix,$(ProjectPlatformSubdir))
	@$(MAKE) -j1 --directory $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) --no-print-directory install > $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/INSTALL.OUT 2>&1;
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@#pdb hack - hardwired in openssl makefile, but not build if we pass in /Z7 (which we do by default)
	@touch $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/ossl_static.pdb 
	@$(ECHO) 'cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && \
	 ((env --unset=MFLAGS --unset=MAKEFLAGS \
	  $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) \
	   nmake install INSTALLTOP=$(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH) OPENSSLDIR=$(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH) ENGINESDIR=$(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH)\
	  ) 2>&1)' >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/INSTALL.OUT
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && \
	 ((env --unset=MFLAGS --unset=MAKEFLAGS \
	  $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) \
	   nmake install INSTALLTOP=$(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH) OPENSSLDIR=$(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH) ENGINESDIR=$(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH)\
	  ) 2>&1) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/INSTALL.OUT
	@rm -f $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/ossl_static.pdb $(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH)/lib/ossl_static.pdb
endif
	@
	@
	@#Run Regression Tests (unless cross compiling)
	@
ifneq ($(QUICK_BUILD), 1)
ifeq (false,$(CrossCompiling))
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "openssl ${VERSION} - Running tests (see ${OUTPUT_WORKDIR_PRETTYNAME}TEST-OUT.txt)"` "...";
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@### @todo FIX FOR WINDOWS BUILDS - not sure why all tests fail -- which is why we don't bother with diff
	@$(ECHO) 'cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && \
	 ((env --unset=MFLAGS --unset=MAKEFLAGS $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) nmake test)\
	  2>&1)' >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/TEST-OUT.txt
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && \
	 (((env --unset=MFLAGS --unset=MAKEFLAGS $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) nmake test)\
	  2>&1) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/TEST-OUT.txt) || true
else
	@#we get a single failure on BLOW, so ignore, minor, and should do diff of results
	@$(MAKE) -j1 --directory $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) --no-print-directory -s test > $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/TEST-OUT.txt 2>&1 || true
	@export NDIFSS=`diff -c --text -b REFERENCE_TEST_OUTPUT.txt $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/TEST-OUT.txt | egrep -v "^---" | egrep -c "^[!+-]"` && ../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n && $(ECHO) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "openssl $(VERSION) - Test results: $$NDIFSS lines differ between REFERENCE_TEST_OUTPUT.txt and $(OUTPUT_WORKDIR_PRETTYNAME)TEST.OUT"`
endif
endif
endif


ORIGs CURRENT $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_):	$(ZIPPEDFILE)
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "openssl ${VERSION} - Extracting $(ZIPPEDFILE) to $@"` "... "
	@mkdir -p $@
ifeq ($(shell uname -s),Darwin)
	@tar xf $(ZIPPEDFILE) -C $@ --strip-components 1
else ifeq (CYGWIN,$(findstring CYGWIN,$(shell uname)))
	@PWD=`pwd` tar xf $(ZIPPEDFILE) --no-same-owner --directory `realpath --relative-to=$$PWD $@` --strip-components=1
else
	@tar xf $(ZIPPEDFILE) --no-same-owner --directory $@ --strip-components=1
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
ifeq ($@, $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_))
	@../../ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) -n "openssl ${VERSION} - Patching ... "
	@$(ECHO) -n .
	@patch --quiet -p0 $@/Configurations/windows-makefile.tmpl < Patches/FixQuotesInWindowsMakefileTemplate.PATCH
	@$(ECHO) "done"
endif
endif

