#
#
#	TODO:
#		@todo	NOTE - try for $(word 1,$(PRODUCED_OUTPUT_ARTIFACTS)) and $(wordlist 2, 9999, $(PRODUCED_OUTPUT_ARTIFACTS)):	$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS))
#				(repeated in most ThirdPartyComponents makefiles)
#				doesn't work if the first produced output artifact is fine, but the others got deleted. No BIGGIE, as you can just make clobber in that
#				exceedingly rare case; but certainly a defect of this approach.
StroikaRoot:=$(abspath ../..)/

ifneq ($(CONFIGURATION),)
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk

	CONFIGURED_EXTRA_ARGS			=	$(qFeatureFlag_OpenSSL_ExtraArgs)
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk

.PHONY:	all clean clobber check doBuild

# As of OpenSSL 3.0, QUICK_BUILD really needed by default since takes like 30 minutes (at least with windows/sanitizer) -- LGP 2021-09-09
QUICK_BUILD ?= 1

#
# CHECK FOR UPDATED VERSIONS https://www.openssl.org/source/
#
VERSION:=3.3.2
#VERSION:=3.3.1
#VERSION:=3.3.0
#VERSION:=3.2.1

BASENAME:=openssl-$(VERSION)
EXTRACTED_DIRNAME:=$(BASENAME)


PER_CONFIGURATION_BUILDS_DIR:=$(shell realpath --canonicalize-missing ../../Builds/$(CONFIGURATION)/ThirdPartyComponents)/
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_:=$(shell realpath --canonicalize-missing ../../IntermediateFiles/$(CONFIGURATION)/ThirdPartyComponents/openssl)/
OUTPUT_WORKDIR_PRETTYNAME="$(shell $(StroikaRoot)ScriptsLib/SubstituteBackVariables "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)")"

# Only needed on MacOS due to make 3.81, I believe --LGP 2024-07-01
ifeq (3.81,$(firstword $(sort $(MAKE_VERSION) 3.81)))
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_OLDMAKEBWA_:=$(shell realpath --canonicalize-missing ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_})
else
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_OLDMAKEBWA_:=$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)
endif

ZIPPEDFILE:=../Origs-Cache/$(BASENAME).tar.gz

FETCHURLS:=
FETCHURLS+=https://github.com/openssl/openssl/releases/download/$(BASENAME)/$(BASENAME).tar.gz
FETCHURLS+=https://www.openssl.org/source/$(BASENAME).tar.gz
FETCHURLS+=http://www.openssl.org/source/$(BASENAME).tar.gz
FETCHURLS+=http://www.openssl.org/source/old/3.1/$(BASENAME).tar.gz
FETCHURLS+=http://www.openssl.org/source/old/3.2/$(BASENAME).tar.gz
FETCHURLS+=ftp://mirror.switch.ch/mirror/openssl/source/$(BASENAME).tar.gz
FETCHURLS+=http://artfiles.org/openssl.org/source/$(BASENAME).tar.gz
FETCHURLS+=https://ftp.heanet.ie/mirrors/ftp.openssl.org/source/$(BASENAME).tar.gz


SHELL=/bin/bash


CONFIG_ARGS=

ifneq ($(CC),)
ifeq (armhf,$(ARCH))
# OpenSSL 3.0 automatically considers the TARGETMACHINE to be this HOST compiling machine and generates -m64 lines that break
# the compile - this works around that issue for at least this configuration
CONFIG_ARGS+=	linux-generic32
else ifeq ($(DETECTED_HOST_OS),Darwin)
CONFIG_ARGS+=	darwin64-$(ARCH)-cc
endif
endif



CONFIG_ARGS+=	-no-shared
ifeq (purify,$(findstring purify,$(CONFIGURED_EXTRA_ARGS)))
#see https://stroika.atlassian.net/browse/STK-400 - -DPURIFY to cleanly valgrind
CONFIG_ARGS+=	-DPURIFY
endif


