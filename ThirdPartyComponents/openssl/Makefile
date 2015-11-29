#
# ROUGH DRAFT - STARTING TO MOVE AWAY FROM OLD PERL STUFF
#

.DEFAULT_GOAL := all
.PHONY:	all clean clobber check doBuild doCreate doPatch
.NOTPARALLEL:

MAKE_INDENT_LEVEL?=$(MAKELEVEL)

#VERSION=1.0.1o
VERSION=1.0.2d

#appears needed for 1.0.2d
BAD_TARFILE_BUG_WORKAROUND_=i

BASENAME=openssl-$(VERSION)
EXTRACTED_DIRNAME=$(BASENAME)

ProjectPlatformSubdir	=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) ProjectPlatformSubdir)
CrossCompiling			=	$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) CrossCompiling)

CONFIGURED_EXTRA_ARGS=		$(shell perl ../../ScriptsLib/PrintConfigurationVariable.pl $(CONFIGURATION) qFeatureFlag_OpenSSL_ExtraArgs)


ZIPEDFILE=../Origs-Cache/$(BASENAME).tar.gz

FETCHURLS=
FETCHURLS+=http://www.openssl.org/source/$(BASENAME).tar.gz


.PHONY:	doCreate doBuild

	
BUILDS_ROOT=../../Builds/


CONFIG_ARGS=
ifeq (Unix,$(ProjectPlatformSubdir))
CONFIG_ARGS+=	-no-shared
CONFIG_ARGS+=	--prefix=$(shell realpath `pwd`/../../)/Builds/$(CONFIGURATION)/ThirdPartyComponents/
ifeq (purify,$(findstring purify,$(CONFIGURED_EXTRA_ARGS)))
#see https://stroika.atlassian.net/browse/STK-400 - -DPURIFY to cleanly valgrind
CONFIG_ARGS+=	-DPURIFY
endif
endif

