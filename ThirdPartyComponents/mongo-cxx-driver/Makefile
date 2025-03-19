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

SHELL=/bin/bash


#not parallel because submakefiles use parallelism, but generally best to sequence these top level requests. Like if you say
# make clobber all you don't want those to happen at the same time. And make libraries samples wouldn't really work since all the libraries
# have to be built before the samples etc...
.NOTPARALLEL:


#
# CHECK FOR UPDATED VERSIONS https://github.com/mongodb/mongo-cxx-driver/tags or https://github.com/mongodb/mongo-cxx-driver/releases
#
MONGOCXXDRIRVER_VERSION_=r4.0.0

ZIPPED_SOURCES_FILE_=../Origs-Cache/mongo-cxx-driver-${MONGOCXXDRIRVER_VERSION_}.tar.gz

FETCHURLS_=
FETCHURLS_+= https://github.com/mongodb/mongo-cxx-driver/archive/refs/tags/${MONGOCXXDRIRVER_VERSION_}.tar.gz

PER_CONFIGURATION_BUILDS_DIR_:=$(shell realpath --canonicalize-missing ../../Builds/${CONFIGURATION}/ThirdPartyComponents)/
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_:=$(shell realpath --canonicalize-missing ../../IntermediateFiles/${CONFIGURATION}/ThirdPartyComponents/mongo-cxx-driver)/
OUTPUT_WORKDIR_PRETTYNAME_="$(shell ${StroikaRoot}ScriptsLib/SubstituteBackVariables "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)")"
COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT_:=$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,${StroikaRoot}/ThirdPartyComponents/mongo-cxx-driver/CURRENT)


## Crazy but mongo build script depends on python, needs to know version, and provides very long elaborate script to find the
## version, but it sometimes doesn't work --LGP 2025-03-10
PYTHON_VERSION_= $(shell (python --version 2>/dev/null || python3 --version 2>/dev/null) | awk '{print $$2}')
CMAKE_ARGS+= -DBUILD_VERSION=${PYTHON_VERSION_}


CMAKE_ARGS+= -DBUILD_SHARED_LIBS:BOOL=OFF


# NOT getting asan errors on windows - til we have basic function working - try disabling
CFLAGS:=$(filter-out -fsanitize=address, $(CFLAGS))
CXXFLAGS:=$(filter-out -fsanitize=address, $(CXXFLAGS))

# [ 72%] [32mBuilding CXX object src/bsoncxx/CMakeFiles/bsoncxx_static.dir/lib/bsoncxx/v_noabi/bsoncxx/array/element.cpp.o[0m
#... 88 |     static const strong_ordering less;
#       |                                  ^~~~
# /Sandbox/Stroika-Dev/ThirdPartyComponents/mongo-cxx-driver/CURRENT/src/bsoncxx/include/bsoncxx/v_noabi/bsoncxx/stdx/operators.hpp:89:34: error: ‘bsoncxx::detail::strong_ordering::greater’ declared weak after being used
#    89 |     static const strong_ordering greater;
#       |                                  ^~~~~~~
# gmake[7]: *** [src/bsoncxx/CMakeFiles/bsoncxx_static.dir/build.make:76: src/bsoncxx/CMakeFiles/bsoncxx_static.dir/lib/bsoncxx/v_noabi/bsoncxx/array/element.cpp.o] Error 1
# gmake[6]: *** [CMakeFiles/Makefile2:2685: src/bsoncxx/CMakeFiles/bsoncxx_static.dir/all] Error 2
# $(info GCC Version is $(shell ${StroikaRoot}ScriptsLib/GetGCCVersion ${CC}))
# $(info COMPARE is $(shell expr $(shell ${StroikaRoot}ScriptsLib/GetGCCVersion ${CC}) \< 12))
ifneq ($(shell ${StroikaRoot}ScriptsLib/GetGCCVersion ${CC}),)
ifeq ($(shell expr "$(shell ${StroikaRoot}ScriptsLib/GetGCCVersion ${CC})" \< 12),1)
CFLAGS:=$(filter-out -fsanitize=undefined, $(CFLAGS))
CXXFLAGS:=$(filter-out -fsanitize=undefined, $(CXXFLAGS))
endif
endif

ifeq (${AssertionsEnabled},1)
CMAKE_BUILD_TYPE_=Debug
else
CMAKE_BUILD_TYPE_=Release
endif

ifeq ($(findstring $(DETECTED_HOST_OS),MSYS-Cygwin),)
LIBPREFIX_=lib
endif


