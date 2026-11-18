# DEPRECATED: make's own build (run by Makefile-Legacy), kept until Stroika v3.0d26 ships - Stroika builds with CMake (CMakeLists.txt)
# (its check-prerequisite-tools, clean and clobber still serve ThirdPartyComponents/Makefile's)
#
#	TODO:
#		@todo	NOTE - try for $(word 1,${PRODUCED_OUTPUT_ARTIFACTS_}) and $(wordlist 2, 9999, ${PRODUCED_OUTPUT_ARTIFACTS_}):	$(word 1,${PRODUCED_OUTPUT_ARTIFACTS_})
#				(repeated in most ThirdPartyComponents makefiles)
#				doesn't work if the first produced output artifact is fine, but the others got deleted. No BIGGIE, as you can just make clobber in that
#				exceedingly rare case; but certainly a defect of this approach.
#
StroikaSourceRoot	:=	$(abspath ../..)/

include ${StroikaSourceRoot}/Build/Lib/Make/SharedMakeVariables-Default.mk
ifneq (${CONFIGURATION},)
include ${StroikaSourceRoot}Build/Lib/Make/Makefile-CMake-Common.mk		# for things like CMAKE_ARGS
endif

#
# CHECK FOR UPDATED VERSIONS https://github.com/google/googletest/tags
#
GOOGLETEST_VERSION_=1.18.0
#GOOGLETEST_VERSION_=1.17.0
#GOOGLETEST_VERSION_=1.16.0
#GOOGLETEST_VERSION_=1.15.2

ZIPPED_SOURCES_FILE_=../Origs-Cache/googletest-${GOOGLETEST_VERSION_}.tar.gz

FETCHURLS_=
FETCHURLS_+= https://github.com/google/googletest/archive/refs/tags/v${GOOGLETEST_VERSION_}.tar.gz


PER_CONFIGURATION_BUILDS_DIR_:=${StroikaPlatformTargetBuildDir}
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_:=$(shell realpath --canonicalize-missing $(StroikaIntermediateFilesRoot)${CONFIGURATION}/ThirdPartyComponents/GoogleTest)/
OUTPUT_WORKDIR_PRETTYNAME_="$(shell ${StroikaSourceRoot}Build/Scripts/SubstituteBackVariables "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)")"
COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT_:=$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,${StroikaSourceRoot}/ThirdPartyComponents/GoogleTest/CURRENT)



CPPFLAGS 	:= 	${PLATFORM_CPPFLAGS}
CFLAGS 		:= 	${PLATFORM_CFLAGS}
# built as C++20 (CMAKE_CXX_STANDARD, below), so without what needs C++26 - g++ refuses -freflection otherwise
CXXFLAGS 	:= 	$(filter-out -freflection,${PLATFORM_CXXFLAGS})



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

PRODUCED_OUTPUT_ARTIFACTS_=	\
	${PER_CONFIGURATION_BUILDS_DIR_}lib/${OUTPUT_LIB_}		\
	${PER_CONFIGURATION_BUILDS_DIR_}lib/pkgconfig/gtest.pc	\
	${PER_CONFIGURATION_BUILDS_DIR_}include/gtest/gtest.h	\


all:
ifeq (${CONFIGURATION},)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@${StroikaSourceRoot}Build/Scripts/PrintProgressLine ${MAKE_INDENT_LEVEL} "Stroika/ThirdPartyComponents/GoogleTest Build ${GOOGLETEST_VERSION_} {${CONFIGURATION}}:"
	@${StroikaSourceRoot}Build/Scripts/CheckValidConfiguration ${CONFIGURATION}
	@#make ZIPPED_SOURCES_FILE_ even though a dependency on CURRENT and PRODUCED_ARTIFACTS for parallel-make so completed before either starts (else one sees file partly downloaded and proceeeds)
	@${MAKE} --no-print-directory --silent ${ZIPPED_SOURCES_FILE_}
	@${MAKE} --no-print-directory --silent ${PRODUCED_OUTPUT_ARTIFACTS_}
ifneq ($(QUICK_BUILD), 1)
	@${MAKE} --no-print-directory --silent CURRENT
endif
	@${MAKE} --no-print-directory MAKE_INDENT_LEVEL=$$((${MAKE_INDENT_LEVEL}+1)) check


# avoid issue with char8_t - link errors on Windows
CMAKE_ARGS+= -DCMAKE_CXX_STANDARD=20



