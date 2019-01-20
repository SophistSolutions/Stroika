#
#	TODO:
#		@todo	Cleanup submake (esp on windows) - so we can call make install instead of manually copying
#				stuff
#
#		@todo	NOTE - try for $(word 1,$(PRODUCED_OUTPUT_ARTIFACTS)) and $(wordlist 2, 9999, $(PRODUCED_OUTPUT_ARTIFACTS)):	$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS))
#				(repeated in most ThirdPartyComponents makefiles)
#				doesn't work if the first produced output artifact is fine, but the others got deleted. No BIGGIE, as you can just make clobber in that
#				exceedingly rare case; but certainly a defect of this approach.
#

#export StroikaRoot?=$(shell realpath ../..)/

ifneq ($(CONFIGURATION),)
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk



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

#lose trailing / on PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_ because on macos make THATDIR doesn't work with trailing slash (old WEIRD_MACOS_MAKEFILE_DIR_BUG -  MacOS XCode 10 - GNU Make 3.81)
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_:=$(shell realpath --canonicalize-missing ../../IntermediateFiles/$(CONFIGURATION)/ThirdPartyComponents/zlib)


ifneq (,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
WIN_CC	=	$(shell cygpath -w "$(CC)")
WIN_AR	=	$(shell cygpath -w "$(AR)")
WIN_AS	=	$(shell cygpath -w "$(AS)")
endif




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






.PHONY:	all
all:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents/zlib Build $(VERSION) {$(CONFIGURATION)}:"
	@../../ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@#make zippedfile even though a dependency on CURRENT and PRODUCED_ARTIFACTS for parallel-make so completed before either starts (else one sees file partly downloaded and proceeeds)
	@$(MAKE) --no-print-directory --silent $(ZIPPEDFILE)
	@$(MAKE) --no-print-directory --silent CURRENT $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) check


#ALL produced artifacts produced by the same thing. But if you try the same rule to make all, if parallel makes invoked on this, you get
#multiple tries to run the same rule. So remap all to making the first.
$(wordlist 2, 9999, $(PRODUCED_OUTPUT_ARTIFACTS)):	$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS))

$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS)):
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(MAKE) --silent $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
	@$(MAKE) --no-print-directory doBuilds_


$(ZIPPEDFILE):
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ../../ScriptsLib/WebGet $(ZIPPEDFILE) $(FETCHURLS)

.PHONY: doBuilds_
doBuilds_:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
ifeq (Unix,$(ProjectPlatformSubdir))
	@../../ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "zlib $(VERSION) - Configure (see $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIGURE.OUT) ..."
	@echo "export CC=$(CC) cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && ./configure" > $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIGURE.OUT
	@(export CC="$(CC)"; cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && ./configure) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIGURE.OUT 2>&1
endif
	@../../ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "zlib $(VERSION) - Build (see $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD.OUT) ..."
	@make doBuildInternal_ > $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD.OUT 2>&1
ifeq (Unix,$(ProjectPlatformSubdir))
	@mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)lib && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/libz.a  $(PER_CONFIGURATION_BUILDS_DIR)lib/
	@mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)include && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/zlib.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/zconf.h $(PER_CONFIGURATION_BUILDS_DIR)include/
else
ifeq ($(CONFIGURATION),Debug-U-32)
	@mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)lib && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/Builds/Debug32/zlib.lib $(PER_CONFIGURATION_BUILDS_DIR)lib/
	@mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)include && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/zlib.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/zconf.h $(PER_CONFIGURATION_BUILDS_DIR)include/
endif
ifeq ($(CONFIGURATION),Release-U-32)
	@mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)lib && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/Builds/Release32/zlib.lib $(PER_CONFIGURATION_BUILDS_DIR)lib/
	@mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)include && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/zlib.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/zconf.h $(PER_CONFIGURATION_BUILDS_DIR)include/
endif
ifeq ($(CONFIGURATION),Release-DbgMemLeaks-U-32)
	@mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)lib && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/Builds/Release32/zlib.lib $(PER_CONFIGURATION_BUILDS_DIR)lib/
	@mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)include && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/zlib.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/zconf.h $(PER_CONFIGURATION_BUILDS_DIR)include/
endif
ifeq ($(CONFIGURATION),Release-Logging-U-32)
	@mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)lib && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/Builds/Release32/zlib.lib $(PER_CONFIGURATION_BUILDS_DIR)lib/
	@mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)include && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/zlib.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/zconf.h $(PER_CONFIGURATION_BUILDS_DIR)include/
endif
ifeq ($(CONFIGURATION),Debug-U-64)
	@mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)lib && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/Builds/Debug64/zlib.lib $(PER_CONFIGURATION_BUILDS_DIR)lib/
	@mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)include && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/zlib.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/zconf.h $(PER_CONFIGURATION_BUILDS_DIR)include/
endif
ifeq ($(CONFIGURATION),Release-U-64)
	@mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)lib && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/Builds/Release64/zlib.lib $(PER_CONFIGURATION_BUILDS_DIR)lib/
	@mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)include && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/zlib.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/zconf.h $(PER_CONFIGURATION_BUILDS_DIR)include/
