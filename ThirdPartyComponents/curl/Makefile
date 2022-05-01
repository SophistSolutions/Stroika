#
#	TODO:
#		@todo	NOTE - try for $(word 1,$(PRODUCED_OUTPUT_ARTIFACTS)) and $(wordlist 2, 9999, $(PRODUCED_OUTPUT_ARTIFACTS)):	$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS))
#				(repeated in most ThirdPartyComponents makefiles)
#				doesn't work if the first produced output artifact is fine, but the others got deleted. No BIGGIE, as you can just make clobber in that
#				exceedingly rare case; but certainly a defect of this approach.

StroikaRoot=$(abspath ../..)/

ifneq ($(CONFIGURATION),)
	#no error if missing cuz could be doing make clobber
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk

include $(StroikaRoot)/ScriptsLib/SharedMakeVariables-Default.mk



VERSION=7.83.0
#VERSION=7.82.0
#VERSION=7.81.0
BASENAME=curl-$(VERSION)
ZIPPEDFILE=../Origs-Cache/$(BASENAME).tar.gz
TARFILE=$(BASENAME).tar
EXTRACTED_DIRNAME=$(BASENAME)
SLINKDIRNAME=$(BASENAME)

#
# CHECK FOR UPDATED VERSIONS https://curl.haxx.se/download.html
#

FETCHURLS=
FETCHURLS+=https://curl.haxx.se/download/curl-$(VERSION).tar.gz
FETCHURLS+=http://curl.askapache.com/download/curl-$(VERSION).tar.gz

PER_CONFIGURATION_BUILDS_DIR:=$(shell realpath --canonicalize-missing ../../Builds/$(CONFIGURATION)/ThirdPartyComponents/)/
#lose trailing / on PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_ because on macos make THATDIR doesn't work with trailing slash (old WEIRD_MACOS_MAKEFILE_DIR_BUG -  MacOS XCode 10 - GNU Make 3.81)
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_:=$(shell realpath --canonicalize-missing ../../IntermediateFiles/$(CONFIGURATION)/ThirdPartyComponents/curl/)

#OUTPUT_WORKDIR_PRETTYNAME="{StroikaRoot}Inter.../Thir.../curl/"
OUTPUT_WORKDIR_PRETTYNAME="$(shell $(StroikaRoot)ScriptsLib/SubstituteBackVariables "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/")"


ifeq (Unix,$(ProjectPlatformSubdir))
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/libcurl.a				\
	$(PER_CONFIGURATION_BUILDS_DIR)include/curl/curl.h
else
PRODUCED_OUTPUT_ARTIFACTS=\
	$(PER_CONFIGURATION_BUILDS_DIR)include/curl/curl.h
endif




#TODO - fix to take into account DEBUG versus RELEASE! Must build all combos!
all:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents/libcurl $(VERSION) Build {$(CONFIGURATION)}:"
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@#make zippedfile even though a dependency on CURRENT and PRODUCED_ARTIFACTS for parallel-make so completed before either starts (else one sees file partly downloaded and proceeeds)
	@$(MAKE) --no-print-directory --silent $(ZIPPEDFILE)
	@$(MAKE) --no-print-directory --silent MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)  $(PRODUCED_OUTPUT_ARTIFACTS)
ifneq ($(QUICK_BUILD), 1)
	@$(MAKE) --no-print-directory --silent MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) CURRENT
endif
	@$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) check


$(ZIPPEDFILE):
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $(StroikaRoot)ScriptsLib/WebGet $(ZIPPEDFILE) $(FETCHURLS)


#ALL produced artifacts produced by the same thing. But if you try the same rule to make all, if parallel makes invoked on this, you get
#multiple tries to run the same rule. So remap all to making the first.
$(wordlist 2, 9999, $(PRODUCED_OUTPUT_ARTIFACTS)):	$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS))

$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS)):
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(MAKE) --silent $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
	@$(MAKE) --no-print-directory doBuilds_


