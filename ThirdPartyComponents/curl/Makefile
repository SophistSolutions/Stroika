export StroikaRoot?=$(shell realpath ../..)/

ifneq ($(CONFIGURATION),)
	include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk

	CC		=	$(shell $(StroikaRoot)ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" CompilerDriver-C)
	CXX		=	$(shell $(StroikaRoot)ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" CompilerDriver-C++)
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk


.DEFAULT_GOAL := all
.PHONY:	all clean clobber check doBuild doCreate doPatch
.NOTPARALLEL:



DoCreateSymLink=0

VERSION=7.62.0
#VERSION=7.61.1
#VERSION=7.60.0
BASENAME=curl-$(VERSION)
ZIPPEDFILE=../Origs-Cache/$(BASENAME).tar.gz
TARFILE=$(BASENAME).tar
EXTRACTED_DIRNAME=$(BASENAME)
SLINKDIRNAME=$(BASENAME)
USE_DIRNAME=CURRENT

#
# CHECK FOR UPDATED VERSIONS https://curl.haxx.se/download.html
#

FETCHURLS=
FETCHURLS+=https://curl.haxx.se/download/curl-$(VERSION).tar.gz
FETCHURLS+=http://curl.askapache.com/download/curl-$(VERSION).tar.gz

INTERMEDIATEFILES_ROOT=../../IntermediateFiles/
BUILDS_ROOT=../../Builds/
PER_CONFIGURATION_BUILDS_DIR:=$(shell realpath --canonicalize-missing ../../Builds/$(CONFIGURATION)/ThirdPartyComponents/)
PER_CONFIGURATION_CURL_INTERMEDIATEFILES_DIR:=$(shell realpath --canonicalize-missing ../../IntermediateFiles/$(CONFIGURATION)/ThirdPartyComponents/curl/)

OUTPUT_WORKDIR_PRETTYNAME="{StroikaRoot}/Inter.../Thir.../curl/"


qFeatureFlag_OpenSSL	=	$(shell ../../ScriptsLib/GetConfigurationParameter "$(CONFIGURATION)" qFeatureFlag_OpenSSL)



ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
WIN_CC32	=	$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) CC_32)")
WIN_CC64	=	$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) CC_64)")
WIN_AS32	=	$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) AS_32)")
WIN_AS64	=	$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) AS_64)")
WIN_LIB32	=	$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) LIB_32)")
WIN_LIB64	=	$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) LIB_64)")
endif

BUILDS_DIR=	$(shell pwd)/Builds/



ifeq (Unix,$(ProjectPlatformSubdir))
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/libcurl.a				\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/curl/curl.h
else
PRODUCED_OUTPUT_ARTIFACTS=\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/include/curl/curl.h
endif




#TODO - fix to take into account DEBUG versus RELEASE! Must build all combos!
all:
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents/libcurl $(VERSION) Build {$(CONFIGURATION)}:"
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@../../ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@$(MAKE) --no-print-directory --silent MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) check


$(ZIPPEDFILE):
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ../../ScriptsLib/WebGet.sh $(ZIPPEDFILE) $(FETCHURLS)


$(PRODUCED_OUTPUT_ARTIFACTS):
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(MAKE) --no-print-directory doCreate
	@$(MAKE) --no-print-directory doBuilds


doCreate:	$(ZIPPEDFILE)
	@rm -rf CURRENT
	@$(MAKE) CURRENT


