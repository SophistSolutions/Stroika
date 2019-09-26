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
StroikaRoot=$(abspath ../..)/

ifneq ($(CONFIGURATION),)
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk

include $(StroikaRoot)/Library/Projects/Unix/SharedMakeVariables-Default.mk


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

OUTPUT_WORKDIR_PRETTYNAME="$(shell $(StroikaRoot)ScriptsLib/SubstituteBackVariables "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/")"


ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
OUTPUT_LIB_=zlib${LIB_SUFFIX}
else
OUTPUT_LIB_=libz${LIB_SUFFIX}
endif



PRODUCED_OUTPUT_ARTIFACTS=	\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/${OUTPUT_LIB_}		\
	$(PER_CONFIGURATION_BUILDS_DIR)include/zlib.h			\
	$(PER_CONFIGURATION_BUILDS_DIR)include/zconf.h






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
ifeq (Unix,$(ProjectPlatformSubdir))
	@../../ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "zlib $(VERSION) - Configure (see $(OUTPUT_WORKDIR_PRETTYNAME)/CONFIGURE.OUT) ..."
	@echo "export CC=$(CC) cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && ./configure" > $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIGURE.OUT
	@(export CC="$(CC)"; cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && ./configure) >> $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/CONFIGURE.OUT 2>&1
endif
	@../../ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "zlib $(VERSION) - Build (see $(OUTPUT_WORKDIR_PRETTYNAME)/BUILD.OUT) ..."
	@make doBuildInternal_ > $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/BUILD.OUT 2>&1
	@mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)lib && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/${OUTPUT_LIB_} $(PER_CONFIGURATION_BUILDS_DIR)lib/
	@mkdir -p $(PER_CONFIGURATION_BUILDS_DIR)include && cp $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/zlib.h $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/zconf.h $(PER_CONFIGURATION_BUILDS_DIR)include/


$(ZIPPEDFILE):
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) ../../ScriptsLib/WebGet $(ZIPPEDFILE) $(FETCHURLS)


ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
ASFLAGS_=/coff /Z7
ifeq ($(ENABLE_ASSERTIONS),1)
CFLAGS+= /DZLIB_DEBUG
endif
CFLAGS+= /DASMV /DASMINF
CFLAGS+= /I.
ifeq ($(ARCH), x86)
OBJA_=inffas32.obj match686.obj
### avoid occasional 'fatal error LNK1281: Unable to generate SAFESEH image.'
ASFLAGS_+= /safeseh
endif
ifeq ($(ARCH), x86_64)
OBJA_=inffasx64.obj gvmat64.obj inffas8664.obj
endif
endif


doBuildInternal_:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	cd $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_) && \
		export CFLAGS="$(call FUNCTION_QUOTE_QUOTE_CHARACTERS_FOR_SHELL,$(CFLAGS))"; \
		export ASFLAGS="$(ASFLAGS_)"; \
		env --unset=MAKEFLAGS --unset=MFLAGS sh -c \
		"$(StroikaRoot)ScriptsLib/RunArgumentsWithCommonBuildVars $(CONFIGURATION) \"nmake -f win32/Makefile.msc OBJA=\\\"$(OBJA_)\\\" AS=\\\"$(WIN_AS)\\\" CC=\\\"$(WIN_CC)\\\" AR=\\\"$(WIN_LIBTOOL)\\\" zlib.lib\""
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
	@if [ "$@" = "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)" ] ; then \
		../../ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "zlib ${VERSION} - Patching `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "$@"` ... "; \
		dos2unix -q $@/win32/Makefile.msc > $@/PATCHING.OUT; \
		patch -p0 $@/win32/Makefile.msc Patches/Makefile.msc.PATCH >> $@/PATCHING.OUT; \
		patch -p0 $@/win32/Makefile.msc Patches/LOSE_CANDAS_FLAGS.PATCH >> $@/PATCHING.OUT; \
		unix2dos -q $@/win32/Makefile.msc >> $@/PATCHING.OUT; \
	fi
endif


check-prerequisite-tools:
	@#nothing


check:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@../../ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "zlib ${VERSION} - Checking ... "
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
	@rm -rf $(StroikaRoot)Builds/*/ThirdPartyComponents/lib/libz${LIB_SUFFIX} $(StroikaRoot)Builds/*/ThirdPartyComponents/include/{zlib,zconf}.h
	@rm -rf CURRENT
else
	@rm -rf $(PRODUCED_OUTPUT_ARTIFACTS)
endif
