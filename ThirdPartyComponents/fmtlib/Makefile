#
#	TODO:
#		@todo	NOTE - try for $(word 1,$(PRODUCED_OUTPUT_ARTIFACTS)) and $(wordlist 2, 9999, $(PRODUCED_OUTPUT_ARTIFACTS)):	$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS))
#				(repeated in most ThirdPartyComponents makefiles)
#				doesn't work if the first produced output artifact is fine, but the others got deleted. No BIGGIE, as you can just make clobber in that
#				exceedingly rare case; but certainly a defect of this approach.
StroikaRoot	:=	$(abspath ../..)/

include $(StroikaRoot)Build/ScriptsLib/SharedMakeVariables-Default.mk
include $(StroikaRoot)Build/ScriptsLib/SharedBuildRules-Default.mk
ifneq (${CONFIGURATION},)
include ${StroikaRoot}Build/ScriptsLib/Makefile-CMake-Common.mk		# for things like CMAKE_ARGS
endif

# because we list multiple targets in PRODUCED_OUTPUT_ARTIFACTS, all of which are built with the same script
.NOTPARALLEL:

# STICK TO 10.2.1 - old fmt lib - cuz newer one ranges support matches too much - its a problem - matches String - for example...
# https://github.com/fmtlib/fmt/releases
#VERSION:=11.0.2
#VERSION:=11.0.1
VERSION:=10.2.1

ZIPPEDFILE:=../Origs-Cache/fmtlib-$(VERSION).zip

FETCHURLS:=
FETCHURLS+=https://github.com/fmtlib/fmt/archive/refs/tags/${VERSION}.zip


PER_CONFIGURATION_BUILDS_DIR_:=${StroikaPlatformTargetBuildDir}
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_:=$(shell realpath --canonicalize-missing ../../IntermediateFiles/$(CONFIGURATION)/ThirdPartyComponents/fmtlib)/

OUTPUT_WORKDIR_PRETTYNAME_="$(shell ${StroikaRoot}Build/ScriptsLib/SubstituteBackVariables "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)")"
COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT_:=$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,${StroikaRoot}/ThirdPartyComponents/fmtlib/CURRENT)



CPPFLAGS 	:= 	${PLATFORM_CPPFLAGS}
CFLAGS 		:= 	${PLATFORM_CFLAGS}
CXXFLAGS 	:= 	${PLATFORM_CXXFLAGS}



ifeq (${AssertionsEnabled},1)
MAYBED_=d
else
MAYBED_=
endif

ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
OUTPUT_LIB_=gtest${LIB_SUFFIX}
else
OUTPUT_LIB_=libgtest${LIB_SUFFIX}
endif


.PHONY:	all clean clobber check

ifeq (Unix,$(BuildPlatform))
FMTLIB=libfmt.a
else ifeq (VisualStudio.Net,$(findstring VisualStudio.Net,$(BuildPlatform)))
FMTLIB=libfmt.lib
endif

ObjDir:=$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)

PRODUCED_OUTPUT_ARTIFACTS=	\
	$(PER_CONFIGURATION_BUILDS_DIR_)include/fmt/core.h	\
	$(PER_CONFIGURATION_BUILDS_DIR_)lib/$(FMTLIB)		\
	$(PER_CONFIGURATION_BUILDS_DIR_)lib/pkgconfig/fmt.pc

$(ZIPPEDFILE):
	@MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) $(StroikaRoot)Build/ScriptsLib/WebGet $(ZIPPEDFILE) $(FETCHURLS)

SILENT_BUILD_FLAG=	-silent
ifeq ($(ECHO_BUILD_LINES),1)
SILENT_BUILD_FLAG=
endif


