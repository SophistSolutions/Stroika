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
# CHECK FOR UPDATED VERSIONS https://github.com/GNOME/libxml2/tags
#
libxml2_VERSION_:=2.12.5
#libxml2_VERSION_=2.12.4
#libxml2_VERSION_=2.12.3

ZIPPED_SOURCES_FILE_:=../Origs-Cache/libxml2-${libxml2_VERSION_}.tar.gz

FETCHURLS_:=
FETCHURLS_+= https://github.com/GNOME/libxml2/archive/refs/tags/v${libxml2_VERSION_}.tar.gz



PER_CONFIGURATION_BUILDS_DIR_:=$(shell realpath --canonicalize-missing ../../Builds/${CONFIGURATION}/ThirdPartyComponents/)/

#lose trailing / on PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_ because on macos make THATDIR doesn't work with trailing slash (old WEIRD_MACOS_MAKEFILE_DIR_BUG -  MacOS XCode 10 - GNU Make 3.81)
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_:=$(shell realpath --canonicalize-missing ../../IntermediateFiles/${CONFIGURATION}/ThirdPartyComponents/libxml2)

OUTPUT_WORKDIR_PRETTYNAME_="$(shell ${StroikaRoot}ScriptsLib/SubstituteBackVariables "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)/")"

COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT_:=$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,${StroikaRoot}/ThirdPartyComponents/libxml2/CURRENT)



ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
ifeq (${AssertionsEnabled},1)
CMAKE_BUILD_TYPE_=Debug
MAYBED_=d
# https://cmake.org/cmake/help/v3.15/variable/CMAKE_MSVC_RUNTIME_LIBRARY.html
CMAKE_ARGS+=	-DCMAKE_MSVC_RUNTIME_LIBRARY="MultiThreadedDebug"
else
CMAKE_BUILD_TYPE_=Release
MAYBED_=
CMAKE_ARGS+=	-DCMAKE_MSVC_RUNTIME_LIBRARY="MultiThreaded"
endif
endif

OUTPUT_LIB_=libxml2${LIB_SUFFIX}
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
OUTPUT_LIB_=libxml2s${MAYBED_}${LIB_SUFFIX}
endif

PRODUCED_OUTPUT_ARTIFACTS_=	\
	${PER_CONFIGURATION_BUILDS_DIR_}lib/${OUTPUT_LIB_}				\
	${PER_CONFIGURATION_BUILDS_DIR_}lib/pkgconfig/libxml-2.0.pc		\
	${PER_CONFIGURATION_BUILDS_DIR_}include/libxml2/libxml/SAX.h

ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
PRODUCED_OUTPUT_ARTIFACTS_ += ${PER_CONFIGURATION_BUILDS_DIR_}lib/xml2${LIB_SUFFIX}
endif

# For some reason, comes out as share by default for zlib
# CMAKE_ARGS+=	-D INSTALL_PKGCONFIG_DIR="$(call FUNCTION_CONVERT_FILES_TO_COMPILER_NATIVE ,${PER_CONFIGURATION_BUILDS_DIR_}lib/pkgconfig)"

# ifeq (Unix,$(ProjectPlatformSubdir))
# # @todo document - not sure needed - maybe related to shared/.so file stuff
# CMAKE_ARGS+=	-D CMAKE_SKIP_INSTALL_RPATH="YES"
# CMAKE_ARGS+=	-D CMAKE_SKIP_RPATH="YES"
# endif
CMAKE_ARGS+=	-D LIBXML2_WITH_ICONV="OFF"
CMAKE_ARGS+=	-D LIBXML2_WITH_HTTP="OFF"

# @todo fix so we can use eventually if we've added that feature
CMAKE_ARGS+=	-D LIBXML2_WITH_ZLIB="OFF"
CMAKE_ARGS+=	-D LIBXML2_WITH_LZMA="OFF"
CMAKE_ARGS+=	-D LIBXML2_WITH_PYTHON="OFF"

#
# Silence a bunch of warnings cuz we don't write this code!
# And weirdly, with link time codegen, these warnings come out at link time, not when I compile this!
# No apparent options to silence this stuff from the linker, but adding it to the compilation step here
# appears to be preserved with the obj file, and used in link time codegen in the linker (visual studio 17.8.3)
# -- LGP 2024-01-02
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
ifeq (-LTCG,$(findstring -LTCG,$(LinkerArgs_ExtraPrefix)))
CFLAGS	+= -wd4701
CFLAGS	+= -wd4702
CFLAGS	+= -wd4703
CFLAGS	+= -wd4706
CFLAGS	+= -wd4789
endif
endif

# Sometimes helpful to debug
CMAKE_XTRA_BLD_FLAGS_=
#CMAKE_XTRA_BLD_FLAGS_ += --verbose

#Sometimes helpful to debug
#CMAKE_ARGS+=	--trace

ifeq ($(qFeatureFlag_ZLib), no)
CMAKE_ARGS+=	-D LIBXML2_WITH_ZLIB="OFF"
endif

