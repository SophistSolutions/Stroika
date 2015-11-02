CONFIGURATION 			?=	$(shell perl ../../ScriptsLib/GetDefaultConfiguration.pl)
ProjectPlatformSubdir	=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)


.PHONY:	all
.DEFAULT_GOAL := all

DoCreateSymLink=0

-include $(shell pwd)/../../IntermediateFiles/DefaultConfiguration/Library/Configuration.mk


VERSION=7_45_0
BASENAME=curl-curl-$(VERSION)
ZIPPEDFILE=../Origs-Cache/$(BASENAME).tar.gz
TARFILE=$(BASENAME).tar
EXTRACTED_DIRNAME=$(BASENAME)
SLINKDIRNAME=$(BASENAME)
USE_DIRNAME=CURRENT

FETCHURL=https://github.com/bagder/curl/archive/curl-$(VERSION).tar.gz

BUILDS_ROOT=../../Builds/


qFeatureFlag_OpenSSL	=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) qFeatureFlag_OpenSSL)


ifeq (Unix,$(ProjectPlatformSubdir))
CC=$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) CompilerDriver-C)
endif

ifneq (,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
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
	@echo "   Building Stroika/ThirdPartyProducts/libcurl $(VERSION)..."
	@$(MAKE) --no-print-directory doCreate
	@$(MAKE) --no-print-directory doBuilds
	@$(MAKE) --no-print-directory check

$(ZIPPEDFILE):
	@echo -n "      ..."
	wget --quiet --tries=10 -O $(ZIPPEDFILE) $(FETCHURL) || (rm -f $(ZIPPEDFILE) && false)


$(TRGDIR):	$(ZIPPEDFILE)
	@$(MAKE) --no-print-directory doCreate


doCreate:	$(ZIPPEDFILE)
	@if [ -e  $(TRGDIR) ]; then\
		echo "      ...CURRENT dir already exists";\
	else\
		echo "      ...Creating $(TRGDIR)...";\
		tar xf $(ZIPPEDFILE) --no-same-owner;\
		mv $(EXTRACTED_DIRNAME) $(TRGDIR);\
	fi


$(BUILDS_DIR):	$(TRGDIR)
	make --no-print-directory doBuilds


PKG_CONFIG=
ifeq ($(qFeatureFlag_OpenSSL), use)
ifeq (Unix,$(ProjectPlatformSubdir))
PKG_CONFIG=$(shell realpath `pwd`/../../Builds/$(CONFIGURATION))/ThirdPartyLibs/lib/pkgconfig
endif
endif

PKG_CONFIG_CONFIGURE_PREFIX=
ifneq (,$(PKG_CONFIG))
PKG_CONFIG_CONFIGURE_PREFIX=	env PKG_CONFIG=$(PKG_CONFIG)
endif




LIBCURL_CONFIGURE =
LIBCURL_CONFIGURE += --enable-static
ifeq ($(qFeatureFlag_OpenSSL), use)
#SOMETHING LIKE THIS BUT NOT WORKING YET --LGP 2015-11-01
#LIBCURL_CONFIGURE += --with-ssl=$(shell realpath ../../Builds/DefaultConfiguration/ThirdPartyLibs/include/openssl/)
#LIBCURL_CONFIGURE += --without-ssl
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
ifneq (Unix,$(ProjectPlatformSubdir))
	@echo "      ....Building libcurl $(TRGDIR) into $(BUILDS_DIR)..."
	@rm -rf $(BUILDS_DIR)
	cd CURRENT && cmd /C buildconf.bat > BUILD-CONFIG.OUT
	@cd $(TRGDIR) && rm -f *.obj *.pdb *.lib

	@perl ../../ScriptsLib/RunArgumentsWithCommonBuildVars.pl "cd $(TRGDIR)/winbuild && MFLAGS= && MAKEFLAGS= && nmake MAKEFLAGS= MFLAGS= -f Makefile.vc mode=static AS='$(WIN_AS32)' CC='$(WIN_CC32)' AR='$(WIN_LIB32)'"
else
	@if [ -e  $(BUILDS_DIR) ]; then\
		echo "      ...Libcurl in $(BUILDS_DIR) complete";\
	else\
		echo "      ...Building libcurl $(TRGDIR)...";\
		echo "         ...into $(BUILDS_DIR)...";\
		echo "         ...checking tools...";\
		$(MAKE) --no-print-directory check_prerequisites_;\
		echo "         ...see $(shell pwd)/CURRENT/BUILD-CONF-TOOLS.OUT for details...";\
		cd CURRENT;\
		./buildconf > BUILD-CONF-TOOLS.OUT 2>&1;\
		echo "         ...TODO - fix to use my copy of ssl, if we build SSL...";\
		echo "         ...see $(shell pwd)/CURRENT/CONFIGURE.OUT for details...";\
		echo "CC=$(CC) $(PKG_CONFIG_CONFIGURE_PREFIX)  ./configure --prefix=$(BUILDS_DIR) $(LIBCURL_CONFIGURE)" > CONFIGURE.OUT;\
		(CC="$(CC)" $(PKG_CONFIG_CONFIGURE_PREFIX) ./configure --prefix=$(BUILDS_DIR) $(LIBCURL_CONFIGURE) 2>&1) >> CONFIGURE.OUT;\
		echo "         ...see $(shell pwd)/CURRENT/MAKE.OUT for details...";\
		make --no-print-directory install CC="$(CC)" > MAKE.OUT 2>&1;\
	fi
endif
ifeq (Unix,$(ProjectPlatformSubdir))
	@mkdir -p $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs && cp Builds/bin/* $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/lib && cp -r Builds/lib/*.a Builds/lib/*.la  Builds/lib/pkgconfig $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/lib/
	@mkdir -p $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/include && cp -r --dereference Builds/include/curl $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/include/
else
	@todo - INCOMPLETE WINDOZE BUILD
endif


check_prerequisites_:
	@type automake > /dev/null || echo "......Missing automake"
	@type libtool > /dev/null || echo "......Missing libtool"


check:
	@##THMPHACK
	@echo -n "      ...Checking..."
	@echo "         ThirdPartyLibs/curl - [Succeeded]"

clean:
	@rm -rf $(TRGDIR)

clobber:
	@rm -rf $(BUILDS_DIR)
	@rm -rf $(TRGDIR)
