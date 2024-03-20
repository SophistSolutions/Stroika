#
#	TODO:
#		@todo	NOTE - try for $(word 1,${PRODUCED_OUTPUT_ARTIFACTS_}) and $(wordlist 2, 9999, ${PRODUCED_OUTPUT_ARTIFACTS_}):	$(word 1,${PRODUCED_OUTPUT_ARTIFACTS_})
#				(repeated in most ThirdPartyComponents makefiles)
#				doesn't work if the first produced output artifact is fine, but the others got deleted. No BIGGIE, as you can just make clobber in that
#				exceedingly rare case; but certainly a defect of this approach.
#
StroikaRoot:=$(abspath ../..)/

ifneq (${CONFIGURATION},)
	-include ${StroikaRoot}IntermediateFiles/${CONFIGURATION}/Configuration.mk
endif

include ${StroikaRoot}ScriptsLib/Makefile-Common.mk
include ${StroikaRoot}/ScriptsLib/SharedMakeVariables-Default.mk
ifneq (${CONFIGURATION},)
include ${StroikaRoot}ScriptsLib/Makefile-CMake-Common.mk		# for things like CMAKE_ARGS
endif

#
# CHECK FOR UPDATED VERSIONS https://github.com/google/googletest/tags
#
GOOGLETEST_VERSION_=1.14.0

ZIPPED_SOURCES_FILE_=../Origs-Cache/googletest-${GOOGLETEST_VERSION_}.tar.gz

FETCHURLS_=
FETCHURLS_+= https://github.com/google/googletest/archive/refs/tags/v${GOOGLETEST_VERSION_}.tar.gz


PER_CONFIGURATION_BUILDS_DIR_:=$(shell realpath --canonicalize-missing ../../Builds/${CONFIGURATION}/ThirdPartyComponents/)/

#lose trailing / on PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_ because on macos make THATDIR doesn't work with trailing slash (old WEIRD_MACOS_MAKEFILE_DIR_BUG -  MacOS XCode 10 - GNU Make 3.81)
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_:=$(shell realpath --canonicalize-missing ../../IntermediateFiles/${CONFIGURATION}/ThirdPartyComponents/GoogleTest)

OUTPUT_WORKDIR_PRETTYNAME_="$(shell ${StroikaRoot}ScriptsLib/SubstituteBackVariables "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/")"

COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT_:=$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,${StroikaRoot}/ThirdPartyComponents/GoogleTest/CURRENT)


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

PRODUCED_OUTPUT_ARTIFACTS_=	\
	${PER_CONFIGURATION_BUILDS_DIR_}lib/${OUTPUT_LIB_}		\
	${PER_CONFIGURATION_BUILDS_DIR_}lib/pkgconfig/gtest.pc	\
	${PER_CONFIGURATION_BUILDS_DIR_}include/gtest/gtest.h	\


# For some reason, comes out as share by default for zlib
# CMAKE_ARGS+=	-D INSTALL_PKGCONFIG_DIR="$(call FUNCTION_CONVERT_FILES_TO_COMPILER_NATIVE ,${PER_CONFIGURATION_BUILDS_DIR_}lib/pkgconfig)"

# ifeq (Unix,$(ProjectPlatformSubdir))
# # @todo document - not sure needed - maybe related to shared/.so file stuff
# CMAKE_ARGS+=	-D CMAKE_SKIP_INSTALL_RPATH="YES"
# CMAKE_ARGS+=	-D CMAKE_SKIP_RPATH="YES"
# endif

all:
ifeq (${CONFIGURATION},)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@${StroikaRoot}ScriptsLib/PrintProgressLine ${MAKE_INDENT_LEVEL} "Stroika/ThirdPartyComponents/GoogleTest Build ${GOOGLETEST_VERSION_} {${CONFIGURATION}}:"
	@${StroikaRoot}ScriptsLib/CheckValidConfiguration ${CONFIGURATION}
	@#make ZIPPED_SOURCES_FILE_ even though a dependency on CURRENT and PRODUCED_ARTIFACTS for parallel-make so completed before either starts (else one sees file partly downloaded and proceeeds)
	@${MAKE} --no-print-directory --silent ${ZIPPED_SOURCES_FILE_}
	@${MAKE} --no-print-directory --silent ${PRODUCED_OUTPUT_ARTIFACTS_}
ifneq ($(QUICK_BUILD), 1)
	@${MAKE} --no-print-directory --silent CURRENT
endif
	@${MAKE} --no-print-directory MAKE_INDENT_LEVEL=$$((${MAKE_INDENT_LEVEL}+1)) check