CURRENT ORIGS:	$(ZIPPEDFILE)
	@../../ScriptsLib/PrintProgressLine.sh  $$(($(MAKE_INDENT_LEVEL)+1)) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "curl ${VERSION} - Extracting $(ZIPPEDFILE) to $@"` "... "
	@mkdir -p ORIGS && rm -rf $@ && mkdir $@
ifeq ($(shell uname -s),Darwin)
	@tar xf $(ZIPPEDFILE) -C $@ --strip-components 1
else
	@tar xf $(ZIPPEDFILE) --no-same-owner --directory $@ --strip-components=1
endif


PKG_CONFIG_PATH=
ifeq ($(qFeatureFlag_OpenSSL), use)
ifeq (Unix,$(ProjectPlatformSubdir))
PKG_CONFIG_PATH=$(shell realpath --canonicalize-missing `pwd`/../../Builds/$(CONFIGURATION))/ThirdPartyComponents/lib/pkgconfig
endif
endif

ENVIRONMENT_CONFIGURE_PREFIX=
ifneq (,$(PKG_CONFIG_PATH))
ENVIRONMENT_CONFIGURE_PREFIX=
ENVIRONMENT_CONFIGURE_PREFIX+= export PKG_CONFIG_PATH=$(PKG_CONFIG_PATH);

###https://stroika.atlassian.net/browse/STK-427 workaround - these are needed else HMAC_xxx detect fails looking for openssl - not sure why it ignores the PKG_CONFIG_PATH - openssl 1.1.1 -LGP 2018-07-06
ENVIRONMENT_CONFIGURE_PREFIX+= export CFLAGS=-I$(shell realpath --canonicalize-missing `pwd`/../../Builds/$(CONFIGURATION)/ThirdPartyComponents/include);
ENVIRONMENT_CONFIGURE_PREFIX+= export LDFLAGS=-L$(shell realpath --canonicalize-missing `pwd`/../../Builds/$(CONFIGURATION)/ThirdPartyComponents/lib);
endif



ifeq (Unix,$(ProjectPlatformSubdir))
CONFIGURE_ARGS=
ifeq ($(CrossCompiling),true)
CONFIGURE_ARGS+= --host $(shell $(CXX) -dumpmachine)
endif
endif



LIBCURL_CONFIGURE =
LIBCURL_CONFIGURE += $(CONFIGURE_ARGS)
LIBCURL_CONFIGURE += --enable-static
ifeq ($(qFeatureFlag_OpenSSL), use)
#finds right one through PKG_CONFIG_PATH
LIBCURL_CONFIGURE += --with-ssl
endif
ifeq ($(qFeatureFlag_OpenSSL), use-system)
LIBCURL_CONFIGURE += --with-ssl
endif
ifeq ($(qFeatureFlag_OpenSSL), no)
LIBCURL_CONFIGURE += --without-ssl
endif
LIBCURL_CONFIGURE += --disable-shared
LIBCURL_CONFIGURE += --without-cyassl
LIBCURL_CONFIGURE += --without-librtmp
### for now - I think we want this one!
LIBCURL_CONFIGURE += --without-libidn
LIBCURL_CONFIGURE += --without-libssh2
LIBCURL_CONFIGURE += --without-nghttp2
LIBCURL_CONFIGURE += --without-axtls
LIBCURL_CONFIGURE += --disable-smtp
LIBCURL_CONFIGURE += --disable-ldap --disable-ldaps
LIBCURL_CONFIGURE += --disable-manual
LIBCURL_CONFIGURE += --disable-rtsp
LIBCURL_CONFIGURE += --disable-gopher
LIBCURL_CONFIGURE += --disable-dict
LIBCURL_CONFIGURE += --disable-telnet
LIBCURL_CONFIGURE += --disable-pop3
LIBCURL_CONFIGURE += --disable-imap
## really want ares but hindrance for static linking for now
LIBCURL_CONFIGURE += --disable-ares
## really want zlib but until we can do relative build/static link... leave out..
LIBCURL_CONFIGURE += --without-zlib
ifeq ($(ENABLE_ASSERTIONS), 1)
LIBCURL_CONFIGURE += --enable-debug
LIBCURL_CONFIGURE += --disable-optimize
LIBCURL_CONFIGURE += --enable-curldebug
else
LIBCURL_CONFIGURE += --disable-debug
LIBCURL_CONFIGURE += --enable-optimize
LIBCURL_CONFIGURE += --disable-curldebug
endif
#seems illrelevant for us one way or the other, but breaks building for centos5.8 - https://stroika.atlassian.net/browse/STK-419
LIBCURL_CONFIGURE += --without-zsh-functions-dir


ifeq ($(qFeatureFlag_OpenSSL), use)
#Avoid:
#	configure:21566: pkg-config: SSL_LIBS: "-lssl -lcrypto "
#	configure:21568: pkg-config: SSL_LDFLAGS: "-L/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Builds/Debug/ThirdPartyComponents/lib "
#	configure:21570: pkg-config: SSL_CPPFLAGS: "-I/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Builds/Debug/ThirdPartyComponents/include "
#	configure:21582: checking for HMAC_Update in -lcrypto
#	configure:21604: gcc -o conftest -g -O0 -pedantic -Wall -W -Wpointer-arith -Wwrite-strings -Wunused -Wshadow -Winline -Wnested-externs -Wmissing-declarations -Wmissing-prototypes -Wno-long-long -Wfloat-equal -Wno-multichar -Wsign-compare -Wundef -Wno-format-nonliteral -Wendif-labels -Wstrict-prototypes -Wdeclaration-after-statement -Wstrict-aliasing=3 -Wcast-align -Wtype-limits -Wold-style-declaration -Wmissing-parameter-type -Wempty-body -Wclobbered -Wignored-qualifiers -Wconversion -Wno-sign-conversion -Wvla -Wno-system-headers -DDEBUGBUILD -DCURLDEBUG -isystem /include -I/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Builds/Debug/ThirdPartyComponents/include   -L/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Builds/Debug/ThirdPartyComponents/lib  conftest.c -lcrypto  -lssl -lcrypto   >&5
#	conftest.c:57:1: warning: function declaration isn't a prototype [-Wstrict-prototypes]
#	char HMAC_Update ();
#	^
#	/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Builds/Debug/ThirdPartyComponents/lib/libcrypto.a(dso_dlfcn.o): In function `dlfcn_globallookup':
#	dso_dlfcn.c:(.text+0x11): undefined reference to `dlopen'
#	dso_dlfcn.c:(.text+0x24): undefined reference to `dlsym'
#	dso_dlfcn.c:(.text+0x2f): undefined reference to `dlclose'
#
LIBCURL_CONFIGURE +=  LIBS='-ldl'