PRODUCED_OUTPUT_ARTIFACTS_=	\
	${PER_CONFIGURATION_BUILDS_DIR_}lib/${LIBPREFIX_}bson-static-1.0${LIB_SUFFIX}		\
	${PER_CONFIGURATION_BUILDS_DIR_}lib/${LIBPREFIX_}mongoc-static-1.0${LIB_SUFFIX}		\
	${PER_CONFIGURATION_BUILDS_DIR_}include/bsoncxx/v_noabi/bsoncxx/json.hpp			\
	${PER_CONFIGURATION_BUILDS_DIR_}include/libbson-1.0/bson.h							\
	${PER_CONFIGURATION_BUILDS_DIR_}include/libmongoc-1.0								\
	${PER_CONFIGURATION_BUILDS_DIR_}include/libmongoc-1.0/mongoc.h						\
	${PER_CONFIGURATION_BUILDS_DIR_}lib/pkgconfig/libbson-static-1.0.pc					\
	${PER_CONFIGURATION_BUILDS_DIR_}lib/pkgconfig/libmongoc-static-1.0.pc				\
	${PER_CONFIGURATION_BUILDS_DIR_}lib/pkgconfig/libbsoncxx-static.pc					\
	${PER_CONFIGURATION_BUILDS_DIR_}lib/pkgconfig/libmongocxx-static.pc



#PKG_CONFIG_STROIKA_DEPENDS_ON filter to avoid warnings about missing/incomplete stuff from stuff being built but not ready yet...
#NB: CFLAGS - for example = depends on PKG_CONFIG_STROIKA_DEPENDS_ON
# ONLY include in PKG_CONFIG_STROIKA_DEPENDS_ON stuff we use here (zlib and openssl)
PKG_CONFIG_STROIKA_DEPENDS_ON := $(filter zlib openssl,$(PKG_CONFIG_STROIKA_DEPENDS_ON))



all:
ifeq (${CONFIGURATION},)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@${StroikaRoot}ScriptsLib/PrintProgressLine ${MAKE_INDENT_LEVEL} "Stroika/ThirdPartyComponents/mongo-cxx-driver Build ${MONGOCXXDRIRVER_VERSION_} {${CONFIGURATION}}:"
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

	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "mongo-cxx-driver ${MONGOCXXDRIRVER_VERSION_} - Configure (see ${OUTPUT_WORKDIR_PRETTYNAME_}CONFIGURE-OUT.txt) ..."
	@if [[ "${PYTHON_VERSION_}" == "" ]] ; then echo "PYTHON_VERSION_ not set"; exit 1; fi
	@rm -rf "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}"; mkdir -p "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}"
	@echo "${CMAKE}" ${CMAKE_ARGS} "${COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT_}" >  "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}CONFIGURE-OUT.txt" 2>&1
	@echo "SEE ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}CMakeCache.txt" >>  "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}CONFIGURE-OUT.txt" 2>&1
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && "${CMAKE}" ${CMAKE_ARGS} ${COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT_} >> CONFIGURE-OUT.txt 2>&1
	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "mongo-cxx-driver ${MONGOCXXDRIRVER_VERSION_} - Build (see ${OUTPUT_WORKDIR_PRETTYNAME_}BUILD-OUT.txt) ..."
ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && "${CMAKE}"  --build . --config=${CMAKE_BUILD_TYPE_} > BUILD-OUT.txt 2>&1
else
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && "${CMAKE}"  --build .  > BUILD-OUT.txt 2>&1
endif
	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "mongo-cxx-driver ${MONGOCXXDRIRVER_VERSION_} - Build (see ${OUTPUT_WORKDIR_PRETTYNAME_}INSTALL-OUT.txt) ..."
ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && "${CMAKE}" --install . --config=${CMAKE_BUILD_TYPE_}  > INSTALL-OUT.txt 2>&1
else
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && "${CMAKE}" --install . > INSTALL-OUT.txt 2>&1
endif
ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
	@#The prefix= line is windows format and refers to intermediatefiles (and windows syntax on windows)- so not good - replace it with pcfiledir relative name
	@#.lib files ignored by pkg-config when using --msvc-syntax, so mapped back to -l (to be re-mapped back to .lib by pkgconfig)
	@sed -i "s/^prefix=.*\$$/prefix=\$${pcfiledir}\/..\/../g" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/libbson-static-1.0.pc"
	@sed -i "s/^prefix=.*\$$/prefix=\$${pcfiledir}\/..\/../g" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/libmongoc-static-1.0.pc"
	@sed -i "s/secur32\.lib/-lsecur32/g" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/libmongoc-static-1.0.pc"
	@sed -i "s/crypt32\.lib/-lcrypt32/g" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/libmongoc-static-1.0.pc"
	@sed -i "s/Bcrypt\.lib/-lBcrypt/g" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/libmongoc-static-1.0.pc"
	@sed -i "s/^prefix=.*\$$/prefix=\$${pcfiledir}\/..\/../g" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/libbsoncxx-static.pc"
	@sed -i "s/^prefix=.*\$$/prefix=\$${pcfiledir}\/..\/../g" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/libmongocxx-static.pc"
