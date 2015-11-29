ifneq ($(CONFIGURATION),)
ProjectPlatformSubdir	=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)
endif



.DEFAULT_GOAL := all
.PHONY:	all clean clobber check doBuild doCreate doPatch
.NOTPARALLEL:

MAKE_INDENT_LEVEL?=$(MAKELEVEL)



DoCreateSymLink=0

-include $(shell pwd)/../../IntermediateFiles/$(CONFIGURATION)/Library/Configuration.mk


VERSION=7_45_0
BASENAME=curl-curl-$(VERSION)
ZIPPEDFILE=../Origs-Cache/$(BASENAME).tar.gz
TARFILE=$(BASENAME).tar
EXTRACTED_DIRNAME=$(BASENAME)
SLINKDIRNAME=$(BASENAME)
USE_DIRNAME=CURRENT

FETCHURLS=
FETCHURLS+=https://github.com/bagder/curl/archive/curl-$(VERSION).tar.gz
FETCHURLS+=http://curl.askapache.com/download/curl-$(VERSION).tar.gz


BUILDS_ROOT=../../Builds/


qFeatureFlag_OpenSSL	=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) qFeatureFlag_OpenSSL)


ifeq (Unix,$(ProjectPlatformSubdir))
CC=$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) CompilerDriver-C)
endif

ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
WIN_CC32=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) CC_32)")
WIN_CC64=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) CC_64)")
WIN_AS32=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) AS_32)")
WIN_AS64=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) AS_64)")
WIN_LIB32=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) LIB_32)")
WIN_LIB64=$(shell cygpath -w "$(shell perl ../../ScriptsLib/PrintEnvVarFromCommonBuildVars.pl $(CONFIGURATION) LIB_64)")
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
	@../../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && echo "Building Stroika/ThirdPartyComponents/libcurl $(VERSION):"
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(MAKE) --no-print-directory --silent CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) check


$(ZIPPEDFILE):
	@../../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && echo -n "..."
	@../../ScriptsLib/WebGet.sh $(ZIPPEDFILE) $(FETCHURLS)


$(PRODUCED_OUTPUT_ARTIFACTS):
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(MAKE) --no-print-directory doCreate CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) 
	@$(MAKE) --no-print-directory doBuilds CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) 


doCreate:	$(ZIPPEDFILE)
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo -n "Creating CURRENT...";
	@rm -rf CURRENT
	@tar xf $(ZIPPEDFILE) --no-same-owner
	@mv $(EXTRACTED_DIRNAME) CURRENT
	@echo "done"


#note - we override PKG_CONFIG because  with-ssl=$(shell realpath ../../Builds/$(CONFIGURATION)/ThirdPartyComponents/include/openssl/)
#didn't work (failed to find crypto lib)
PKG_CONFIG=
ifeq ($(qFeatureFlag_OpenSSL), use)
ifeq (Unix,$(ProjectPlatformSubdir))
PKG_CONFIG=$(shell realpath `pwd`/../../Builds/$(CONFIGURATION))/ThirdPartyComponents/lib/pkgconfig
endif
endif

PKG_CONFIG_CONFIGURE_PREFIX=
ifneq (,$(PKG_CONFIG))
PKG_CONFIG_CONFIGURE_PREFIX=	env PKG_CONFIG=$(PKG_CONFIG)
endif




LIBCURL_CONFIGURE =
LIBCURL_CONFIGURE += --enable-static
ifeq ($(qFeatureFlag_OpenSSL), use)
#finds right one through PKG_CONFIG
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
ifeq ($(qFeatureFlag_EnableAssertions), 1)
LIBCURL_CONFIGURE += --enable-debug
LIBCURL_CONFIGURE += --disable-optimize
LIBCURL_CONFIGURE += --enable-curldebug
else
LIBCURL_CONFIGURE += --disable-debug
LIBCURL_CONFIGURE += --enable-optimize
LIBCURL_CONFIGURE += --disable-curldebug
endif


doBuilds:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
ifeq (Unix,$(ProjectPlatformSubdir))
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo "Building libcurl CURRENT..."
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo "into $(BUILDS_DIR)..."
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo "checking tools..."
	@$(MAKE) --no-print-directory check_prerequisites_ CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL) 
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo "see $(shell pwd)/CURRENT/BUILD-CONF-TOOLS.OUT for details..."
	@cd CURRENT;./buildconf > BUILD-CONF-TOOLS.OUT 2>&1
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo "see $(shell pwd)/CURRENT/CONFIGURE.OUT for details..."
	@echo "CC=$(CC) $(PKG_CONFIG_CONFIGURE_PREFIX)  ./configure --prefix=$(BUILDS_DIR) $(LIBCURL_CONFIGURE)" > CURRENT/CONFIGURE.OUT
	@cd CURRENT && (CC="$(CC)" $(PKG_CONFIG_CONFIGURE_PREFIX) ./configure --prefix=$(BUILDS_DIR) $(LIBCURL_CONFIGURE) 2>&1) >> CONFIGURE.OUT
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo "see $(shell pwd)/CURRENT/MAKE.OUT for details..."
	@$(MAKE) --directory CURRENT --no-print-directory install CC="$(CC)" > CURRENT/MAKE.OUT 2>&1
	@mkdir -p $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents && cp Builds/bin/* $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/
	@mkdir -p $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib && cp -r Builds/lib/*.a Builds/lib/*.la Builds/lib/pkgconfig $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include && cp -r --dereference Builds/include/curl $(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo ".Building libcurl CURRENT into $(BUILDS_DIR)..."
	@rm -rf $(BUILDS_DIR)
	@cd CURRENT && cmd /C buildconf.bat > BUILD-CONFIG.OUT
	@cd CURRENT && rm -f *.obj *.pdb *.lib
	@perl ../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) "cd CURRENT/winbuild && MFLAGS= && MAKEFLAGS= && nmake MAKEFLAGS= MFLAGS= -f Makefile.vc mode=static AS='$(WIN_AS32)' CC='$(WIN_CC32)' AR='$(WIN_LIB32)'"
	@todo - INCOMPLETE WINDOZE BUILD
endif


check_prerequisites_:
	@type automake > /dev/null || echo "......Missing automake"
	@type libtool > /dev/null || echo "......Missing libtool"


check:
	@../../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && echo -n "Checking..."
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@for i in $(PRODUCED_OUTPUT_ARTIFACTS) ; do \
		if [ ! -e $$i ]; then\
			echo "FAILED: missing $$i";\
			exit 1;\
		fi;\
	done
	@echo " Stroika/ThirdPartyComponents/curl -    [Succeeded]";


clean:
	@rm -rf CURRENT


clobber:	clean
	@rm -rf $(BUILDS_DIR)
ifeq ($(CONFIGURATION),)
	@rm -rf $(BUILDS_ROOT)*/ThirdPartyComponents/lib/{libcurl}* $(BUILDS_ROOT)*/ThirdPartyComponents/include/curl
else
	@rm -f $(PRODUCED_OUTPUT_ARTIFACTS)
endif