endif
ifeq ($(CONFIGURATION),Release-Logging-U-64)
	@mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)lib && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/Builds/Release64/zlib.lib $(PER_CONFIGURATION_BUILDS_DIR)lib/
	@mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)include && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/zlib.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/zconf.h $(PER_CONFIGURATION_BUILDS_DIR)include/
endif
endif

	
doBuildInternal_:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
ifeq (U-32,$(findstring U-32,$(CONFIGURATION)))
ifeq (Debug,$(findstring Debug,$(CONFIGURATION)))
	cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && env --unset=MAKEFLAGS --unset=MFLAGS sh -c "$(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) \"nmake -f win32/Makefile.msc LOC=\\\"/safeseh /MTd /DZLIB_DEBUG /DASMV /DASMINF\\\" OBJA=\\\"inffas32.obj match686.obj\\\" AS=\\\"$(WIN_AS)\\\" CC=\\\"$(WIN_CC)\\\" AR=\\\"$(WIN_AR)\\\" zlib.lib\""
	mkdir -p $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/Builds/Debug32
	mv $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/zlib.lib $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/Builds/Debug32/
endif
ifeq (Release,$(findstring Release,$(CONFIGURATION)))
	cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && env --unset=MAKEFLAGS --unset=MFLAGS sh -c "$(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) \"nmake -f win32/Makefile.msc LOC=\\\"/safeseh /MT /DASMV /DASMINF\\\" OBJA=\\\"inffas32.obj match686.obj\\\" AS=\\\"$(WIN_AS)\\\" CC=\\\"$(WIN_CC)\\\" AR=\\\"$(WIN_AR)\\\" zlib.lib\""
	mkdir -p $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/Builds/Release32
	mv $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/zlib.lib $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/Builds/Release32/
endif
endif
ifeq (U-64,$(findstring U-64,$(CONFIGURATION)))
ifeq (Debug,$(findstring Debug,$(CONFIGURATION)))
	cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && env --unset=MAKEFLAGS --unset=MFLAGS sh -c "$(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) \"nmake -f win32/Makefile.msc LOC=\\\"/safeseh /MTd /DZLIB_DEBUG  /DASMV /DASMINF /I.\\\" OBJA=\\\"inffasx64.obj gvmat64.obj inffas8664.obj\\\" AS=\\\"$(WIN_AS)\\\" CC=\\\"$(WIN_CC)\\\" AR=\\\"$(WIN_AR)\\\" zlib.lib\""
	mkdir -p $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/Builds/Debug64
	mv $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/zlib.lib $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/Builds/Debug64/
endif
ifeq (Release,$(findstring Release,$(CONFIGURATION)))
	cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && env --unset=MAKEFLAGS --unset=MFLAGS sh -c "$(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) \"nmake -f win32/Makefile.msc LOC=\\\"/safeseh /MT /DASMV /DASMINF /I.\\\" OBJA=\\\"inffasx64.obj gvmat64.obj inffas8664.obj\\\" AS=\\\"$(WIN_AS)\\\" CC=\\\"$(WIN_CC)\\\" AR=\\\"$(WIN_AR)\\\" zlib.lib\""
	mkdir -p $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/Builds/Release64
	mv $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/zlib.lib $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/Builds/Release64/
endif
endif
else
	@$(MAKE) --directory $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) --no-print-directory CC="$(CC)"
endif


ORIGs CURRENT $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_):	$(ZIPPEDFILE)
	@../../ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "zlib ${VERSION} - Extracting $(ZIPPEDFILE) to $@"` "... "
	@mkdir -p $@
ifeq ($(shell uname -s),Darwin)
	@tar xf $(ZIPPEDFILE) -C $@ --strip-components 1
else ifeq (CYGWIN,$(findstring CYGWIN,$(shell uname)))
	@PWD=`pwd` tar xf $(ZIPPEDFILE) --no-same-owner --directory `realpath --relative-to=$$PWD $@` --strip-components=1
else
	@tar xf $(ZIPPEDFILE) --no-same-owner --directory $@ --strip-components=1
endif
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	@if [ "$@" == "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)" ] ; then \
		../../ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "zlib ${VERSION} - Patching `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "$@"` ... "; \
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
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "Checking ... "
	@$(StroikaRoot)ScriptsLib/CheckFileExists $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(ECHO) " Stroika/ThirdPartyComponents/zlib -    [Succeeded]";


clean:
ifeq ($(CONFIGURATION),)
	@rm -rf $(StroikaRoot)IntermediateFiles/*/ThirdPartyComponents/zlib
else
	@rm -rf $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
endif


clobber:	clean
ifeq ($(CONFIGURATION),)
	@rm -rf $(StroikaRoot)Builds/*/ThirdPartyComponents/lib/libz.a $(StroikaRoot)Builds/*/ThirdPartyComponents/include/{zlib,zconf}.h
	@rm -rf CURRENT
else
	@rm -rf $(PRODUCED_OUTPUT_ARTIFACTS)
endif
