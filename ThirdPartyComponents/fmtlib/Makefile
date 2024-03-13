#
#	TODO:
#		@todo	NOTE - try for $(word 1,$(PRODUCED_OUTPUT_ARTIFACTS)) and $(wordlist 2, 9999, $(PRODUCED_OUTPUT_ARTIFACTS)):	$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS))
#				(repeated in most ThirdPartyComponents makefiles)
#				doesn't work if the first produced output artifact is fine, but the others got deleted. No BIGGIE, as you can just make clobber in that
#				exceedingly rare case; but certainly a defect of this approach.
StroikaRoot:=$(abspath ../..)/

ifneq ($(CONFIGURATION),)
	-include $(StroikaRoot)IntermediateFiles/$(CONFIGURATION)/Configuration.mk
endif

include $(StroikaRoot)ScriptsLib/Makefile-Common.mk
include $(StroikaRoot)ScriptsLib/SharedMakeVariables-Default.mk

include $(StroikaRoot)ScriptsLib/SharedBuildRules-Default.mk
ifneq (${CONFIGURATION},)
include ${StroikaRoot}ScriptsLib/Makefile-CMake-Common.mk		# for things like CMAKE_ARGS
endif

# because we list multiple targets in PRODUCED_OUTPUT_ARTIFACTS, all of which are built with the same script
.NOTPARALLEL:

# https://github.com/fmtlib/fmt/releases
VERSION:=10.2.1

ZIPPEDFILE:=../Origs-Cache/fmtlib-$(VERSION).zip

FETCHURLS:=
FETCHURLS+=https://github.com/fmtlib/fmt/archive/refs/tags/${VERSION}.zip


PER_CONFIGURATION_BUILDS_DIR:=$(shell realpath --canonicalize-missing ../../Builds/$(CONFIGURATION)/ThirdPartyComponents/)/

#lose trailing / on PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_ because on macos make THATDIR doesn't work with trailing slash (old WEIRD_MACOS_MAKEFILE_DIR_BUG -  MacOS XCode 10 - GNU Make 3.81)
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_:=$(shell realpath --canonicalize-missing ../../IntermediateFiles/$(CONFIGURATION)/ThirdPartyComponents/fmtlib)

OUTPUT_WORKDIR_PRETTYNAME_="$(shell ${StroikaRoot}ScriptsLib/SubstituteBackVariables "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/")"



ifeq (${AssertionsEnabled},1)
CMAKE_BUILD_TYPE_=Debug
MAYBED_=d
else
CMAKE_BUILD_TYPE_=Release
MAYBED_=
endif

ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
OUTPUT_LIB_=gtest${LIB_SUFFIX}
else
OUTPUT_LIB_=libgtest${LIB_SUFFIX}
endif




.PHONY:	all clean clobber check

ifeq (Unix,$(ProjectPlatformSubdir))
FMTLIB=libfmt.a
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(ProjectPlatformSubdir)))
FMTLIB=libfmt.lib
endif
# TARGETDIR			:=	$(PER_CONFIGURATION_BUILDS_DIR)bin/
# TARGETEXE			:=	$(TARGETDIR)fmtlib${EXE_SUFFIX}

ObjDir:=$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/

PRODUCED_OUTPUT_ARTIFACTS=	\
	$(PER_CONFIGURATION_BUILDS_DIR)include/fmt/core.h	\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/$(FMTLIB)		\
	$(PER_CONFIGURATION_BUILDS_DIR)lib/pkgconfig/fmt.pc

$(ZIPPEDFILE):
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $(StroikaRoot)ScriptsLib/WebGet $(ZIPPEDFILE) $(FETCHURLS)

SILENT_BUILD_FLAG=	-silent
ifeq ($(ECHO_BUILD_LINES),1)
SILENT_BUILD_FLAG=
endif


all:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents/fmtlib Build $(VERSION) {$(CONFIGURATION)}:"
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(StroikaRoot)ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@#make zippedfile even though a dependency on CURRENT and PRODUCED_ARTIFACTS for parallel-make so completed before either starts (else one sees file partly downloaded and proceeeds)
	@$(MAKE) --no-print-directory $(SILENT_BUILD_FLAG) $(ZIPPEDFILE)
	@$(MAKE) --no-print-directory $(SILENT_BUILD_FLAG) $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) check


COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT_:=$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,${StroikaRoot}/ThirdPartyComponents/fmtlib/CURRENT)


 
CMAKE_ARGS+=	-D FMT_TEST="NO"


