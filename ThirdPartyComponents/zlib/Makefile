export StroikaRoot?=$(shell realpath ../..)/

ifneq ($(CONFIGURATION),)
	include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk


.PHONY:	all
.DEFAULT_GOAL := all


DoCreateSymLink=0

# last stable version
VERSION=1.2.8
# crashes in regtest for inflate on Win32 (but not Win64), 1.2.9 thru 1.2.11
#VERSION=1.2.11


BASENAME=zlib-$(VERSION)
ZIPPEDFILE=../Origs-Cache/$(BASENAME).tar.gz
TARFILE=$(BASENAME).tar

#
# CHECK FOR UPDATED VERSIONS http://zlib.net/
#

FETCHURLS=
FETCHURLS+=https://github.com/madler/zlib/archive/v$(VERSION).tar.gz
FETCHURLS+=http://zlib.net/zlib-$(VERSION).tar.gz
FETCHURLS+=http://prdownloads.sourceforge.net/libpng/zlib-$(VERSION).tar.gz?download
#FETCHURLS+=http://sourceforge.net/projects/libpng/files/zlib/$(VERSION)/zlib-$(VERSION).tar.gz/download



PER_CONFIGURATION_BUILDS_DIR:=$(shell realpath --canonicalize-missing ../../Builds/$(CONFIGURATION)/ThirdPartyComponents/)/

#lose trailing / on this variable because on macos make THATDIR doesn't work with trailing slash
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR:=$(shell realpath --canonicalize-missing ../../IntermediateFiles/$(CONFIGURATION)/ThirdPartyComponents/zlib)



ifneq (,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
WIN_CC	=	$(shell cygpath -w "$(CC)")
WIN_AR	=	$(shell cygpath -w "$(AR)")
WIN_AS	=	$(shell cygpath -w "$(AS)")
endif

BUILDS_ROOT=../../Builds/



ifeq (Unix,$(ProjectPlatformSubdir))
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/libz.a
else
PRODUCED_OUTPUT_ARTIFACTS=	\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/zlib.lib
endif
PRODUCED_OUTPUT_ARTIFACTS+=	\
	$(PER_CONFIGURATION_BUILDS_DIR)include/zlib.h		\
	$(PER_CONFIGURATION_BUILDS_DIR)include/zconf.h

.NOTPARALLEL:




BUILDS_DIR=	$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/Builds/


all:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents/zlib Build $(VERSION) {$(CONFIGURATION)}:"
	@../../ScriptsLib/CheckValidConfiguration.sh $(CONFIGURATION)
	@$(MAKE) --no-print-directory --silent CURRENT $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) check


$(PRODUCED_OUTPUT_ARTIFACTS):
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(MAKE) --silent CURRENT $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)
	@$(MAKE) --no-print-directory doBuilds


$(ZIPPEDFILE):
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ../../ScriptsLib/WebGet.sh $(ZIPPEDFILE) $(FETCHURLS)


doBuilds:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
ifeq (Unix,$(ProjectPlatformSubdir))
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) "zlib $(VERSION) - Configure (see $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/CONFIGURE.OUT) ..."
	@echo "export CC=$(CC) cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR) && ./configure" > $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/CONFIGURE.OUT
	@(export CC="$(CC)"; cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR) && ./configure) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/CONFIGURE.OUT 2>&1
endif
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) "zlib $(VERSION) - Build (see $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/BUILD.OUT) ..."
	@make doBuildInternal_ > $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/BUILD.OUT 2>&1
ifeq (Unix,$(ProjectPlatformSubdir))
	@mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)lib && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/libz.a  $(PER_CONFIGURATION_BUILDS_DIR)lib/
	@mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)include && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/zlib.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/zconf.h $(PER_CONFIGURATION_BUILDS_DIR)include/
else
ifeq ($(CONFIGURATION),Debug-U-32)
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/lib && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/Builds/Debug32/zlib.lib $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/include && cp  $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/zlib.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/zconf.h $(BUILDS_ROOT)Debug-U-32/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Release-U-32)
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/lib && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/Builds/Release32/zlib.lib $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/include && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/zlib.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/zconf.h $(BUILDS_ROOT)Release-U-32/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Release-DbgMemLeaks-U-32)
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/lib && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/Builds/Release32/zlib.lib $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/include && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/zlib.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/zconf.h $(BUILDS_ROOT)Release-DbgMemLeaks-U-32/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Release-Logging-U-32)
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/lib && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/Builds/Release32/zlib.lib $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/include && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/zlib.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/zconf.h $(BUILDS_ROOT)Release-Logging-U-32/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Debug-U-64)
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/lib && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/Builds/Debug64/zlib.lib $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/include && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/zlib.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/zconf.h $(BUILDS_ROOT)Debug-U-64/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Release-U-64)
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/lib && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/Builds/Release64/zlib.lib $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/include && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/zlib.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/zconf.h $(BUILDS_ROOT)Release-U-64/ThirdPartyComponents/include/
endif
ifeq ($(CONFIGURATION),Release-Logging-U-64)
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/lib && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/Builds/Release64/zlib.lib $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/lib/
	@mkdir -p $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/include && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/zlib.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/zconf.h $(BUILDS_ROOT)Release-Logging-U-64/ThirdPartyComponents/include/
endif
endif

	
doBuildInternal_:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	rm -rf $(BUILDS_DIR)
	cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR) && rm -f *.obj *.pdb *.lib