ORIGs CURRENT $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_):	$(ZIPPEDFILE)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine  $$(($(MAKE_INDENT_LEVEL)+1)) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "curl ${VERSION} - Extracting $(ZIPPEDFILE) to $@"` "... "
	@mkdir -p ORIGS && rm -rf $@ && mkdir -p $@
ifeq ($(DETECTED_HOST_OS),Darwin)
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
ENVIRONMENT_CONFIGURE_PREFIX+= export ARCH=$(ARCH);
ENVIRONMENT_CONFIGURE_PREFIX+= export CC=$(CC);
ENVIRONMENT_CONFIGURE_PREFIX+= export CXX=$(CXX);
ENVIRONMENT_CONFIGURE_PREFIX+= export AR=$(AR);
ENVIRONMENT_CONFIGURE_PREFIX+= export RANLIB=$(RANLIB);
ENVIRONMENT_CONFIGURE_PREFIX+= export CFLAGS="$(call FUNCTION_QUOTE_QUOTE_CHARACTERS_FOR_SHELL,$(CFLAGS))";
ENVIRONMENT_CONFIGURE_PREFIX+= export CPPFLAGS="$(call FUNCTION_QUOTE_QUOTE_CHARACTERS_FOR_SHELL,$(CPPFLAGS))";
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
ifeq ($(qFeatureFlag_OpenSSL), no)
LIBCURL_CONFIGURE += --without-ssl
else
#finds right one through PKG_CONFIG_PATH
LIBCURL_CONFIGURE += --with-ssl
endif
ifeq ($(qFeatureFlag_ZLib), no)
LIBCURL_CONFIGURE += --without-zlib
else
#LIBCURL_CONFIGURE += --with-zlib=$(PER_CONFIGURATION_BUILDS_DIR)
#finds right one through PKG_CONFIG_PATH
LIBCURL_CONFIGURE += --with-zlib
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
ifeq ($(ENABLE_ASSERTIONS), 1)
LIBCURL_CONFIGURE += --enable-debug
LIBCURL_CONFIGURE += --disable-optimize
LIBCURL_CONFIGURE += --enable-curldebug
else
LIBCURL_CONFIGURE += --disable-debug
LIBCURL_CONFIGURE += --enable-optimize
LIBCURL_CONFIGURE += --disable-curldebug
endif

ifeq ($(findstring $(DETECTED_HOST_OS),MSYS-Cygwin),)
# https://stroika.atlassian.net/browse/STK-759
### On M1 Mac (even if running under rosetta) - really just one mac tested - 
### Get error:
#		undef: _BrotliDecoderDestroyInstance
#		undef: _BrotliDecoderVersion
#		undef: _BrotliDecoderDecompressStream
#		undef: _BrotliDecoderGetErrorCode
#		undef: _BrotliDecoderCreateInstance
#		Undefined symbols for architecture x86_64:
#		"_BrotliDecoderDestroyInstance", referenced from:
#			_brotli_unencode_write in lto.o
#			_brotli_close_writer in lto.o
#		"_BrotliDecoderVersion", referenced from:
#			_curl_version in lto.o
#			_curl_version_info in lto.o
#		"_BrotliDecoderDecompressStream", referenced from:
#			_brotli_unencode_write in lto.o
#		"_BrotliDecoderGetErrorCode", referenced from:
#			_brotli_unencode_write in lto.o
#		"_BrotliDecoderCreateInstance", referenced from:
#			_brotli_init_writer in lto.o
#		ld: symbol(s) not found for architecture x86_64
#
# BUT THEN got cross-compiling for raspberry pi on Ubuntu 22.04, so try a more sophisticated check
ifneq ($(Linker),)
ifeq ($(shell $(StroikaRoot)ScriptsLib/CheckForLibrary "$(Linker) $(EXTRA_PREFIX_LINKER_ARGS) $(EXTRA_SUFFIX_LINKER_ARGS)" brotlicommon),false)
LIBCURL_CONFIGURE += --without-brotli
endif
endif
endif


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

ifeq ($(DETECTED_HOST_OS), Darwin)
# https://stroika.atlassian.net/browse/STK-759
# Undefined symbols for architecture x86_64: "_SCDynamicStoreCopyProxies", referenced from:  _Curl_resolv in libcurl.a(libcurl_la-hostip.o)
LIBCURL_CONFIGURE +=  LIBS='-framework Foundation -framework SystemConfiguration'
endif



	
.PHONY:	doBuilds_
doBuilds_:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
ifeq (Unix,$(ProjectPlatformSubdir))
	@$(MAKE) --no-print-directory check_prerequisites_
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "curl ${VERSION} - Build configure tools (see ${OUTPUT_WORKDIR_PRETTYNAME}BUILD-CONF-TOOLS-OUT.txt) ... "`
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_);autoreconf -fi > BUILD-CONF-TOOLS-OUT.txt 2>&1
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "curl ${VERSION} - Configure (see ${OUTPUT_WORKDIR_PRETTYNAME}CONFIGURE-OUT.txt) ... "`
	@$(ECHO) "$(ENVIRONMENT_CONFIGURE_PREFIX) ./configure --prefix=$(PER_CONFIGURATION_BUILDS_DIR) $(LIBCURL_CONFIGURE)" > $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIGURE-OUT.txt
	@cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && ($(ENVIRONMENT_CONFIGURE_PREFIX) ./configure --prefix=$(PER_CONFIGURATION_BUILDS_DIR) $(LIBCURL_CONFIGURE) 2>&1) >> CONFIGURE-OUT.txt
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "curl $(VERSION) - Compiling (see ${OUTPUT_WORKDIR_PRETTYNAME}BUILD-OUT.txt) ... "`
	@echo $(MAKE) --directory $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) --no-print-directory install CC="$(CC)" > $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD-OUT.txt
	@$(MAKE) --directory $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) --no-print-directory install CC="$(CC)" >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD-OUT.txt 2>&1
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "curl ${VERSION} - Build CURRENT into $(BUILDS_DIR) ... "
	@cd CURRENT && cmd /C buildconf.bat > BUILD-CONFIG-OUT.txt
	@cd CURRENT && rm -f *.obj *.pdb *.lib
	@$(StroikaRoot)RunArgumentsWithCommonBuildVars $(CONFIGURATION) "cd CURRENT/winbuild && MFLAGS= && MAKEFLAGS= && nmake MAKEFLAGS= MFLAGS= -f Makefile.vc mode=static AS='$(AS)' CC='$(CC)' AR='$(WIN_LIB32)'"
	@todo - INCOMPLETE WINDOZE BUILD
