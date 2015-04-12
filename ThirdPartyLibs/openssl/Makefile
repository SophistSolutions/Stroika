#
# ROUGH DRAFT - STARTING TO MOVE AWAY FROM OLD PERL STUFF
#
.PHONY:	all clean clobber check

VERSION=1.0.1m
BASENAME=openssl-$(VERSION)
TRGDIR=CURRENT
EXTRACTED_DIRNAME=$(BASENAME)

ZIPEDFILE=../Origs-Cache/$(BASENAME).tar.gz

FETCHURL=http://www.openssl.org/source/$(BASENAME).tar.gz
	
UNAME := $(shell uname)

BUILDS_ROOT=../../Builds/

ifeq (,$(findstring CYGWIN,$(UNAME)))
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/libcrypto.a			\
	$(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/libssl.a				\
	$(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/openssl				\
	$(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/includes/openssl/aes.h
else
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/libeay32.lib						\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/ssleay32.lib						\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/openssl.exe						\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/openssl.pdb						\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/includes/openssl/aes.h				\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/libeay32.lib						\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/ssleay32.lib						\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/openssl.exe						\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/openssl.pdb						\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/includes/openssl/aes.h				\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/libeay32.lib			\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/ssleay32.lib			\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/openssl.exe			\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/openssl.pdb			\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/includes/openssl/aes.h	\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/libeay32.lib				\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/ssleay32.lib				\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/openssl.exe				\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/openssl.pdb				\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/includes/openssl/aes.h		\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/libeay32.lib						\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/ssleay32.lib						\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/openssl.exe						\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/openssl.pdb						\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/includes/openssl/aes.h				\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/libeay32.lib						\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/ssleay32.lib						\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/openssl.exe						\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/openssl.pdb						\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/includes/openssl/aes.h				\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/libeay32.lib				\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/ssleay32.lib				\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/openssl.exe				\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/openssl.pdb				\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/includes/openssl/aes.h
endif


# COULD make this conditional
STRIP_ANNOYING_MESSAGES=		1

all:
	@echo Building Stroika/ThirdPartyLibs/OpenSSL $(VERSION)...
	@$(MAKE) --no-print-directory doCreate
	@$(MAKE) --no-print-directory doBuilds
	@$(MAKE) --no-print-directory check

clean:
	@rm -rf $(TRGDIR)

clobber:	clean
	@rm -f BUILD.OUT

check:
	@echo -n "   ...Checking..."
	@for i in $(PRODUCED_OUTPUT_ARTIFACTS) ; do \
		if [ ! -e $$i ]; then\
			echo "   FAILED: missing $$i";\
			exit 1;\
		fi;\
	done
	@echo "   Stroika/ThirdPartyLibs/OpenSSL - [Succeeded]";

$(ZIPEDFILE):
	@echo -n "   ..." 
	wget --quiet --tries=10 --no-check-certificate -O $(ZIPEDFILE) $(FETCHURL) || (rm -f $(ZIPEDFILE) && false)

$(TRGDIR):	$(ZIPPEDFILE)
	@$(MAKE) --no-print-directory doCreate


doPatch:
	@echo -n "   ...Patching OpenSSL...   "
ifneq (,$(findstring CYGWIN,$(UNAME)))
	@echo -n .
	@patch --quiet -p0 CURRENT/util/pl/VC-32.pl < Patches/VC-32.pl-Z7InsteadOfZIDebugSys.PATCH
endif
ifneq (,$(findstring CYGWIN,$(UNAME)))
ifeq (1,$(STRIP_ANNOYING_MESSAGES))
	@echo -n .
	@patch --quiet -p0 CURRENT/util/copy.pl < Patches/LosePRINTOfCopyingInUtils.PATCH
endif
endif
	@echo "   ...done"

doCreate:	$(ZIPEDFILE)
	@if [ -e  $(TRGDIR) ]; then\
		echo "   ...CURRENT dir already exists";\
	else\
		echo "   ...Extract $(ZIPEDFILE) to $(TRGDIR)...";\
		tar xf $(ZIPEDFILE) --no-same-owner;\
		sleep 1;	# hack cuz sometimes it appears command not fully done writing - and we get sporadic failures on next stop on win7;\
		mv $(EXTRACTED_DIRNAME) $(TRGDIR);\
		$(MAKE) --no-print-directory doPatch;\
	fi

doBuilds:
	@if [ -e  BUILD.OUT ]; then\
		echo "   ...BUILD.OUT already exists";\
	else\
		echo "   ...Buidling, and redirecting messages to BUILD.OUT ...";\
		./buildall_.pl > BUILD.OUT 2>&1;\
	fi
	@echo "   ...Copying builds to ... $(BUILDS_ROOT)"
ifeq (,$(findstring CYGWIN,$(shell uname)))
	@mkdir -p $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs && cp CURRENT/libcrypto.a CURRENT/libssl.a CURRENT/app/openssl $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/
else
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs && cp -r CURRENT/Builds/Debug32/* $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs && cp -r CURRENT/Builds/Release32/* $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs && cp -r CURRENT/Builds/Release32/* $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs && cp -r CURRENT/Builds/Release32/* $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs && cp -r CURRENT/Builds/Debug64/* $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs && cp -r CURRENT/Builds/Release64/* $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs && cp -r CURRENT/Builds/Release64/* $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/
endif


ORIG:	$(ZIPEDFILE)
	@tar xf $(ZIPEDFILE) --no-same-owner
	@mv $(EXTRACTED_DIRNAME) ORIGS
