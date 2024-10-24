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
# CHECK FOR UPDATED VERSIONS http://zlib.net/
#
ZLIB_VERSION_=1.3.1
#ZLIB_VERSION_=1.3
#ZLIB_VERSION_=1.2.13

ZIPPED_SOURCES_FILE_=../Origs-Cache/zlib-${ZLIB_VERSION_}.tar.gz

FETCHURLS_=
FETCHURLS_+=https://github.com/madler/zlib/archive/v${ZLIB_VERSION_}.tar.gz
FETCHURLS_+=http://zlib.net/zlib-${ZLIB_VERSION_}.tar.gz
FETCHURLS_+=http://prdownloads.sourceforge.net/libpng/zlib-${ZLIB_VERSION_}.tar.gz?download
#FETCHURLS_+=http://sourceforge.net/projects/libpng/files/zlib/${ZLIB_VERSION_}/zlib-${ZLIB_VERSION_}.tar.gz/download


PER_CONFIGURATION_BUILDS_DIR_:=$(shell realpath --canonicalize-missing ../../Builds/${CONFIGURATION}/ThirdPartyComponents)/
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_:=$(shell realpath --canonicalize-missing ../../IntermediateFiles/${CONFIGURATION}/ThirdPartyComponents/zlib)/
OUTPUT_WORKDIR_PRETTYNAME_="$(shell ${StroikaRoot}ScriptsLib/SubstituteBackVariables "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)")"
COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT_:=$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,${StroikaRoot}/ThirdPartyComponents/zlib/CURRENT)


ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
ifeq (${AssertionsEnabled},1)
CMAKE_BUILD_TYPE_=Debug
MAYBED_=d
else
CMAKE_BUILD_TYPE_=Release
MAYBED_=
endif
endif

#@todo dont understand why config=${CMAKE_BUILD_TYPE_} NEEDED for windows, and needs to not exist on unix
ifeq (${CMAKE_BUILD_TYPE_},)
CMAKE_BUILD_TYPE_EXTRA_ARG_=
else
CMAKE_BUILD_TYPE_EXTRA_ARG_= --config=${CMAKE_BUILD_TYPE_}
endif


ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
OUTPUT_LIB_=zlibstatic${MAYBED_}${LIB_SUFFIX}
else
OUTPUT_LIB_=libz${LIB_SUFFIX}
endif


PRODUCED_OUTPUT_ARTIFACTS_=	\
	${PER_CONFIGURATION_BUILDS_DIR_}lib/${OUTPUT_LIB_}		\
	${PER_CONFIGURATION_BUILDS_DIR_}include/zlib.h			\
	${PER_CONFIGURATION_BUILDS_DIR_}include/zconf.h


# For some reason, comes out as share by default for zlib
CMAKE_ARGS+=	-D INSTALL_PKGCONFIG_DIR="$(call FUNCTION_CONVERT_FILES_TO_COMPILER_NATIVE ,${PER_CONFIGURATION_BUILDS_DIR_}lib/pkgconfig)"

# Verbose output to debug cmake processing
#CMAKE_ARGS+= --trace
#CMAKE_BUILD_AND_INSTALL_ARGS+= --trace

ifeq (Unix,$(BuildPlatform))
# @todo document - not sure needed - maybe related to shared/.so file stuff
CMAKE_ARGS+=	-D CMAKE_SKIP_INSTALL_RPATH="YES"
CMAKE_ARGS+=	-D CMAKE_SKIP_RPATH="YES"
endif

all:
ifeq (${CONFIGURATION},)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@${StroikaRoot}ScriptsLib/PrintProgressLine ${MAKE_INDENT_LEVEL} "Stroika/ThirdPartyComponents/zlib Build ${ZLIB_VERSION_} {${CONFIGURATION}}:"
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

	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "zlib ${ZLIB_VERSION_} - Configure (see ${OUTPUT_WORKDIR_PRETTYNAME_}CONFIGURE-OUT.txt) ..."
	@rm -rf "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}"; mkdir -p "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}"
	@echo "${CMAKE}" ${CMAKE_ARGS} "${COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT_}" >  "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}CONFIGURE-OUT.txt" 2>&1
	@echo "SEE ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}CMakeCache.txt" >>  "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}CONFIGURE-OUT.txt" 2>&1
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && "${CMAKE}" ${CMAKE_ARGS} ${COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT_} >> CONFIGURE-OUT.txt 2>&1
	@
ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
	@# Since version 1.3, these vcxproj have the wrong value set, so we must correct --LGP 2024-02-06
	@sed -i 's/MultiThreadedDLL/MultiThreaded/g' ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}zlibstatic.vcxproj
	@sed -i 's/MultiThreadedDebugDLL/MultiThreadedDebug/g' ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}zlibstatic.vcxproj