ORIGs CURRENT:	$(ZIPPEDFILE)
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) `$(StroikaRoot)ScriptsLib/SubstituteBackVariables "fmtlib ${VERSION} - Extracting $(ZIPPEDFILE) to $@"` "... "
	@rm -rf fmt-${VERSION}/
	@unzip -q $(ZIPPEDFILE)
	@mv fmt-${VERSION}/ $@


# trick of using word 1 is to get this triggered just once
$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS)):
	@${MAKE} --silent CURRENT

	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "fmtlib ${VERSION} - Configure (see ${OUTPUT_WORKDIR_PRETTYNAME_}CONFIGURE-OUT.txt) ..."
	@rm -rf "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}"; mkdir -p "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}"
	@echo "${CMAKE}" ${CMAKE_ARGS} "${COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT_}" >  "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}/CONFIGURE-OUT.txt" 2>&1
	@echo "SEE ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}/CMakeCache.txt" >>  "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}/CONFIGURE-OUT.txt" 2>&1
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}" && "${CMAKE}" ${CMAKE_ARGS} ${COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT_} >> CONFIGURE-OUT.txt 2>&1
	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "fmtlib ${VERSION} - Build (see ${OUTPUT_WORKDIR_PRETTYNAME_}BUILD-OUT.txt) ..."
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}" && "${CMAKE}"   --build . --config=${CMAKE_BUILD_TYPE_} ${CMAKE_XTRA_BLD_FLAGS_}  > BUILD-OUT.txt 2>&1
else
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}" && "${CMAKE}"  --build .  > BUILD-OUT.txt 2>&1
endif
	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "fmtlib ${VERSION} - Build (see ${OUTPUT_WORKDIR_PRETTYNAME_}INSTALL-OUT.txt) ..."
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}" && "${CMAKE}" --install . --config=${CMAKE_BUILD_TYPE_}  > INSTALL-OUT.txt 2>&1
	@# Match what .pc file generates with --msvc pkg-config option with symbolic link to avoid having to patch .pc file
# ifeq ($(DETECTED_HOST_OS),Cygwin)
# 	@#cygwin seems to (sometimes) generate junction instead of symbolic link?
# 	@cp "${PER_CONFIGURATION_BUILDS_DIR_}"/lib/fmtlibs${MAYBED_}.lib "${PER_CONFIGURATION_BUILDS_DIR_}"/lib/xml2.lib
# else
# 	@ln -s -f "${PER_CONFIGURATION_BUILDS_DIR_}"/lib/fmtlibs${MAYBED_}.lib "${PER_CONFIGURATION_BUILDS_DIR_}"/lib/xml2.lib
# endif
	@#existing produced .pc file bad in several ways for windows, and want to try to use pkg-config so hopefully this hack adequate
	@cp PatchReplacements/libxml-2.0.pc "${PER_CONFIGURATION_BUILDS_DIR_}"/lib/pkgconfig/
else
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}" && "${CMAKE}" --install .  > INSTALL-OUT.txt 2>&1
endif

# ifeq (1,$(qCompiler_HasNoMisleadingIndentation_Flag))
# CFLAGS	+= -Wno-misleading-indentation
# endif


vpath %c $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)


# $(ObjDir)%${OBJ_SUFFIX} : %.c
# 	@if [ $(ECHO_BUILD_LINES) -eq 1 ]; then\
# 	    $(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) '$(call DEFAULT_CC_LINE,$<,$@)';\
# 	fi
# 	@mkdir -p `dirname $@`
# 	@$(call DEFAULT_CC_LINE,$<,$@)


check-prerequisite-tools:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "fmtlib $(VERSION) - Checking Prerequisite tools ... "
	@{ type unzip > /dev/null; } || { $(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Checking required build tools ... " && $(StroikaRoot)ScriptsLib/GetMessageForMissingTool unzip && exit 1; }
	@$(ECHO) "done";


check:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "fmtlib $(VERSION) - Checking ... "
	@$(StroikaRoot)ScriptsLib/CheckFileExists $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(ECHO) " Stroika/ThirdPartyComponents/fmtlib -  [Succeeded]";


clobber::	clean
ifeq ($(CONFIGURATION),)
	@rm -rf CURRENT ORIGS
	@rm -rf  $(StroikaRoot)Builds/*/ThirdPartyComponents/include/fmtlib $(StroikaRoot)Builds/*/ThirdPartyComponents/lib/libfmt.*
else
	@rm -rf $(PRODUCED_OUTPUT_ARTIFACTS) $(PER_CONFIGURATION_BUILDS_DIR)include/fmt
endif


clean::
	@rm -rf $(AMALGAMATION_WITH_VERSION)
ifeq ($(CONFIGURATION),)
	@rm -rf  $(StroikaRoot)IntermediateFiles/*/ThirdPartyComponents/libfmt
else
	@rm -rf $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
endif