#https://stroika.atlassian.net/browse/STK-527
#This hack is NO GOOD - but without it - we don't get ssl feature enabled!
LIBCURL_CONFIGURE +=  LIBS='-ldl -lpthread'

endif



	
doBuilds:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
ifeq (Unix,$(ProjectPlatformSubdir))
	@$(MAKE) --no-print-directory check_prerequisites_
	@#../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "Copy/Create/Patch (destination: $(PER_CONFIGURATION_CURL_INTERMEDIATEFILES_DIR)) ... "`
	@mkdir -p $(PER_CONFIGURATION_CURL_INTERMEDIATEFILES_DIR)
	@cp -r CURRENT/* $(PER_CONFIGURATION_CURL_INTERMEDIATEFILES_DIR)
	@patch --quiet -p0 CURRENT/scripts/Makefile.am < Patches/ZSH-Completion-Buggy.patch
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "curl ${VERSION} - Build configure tools (see ${OUTPUT_WORKDIR_PRETTYNAME}BUILD-CONF-TOOLS.OUT) ... "`
	@cd $(PER_CONFIGURATION_CURL_INTERMEDIATEFILES_DIR);./buildconf > BUILD-CONF-TOOLS.OUT 2>&1
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "curl ${VERSION} - Configure (see ${OUTPUT_WORKDIR_PRETTYNAME}/CONFIGURE.OUT) ... "`
	@$(ECHO) "export CC=$(CC); export CXX=$(CXX); export AR=$(AR); export RANLIB=$(RANLIB); $(ENVIRONMENT_CONFIGURE_PREFIX)./configure --prefix=$(PER_CONFIGURATION_BUILDS_DIR) $(LIBCURL_CONFIGURE)" > $(PER_CONFIGURATION_CURL_INTERMEDIATEFILES_DIR)/CONFIGURE.OUT
	@cd $(PER_CONFIGURATION_CURL_INTERMEDIATEFILES_DIR) && (export CC=$(CC); export CXX=$(CXX); export AR=$(AR); export RANLIB=$(RANLIB); $(ENVIRONMENT_CONFIGURE_PREFIX)./configure --prefix=$(PER_CONFIGURATION_BUILDS_DIR) $(LIBCURL_CONFIGURE) 2>&1) >> CONFIGURE.OUT
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "curl $(VERSION) - Compiling (see ${OUTPUT_WORKDIR_PRETTYNAME}/BUILD.OUT) ... "`
	@echo $(MAKE) --directory $(PER_CONFIGURATION_CURL_INTERMEDIATEFILES_DIR) --no-print-directory install CC="$(CC)" > $(PER_CONFIGURATION_CURL_INTERMEDIATEFILES_DIR)/BUILD.OUT
	@$(MAKE) --directory $(PER_CONFIGURATION_CURL_INTERMEDIATEFILES_DIR) --no-print-directory install CC="$(CC)" >> $(PER_CONFIGURATION_CURL_INTERMEDIATEFILES_DIR)/BUILD.OUT 2>&1

endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) "curl ${VERSION} - Build CURRENT into $(BUILDS_DIR) ... "
	@rm -rf $(BUILDS_DIR)
	@cd CURRENT && cmd /C buildconf.bat > BUILD-CONFIG.OUT
	@cd CURRENT && rm -f *.obj *.pdb *.lib
	@perl ../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) "cd CURRENT/winbuild && MFLAGS= && MAKEFLAGS= && nmake MAKEFLAGS= MFLAGS= -f Makefile.vc mode=static AS='$(WIN_AS32)' CC='$(WIN_CC32)' AR='$(WIN_LIB32)'"
	@todo - INCOMPLETE WINDOZE BUILD
endif


check_prerequisites_:
	@type automake > /dev/null 2>&1 || (../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n "checking tools......... " && ../../ScriptsLib/GetMessageForMissingTool.sh automake && exit 1)
	@type autoconf > /dev/null 2>&1 || (../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n "checking tools......... " && ../../ScriptsLib/GetMessageForMissingTool.sh autoconf && exit 1)
	@type libtool > /dev/null 2>&1 || (../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) -n "checking tools... " && ../../ScriptsLib/GetMessageForMissingTool.sh libtool && exit 1)



check:
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Checking ... "
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@for i in $(PRODUCED_OUTPUT_ARTIFACTS) ; do \
		if [ ! -e $$i ]; then\
			$(ECHO) "FAILED: missing $$i";\
			exit 1;\
		fi;\
	done
	@$(ECHO) " Stroika/ThirdPartyComponents/curl -    [Succeeded]";


check-prerequisite-tools:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Checking Prerequisite tools for curl ... "
	@$(MAKE) --no-print-directory --silent check_prerequisites_
	@$(ECHO) "done";


clean:
	@rm -rf CURRENT ORIGS
ifeq ($(CONFIGURATION),)
	@rm -rf =../../IntermediateFiles/*/ThirdPartyComponents/curl/
else
	@rm -rf $(PER_CONFIGURATION_CURL_INTERMEDIATEFILES_DIR)
endif


clobber:	clean
	@rm -rf $(BUILDS_DIR)
ifeq ($(CONFIGURATION),)
	@rm -rf $(BUILDS_ROOT)*/ThirdPartyComponents/lib/{libcurl}* $(BUILDS_ROOT)*/ThirdPartyComponents/include/curl
else
	@rm -f $(PRODUCED_OUTPUT_ARTIFACTS)
endif