endif
	@
	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "zlib ${ZLIB_VERSION_} - Build (see ${OUTPUT_WORKDIR_PRETTYNAME_}BUILD-OUT.txt) ..."
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && "${CMAKE}" --build . --target zlibstatic --target zlib ${CMAKE_BUILD_AND_INSTALL_ARGS} ${CMAKE_BUILD_TYPE_EXTRA_ARG_}  > BUILD-OUT.txt 2>&1
	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "zlib ${ZLIB_VERSION_} - Build (see ${OUTPUT_WORKDIR_PRETTYNAME_}INSTALL-OUT.txt) ..."
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && "${CMAKE}" --install . ${CMAKE_BUILD_TYPE_EXTRA_ARG_} ${CMAKE_BUILD_AND_INSTALL_ARGS} > INSTALL-OUT.txt 2>&1
ifeq (Unix,$(BuildPlatform))
	@# Don't leave around dynamic link libraries, because if we link against those (stored in Builds/CFG/ThirdPartyComponents/libs) - they wont
	@# be found at runtime (for example breaking curl build but thats the tip of the iceburg) --LGP 2024-05-05
	@rm -f ${PER_CONFIGURATION_BUILDS_DIR_}lib/libz.so* ${PER_CONFIGURATION_BUILDS_DIR_}lib/libz*.dylib
endif


${ZIPPED_SOURCES_FILE_}:
	@MAKE_INDENT_LEVEL=$$((${MAKE_INDENT_LEVEL}+1)) ${StroikaRoot}ScriptsLib/WebGet ${ZIPPED_SOURCES_FILE_} ${FETCHURLS_}


ORIGs CURRENT:	${ZIPPED_SOURCES_FILE_}
	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) `${StroikaRoot}ScriptsLib/SubstituteBackVariables "zlib ${ZLIB_VERSION_} - Extracting ${ZIPPED_SOURCES_FILE_} to $@"` "... "
	@mkdir -p $@
ifeq ($(DETECTED_HOST_OS),Darwin)
	@tar xf ${ZIPPED_SOURCES_FILE_} -C $@ --strip-components 1
else ifneq ($(findstring $(DETECTED_HOST_OS),MSYS-Cygwin),)
	@PWD=`pwd` tar xf ${ZIPPED_SOURCES_FILE_} --no-same-owner --directory `realpath --relative-to=$$PWD $@` --strip-components=1
else
	@tar xf ${ZIPPED_SOURCES_FILE_} --no-same-owner --directory $@ --strip-components=1
endif


check-prerequisite-tools:
	@#nothing


check:
ifeq (${CONFIGURATION},)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@${StroikaRoot}ScriptsLib/PrintProgressLine ${MAKE_INDENT_LEVEL} -n "zlib ${ZLIB_VERSION_} - Checking ... "
	@${StroikaRoot}ScriptsLib/CheckFileExists ${PRODUCED_OUTPUT_ARTIFACTS_}
	@${ECHO} " Stroika/ThirdPartyComponents/zlib -    [Succeeded]";


clean:
ifeq (${CONFIGURATION},)
	@rm -rf ${StroikaRoot}IntermediateFiles/*/ThirdPartyComponents/zlib
else
	@rm -rf $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)
endif


clobber:	clean
ifeq (${CONFIGURATION},)
	@rm -rf ${StroikaRoot}Builds/*/ThirdPartyComponents/lib/libz${LIB_SUFFIX} ${StroikaRoot}Builds/*/ThirdPartyComponents/include/{zlib,zconf}.h
	@rm -rf CURRENT
else
	@rm -rf ${PRODUCED_OUTPUT_ARTIFACTS_}
endif
