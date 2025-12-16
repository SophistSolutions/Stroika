#
#	TODO:
#		@todo	NOTE - try for $(word 1,${PRODUCED_OUTPUT_ARTIFACTS_}) and $(wordlist 2, 9999, ${PRODUCED_OUTPUT_ARTIFACTS_}):	$(word 1,${PRODUCED_OUTPUT_ARTIFACTS_})
#				(repeated in most ThirdPartyComponents makefiles)
#				doesn't work if the first produced output artifact is fine, but the others got deleted. No BIGGIE, as you can just make clobber in that
#				exceedingly rare case; but certainly a defect of this approach.
#
StroikaRoot	:=	$(abspath ../..)/

include ${StroikaRoot}/ScriptsLib/SharedMakeVariables-Default.mk


CPPFLAGS 	:= 	${PLATFORM_CPPFLAGS}
CFLAGS 		:= 	${PLATFORM_CFLAGS}
CXXFLAGS 	:= 	${PLATFORM_CXXFLAGS}


ifneq (${CONFIGURATION},)
include ${StroikaRoot}ScriptsLib/Makefile-CMake-Common.mk		# for CMAKE_ARGS etc
endif

#
# CHECK FOR UPDATED VERSIONS https://github.com/facebook/zstd/tags
#
ZSTD_VERSION_=1.5.7
#ZSTD_VERSION_=1.5.6

ZIPPED_SOURCES_FILE_=../Origs-Cache/zstd-${ZSTD_VERSION_}.tar.gz


FETCHURLS_=https://github.com/facebook/zstd/releases/download/v${ZSTD_VERSION_}/zstd-${ZSTD_VERSION_}.tar.gz

PER_CONFIGURATION_BUILDS_DIR_:=${StroikaPlatformTargetBuildDir}
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_:=$(shell realpath --canonicalize-missing ../../IntermediateFiles/${CONFIGURATION}/ThirdPartyComponents/zstd)/
OUTPUT_WORKDIR_PRETTYNAME_="$(shell ${StroikaRoot}ScriptsLib/SubstituteBackVariables "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)")"
COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT_:=$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,${StroikaRoot}/ThirdPartyComponents/zstd/CURRENT)/


ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
ifeq (${AssertionsEnabled},1)
CMAKE_BUILD_TYPE_=Debug
else
CMAKE_BUILD_TYPE_=Release
endif
endif

#@todo dont understand why config=${CMAKE_BUILD_TYPE_} NEEDED for windows, and needs to not exist on unix
ifeq (${CMAKE_BUILD_TYPE_},)
CMAKE_BUILD_TYPE_EXTRA_ARG_=
else
CMAKE_BUILD_TYPE_EXTRA_ARG_= --config=${CMAKE_BUILD_TYPE_}
endif

# This is NEEDED to build on Windows, as the library doesn't appear to use c++
ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
 CMAKE_ARGS+= -UCMAKE_CXX_COMPILER
endif


ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
OUTPUT_LIB_=zstd${LIB_SUFFIX}
else
OUTPUT_LIB_=libzstd${LIB_SUFFIX}
endif


PRODUCED_OUTPUT_ARTIFACTS_=	\
	${PER_CONFIGURATION_BUILDS_DIR_}lib/${OUTPUT_LIB_}		\
	${PER_CONFIGURATION_BUILDS_DIR_}include/zstd.h


# Verbose output to debug cmake processing
#CMAKE_ARGS+= --trace
#CMAKE_BUILD_AND_INSTALL_ARGS+= --trace


#In macOS and Xcode, the linker prioritizes dynamic libraries (.dylib) over static libraries (.a) when both are present in
#But disable shared for all, cuz I prefer static
CMAKE_ARGS+=	-D ZSTD_BUILD_STATIC="ON"
CMAKE_ARGS+=	-D ZSTD_BUILD_SHARED="OFF"