ifeq (U-32,$(findstring U-32,$(CONFIGURATION)))
ifeq (Debug,$(findstring Debug,$(CONFIGURATION)))
	cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR) && env --unset=MAKEFLAGS --unset=MFLAGS sh -c "perl $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) \"nmake -f win32/Makefile.msc LOC=\\\"/safeseh /MTd /DZLIB_DEBUG /DASMV /DASMINF\\\" OBJA=\\\"inffas32.obj match686.obj\\\" AS=\\\"$(WIN_AS)\\\" CC=\\\"$(WIN_CC)\\\" AR=\\\"$(WIN_AR)\\\" zlib.lib\""
	mkdir -p $(BUILDS_DIR)Debug32
	mv $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/zlib.lib $(BUILDS_DIR)Debug32/
endif
ifeq (Release,$(findstring Release,$(CONFIGURATION)))
	cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR) && env --unset=MAKEFLAGS --unset=MFLAGS sh -c "perl $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) \"nmake -f win32/Makefile.msc LOC=\\\"/safeseh /MT /DASMV /DASMINF\\\" OBJA=\\\"inffas32.obj match686.obj\\\" AS=\\\"$(WIN_AS)\\\" CC=\\\"$(WIN_CC)\\\" AR=\\\"$(WIN_AR)\\\" zlib.lib\""
	mkdir -p $(BUILDS_DIR)Release32
	mv $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/zlib.lib $(BUILDS_DIR)Release32/
endif
endif
ifeq (U-64,$(findstring U-64,$(CONFIGURATION)))
ifeq (Debug,$(findstring Debug,$(CONFIGURATION)))
	cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR) && env --unset=MAKEFLAGS --unset=MFLAGS sh -c "perl $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) \"nmake -f win32/Makefile.msc LOC=\\\"/safeseh /MTd /DZLIB_DEBUG  /DASMV /DASMINF /I.\\\" OBJA=\\\"inffasx64.obj gvmat64.obj inffas8664.obj\\\" AS=\\\"$(WIN_AS)\\\" CC=\\\"$(WIN_CC)\\\" AR=\\\"$(WIN_AR)\\\" zlib.lib\""
	mkdir -p $(BUILDS_DIR)Debug64
	mv $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/zlib.lib $(BUILDS_DIR)Debug64/
endif
ifeq (Release,$(findstring Release,$(CONFIGURATION)))
	cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR) && env --unset=MAKEFLAGS --unset=MFLAGS sh -c "perl $(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars.pl $(CONFIGURATION) \"nmake -f win32/Makefile.msc LOC=\\\"/safeseh /MT /DASMV /DASMINF /I.\\\" OBJA=\\\"inffasx64.obj gvmat64.obj inffas8664.obj\\\" AS=\\\"$(WIN_AS)\\\" CC=\\\"$(WIN_CC)\\\" AR=\\\"$(WIN_AR)\\\" zlib.lib\""
	mkdir -p $(BUILDS_DIR)Release64
	mv $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)/zlib.lib $(BUILDS_DIR)Release64/
endif
endif
else
	@$(MAKE) --directory $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR) --no-print-directory CC="$(CC)"
endif


ORIGs CURRENT $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR):	$(ZIPPEDFILE)
	@../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "zlib ${VERSION} - Extracting $(ZIPPEDFILE) to $@"` "... "
	@mkdir -p $@
ifeq ($(shell uname -s),Darwin)
	@tar xf $(ZIPPEDFILE) -C $@ --strip-components 1
else ifeq (CYGWIN,$(findstring CYGWIN,$(shell uname)))
	@PWD=`pwd` tar xf $(ZIPPEDFILE) --no-same-owner --directory `realpath --relative-to=$$PWD $@` --strip-components=1
else
	@tar xf $(ZIPPEDFILE) --no-same-owner --directory $@ --strip-components=1
endif
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	@if [ "$@" == "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)" ] ; then \
		../../ScriptsLib/PrintProgressLine.sh $$(($(MAKE_INDENT_LEVEL)+1)) "zlib ${VERSION} - Patching `$(StroikaRoot)ScriptsLib/SubstituteBackVariables.sh "$@"` ... "; \
		dos2unix -q $@/win32/Makefile.msc > $@/PATCHING.OUT; \
		patch -p0 $@/win32/Makefile.msc Patches/Makefile.msc.PATCH >> $@/PATCHING.OUT; \
		patch -p0 $@/win32/Makefile.msc Patches/Makefile.msc.Z7.PATCH >> $@/PATCHING.OUT; \
		unix2dos -q $@/win32/Makefile.msc >> $@/PATCHING.OUT; \
	fi
endif


check-prerequisite-tools:
	@#nothing


check:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@../../ScriptsLib/PrintProgressLine.sh $(MAKE_INDENT_LEVEL) -n "Checking ... "
	@for i in $(PRODUCED_OUTPUT_ARTIFACTS) ; do \
		if [ ! -e $$i ]; then\
			$(ECHO) "   FAILED: missing $$i";\
			exit 1;\
		fi;\
	done
	@$(ECHO) " Stroika/ThirdPartyComponents/zlib -    [Succeeded]";


clean:
	@rm -rf $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR)


clobber:	clean
ifeq ($(CONFIGURATION),)
	@rm -rf $(BUILDS_ROOT)*/ThirdPartyComponents/lib/libz.a $(BUILDS_ROOT)*/ThirdPartyComponents/include/{zlib,zconf}.h
	@rm -rf CURRENT
else
	@rm -f $(PRODUCED_OUTPUT_ARTIFACTS)
endif