BUILDS_OUTPUT_DIR=$(shell realpath `pwd`/../../)/Builds/$(CONFIGURATION)/ThirdPartyComponents/
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(BuildPlatform)))
BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH=$(shell cygpath --mixed $(BUILDS_OUTPUT_DIR))
endif

ifeq (Unix,$(BuildPlatform))
CONFIG_ARGS+=	--prefix=$(BUILDS_OUTPUT_DIR)
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(BuildPlatform)))
CONFIG_ARGS+=	--prefix=$(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH)
endif


ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(BuildPlatform)))
#https://stroika.atlassian.net/browse/STK-528
#CONFIG_ARGS+=	-asm
CONFIG_ARGS+=	-no-asm
endif

ifeq ($(QUICK_BUILD), 1)
CONFIG_ARGS+=	-no-tests
endif

ifeq ($(QUICK_BUILD), 1)
INSTALL_TARGET=install_sw install_ssldirs
else
INSTALL_TARGET=install
endif

# autoconfigure sometimes (linux) detects multilib and then changes default libdir to lib64... - so override to look where rest of code looks for it
ifeq (Unix,$(BuildPlatform))
CONFIG_ARGS+=	--libdir=lib
endif


ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(BuildPlatform)))
WIN_CRYPTO_LIB_BASENAME=libcrypto
WIN_SSL_LIB_BASENAME=libssl
endif

PRODUCED_OUTPUT_ARTIFACTS=	\
	$(PER_CONFIGURATION_BUILDS_DIR)include/openssl/aes.h
ifeq (Unix,$(BuildPlatform))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/libcrypto.a							\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/libssl.a								\
	$(PER_CONFIGURATION_BUILDS_DIR)bin/openssl
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(BuildPlatform)))
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/$(WIN_CRYPTO_LIB_BASENAME).lib		\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/$(WIN_SSL_LIB_BASENAME).lib			\
	$(PER_CONFIGURATION_BUILDS_DIR)bin/openssl.exe							\
	$(PER_CONFIGURATION_BUILDS_DIR)bin/openssl.pdb							\
	$(PER_CONFIGURATION_BUILDS_DIR)bin/c_rehash.pl
endif


all:
ifeq ($(CONFIGURATION),)
	@for i in `$(StroikaRoot)GetConfigurations` ; do\
		$(MAKE) --no-print-directory all CONFIGURATION=$$i;\
	done
else
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents/OpenSSL Build $(VERSION) {$(CONFIGURATION)}:"
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
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
	@$(MAKE) --no-print-directory $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_OLDMAKEBWA_)
	@$(MAKE) --no-print-directory doBuilds_