#ALL produced artifacts produced by the same thing. But if you try the same rule to make all, if parallel makes invoked on this, you get
#multiple tries to run the same rule. So remap all to making the first.
$(wordlist 2, 9999, ${PRODUCED_OUTPUT_ARTIFACTS_}):	$(word 1,${PRODUCED_OUTPUT_ARTIFACTS_})

$(word 1,${PRODUCED_OUTPUT_ARTIFACTS_}):
ifeq (${CONFIGURATION},)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@${MAKE} --silent CURRENT

	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "GoogleTest ${GOOGLETEST_VERSION_} - Configure (see ${OUTPUT_WORKDIR_PRETTYNAME_}CONFIGURE-OUT.txt) ..."
	@rm -rf "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}"; mkdir -p "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}"
	@echo "${CMAKE}" ${CMAKE_ARGS} "${COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT_}" >  "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}/CONFIGURE-OUT.txt" 2>&1
	@echo "SEE ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}/CMakeCache.txt" >>  "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}/CONFIGURE-OUT.txt" 2>&1
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}" && "${CMAKE}" ${CMAKE_ARGS} ${COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT_} >> CONFIGURE-OUT.txt 2>&1
	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "GoogleTest ${GOOGLETEST_VERSION_} - Build (see ${OUTPUT_WORKDIR_PRETTYNAME_}BUILD-OUT.txt) ..."
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}" && "${CMAKE}"  --build . --config=${CMAKE_BUILD_TYPE_}  > BUILD-OUT.txt 2>&1
else
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}" && "${CMAKE}"  --build .  > BUILD-OUT.txt 2>&1
endif
	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "GoogleTest ${GOOGLETEST_VERSION_} - Build (see ${OUTPUT_WORKDIR_PRETTYNAME_}INSTALL-OUT.txt) ..."
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}" && "${CMAKE}" --install . --config=${CMAKE_BUILD_TYPE_}  > INSTALL-OUT.txt 2>&1
else
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}" && "${CMAKE}" --install .  > INSTALL-OUT.txt 2>&1
endif


${ZIPPED_SOURCES_FILE_}:
	@MAKE_INDENT_LEVEL=$$((${MAKE_INDENT_LEVEL}+1)) ${StroikaRoot}ScriptsLib/WebGet ${ZIPPED_SOURCES_FILE_} ${FETCHURLS_}


ORIGs CURRENT:	${ZIPPED_SOURCES_FILE_}
	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) `${StroikaRoot}ScriptsLib/SubstituteBackVariables "GoogleTest ${GOOGLETEST_VERSION_} - Extracting ${ZIPPED_SOURCES_FILE_} to $@"` "... "
	@mkdir -p $@
ifeq ($(DETECTED_HOST_OS),Darwin)
	@tar xf ${ZIPPED_SOURCES_FILE_} -C $@ --strip-components 1
else ifneq ($(findstring $(DETECTED_HOST_OS),MSYS-Cygwin),)
	@PWD=`pwd` tar xf ${ZIPPED_SOURCES_FILE_} --no-same-owner --directory `realpath --relative-to=$$PWD $@` --strip-components=1
else
	@tar xf ${ZIPPED_SOURCES_FILE_} --no-same-owner --directory $@ --strip-components=1
endif
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	@if [ "$@" != "ORIGs" ] ; then \
		$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) "GoogleTest ${VERSION} - Patching ... ";\
		patch -p0 $@/googletest/cmake/gtest.pc.in Patches/Patch-PKGConfig4Windoze.patch >> $@/PATCHING.OUT;\
	fi
endif

check-prerequisite-tools:
	@#nothing


check:
ifeq (${CONFIGURATION},)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@${StroikaRoot}ScriptsLib/PrintProgressLine ${MAKE_INDENT_LEVEL} -n "GoogleTest ${GOOGLETEST_VERSION_} - Checking ... "
	@${StroikaRoot}ScriptsLib/CheckFileExists ${PRODUCED_OUTPUT_ARTIFACTS_}
	@${ECHO} " Stroika/ThirdPartyComponents/GoogleTest -    [Succeeded]";


clean:
ifeq (${CONFIGURATION},)
	@rm -rf ${StroikaRoot}IntermediateFiles/*/ThirdPartyComponents/GoogleTest
else
	@rm -rf $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
endif


clobber:	clean
ifeq (${CONFIGURATION},)
	# @rm -rf ${StroikaRoot}Builds/*/ThirdPartyComponents/lib/libz${LIB_SUFFIX} ${StroikaRoot}Builds/*/ThirdPartyComponents/include/{zlib,zconf}.h
	@rm -rf CURRENT
else
	@rm -rf ${PRODUCED_OUTPUT_ARTIFACTS_}
endif