all:
ifeq (${CONFIGURATION},)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@${StroikaRoot}ScriptsLib/PrintProgressLine ${MAKE_INDENT_LEVEL} "Stroika/ThirdPartyComponents/zstd Build ${ZSTD_VERSION_} {${CONFIGURATION}}:"
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

	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "zstd ${ZSTD_VERSION_} - Configure (see ${OUTPUT_WORKDIR_PRETTYNAME_}CONFIGURE-OUT.txt) ..."
	@rm -rf "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}"; mkdir -p "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}"
	@echo "${CMAKE}" ${CMAKE_ARGS} -S "${COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT_}build/cmake" -B . >  "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}CONFIGURE-OUT.txt" 2>&1
	@echo "SEE ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}CMakeCache.txt" >>  "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}CONFIGURE-OUT.txt" 2>&1
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && "${CMAKE}" ${CMAKE_ARGS} -S ${COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT_}build/cmake -B . >> CONFIGURE-OUT.txt 2>&1
	@
	@
	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "zstd ${ZSTD_VERSION_} - Build (see ${OUTPUT_WORKDIR_PRETTYNAME_}BUILD-OUT.txt) ..."
	@+cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && "${CMAKE}" --build .  ${CMAKE_BUILD_AND_INSTALL_ARGS} ${CMAKE_BUILD_TYPE_EXTRA_ARG_}  > BUILD-OUT.txt 2>&1
	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "zstd ${ZSTD_VERSION_} - Build (see ${OUTPUT_WORKDIR_PRETTYNAME_}INSTALL-OUT.txt) ..."
	@+cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && "${CMAKE}" --install . ${CMAKE_BUILD_TYPE_EXTRA_ARG_} ${CMAKE_BUILD_AND_INSTALL_ARGS} > INSTALL-OUT.txt 2>&1
	@# on windows, file named zstd_static.lib but referenced as zstd.lib in .pc file
ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
	@mv ${PER_CONFIGURATION_BUILDS_DIR_}lib/zstd_static${LIB_SUFFIX} ${PER_CONFIGURATION_BUILDS_DIR_}lib/zstd${LIB_SUFFIX}
endif

${ZIPPED_SOURCES_FILE_}:
	@MAKE_INDENT_LEVEL=$$((${MAKE_INDENT_LEVEL}+1)) ${StroikaRoot}ScriptsLib/WebGet ${ZIPPED_SOURCES_FILE_} ${FETCHURLS_}


ORIGs CURRENT:	${ZIPPED_SOURCES_FILE_}
	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) `${StroikaRoot}ScriptsLib/SubstituteBackVariables "zstd ${ZSTD_VERSION_} - Extracting ${ZIPPED_SOURCES_FILE_} to $@"` "... "
	@mkdir -p $@
ifeq ($(DETECTED_HOST_OS),Darwin)
	@tar xf ${ZIPPED_SOURCES_FILE_} -C $@ --strip-components 1
else ifneq ($(findstring $(DETECTED_HOST_OS),MSYS-Cygwin),)
	@PWD=`pwd` tar xf ${ZIPPED_SOURCES_FILE_} --no-same-owner --directory `realpath --relative-to=$$PWD $@` --strip-components=1
else
	@tar xf ${ZIPPED_SOURCES_FILE_} --no-same-owner --directory $@ --strip-components=1
endif


check-prerequisite-tools:
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "zstd ${VERSION} - Checking Prerequisite tools ... "
	@{ type cmake > /dev/null 2>&1; } || { $(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) && $(StroikaRoot)ScriptsLib/GetMessageForMissingTool cmake && exit 1; }
	@$(ECHO) "done";


check:
ifeq (${CONFIGURATION},)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@${StroikaRoot}ScriptsLib/PrintProgressLine ${MAKE_INDENT_LEVEL} -n "zstd ${ZSTD_VERSION_} - Checking ... "
	@${StroikaRoot}ScriptsLib/CheckFileExists ${PRODUCED_OUTPUT_ARTIFACTS_}
	@${ECHO} " Stroika/ThirdPartyComponents/zstd -    [Succeeded]";


clean:
ifeq (${CONFIGURATION},)
	@rm -rf ${StroikaRoot}IntermediateFiles/*/ThirdPartyComponents/zstd
else
	@rm -rf $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)
endif


clobber:	clean
ifeq (${CONFIGURATION},)
	@rm -rf ${StroikaRoot}Builds/*/lib/*zstd${LIB_SUFFIX} ${StroikaRoot}Builds/*/include/{zstd}.h
	@rm -rf CURRENT
else
	@rm -rf ${PRODUCED_OUTPUT_ARTIFACTS_}
endif