clean:
ifeq ($(CONFIGURATION),)
	@rm -rf ../../IntermediateFiles/*/ThirdPartyComponents/openssl
else
	@rm -rf $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)
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
	$(error This makefile requires a recognized BuildPlatform)
endif
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "openssl ${VERSION} - Checking ... "
	@$(StroikaRoot)ScriptsLib/CheckFileExists $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(ECHO) " Stroika/ThirdPartyComponents/OpenSSL - [Succeeded]"


$(ZIPPEDFILE):
	@MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) $(StroikaRoot)ScriptsLib/WebGet $(ZIPPEDFILE) $(FETCHURLS)



ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(BuildPlatform)))
ifeq ($(ARCH), x86)
ifeq (0,$(AssertionsEnabled))
USE_CONFIGURE_TARGET_NAME_=VC-WIN32
else
USE_CONFIGURE_TARGET_NAME_=debug-VC-WIN32
endif
else ifeq ($(ARCH), x86_64)
ifeq (0,$(AssertionsEnabled))
USE_CONFIGURE_TARGET_NAME_=VC-WIN64A
else
USE_CONFIGURE_TARGET_NAME_=debug-VC-WIN64A
endif
endif
endif


#
# Just for test apps that get built. DONT include LinkerArgs_LibDependencies because that includes other third party components
# Unsure about most of this... --LGP 2022-01-22
#LDFLAGS:=$(LinkerArgs_ExtraPrefix) $(LinkerArgs_LibPath) $(LinkerArgs_LibDependencies) $(LinkerArgs_ExtraSuffix)
LDFLAGS:=$(LinkerArgs_ExtraPrefix) $(LinkerArgs_LibPath) $(LinkerArgs_ExtraSuffix)


#
# openssl with sanitizers doesn't work (causes issues with building libcurl)
# see ... IntermediateFiles\Debug\ThirdPartyComponents\curl\config.log for failure this causes (could fix by having this patch pkgconfig file)
# -- LGP 2019-09-25
#
CFLAGS:=$(filter-out -fsanitize=%, $(CFLAGS))
CXXFLAGS:=$(filter-out -fsanitize=%, $(CXXFLAGS))
LDFLAGS:=$(filter-out -fsanitize=%, $(LDFLAGS))

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


# Avoid having warnings leak from here to linking code when openssl is used --LGP 2022-01-18
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(BuildPlatform)))
ifneq ($(findstring -GL,$(CXXFLAGS)),)
CFLAGS+= -wd4701 -wd4702 -wd4706
CXXFLAGS+= -wd4701 -wd4702 -wd4706
endif
endif

#
# from https://wiki.openssl.org/index.php/Compilation_and_Installation: -anythingelse, +anythingelse	Compiler options, will become part of CFLAGS.
# and appears to default incorrectly to /MT
#
# However, adding all of CFLAGs to the arguments of openssl configure didn't work. But looking through the code for configure, it appears to respect the passed in CFLAGS (as do most
# configure processes) - so use that trick here.
#
export CFLAGS
export CXXFLAGS
export LDFLAGS

#
# Running on MSYS in interactive mode, this isn't needed. The library builds fine. But it fails badly
# otherwise. Apparently, this is something todo with running the /etc/profile script which sets a bunch of path
# and other variables.
#
# I tried many other tricks (like setting .SHELLFLAGS), but this appears to work best (setting -i or -l .SHELLFLAGS also works
# but creates other problems)
#		--LGP 2022-01-24
#
# This hack/workaround caused failure of build in various subtle ways (complaint about cannot find rule to make ../Origs-Cache... stuff
# and then failure inside RunExteneralVStudioScripts... -  probably due to the resetting of the PWD, and other things.
#
# Eventually settiled on ONLY setting BASH_ENV when MSYSTEM has not been defined (so we ran - eg docker exec C make ... from .github actions)
# But NOT RESETTING it in case when its already been set, like from vscode terminal shell (where setting BASH_ENV unconditionally caused failrues).
#		-- LGP 2022-03-26
# 
ifeq ($(DETECTED_HOST_OS),MSYS)
ifeq ($(MSYSTEM), )
# From https://linux.die.net/man/1/bash
#		When bash is started non-interactively, to run a shell script, for example, it looks
#		for the variable BASH_ENV in the environment, expands its value if it appears there, 
#		and uses the expanded value as the name of a file to read and execute.
export BASH_ENV=/etc/profile
export CHERE_INVOKING=1		# even if we need vars set, dont change CWD
endif
endif

ifeq ($(qFeatureFlag_StrawberryPerl), use)
PERL_VERSION_PATCH_=set PATH=$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaRoot))ThirdPartyComponents/StrawberryPerl/CURRENT/perl/bin;%PATH%
endif
ifeq (${PERL_VERSION_PATCH_},)
PERL_VERSION_PATCH_=echo 'using installed perl'
endif


ifeq ($(qFeatureFlag_StrawberryPerl), use)
# These perl scripts require \ not / in pathnames (at least on MSYS) - not sure why not needed for cygwin
# and THEN, only when run inside docker? So very confusing...
# --LGP 2023-06-10
CFLAGS:=$(shell echo ${CFLAGS} | sed 's/\//\\/g')
endif



doBuilds_:
	@
	@#Run openssl configure scripts
	@
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "openssl ${VERSION} - Configure (see ${OUTPUT_WORKDIR_PRETTYNAME}CONFIG-OUT.txt) ..."` " "
ifeq (Unix,$(BuildPlatform))
	@rm -f $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)CONFIG-OUT.txt && touch $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)CONFIG-OUT.txt
	@$(ECHO) "CONFIG_ARGS: $(CONFIG_ARGS)" >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)CONFIG-OUT.txt
	@$(ECHO) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)CONFIG-OUT.txt
	@($(ECHO) "CONFIG -t output: "; export CC="$(CC)"; export AR="$(AR)"; export RANLIB=$(RANLIB); export MACHINE=$(ARCH); export MAKEFLAGS=; cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_) && ./config -t) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)CONFIG-OUT.txt 2>&1
	@$(ECHO) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)CONFIG-OUT.txt
	@$(ECHO) "EXECUTE CONFIG LINE: export CC=$(CC); export AR=$(AR); export RANLIB=$(RANLIB); export MACHINE=$(ARCH); cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_) && ./config $(CONFIG_ARGS)" >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)CONFIG-OUT.txt
	@$(ECHO) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)CONFIG-OUT.txt
	@(export CC="$(CC)"; export AR="$(AR)"; export RANLIB=$(RANLIB); export MACHINE=$(ARCH); export MAKEFLAGS=; cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_) && ./config $(CONFIG_ARGS)) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)CONFIG-OUT.txt 2>&1
	@$(ECHO) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)CONFIG-OUT.txt
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_) && perl configdata.pm --dump >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)CONFIG-OUT.txt
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(BuildPlatform)))
	@#nb: need PERL_VERSION_PATCH_ INSIDE args to RunArgumentsWithCommonBuildVars since RunArgumentsWithCommonBuildVars re-orders path
	@$(ECHO) 'cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_) && \
	 env --unset=MFLAGS --unset=MAKEFLAGS \
	 $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) \"$(PERL_VERSION_PATCH_) && perl Configure $(USE_CONFIGURE_TARGET_NAME_) $(CONFIG_ARGS)\"' >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)CONFIG-OUT.txt 2>&1
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_) && \
	 env --unset=MFLAGS --unset=MAKEFLAGS \
	 $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) "$(PERL_VERSION_PATCH_) && perl Configure $(USE_CONFIGURE_TARGET_NAME_) $(CONFIG_ARGS)" >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)CONFIG-OUT.txt 2>&1
endif
	@
	@
	@#Do actual OpenSSL build
	@
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "openssl ${VERSION} - Build (see ${OUTPUT_WORKDIR_PRETTYNAME}BUILD-OUT.txt)"` " ... ";
ifeq (Unix,$(BuildPlatform))
	@$(MAKE) --directory $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_) --no-print-directory all > $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)BUILD-OUT.txt 2>&1;
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(BuildPlatform)))
	@# Note, lines in BUILD-OUT.txt like
	@#		cl : Command line warning D9025 : overriding '/MT' with '/MTd'
	@#		cl : Command line warning D9025 : overriding '/Zi' with '/Z7'
	@# are OK to ignore because openssl makefile structures rules that have defaults
	@# for these values, and then overrides them with the provided values (from our calling script)
	@$(ECHO) 'cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_) && \
	 ((env --unset=MFLAGS --unset=MAKEFLAGS $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) nmake) \
	 2>&1)' >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)BUILD-OUT.txt
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_) && \
	 ((env --unset=MFLAGS --unset=MAKEFLAGS $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) nmake) \
	 2>&1) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)BUILD-OUT.txt
endif
	@
	@
	@# OpenSSL build to INSTALLDIR (inside Stroika Builds)
	@
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "openssl ${VERSION} - Installing (see ${OUTPUT_WORKDIR_PRETTYNAME}INSTALL-OUT.txt) ... "
ifeq (Unix,$(BuildPlatform))
	@$(MAKE) -j1 --directory $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_) --no-print-directory $(INSTALL_TARGET) > $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)INSTALL-OUT.txt 2>&1;
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(BuildPlatform)))
	@#pdb hack - hardwired in openssl makefile, but not build if we pass in /Z7 (which we do by default)
	@touch $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)ossl_static.pdb 
	@$(ECHO) 'cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_) && \
	 ((env --unset=MFLAGS --unset=MAKEFLAGS \
	  $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) \
	   nmake $(INSTALL_TARGET) INSTALLTOP=$(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH) OPENSSLDIR=$(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH) ENGINESDIR=$(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH)\
	  ) 2>&1)' >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)INSTALL-OUT.txt
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_) && \
	 ((env --unset=MFLAGS --unset=MAKEFLAGS \
	  $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) \
	   nmake $(INSTALL_TARGET) INSTALLTOP=$(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH) OPENSSLDIR=$(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH) ENGINESDIR=$(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH)\
	  ) 2>&1) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)INSTALL-OUT.txt
	@rm -f $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)ossl_static.pdb $(BUILDS_OUTPUT_DIR_AS_WINDOWS_PATH)/lib/ossl_static.pdb
endif
	@
	@
	@#Run Regression Tests (unless cross compiling)
	@
ifneq ($(QUICK_BUILD), 1)
ifeq (false,$(CrossCompiling))
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "openssl ${VERSION} - Running tests (see ${OUTPUT_WORKDIR_PRETTYNAME}TEST-OUT.txt)"` "...";
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(BuildPlatform)))
	@### @todo FIX FOR WINDOWS BUILDS - not sure why all tests fail -- which is why we don't bother with diff
	@$(ECHO) 'cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_) && \
	 ((env --unset=MFLAGS --unset=MAKEFLAGS $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) nmake test)\
	  2>&1)' >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)TEST-OUT.txt
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_) && \
	 (((env --unset=MFLAGS --unset=MAKEFLAGS $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) nmake test)\
	  2>&1) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)TEST-OUT.txt) || true
else
	@#we get a single failure on BLOW, so ignore, minor, and should do diff of results
	@$(MAKE) -j1 --directory $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_) --no-print-directory -s test > $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)TEST-OUT.txt 2>&1 || true
	@export NDIFSS=`diff -c --text -b REFERENCE_TEST_OUTPUT.txt $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)TEST-OUT.txt | egrep -v "^---" | egrep -c "^[!+-]"` && $(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n && $(ECHO) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "openssl $(VERSION) - Test results: $$NDIFSS lines differ between REFERENCE_TEST_OUTPUT.txt and $(OUTPUT_WORKDIR_PRETTYNAME)TEST.OUT"`
endif
endif
endif


ORIGs CURRENT $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_OLDMAKEBWA_):	$(ZIPPEDFILE)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "openssl ${VERSION} - Extracting $(ZIPPEDFILE) to $@"` "... "
	@mkdir -p $@
ifeq ($(DETECTED_HOST_OS),Darwin)
	@tar xf $(ZIPPEDFILE) -C $@ --strip-components 1
else ifneq ($(findstring $(DETECTED_HOST_OS),MSYS-Cygwin),)
	@PWD=`pwd` tar xf $(ZIPPEDFILE) --no-same-owner --directory `realpath --relative-to=$$PWD $@` --strip-components=1
else
	@tar xf $(ZIPPEDFILE) --no-same-owner --directory $@ --strip-components=1
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(BuildPlatform)))
ifeq ($@, $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_))
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) -n "openssl ${VERSION} - Patching ... "
	@$(ECHO) -n .
	@patch --quiet -p0 $@/Configurations/windows-makefile.tmpl < Patches/FixQuotesInWindowsMakefileTemplate.PATCH
	@$(ECHO) "done"
endif
endif