ifeq (Unix,$(ProjectPlatformSubdir))
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/libcrypto.a				\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/lib/libssl.a					\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/bin/openssl					\
	$(BUILDS_ROOT)$(CONFIGURATION)/ThirdPartyComponents/include/openssl/aes.h
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/lib/libeay32.lib						\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/lib/ssleay32.lib						\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/lib/lib.pdb							\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/openssl.exe							\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/openssl.pdb							\
	$(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/include/openssl/aes.h					\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/lib/libeay32.lib						\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/lib/ssleay32.lib						\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/lib/lib.pdb							\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/openssl.exe							\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/openssl.pdb							\
	$(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/include/openssl/aes.h					\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/lib/libeay32.lib			\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/lib/ssleay32.lib			\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/lib/lib.pdb				\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/openssl.exe				\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/openssl.pdb				\
	$(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/include/openssl/aes.h		\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/lib/libeay32.lib				\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/lib/ssleay32.lib				\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/lib/lib.pdb					\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/openssl.exe					\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/openssl.pdb					\
	$(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/include/openssl/aes.h			\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/lib/libeay32.lib						\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/lib/ssleay32.lib						\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/lib/lib.pdb							\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/openssl.exe							\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/openssl.pdb							\
	$(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/include/openssl/aes.h					\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/lib/libeay32.lib						\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/lib/ssleay32.lib						\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/lib/lib.pdb							\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/openssl.exe							\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/openssl.pdb							\
	$(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/include/openssl/aes.h					\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/lib/libeay32.lib				\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/lib/ssleay32.lib				\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/lib/lib.pdb					\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/openssl.exe					\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/openssl.pdb					\
	$(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/include/openssl/aes.h
endif

# COULD make this conditional
STRIP_ANNOYING_MESSAGES=		1

all:
	@../../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && echo "Building Stroika/ThirdPartyComponents/OpenSSL $(VERSION):"
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(MAKE) --no-print-directory --silent CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)  $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)  check


$(PRODUCED_OUTPUT_ARTIFACTS):
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
ifeq ($(PRODUCED_OUTPUT_ARTIFACTS),)
	$(error This makefile requires a recognized ProjectPlatformSubdir)
endif
	$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) doCreate
	$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) doBuilds


clean:
	@rm -rf CURRENT


clobber:	clean
	@rm -f BUILD.OUT INSTALL.OUT CONFIG.OUT
ifeq ($(CONFIGURATION),)
	@rm -rf $(BUILDS_ROOT)*/ThirdPartyComponents/lib/{libcrypto,libssl}* $(BUILDS_ROOT)*/ThirdPartyComponents/include/openssl $(BUILDS_ROOT)*/ThirdPartyComponents/bin/openssl
else
	@rm -f $(PRODUCED_OUTPUT_ARTIFACTS)
endif


check:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
ifeq ($(PRODUCED_OUTPUT_ARTIFACTS),)
	$(error This makefile requires a recognized ProjectPlatformSubdir)
endif
	@../../ScriptsLib/PrintLevelLeader.sh $(MAKE_INDENT_LEVEL) && echo -n "...Checking..."
	@for i in $(PRODUCED_OUTPUT_ARTIFACTS) ; do \
		if [ ! -e $$i ]; then\
			echo "      FAILED: missing $$i";\
			exit 1;\
		fi;\
	done
ifeq (false,$(CrossCompiling))
ifeq (Unix,$(ProjectPlatformSubdir))
	@X1=`diff -b CURRENT/TEST-OUT.txt REFERENCE_UNIX_OUTPUT.txt | wc -l`;\
	 if [ $$X1 -gt 2236 ]; then\
		echo "  Test DIFFS";\
		echo "    Diff should be less than 2236 to be safe.";\
		echo "      diff -b CURRENT/TEST-OUT.txt REFERENCE_UNIX_OUTPUT.txt | wc -l: $$X1";\
		exit 1;\
	 fi
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
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
endif
	@echo "Stroika/ThirdPartyComponents/OpenSSL - [Succeeded]";

$(ZIPEDFILE):
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo -n "..."
	@../../ScriptsLib/WebGet.sh $(ZIPEDFILE) $(FETCHURLS)


doPatch:
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo -n "...Patching OpenSSL...   "
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
ifeq (1,$(STRIP_ANNOYING_MESSAGES))
	@echo -n .
	@patch --quiet -p0 CURRENT/util/copy.pl < Patches/LosePRINTOfCopyingInUtils.PATCH
endif
endif
	@echo "done"


doCreate:	$(ZIPEDFILE)
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo "...Extract $(ZIPEDFILE) to CURRENT..."
	@rm -rf CURRENT
	@tar $(BAD_TARFILE_BUG_WORKAROUND_)xf $(ZIPEDFILE) --no-same-owner
	@mv $(EXTRACTED_DIRNAME) CURRENT;
	@$(MAKE) --no-print-directory CONFIGURATION=$(CONFIGURATION) MAKE_INDENT_LEVEL=$(MAKE_INDENT_LEVEL)  doPatch


doBuilds:
ifeq (Unix,$(ProjectPlatformSubdir))
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo "...Creating CURRENT/CONFIG.OUT - new configuration"
	@BASE_CONFIG_LINE=`cd CURRENT && ./config -t | tail -1`; echo "cd CURRENT ; $$BASE_CONFIG_LINE $(CONFIG_ARGS)" > CONFIG.OUT; (cd CURRENT ; $$BASE_CONFIG_LINE $(CONFIG_ARGS)) >> CONFIG.OUT 2>&1
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo "...Buidling, and redirecting messages to BUILD.OUT ...";
	@$(MAKE) --directory CURRENT --no-print-directory all > BUILD.OUT 2>&1;
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo "...Installing (to CURRENT/Builds), and redirecting messages to INSTALL.OUT ...";
	@$(MAKE) -j1 --directory CURRENT --no-print-directory install > INSTALL.OUT 2>&1;
ifeq (false,$(CrossCompiling))
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo "...Running openssl tests (OUTPUT TO CURRENT/TEST-OUT.txt)...";
	@($(MAKE) --directory CURRENT --no-print-directory -s rehash 2>&1) > /dev/null;
	@($(MAKE) -j1 --directory CURRENT --no-print-directory -s test > CURRENT/TEST-OUT.txt 2>&1);
endif
endif
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo "...Buidling, and redirecting messages to BUILD.OUT ...";
	@CONFIGURATION=$(CONFIGURATION) ./buildall_windows_.pl > BUILD.OUT 2>&1;
endif
	@../../ScriptsLib/PrintLevelLeader.sh $$(($(MAKE_INDENT_LEVEL)+1)) && echo "...Copying builds to $(BUILDS_ROOT) ..."
ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents && cp CURRENT/Builds/Debug32/openssl.* $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/lib && cp -r CURRENT/Builds/Debug32/libeay32.* CURRENT/Builds/Debug32/ssleay32.* CURRENT/Builds/Debug32/lib.pdb $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/include && cp -r --dereference CURRENT/include/openssl $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/include/
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents && cp CURRENT/Builds/Release32/openssl.* $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/lib && cp -r CURRENT/Builds/Release32/libeay32.* CURRENT/Builds/Release32/ssleay32.* CURRENT/Builds/Release32/lib.pdb $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/include && cp -r --dereference CURRENT/include/openssl $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/include/
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents && cp CURRENT/Builds/Release32/openssl.* $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/lib && cp -r CURRENT/Builds/Release32/libeay32.* CURRENT/Builds/Release32/ssleay32.* CURRENT/Builds/Release32/lib.pdb $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/include && cp -r --dereference CURRENT/include/openssl $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/include/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents && cp CURRENT/Builds/Release32/openssl.* $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/lib && cp -r CURRENT/Builds/Release32/libeay32.* CURRENT/Builds/Release32/ssleay32.* CURRENT/Builds/Release32/lib.pdb $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/include && cp -r --dereference CURRENT/include/openssl $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/include/
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents && cp CURRENT/Builds/Debug64/openssl.* $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/lib && cp -r CURRENT/Builds/Debug64/libeay32.* CURRENT/Builds/Debug64/ssleay32.* CURRENT/Builds/Debug64/lib.pdb $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/include && cp -r --dereference CURRENT/include/openssl $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/include/
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents && cp CURRENT/Builds/Release64/openssl.* $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/lib && cp -r CURRENT/Builds/Release64/libeay32.* CURRENT/Builds/Release64/ssleay32.* CURRENT/Builds/Release64/lib.pdb $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/include && cp -r --dereference CURRENT/include/openssl $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/include/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents && cp CURRENT/Builds/Release64/openssl.* $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/lib && cp -r CURRENT/Builds/Release64/libeay32.* CURRENT/Builds/Release64/ssleay32.* CURRENT/Builds/Release64/lib.pdb $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/include && cp -r --dereference CURRENT/include/openssl $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/include/
endif


ORIG:	$(ZIPEDFILE)
	@tar $(BAD_TARFILE_BUG_WORKAROUND_)xf $(ZIPEDFILE) --no-same-owner
	@mv $(EXTRACTED_DIRNAME) ORIGS

