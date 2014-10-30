#
# ROUGH DRAFT - STARTING TO MOVE AWAY FROM OLD PERL STUFF
#
.PHONY:	all clean clobber check

VERSION=1.0.1j
BASENAME=openssl-$(VERSION)
TRGDIR=CURRENT
EXTRACTED_DIRNAME=$(BASENAME)

ZIPEDFILE=../Origs-Cache/$(BASENAME).tar.gz

FETCHURL=http://www.openssl.org/source/$(BASENAME).tar.gz

UNAME := $(shell uname)

# COULD make this conditional
STRIP_ANNOYING_MESSAGES=		1

all:
	@echo Building Stroika/ThirdPartyLibs/OpenSSL $(VERSION)...
	@$(MAKE) --no-print-directory doCreate
	@$(MAKE) --no-print-directory doBuilds
	@$(MAKE) --no-print-directory check

clean:
	@rm -rf $(TRGDIR)

check:
	@./checkall_.pl

clobber:
	@rm -rf $(TRGDIR) ORIG

$(ZIPEDFILE):
	@echo -n "   ..." 
	wget --quiet --tries=10 --no-check-certificate -O $(ZIPEDFILE) $(FETCHURL) || (rm -f $(ZIPEDFILE) && false)

$(TRGDIR):	$(ZIPPEDFILE)
	@$(MAKE) --no-print-directory doCreate


doCreate:	$(ZIPEDFILE)
	@echo "   ...Extract $(ZIPEDFILE) to $(TRGDIR)..."
	@tar xf $(ZIPEDFILE) --no-same-owner
	@sleep 1;	# hack cuz sometimes it appears command not fully done writing - and we get sporadic failures on next stop on win7
	@mv $(EXTRACTED_DIRNAME) $(TRGDIR)
ifneq (,$(findstring CYGWIN,$(UNAME)))
	@echo "   ...Patching OpenSSL..."
endif
ifneq (,$(findstring CYGWIN,$(UNAME)))
	@dos2unix -q Patches/VC-32.pl-Z7InsteadOfZIDebugSys.PATCH
	@patch -p0 CURRENT/util/pl/VC-32.pl < Patches/VC-32.pl-Z7InsteadOfZIDebugSys.PATCH
	@unix2dos -q Patches/VC-32.pl-Z7InsteadOfZIDebugSys.PATCH
endif
ifneq (,$(findstring CYGWIN,$(UNAME)))
ifeq (1,$(STRIP_ANNOYING_MESSAGES))
	@dos2unix -q Patches/LosePRINTOfCopyingInUtils.PATCH
	@patch -p0 CURRENT/util/copy.pl < Patches/LosePRINTOfCopyingInUtils.PATCH
	@unix2dos -q Patches/LosePRINTOfCopyingInUtils.PATCH
endif
endif

doBuilds:
	@echo "   ...Buidling, and redirecting messages to BUILD.OUT ..."
	@./buildall_.pl > BUILD.OUT 2>&1


ORIG:	$(ZIPEDFILE)
	@tar xf $(ZIPEDFILE) --no-same-owner
	@mv $(EXTRACTED_DIRNAME) ORIGS
