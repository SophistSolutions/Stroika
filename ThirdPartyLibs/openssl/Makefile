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


CONFIG_ARGS=
ifeq (,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
CONFIG_ARGS+=	-no-shared
CONFIG_ARGS+=	--prefix=$(shell realpath `pwd`/../../)/Builds/$(CONFIGURATION)/ThirdPartyLibs/
else
endif

ifeq (,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/lib/libcrypto.a				\
	$(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/lib/libssl.a					\
	$(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/openssl						\
	$(BUILDS_ROOT)DefaultConfiguration/ThirdPartyLibs/include/openssl/aes.h
else
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/lib/libeay32.lib						\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/lib/ssleay32.lib						\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/lib/lib.pdb							\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/openssl.exe							\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/openssl.pdb							\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/include/openssl/aes.h					\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/lib/libeay32.lib						\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/lib/ssleay32.lib						\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/lib/lib.pdb							\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/openssl.exe							\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/openssl.pdb							\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/include/openssl/aes.h					\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/lib/libeay32.lib			\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/lib/ssleay32.lib			\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/lib/lib.pdb				\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/openssl.exe				\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/openssl.pdb				\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/include/openssl/aes.h		\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/lib/libeay32.lib				\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/lib/ssleay32.lib				\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/lib/lib.pdb					\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/openssl.exe					\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/openssl.pdb					\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/include/openssl/aes.h			\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/lib/libeay32.lib						\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/lib/ssleay32.lib						\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/lib/lib.pdb							\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/openssl.exe							\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/openssl.pdb							\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/include/openssl/aes.h					\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/lib/libeay32.lib						\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/lib/ssleay32.lib						\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/lib/lib.pdb							\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/openssl.exe							\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/openssl.pdb							\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/include/openssl/aes.h					\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/lib/libeay32.lib				\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/lib/ssleay32.lib				\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/lib/lib.pdb					\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/openssl.exe					\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/openssl.pdb					\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/include/openssl/aes.h
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
	@rm -f BUILD.OUT INSTALL.OUT CONFIG.OUT

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
		echo "  Test DIFFS";\
		echo "    Diff should be less than 2236 to be safe.";\
		echo "      diff -b CURRENT/TEST-OUT.txt REFERENCE_UNIX_OUTPUT.txt | wc -l: $$X1";\
		exit 1;\
	 fi
else
	@X1=`diff -b CURRENT/TEST32-OUT.txt REFERENCE_OUTPUT.txt | wc -l`;\
	 X2=`diff -b CURRENT/TEST32-DBG-OUT.txt REFERENCE_OUTPUT.txt | wc -l`;\
	 if [ $$X1 -gt 50 ] || [ $$X2 -gt 50 ]; then\
		echo "  Test DIFFS";\
		echo "    2 lines coming next - each should be less than 50/50 to be safe.";\
		echo "      diff -b CURRENT/TEST32-OUT.txt REFERENCE_OUTPUT.txt | wc -l: $$X1";\
		echo "      diff -b CURRENT/TEST32-DBG-OUT.txt REFERENCE_OUTPUT.txt | wc -l: $$X2";\
		exit 1;\
	 fi
	@X1=`diff -b CURRENT/TEST64-OUT.txt REFERENCE_OUTPUT.txt | wc -l`;\
	 X2=`diff -b CURRENT/TEST64-DBG-OUT.txt REFERENCE_OUTPUT.txt | wc -l`;\
	 if [ $$X1 -gt 50 ] || [ $$X2 -gt 50 ]; then\
		echo "  Test DIFFS";\
		echo "    2 lines coming next - each should be less than 50/50 to be safe.";\
		echo "      diff -b CURRENT/TEST64-OUT.txt REFERENCE_OUTPUT.txt | wc -l: $$X1";\
		echo "      diff -b CURRENT/TEST64-DBG-OUT.txt REFERENCE_OUTPUT.txt | wc -l: $$X2";\
		exit 1;\
	 fi
endif
	@echo "   Stroika/ThirdPartyLibs/OpenSSL - [Succeeded]";

$(ZIPEDFILE):
	@echo -n "      ..."
	wget --quiet --tries=10 -O $(ZIPEDFILE) $(FETCHURL) || (rm -f $(ZIPEDFILE) && false)

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
ifeq (,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@if [ -e  BUILD.OUT ]; then\
		echo "      ...BUILD.OUT already exists";\
	else\
		echo "      ...Creating CURRENT/CONFIG.OUT - new configuration";\
		BASE_CONFIG_LINE=`cd CURRENT && ./config -t | tail -1`;\
		echo "cd CURRENT ; $$BASE_CONFIG_LINE $(CONFIG_ARGS)" > CONFIG.OUT;\
		(cd CURRENT ; $$BASE_CONFIG_LINE $(CONFIG_ARGS)) >> CONFIG.OUT 2>&1;\
		echo "      ...Buidling, and redirecting messages to BUILD.OUT ...";\
		$(MAKE) --directory CURRENT --no-print-directory all > BUILD.OUT 2>&1;\
		echo "      ...Installing (to CURRENT/Builds), and redirecting messages to INSTALL.OUT ...";\
		$(MAKE) -j1 --directory CURRENT --no-print-directory install > INSTALL.OUT 2>&1;\
		echo "      ...Running openssl tests (OUTPUT TO CURRENT/TEST-OUT.txt)...";\
		($(MAKE) --directory CURRENT --no-print-directory -s rehash 2>&1) > /dev/null;\
		($(MAKE) -j1 --directory CURRENT --no-print-directory -s test > CURRENT/TEST-OUT.txt 2>&1);\
	fi
else
	@if [ -e  BUILD.OUT ]; then\
		echo "      ...BUILD.OUT already exists";\
	else\
		echo "      ...Buidling, and redirecting messages to BUILD.OUT ...";\
		./buildall_windows_.pl > BUILD.OUT 2>&1;\
	fi
endif
	@echo "      ...Copying builds to $(BUILDS_ROOT) ..."
ifneq (,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs && cp CURRENT/Builds/Debug32/openssl.* $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/lib && cp -r CURRENT/Builds/Debug32/libeay32.* CURRENT/Builds/Debug32/ssleay32.* CURRENT/Builds/Debug32/lib.pdb $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/lib/
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/include && cp -r --dereference CURRENT/include/openssl $(BUILDS_ROOT)Debug-U-32/ThirdPartyLibs/include/
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs && cp CURRENT/Builds/Release32/openssl.* $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/lib && cp -r CURRENT/Builds/Release32/libeay32.* CURRENT/Builds/Release32/ssleay32.* CURRENT/Builds/Release32/lib.pdb $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/lib/
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/include && cp -r --dereference CURRENT/include/openssl $(BUILDS_ROOT)Release-U-32/ThirdPartyLibs/include/
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs && cp CURRENT/Builds/Release32/openssl.* $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/lib && cp -r CURRENT/Builds/Release32/libeay32.* CURRENT/Builds/Release32/ssleay32.* CURRENT/Builds/Release32/lib.pdb $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/lib/
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/include && cp -r --dereference CURRENT/include/openssl $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyLibs/include/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs && cp CURRENT/Builds/Release32/openssl.* $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/lib && cp -r CURRENT/Builds/Release32/libeay32.* CURRENT/Builds/Release32/ssleay32.* CURRENT/Builds/Release32/lib.pdb $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/lib/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/include && cp -r --dereference CURRENT/include/openssl $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyLibs/include/
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs && cp CURRENT/Builds/Debug64/openssl.* $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/lib && cp -r CURRENT/Builds/Debug64/libeay32.* CURRENT/Builds/Debug64/ssleay32.* CURRENT/Builds/Debug64/lib.pdb $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/lib/
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/include && cp -r --dereference CURRENT/include/openssl $(BUILDS_ROOT)Debug-U-64/ThirdPartyLibs/include/
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs && cp CURRENT/Builds/Release64/openssl.* $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/lib && cp -r CURRENT/Builds/Release64/libeay32.* CURRENT/Builds/Release64/ssleay32.* CURRENT/Builds/Release64/lib.pdb $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/lib/
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/include && cp -r --dereference CURRENT/include/openssl $(BUILDS_ROOT)Release-U-64/ThirdPartyLibs/include/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs && cp CURRENT/Builds/Release64/openssl.* $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/lib && cp -r CURRENT/Builds/Release64/libeay32.* CURRENT/Builds/Release64/ssleay32.* CURRENT/Builds/Release64/lib.pdb $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/lib/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/include && cp -r --dereference CURRENT/include/openssl $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyLibs/include/
endif


ORIG:	$(ZIPEDFILE)
	@tar $(BAD_TARFILE_BUG_WORKAROUND_)xf $(ZIPEDFILE) --no-same-owner
	@mv $(EXTRACTED_DIRNAME) ORIGS

