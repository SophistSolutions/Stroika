#
# ROUGH DRAFT - STARTING TO MOVE AWAY FROM OLD PERL STUFF
#
.PHONY:	all clean clobber check

#VERSION=1.0.1o
VERSION=1.0.2d

#appears needed for 1.0.2d
BAD_TARFILE_BUG_WORKAROUND_=i

BASENAME=openssl-$(VERSION)
TRGDIR=CURRENT
EXTRACTED_DIRNAME=$(BASENAME)

CONFIGURATION			?=   $(shell perl ../../ScriptsLib/GetDefaultConfiguration.pl)
ProjectPlatformSubdir	=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)

ZIPEDFILE=../Origs-Cache/$(BASENAME).tar.gz

FETCHURL=http://www.openssl.org/source/$(BASENAME).tar.gz
	
BUILDS_ROOT=../../Builds/

ifeq (,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Libs/libcrypto.a				\
	$(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Libs/libssl.a					\
	$(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/openssl						\
	$(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Includes/openssl/aes.h
else
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Libs/libeay32.lib						\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Libs/ssleay32.lib						\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Libs/lib.pdb							\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/openssl.exe							\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/openssl.pdb							\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Includes/openssl/aes.h					\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Libs/libeay32.lib						\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Libs/ssleay32.lib						\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Libs/lib.pdb							\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/openssl.exe							\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/openssl.pdb							\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Includes/openssl/aes.h					\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Libs/libeay32.lib			\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Libs/ssleay32.lib			\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Libs/lib.pdb				\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/openssl.exe				\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/openssl.pdb				\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Includes/openssl/aes.h		\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Libs/libeay32.lib				\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Libs/ssleay32.lib				\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Libs/lib.pdb					\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/openssl.exe					\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/openssl.pdb					\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Includes/openssl/aes.h			\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Libs/libeay32.lib						\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Libs/ssleay32.lib						\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Libs/lib.pdb							\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/openssl.exe							\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/openssl.pdb							\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Includes/openssl/aes.h					\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Libs/libeay32.lib						\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Libs/ssleay32.lib						\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Libs/lib.pdb							\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/openssl.exe							\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/openssl.pdb							\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Includes/openssl/aes.h					\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Libs/libeay32.lib				\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Libs/ssleay32.lib				\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Libs/lib.pdb					\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/openssl.exe					\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/openssl.pdb					\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Includes/openssl/aes.h
endif


# COULD make this conditional
STRIP_ANNOYING_MESSAGES=		1

all:		$(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory check


$(PRODUCED_OUTPUT_ARTIFACTS):	CURRENT/Builds
	
CURRENT/Builds:
	@echo "   Building Stroika/ThirdPartyLibs/OpenSSL $(VERSION)..."
	@$(MAKE) --no-print-directory doCreate
	@$(MAKE) --no-print-directory doBuilds


clean:
	@rm -rf $(TRGDIR)

clobber:	clean
	@rm -f BUILD.OUT

check:
	@echo -n "      ...Checking..."
	@for i in $(PRODUCED_OUTPUT_ARTIFACTS) ; do \
		if [ ! -e $$i ]; then\
			echo "      FAILED: missing $$i";\
			exit 1;\
		fi;\
	done
ifeq (,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@X1=`diff -b CURRENT/TEST-OUT.txt REFERENCE_UNIX_OUTPUT.txt | wc -l`;\
	 if [ $$X1 -gt 2236 ]; then\
		echo "  Test DIFFS (REDO THIS IN PERL WHERE ITS EASIER TO COUNT LINES ETC.";\
		echo "    Diff should be less than 2236 to be safe.";\
		echo "      diff -b CURRENT/TEST-OUT.txt REFERENCE_UNIX_OUTPUT.txt | wc -l: $$X1";\
		exit 1;\
	 fi
else
	@X1=`diff -b CURRENT/TEST-OUT.txt REFERENCE_OUTPUT.txt | wc -l`;\
	 X2=`diff -b CURRENT/TEST-DBG-OUT.txt REFERENCE_OUTPUT.txt | wc -l`;\
	 if [ $$X1 -gt 50 ] || [ $$X2 -gt 50 ]; then\
		echo "  Test DIFFS (REDO THIS IN PERL WHERE ITS EASIER TO COUNT LINES ETC.";\
		echo "    2 lines coming next - each should be less than 50/50 to be safe.";\
		echo "      diff -b CURRENT/TEST-OUT.txt REFERENCE_OUTPUT.txt | wc -l: $$X1";\
		echo "      diff -b CURRENT/TEST-DBG-OUT.txt REFERENCE_OUTPUT.txt | wc -l: $$X2";\
		exit 1;\
	 fi
endif
	@echo "   Stroika/ThirdPartyLibs/OpenSSL - [Succeeded]";

$(ZIPEDFILE):
	@echo -n "      ..." 
	wget --quiet --tries=10 --no-check-certificate -O $(ZIPEDFILE) $(FETCHURL) || (rm -f $(ZIPEDFILE) && false)

$(TRGDIR):	$(ZIPPEDFILE)
	@$(MAKE) --no-print-directory doCreate


doPatch:
	@echo -n "      ...Patching OpenSSL...   "
ifneq (,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
ifeq (1,$(STRIP_ANNOYING_MESSAGES))
	@echo -n .
	@patch --quiet -p0 CURRENT/util/copy.pl < Patches/LosePRINTOfCopyingInUtils.PATCH
endif
endif
	@echo "done"

doCreate:	$(ZIPEDFILE)
	@if [ -e  $(TRGDIR) ]; then\
		echo "      ...CURRENT dir already exists";\
	else\
		echo "      ...Extract $(ZIPEDFILE) to $(TRGDIR)...";\
		tar $(BAD_TARFILE_BUG_WORKAROUND_)xf $(ZIPEDFILE) --no-same-owner;\
		sleep 1;	# hack cuz sometimes it appears command not fully done writing - and we get sporadic failures on next stop on win7;\
		mv $(EXTRACTED_DIRNAME) $(TRGDIR);\
		$(MAKE) --no-print-directory doPatch;\
	fi

doBuilds:
	@if [ -e  BUILD.OUT ]; then\
		echo "      ...BUILD.OUT already exists";\
	else\
		echo "      ...Buidling, and redirecting messages to BUILD.OUT ...";\
		./buildall_.pl > BUILD.OUT 2>&1;\
	fi
	@echo "      ...Copying builds to ... $(BUILDS_ROOT)"
ifeq (,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@mkdir -p $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs && cp CURRENT/apps/openssl $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Libs && cp CURRENT/libcrypto.a CURRENT/libssl.a  $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Includes && cp -r --dereference CURRENT/include/openssl $(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/Includes/
else
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs && cp CURRENT/Builds/Debug32/openssl.* $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Libs && cp -r CURRENT/Builds/Debug32/libeay32.* CURRENT/Builds/Debug32/ssleay32.* CURRENT/Builds/Debug32/lib.pdb $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Includes && cp -r --dereference CURRENT/include/openssl $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/Includes/
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs && cp CURRENT/Builds/Release32/openssl.* $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Libs && cp -r CURRENT/Builds/Release32/libeay32.* CURRENT/Builds/Release32/ssleay32.* CURRENT/Builds/Release32/lib.pdb $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Includes && cp -r --dereference CURRENT/include/openssl $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/Includes/
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs && cp CURRENT/Builds/Release32/openssl.* $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Libs && cp -r CURRENT/Builds/Release32/libeay32.* CURRENT/Builds/Release32/ssleay32.* CURRENT/Builds/Release32/lib.pdb $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Includes && cp -r --dereference CURRENT/include/openssl $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/Includes/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs && cp CURRENT/Builds/Release32/openssl.* $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Libs && cp -r CURRENT/Builds/Release32/libeay32.* CURRENT/Builds/Release32/ssleay32.* CURRENT/Builds/Release32/lib.pdb $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Includes && cp -r --dereference CURRENT/include/openssl $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/Includes/
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs && cp CURRENT/Builds/Debug64/openssl.* $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Libs && cp -r CURRENT/Builds/Debug64/libeay32.* CURRENT/Builds/Debug64/ssleay32.* CURRENT/Builds/Debug64/lib.pdb $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Includes && cp -r --dereference CURRENT/include/openssl $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/Includes/
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs && cp CURRENT/Builds/Release64/openssl.* $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Libs && cp -r CURRENT/Builds/Release64/libeay32.* CURRENT/Builds/Release64/ssleay32.* CURRENT/Builds/Release64/lib.pdb $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Includes && cp -r --dereference CURRENT/include/openssl $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/Includes/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs && cp CURRENT/Builds/Release64/openssl.* $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Libs && cp -r CURRENT/Builds/Release64/libeay32.* CURRENT/Builds/Release64/ssleay32.* CURRENT/Builds/Release64/lib.pdb $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Libs/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Includes && cp -r --dereference CURRENT/include/openssl $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/Includes/
endif


ORIG:	$(ZIPEDFILE)
	@tar $(BAD_TARFILE_BUG_WORKAROUND_)xf $(ZIPEDFILE) --no-same-owner
	@mv $(EXTRACTED_DIRNAME) ORIGS