#ALL produced artifacts produced by the same thing. But if you try the same rule to make all, if parallel makes invoked on this, you get
#multiple tries to run the same rule. So remap all to making the first.
$(wordlist 2, 9999, ${PRODUCED_OUTPUT_ARTIFACTS_}):	$(word 1,${PRODUCED_OUTPUT_ARTIFACTS_})

$(word 1,${PRODUCED_OUTPUT_ARTIFACTS_}):
ifeq (${CONFIGURATION},)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@${MAKE} --silent CURRENT

	@${StroikaSourceRoot}Build/Scripts/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "GoogleTest ${GOOGLETEST_VERSION_} - Configure (see ${OUTPUT_WORKDIR_PRETTYNAME_}CONFIGURE-OUT.txt) ..."
	@rm -rf "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}"; mkdir -p "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}"
	@echo "${CMAKE}" ${CMAKE_ARGS} "${COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT_}" >  "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}CONFIGURE-OUT.txt" 2>&1
	@echo "SEE ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}CMakeCache.txt" >>  "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}CONFIGURE-OUT.txt" 2>&1
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && "${CMAKE}" ${CMAKE_ARGS} ${COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT_} >> CONFIGURE-OUT.txt 2>&1
	@${StroikaSourceRoot}Build/Scripts/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "GoogleTest ${GOOGLETEST_VERSION_} - Build (see ${OUTPUT_WORKDIR_PRETTYNAME_}BUILD-OUT.txt) ..."
ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
	@+cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && "${CMAKE}"  --build . --config=${CMAKE_BUILD_TYPE}  > BUILD-OUT.txt 2>&1
else
	@+cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && "${CMAKE}"  --build .  > BUILD-OUT.txt 2>&1
endif
	@${StroikaSourceRoot}Build/Scripts/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "GoogleTest ${GOOGLETEST_VERSION_} - Build (see ${OUTPUT_WORKDIR_PRETTYNAME_}INSTALL-OUT.txt) ..."
ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
	@+cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && "${CMAKE}" --install . --config=${CMAKE_BUILD_TYPE}  > INSTALL-OUT.txt 2>&1
else
	@+cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && "${CMAKE}" --install .  > INSTALL-OUT.txt 2>&1
endif
ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
	@sed -i "s/^libdir=.*\$$/libdir=\$${pcfiledir}\/../g" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/gtest_main.pc"
	@sed -i "s/^includedir=.*\$$/includedir=\$${pcfiledir}\/..\/..\/include/g" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/gtest_main.pc"
endif



${ZIPPED_SOURCES_FILE_}:
	@MAKE_INDENT_LEVEL=$$((${MAKE_INDENT_LEVEL}+1)) ${StroikaSourceRoot}Build/Scripts/WebGet ${ZIPPED_SOURCES_FILE_} ${FETCHURLS_}


ORIGs CURRENT:	${ZIPPED_SOURCES_FILE_}
	@${StroikaSourceRoot}Build/Scripts/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) `${StroikaSourceRoot}Build/Scripts/SubstituteBackVariables "GoogleTest ${GOOGLETEST_VERSION_} - Extracting ${ZIPPED_SOURCES_FILE_} to $@"` "... "
	@mkdir -p $@
ifeq ($(DETECTED_HOST_OS),Darwin)
	@tar xf ${ZIPPED_SOURCES_FILE_} -C $@ --strip-components 1
else ifneq ($(filter Windows,$(StroikaHostOSNames)),)
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
	@${StroikaSourceRoot}Build/Scripts/PrintProgressLine ${MAKE_INDENT_LEVEL} -n "GoogleTest ${GOOGLETEST_VERSION_} - Checking ... "
	@${StroikaSourceRoot}Build/Scripts/CheckFileExists ${PRODUCED_OUTPUT_ARTIFACTS_}
	@${ECHO} " Stroika/ThirdPartyComponents/GoogleTest -    [Succeeded]";


clean:
ifeq (${CONFIGURATION},)
	@rm -rf ${StroikaIntermediateFilesRoot}*/ThirdPartyComponents/GoogleTest
else
	@rm -rf $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)
endif


clobber:	clean
ifeq (${CONFIGURATION},)
	# @rm -rf ${StroikaBuildsRoot}*/lib/{gmock,gtest,gtest_main}${LIB_SUFFIX} ${StroikaBuildsRoot}*/include/{gmock,gtest}
	@rm -rf CURRENT
else
	@rm -rf ${PRODUCED_OUTPUT_ARTIFACTS_}
endif