all:
	@$(StroikaRoot)Build/ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Stroika/ThirdPartyComponents/fmtlib Build $(VERSION) {$(CONFIGURATION)}:"
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(StroikaRoot)Build/ScriptsLib/CheckValidConfiguration $(CONFIGURATION)
	@$(MAKE) --no-print-directory $(SILENT_BUILD_FLAG) $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(MAKE) --no-print-directory MAKE_INDENT_LEVEL=$$(($(MAKE_INDENT_LEVEL)+1)) check



CMAKE_ARGS+=	-D FMT_TEST="NO"


ORIGs CURRENT:	$(ZIPPEDFILE)
	@$(StroikaRoot)Build/ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) `$(StroikaRoot)Build/ScriptsLib/SubstituteBackVariables "fmtlib ${VERSION} - Extracting $(ZIPPEDFILE) to $@"` "... "
	@rm -rf fmt-${VERSION}/ $@
	@unzip -q $(ZIPPEDFILE)
	@mv fmt-${VERSION}/ $@
	@touch CURRENT ### so not built twice


# trick of using word 1 is to get this triggered just once
$(word 1,$(PRODUCED_OUTPUT_ARTIFACTS)): CURRENT
	@${StroikaRoot}Build/ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "fmtlib ${VERSION} - Configure (see ${OUTPUT_WORKDIR_PRETTYNAME_}CONFIGURE-OUT.txt) ..."
	@rm -rf "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}"; mkdir -p "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}"
	@echo "${CMAKE}" ${CMAKE_ARGS} "${COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT_}" >  "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}CONFIGURE-OUT.txt" 2>&1
	@echo "SEE ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}CMakeCache.txt" >>  "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}CONFIGURE-OUT.txt" 2>&1
	@+cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && "${CMAKE}" ${CMAKE_ARGS} ${COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT_} >> CONFIGURE-OUT.txt 2>&1
	@${StroikaRoot}Build/ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "fmtlib ${VERSION} - Build (see ${OUTPUT_WORKDIR_PRETTYNAME_}BUILD-OUT.txt) ..."
ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
	+cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && "${CMAKE}" --build . --config=${CMAKE_BUILD_TYPE} ${CMAKE_XTRA_BLD_FLAGS_} > BUILD-OUT.txt 2>&1
else
	@+cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && "${CMAKE}" --build . > BUILD-OUT.txt 2>&1
endif
	@${StroikaRoot}Build/ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "fmtlib ${VERSION} - Build (see ${OUTPUT_WORKDIR_PRETTYNAME_}INSTALL-OUT.txt) ..."
	@+cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && "${CMAKE}" --install .  > INSTALL-OUT.txt 2>&1

vpath %c $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)


check-prerequisite-tools:
	@$(StroikaRoot)Build/ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "fmtlib $(VERSION) - Checking Prerequisite tools ... "
	@{ type unzip > /dev/null; } || { $(StroikaRoot)Build/ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "Checking required build tools ... " && $(StroikaRoot)Build/ScriptsLib/GetMessageForMissingTool unzip && exit 1; }
	@$(ECHO) "done";


check:
	@$(StroikaRoot)Build/ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "fmtlib $(VERSION) - Checking ... "
	@$(StroikaRoot)Build/ScriptsLib/CheckFileExists $(PRODUCED_OUTPUT_ARTIFACTS)
	@$(ECHO) " Stroika/ThirdPartyComponents/fmtlib -  [Succeeded]";


clobber::	clean
ifeq ($(CONFIGURATION),)
	@rm -rf CURRENT ORIGS
	@rm -rf  $(StroikaRoot)Builds/*/include/fmtlib $(StroikaRoot)Builds/*/lib/libfmt.*
else
	@rm -rf $(PRODUCED_OUTPUT_ARTIFACTS) $(PER_CONFIGURATION_BUILDS_DIR_)include/fmt
endif


clean::
	@rm -rf $(AMALGAMATION_WITH_VERSION)
ifeq ($(CONFIGURATION),)
	@rm -rf  $(StroikaRoot)IntermediateFiles/*/ThirdPartyComponents/libfmt
else
	@rm -rf $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)
endif