endif


${ZIPPED_SOURCES_FILE_}:
	@MAKE_INDENT_LEVEL=$$((${MAKE_INDENT_LEVEL}+1)) ${StroikaRoot}ScriptsLib/WebGet ${ZIPPED_SOURCES_FILE_} ${FETCHURLS_}


ORIGs CURRENT:	${ZIPPED_SOURCES_FILE_}
	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) `${StroikaRoot}ScriptsLib/SubstituteBackVariables "mongo-cxx-driver ${MONGOCXXDRIRVER_VERSION_} - Extracting ${ZIPPED_SOURCES_FILE_} to $@"` "... "
	@mkdir -p $@
ifeq ($(DETECTED_HOST_OS),Darwin)
	@tar xf ${ZIPPED_SOURCES_FILE_} -C $@ --strip-components 1
else ifneq ($(findstring $(DETECTED_HOST_OS),MSYS-Cygwin),)
	@PWD=`pwd` tar xf ${ZIPPED_SOURCES_FILE_} --no-same-owner --directory `realpath --relative-to=$$PWD $@` --strip-components=1
else
	@tar xf ${ZIPPED_SOURCES_FILE_} --no-same-owner --directory $@ --strip-components=1
endif

check-prerequisite-tools:
ifeq ($(CONFIGURATION),)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@$(StroikaRoot)ScriptsLib/PrintProgressLine $(MAKE_INDENT_LEVEL) -n "mongo-cxx-driver ${MONGOCXXDRIRVER_VERSION_} - Checking Prerequisite tools ... "
	@type python3 > /dev/null 2>&1 || ($(StroikaRoot)ScriptsLib/PrintProgressLine $$(($(MAKE_INDENT_LEVEL)+1)) -n "checking tools......... " && $(StroikaRoot)ScriptsLib/GetMessageForMissingTool python3 && exit 1)
	@$(ECHO) "done";


check:
ifeq (${CONFIGURATION},)
	$(error This makefile requires a CONFIGURATION= parameter, or environment variable set)
endif
	@${StroikaRoot}ScriptsLib/PrintProgressLine ${MAKE_INDENT_LEVEL} -n "mongo-cxx-driver ${MONGOCXXDRIRVER_VERSION_} - Checking ... "
	@${StroikaRoot}ScriptsLib/CheckFileExists ${PRODUCED_OUTPUT_ARTIFACTS_}
	@${ECHO} " Stroika/ThirdPartyComponents/mongo-cxx-driver -    [Succeeded]";


clean:
ifeq (${CONFIGURATION},)
	@rm -rf ${StroikaRoot}IntermediateFiles/*/ThirdPartyComponents/mongo-cxx-driver
else
	@rm -rf $(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)
endif


clobber:	clean
ifeq (${CONFIGURATION},)
	# @rm -rf ${StroikaRoot}Builds/*/ThirdPartyComponents/lib/libz${LIB_SUFFIX} ${StroikaRoot}Builds/*/ThirdPartyComponents/include/{zlib,zconf}.h
	@rm -rf CURRENT
else
	@rm -rf ${PRODUCED_OUTPUT_ARTIFACTS_}
	@rm -rf	${StroikaRoot}Builds/${CONFIGURATION}/ThirdPartyComponents/lib/{*bson*,*mongo*}
	@rm -rf	${StroikaRoot}Builds/${CONFIGURATION}/ThirdPartyComponents/lib/cmake/{*bson*,*mongoc*} 
	@rm -rf	${StroikaRoot}Builds/${CONFIGURATION}/ThirdPartyComponents/lib/pkgconfig/{*bson*,*mongo*}
	@rm -rf	${StroikaRoot}Builds/${CONFIGURATION}/ThirdPartyComponents/include/{*bson*,*mongoc*} 
endif