endif


check_prerequisites_:
ifeq (Unix,$(ProjectPlatformSubdir))
	@type automake > /dev/null 2>&1 || ($(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) -n "checking tools......... " && $(StroikaRoot)ScriptsLib/GetMessageForMissingTool automake && exit 1)
	@type autoconf > /dev/null 2>&1 || ($(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) -n "checking tools......... " && $(StroikaRoot)ScriptsLib/GetMessageForMissingTool autoconf && exit 1)
	@type autoreconf > /dev/null 2>&1 || ($(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) -n "checking tools......... " && $(StroikaRoot)ScriptsLib/GetMessageForMissingTool autoreconf && exit 1)
	@type libtool > /dev/null 2>&1 || ($(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) -n "checking tools... " && $(StroikaRoot)ScriptsLib/GetMessageForMissingTool libtool && exit 1)
endif


check:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "curl ${VERSION} - Checking ... "
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(StroikaRoot)ScriptsLib/CheckFileExists $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(ECHO) " Stroika/ThirdPartyComponents/curl -    [Succeeded]";


check-prerequisite-tools:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "curl ${VERSION} - Checking Prerequisite tools ... "
	@$(MAKE) --no-print-directory --silent check_prerequisites_
	@$(ECHO) "done";


clean:
ifeq ($(CONFIGURATION),)
	@rm -rf =../../IntermediateFiles/*/ThirdPartyComponents/curl/
else
	@rm -rf $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
endif


clobber:	clean
ifeq ($(CONFIGURATION),)
	@rm -rf CURRENT ORIGs
	@rm -rf $(StroikaRoot)Builds/*/ThirdPartyComponents/lib/{libcurl}* $(StroikaRoot)Builds/*/ThirdPartyComponents/include/curl
else
	@rm -rf $(PRODUCED_OUTPUT_ARTIFACTS) $(PER_CONFIGURATION_BUILDS_DIR)include/curl
endif
