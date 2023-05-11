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

include $(StroikaRoot)/ScriptsLib/SharedMakeVariables-Default.mk



ifneq ($(CONFIGURATION),)
include $(StroikaRoot)ScriptsLib/Makefile-CMake-Common.mk
endif

# # For windows build with nmake used (maybe replace this with use of NMAKE variable use/setting/call/explicit fullpath?)
# $(call PATCH_PATH_FOR_TOOLPATH_ADDITION_IF_NEEDED)

VERSION=1.2.13
#VERSION=1.2.12


BASENAME=zlib-$(VERSION)
ZIPPED_SOURCES_FILE=../Origs-Cache/$(BASENAME).tar.gz
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

COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT:=$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,$(StroikaRoot)/ThirdPartyComponents/zlib/CURRENT)



ifeq (${ENABLE_ASSERTIONS},1)
CMAKE_BUILD_TYPE=Debug
MAYBED=d
else
CMAKE_BUILD_TYPE=Release
endif

ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
OUTPUT_LIB_=zlibstatic${MAYBED}${LIB_SUFFIX}
else
OUTPUT_LIB_=libz${LIB_SUFFIX}
endif


PRODUCED_OUTPUT_ARTIFACTS=	\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/${OUTPUT_LIB_}		\
	$(PER_CONFIGURATION_BUILDS_DIR)include/zlib.h			\
	$(PER_CONFIGURATION_BUILDS_DIR)include/zconf.h




ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
# See comments at head of win32/Makefile.msc for the meaning of these flags (DASMV, etc)
# @todo - https://stroika.atlassian.net/browse/STK-905 - rewrite the windows build stuff (and unix) using cmake) - sb simpler/cleaner build and
#         get asm stuff working again
ASFLAGS_=-coff -Z7
ifeq ($(ENABLE_ASSERTIONS),1)
CFLAGS+= -DZLIB_DEBUG
endif
CFLAGS+= -I.
ifeq ($(ARCH), x86)
# For reasons not worth debugging, this ASM code crashes on x86, so just disable - https://stroika.atlassian.net/browse/STK-568
#CFLAGS+= -DASMV -DASMINF
#OBJA_=inffas32.obj match686.obj
### avoid occasional 'fatal error LNK1281: Unable to generate SAFESEH image.'
#ASFLAGS_+= -safeseh
endif
# ifeq ($(ARCH), x86_64)
# CFLAGS+= -DASMV -DASMINF
# OBJA_=inffasx64.obj gvmat64.obj inffas8664.obj
# endif
endif


### MERGE.RETRIFUT INCLDUE_CMAKE,xxxx file with this stuff


# For some reason, comes out as share by default for zlib
CMAKE_ARGS+=	-D INSTALL_PKGCONFIG_DIR="$(call FUNCTION_CONVERT_FILES_TO_COMPILER_NATIVE ,${PER_CONFIGURATION_BUILDS_DIR}/lib/pkgconfig)"

CMAKE_ARGS+=	-D CMAKE_SKIP_INSTALL_RPATH="YES"
CMAKE_ARGS+=	-D CMAKE_SKIP_RPATH="YES"

all:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents/zlib Build $(VERSION) {$(CONFIGURATION)}:"
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@#make ZIPPED_SOURCES_FILE even though a dependency on CURRENT and PRODUCED_ARTIFACTS for parallel-make so completed before either starts (else one sees file partly downloaded and proceeeds)
	@$(MAKE) --no-print-directory --silent $(ZIPPED_SOURCES_FILE)
	@$(MAKE) --no-print-directory --silent $(PRODUCED_OUTPUT_ARTIFACTS)
ifneq ($(QUICK_BUILD), 1)
	@$(MAKE) --no-print-directory --silent CURRENT
endif
	@$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) check


#ALL produced artifacts produced by the same thing. But if you try the same rule to make all, if parallel makes invoked on this, you get
#multiple tries to run the same rule. So remap all to making the first.
$(wordlist 2, 9999, $(PRODUCED_OUTPUT_ARTIFACTS)):	$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS))

$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS)):
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@${MAKE} --silent CURRENT

	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "zlib $(VERSION) - Configure (see $(OUTPUT_WORKDIR_PRETTYNAME)CONFIGURE-OUT.txt) ..."
	@rm -rf "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}"; mkdir -p "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}"
	@echo "${CMAKE}" ${CMAKE_ARGS} "${COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT}" >  "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}/CONFIGURE-OUT.txt" 2>&1
	@echo "SEE ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}/CMakeCache.txt" >>  "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}/CONFIGURE-OUT.txt" 2>&1
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}" && "${CMAKE}" ${CMAKE_ARGS} ${COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT} >> CONFIGURE-OUT.txt 2>&1
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "zlib $(VERSION) - Build (see $(OUTPUT_WORKDIR_PRETTYNAME)BUILD-OUT.txt) ..."
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}" && "${CMAKE}" --build . --target zlibstatic --target zlib --config=${CMAKE_BUILD_TYPE} > BUILD-OUT.txt 2>&1
else
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}" && "${CMAKE}" --build . --target zlibstatic --target zlib  > BUILD-OUT.txt 2>&1
endif
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) "zlib $(VERSION) - Build (see $(OUTPUT_WORKDIR_PRETTYNAME)INSTALL-OUT.txt) ..."
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}" && "${CMAKE}" --install . --config=${CMAKE_BUILD_TYPE}  > INSTALL-OUT.txt 2>&1
else
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}" && "${CMAKE}" --install .  > INSTALL-OUT.txt 2>&1
endif
ifeq (Unix,$(ProjectPlatformSubdir))
	@#some hacks to make more similar to what I had before - not sure why this fixes things....
	@rm -f $(PER_CONFIGURATION_BUILDS_DIR)/lib/libz.so* $(PER_CONFIGURATION_BUILDS_DIR)/lib/pkgconfig/zlib.pc
endif

$(ZIPPED_SOURCES_FILE):
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $(StroikaRoot)ScriptsLib/WebGet $(ZIPPED_SOURCES_FILE) $(FETCHURLS)



ORIGs CURRENT:	$(ZIPPED_SOURCES_FILE)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "zlib ${VERSION} - Extracting $(ZIPPED_SOURCES_FILE) to $@"` "... "
	@mkdir -p $@
ifeq ($(DETECTED_HOST_OS),Darwin)
	@tar xf $(ZIPPED_SOURCES_FILE) -C $@ --strip-components 1
else ifneq ($(findstring $(DETECTED_HOST_OS),MSYS-Cygwin),)
	@PWD=`pwd` tar xf $(ZIPPED_SOURCES_FILE) --no-same-owner --directory `realpath --relative-to=$$PWD $@` --strip-components=1
else
	@tar xf $(ZIPPED_SOURCES_FILE) --no-same-owner --directory $@ --strip-components=1
endif


check-prerequisite-tools:
	@#nothing


check:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "zlib ${VERSION} - Checking ... "
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