# needed on to get static libs built
CMAKE_ARGS+=	-D BUILD_SHARED_LIBS="OFF"
all:
ifeq (${CONFIGURATION},)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@${StroikaRoot}ScriptsLib/PrintProgressLine ${MAKE_INDENT_LEVEL} "Stroika/ThirdPartyComponents/libxml2 Build ${libxml2_VERSION_} {${CONFIGURATION}}:"
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

	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "libxml2 ${libxml2_VERSION_} - Configure (see ${OUTPUT_WORKDIR_PRETTYNAME_}CONFIGURE-OUT.txt) ..."
	@rm -rf "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}"; mkdir -p "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}"
	@echo "${CMAKE}" ${CMAKE_ARGS} "${COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT_}" >  "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}/CONFIGURE-OUT.txt" 2>&1
	@echo "SEE ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}/CMakeCache.txt" >>  "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}/CONFIGURE-OUT.txt" 2>&1
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}" && "${CMAKE}" ${CMAKE_ARGS} ${COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT_} >> CONFIGURE-OUT.txt 2>&1
	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "libxml2 ${libxml2_VERSION_} - Build (see ${OUTPUT_WORKDIR_PRETTYNAME_}BUILD-OUT.txt) ..."
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}" && "${CMAKE}"   --build . --config=${CMAKE_BUILD_TYPE_} ${CMAKE_XTRA_BLD_FLAGS_}  > BUILD-OUT.txt 2>&1
else
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}" && "${CMAKE}"  --build .  > BUILD-OUT.txt 2>&1
endif
	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "libxml2 ${libxml2_VERSION_} - Build (see ${OUTPUT_WORKDIR_PRETTYNAME_}INSTALL-OUT.txt) ..."
ifeq (VisualStudio,$(findstring VisualStudio,$(ProjectPlatformSubdir)))
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}" && "${CMAKE}" --install . --config=${CMAKE_BUILD_TYPE_}  > INSTALL-OUT.txt 2>&1
	@# Match what .pc file generates with --msvc pkg-config option with symbolic link to avoid having to patch .pc file
ifeq ($(DETECTED_HOST_OS),Cygwin)
	@#cygwin seems to (sometimes) generate junction instead of symbolic link?
	@cp "${PER_CONFIGURATION_BUILDS_DIR_}"/lib/libxml2s${MAYBED_}.lib "${PER_CONFIGURATION_BUILDS_DIR_}"/lib/xml2.lib
else
	@ln -s -f "${PER_CONFIGURATION_BUILDS_DIR_}"/lib/libxml2s${MAYBED_}.lib "${PER_CONFIGURATION_BUILDS_DIR_}"/lib/xml2.lib
endif
	@#existing produced .pc file bad in several ways for windows, and want to try to use pkg-config so hopefully this hack adequate
	@cp PatchReplacements/libxml-2.0.pc "${PER_CONFIGURATION_BUILDS_DIR_}"/lib/pkgconfig/
else
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_}" && "${CMAKE}" --install .  > INSTALL-OUT.txt 2>&1
endif


${ZIPPED_SOURCES_FILE_}:
	@MAKE_INDENT_LEVEL=$$((${MAKE_INDENT_LEVEL}+1)) ${StroikaRoot}ScriptsLib/WebGet ${ZIPPED_SOURCES_FILE_} ${FETCHURLS_}


ORIGs CURRENT:	${ZIPPED_SOURCES_FILE_}
	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) `${StroikaRoot}ScriptsLib/SubstituteBackVariables "libxml2 ${libxml2_VERSION_} - Extracting ${ZIPPED_SOURCES_FILE_} to $@"` "... "
	@mkdir -p $@
ifeq ($(DETECTED_HOST_OS),Darwin)
	@tar xf ${ZIPPED_SOURCES_FILE_} -C $@ --strip-components 1
else ifneq ($(findstring $(DETECTED_HOST_OS),MSYS-Cygwin),)
	@# Under docker, we get failure to extract slink to this file unless its there first - but this works around the issue --LGP 2023-12-18
	@tar xf ${ZIPPED_SOURCES_FILE_} --no-same-owner --directory $@ --strip-components=1 libxml2-${libxml2_VERSION_}/test/relaxng/tutorA.rng
	@tar xf ${ZIPPED_SOURCES_FILE_} --no-same-owner --directory $@ --strip-components=1
else
	@tar xf ${ZIPPED_SOURCES_FILE_} --no-same-owner --directory $@ --strip-components=1
endif

check-prerequisite-tools:
	@#nothing


check:
ifeq (${CONFIGURATION},)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@${StroikaRoot}ScriptsLib/PrintProgressLine ${MAKE_INDENT_LEVEL} -n "libxml2 ${libxml2_VERSION_} - Checking ... "
	@${StroikaRoot}ScriptsLib/CheckFileExists ${PRODUCED_OUTPUT_ARTIFACTS_}
	@${ECHO} " Stroika/ThirdPartyComponents/libxml2 -    [Succeeded]";


clean:
ifeq (${CONFIGURATION},)
	@rm -rf ${StroikaRoot}IntermediateFiles/*/ThirdPartyComponents/libxml2
else
	@rm -rf $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_NOSLASH_)
endif


clobber:	clean
ifeq (${CONFIGURATION},)
	@rm -rf ${StroikaRoot}Builds/*/ThirdPartyComponents/lib/libxml2* ${StroikaRoot}Builds/*/ThirdPartyComponents/include/libxml2 ${StroikaRoot}Builds/*/ThirdPartyComponents/lib/pkgconfig/libxml-2.0.pc
	@rm -rf CURRENT
else
	@rm -rf ${PRODUCED_OUTPUT_ARTIFACTS_}
endif
