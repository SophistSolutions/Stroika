#
#	TODO:
#		@todo	NOTE - try for $(word 1,${PRODUCED_OUTPUT_ARTIFACTS_}) and $(wordlist 2, 9999, ${PRODUCED_OUTPUT_ARTIFACTS_}):	$(word 1,${PRODUCED_OUTPUT_ARTIFACTS_})
#				(repeated in most ThirdPartyComponents makefiles)
#				doesn't work if the first produced output artifact is fine, but the others got deleted. No BIGGIE, as you can just make clobber in that
#				exceedingly rare case; but certainly a defect of this approach.
#
#	NOTE this project/makefile DOESNT explicitly list the MongoC driver or other dependencies - these are automatically fetched
#	by the MongoCXX build scripts.
#
StroikaRoot	:=	$(abspath ../..)/

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
#		(NOTE WHEN CHANGING THIS - REVIEW THE PATCHES THAT DEPEND ON THIS VERSION BELOW)
#
MONGOCXXDRIRVER_VERSION_=r4.1.4
#MONGOCXXDRIRVER_VERSION_=r4.1.1
#MONGOCXXDRIRVER_VERSION_=r4.1.0
#MONGOCXXDRIRVER_VERSION_=r4.0.0

ZIPPED_SOURCES_FILE_=../Origs-Cache/mongo-cxx-driver-${MONGOCXXDRIRVER_VERSION_}.tar.gz

FETCHURLS_=
FETCHURLS_+= https://github.com/mongodb/mongo-cxx-driver/archive/refs/tags/${MONGOCXXDRIRVER_VERSION_}.tar.gz

PER_CONFIGURATION_BUILDS_DIR_:=${StroikaPlatformTargetBuildDir}
PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_:=$(shell realpath --canonicalize-missing ../../IntermediateFiles/${CONFIGURATION}/ThirdPartyComponents/mongo-cxx-driver)/
OUTPUT_WORKDIR_PRETTYNAME_="$(shell ${StroikaRoot}ScriptsLib/SubstituteBackVariables "$(PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_)")"
COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT_:=$(call FUNCTION_CONVERT_FILEPATH_TO_COMPILER_NATIVE,${StroikaRoot}/ThirdPartyComponents/mongo-cxx-driver/CURRENT)




CPPFLAGS 	:= 	${PLATFORM_CPPFLAGS}
CFLAGS 		:= 	${PLATFORM_CFLAGS}
CXXFLAGS 	:= 	${PLATFORM_CXXFLAGS}




## Crazy but mongo build script depends on python, needs to know version, and provides very long elaborate script to find the
## version, but it sometimes doesn't work --LGP 2025-03-10
PYTHON_VERSION_= $(shell (python --version 2>/dev/null || python3 --version 2>/dev/null) | awk '{print $$2}')
CMAKE_ARGS+= -DBUILD_VERSION=${PYTHON_VERSION_}


CMAKE_ARGS+= -DBUILD_SHARED_LIBS:BOOL=OFF

# If we have zlib, use it, instead of the one included in the mongo-cxx-driver
# makes sense todo, but triggered linker errors for me, so diasble for now...--LGP 2025-03-19
# ifneq (${qFeatureFlag_ZLib}, no)
# CMAKE_ARGS+= -DENABLE_ZLIB=SYSTEM
# endif


#
# Silence a bunch of warnings cuz we don't write this code!
# And weirdly, with link time codegen, these warnings come out at link time, not when I compile this!
# No apparent options to silence this stuff from the linker, but adding it to the compilation step here
# appears to be preserved with the obj file, and used in link time codegen in the linker
#
ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
ifeq (-LTCG,$(findstring -LTCG,$(Platform_LDFLAGS)))
CFLAGS	+= -wd4701
CFLAGS	+= -wd4702
CFLAGS	+= -wd4724
endif
endif

# Avoid warnings like these:
# 	In function 'bson_free',
#     	inlined from 'bson_destroy' at /home/lewis/Sandbox/Stroika-Build-Dir-Ubuntu2404_x86_64/IntermediateFiles/g++-14-release++23/ThirdPartyComponents/mongo-cxx-driver/_deps/mongo-c-driver-src/src/libbson/src/bson/bson.c:2213:7,
#     	inlined from '_mongoc_bulk_insert_opts_cleanup' at /home/lewis/Sandbox/Stroika-Build-Dir-Ubuntu2404_x86_64/IntermediateFiles/g++-14-release++23/ThirdPartyComponents/mongo-cxx-driver/_deps/mongo-c-driver-src/src/libmongoc/src/mongoc/mongoc-opts.c:1150:0,
#     	inlined from '_server_session_destroy' at /home/lewis/Sandbox/Stroika-Build-Dir-Ubuntu2404_x86_64/IntermediateFiles/g++-14-release++23/ThirdPartyComponents/mongo-cxx-driver/_deps/mongo-c-driver-src/src/libmongoc/src/mongoc/mongoc-topology.c:214:4:
# 	/home/lewis/Sandbox/Stroika-Build-Dir-Ubuntu2404_x86_64/IntermediateFiles/g++-14-release++23/ThirdPartyComponents/mongo-cxx-driver/_deps/mongo-c-driver-src/src/libbson/src/bson/bson-memory.c:337:4: warning: 'free' called on pointer 'session_2(D)' with nonzero offset 8 [-Wfree-nonheap-object]
# DIDNT WORK:
#	CFLAGS	+= -Wno-free-nonheap-object
# 	CXXFLAGS	+= -Wno-free-nonheap-object
# 	LDFLAGS	+= -Wno-free-nonheap-object
# DID WORK:
# 		-- LGP 2025-05-22
ifneq (,$(shell ${StroikaRoot}ScriptsLib/GetGCCVersion $(CC)))
CFLAGS:=$(filter-out -flto, $(CFLAGS))
CXXFLAGS:=$(filter-out -flto, $(CXXFLAGS))
endif


# @todo RETEST!, and if needed, better document why/what fails...
# NOTE getting asan errors on windows - til we have basic function working - try disabling
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
	${PER_CONFIGURATION_BUILDS_DIR_}include/bsoncxx/v_noabi/bsoncxx/json.hpp			\
	${PER_CONFIGURATION_BUILDS_DIR_}lib/pkgconfig/libbsoncxx-static.pc					\
	${PER_CONFIGURATION_BUILDS_DIR_}lib/pkgconfig/libmongocxx-static.pc

ifeq (${MONGOCXXDRIRVER_VERSION_},r4.0.0)
PRODUCED_OUTPUT_ARTIFACTS_+= \
	${PER_CONFIGURATION_BUILDS_DIR_}include/libbson-1.0/								\
	${PER_CONFIGURATION_BUILDS_DIR_}include/libbson-1.0/bson.h							\
	${PER_CONFIGURATION_BUILDS_DIR_}include/libmongoc-1.0								\
	${PER_CONFIGURATION_BUILDS_DIR_}include/libmongoc-1.0/mongoc.h  					\
	${PER_CONFIGURATION_BUILDS_DIR_}lib/${LIBPREFIX_}bson-static-1.0${LIB_SUFFIX}  		\
	${PER_CONFIGURATION_BUILDS_DIR_}lib/${LIBPREFIX_}mongoc-static-1.0${LIB_SUFFIX}		\
	${PER_CONFIGURATION_BUILDS_DIR_}lib/pkgconfig/libbson-static-1.0.pc					\
	${PER_CONFIGURATION_BUILDS_DIR_}lib/pkgconfig/libmongoc-static-1.0.pc
else
#todo add more
PRODUCED_OUTPUT_ARTIFACTS_+= \
	${PER_CONFIGURATION_BUILDS_DIR_}lib/${LIBPREFIX_}mongoc2${LIB_SUFFIX}				\
	${PER_CONFIGURATION_BUILDS_DIR_}lib/pkgconfig/bson2-static.pc						\
	${PER_CONFIGURATION_BUILDS_DIR_}lib/pkgconfig/mongoc2-static.pc
endif


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
	@echo "${CMAKE}" ${CMAKE_ARGS} "${COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT_}" > "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}CONFIGURE-OUT.txt" 2>&1
	@echo "SEE ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}CMakeCache.txt" >> "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}CONFIGURE-OUT.txt" 2>&1
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && "${CMAKE}" ${CMAKE_ARGS} ${COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT_} >> CONFIGURE-OUT.txt 2>&1
	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "mongo-cxx-driver ${MONGOCXXDRIRVER_VERSION_} - Build (see ${OUTPUT_WORKDIR_PRETTYNAME_}BUILD-OUT.txt) ..."
ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && ${ECHO} "${CMAKE} --build . --config=${CMAKE_BUILD_TYPE_}" > BUILD-OUT.txt
	@+cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && "${CMAKE}"  --build . --config=${CMAKE_BUILD_TYPE_} >> BUILD-OUT.txt 2>&1
else
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && ${ECHO} "${CMAKE} --build ."  > BUILD-OUT.txt 2>&1
	@+cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && "${CMAKE}"  --build .  >> BUILD-OUT.txt 2>&1
endif
	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "mongo-cxx-driver ${MONGOCXXDRIRVER_VERSION_} - Build (see ${OUTPUT_WORKDIR_PRETTYNAME_}INSTALL-OUT.txt) ..."
ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
	@+cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && "${CMAKE}" --install . --config=${CMAKE_BUILD_TYPE_}  > INSTALL-OUT.txt 2>&1
else
	@+cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && "${CMAKE}" --install . > INSTALL-OUT.txt 2>&1
endif
ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
ifeq (${MONGOCXXDRIRVER_VERSION_},r4.0.0)
	@#The prefix= line is windows format and refers to intermediatefiles (and windows syntax on windows)- so not good - replace it with pcfiledir relative name
	@#.lib files ignored by pkg-config when using --msvc-syntax, so mapped back to -l (to be re-mapped back to .lib by pkgconfig)
	@${SED} -i "s/^prefix=.*\$$/prefix=\$${pcfiledir}\/..\/../g" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/libbson-static-1.0.pc"
	@${SED} -i "s/^prefix=.*\$$/prefix=\$${pcfiledir}\/..\/../g" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/libmongoc-static-1.0.pc"
	@${SED} -i "s/secur32\.lib/-lsecur32/g" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/libmongoc-static-1.0.pc"
	@${SED} -i "s/crypt32\.lib/-lcrypt32/g" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/libmongoc-static-1.0.pc"
	@${SED} -i "s/Bcrypt\.lib/-lBcrypt/g" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/libmongoc-static-1.0.pc"
	@${SED} -i "s/^prefix=.*\$$/prefix=\$${pcfiledir}\/..\/../g" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/libbsoncxx-static.pc"
	@${SED} -i "s/^prefix=.*\$$/prefix=\$${pcfiledir}\/..\/../g" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/libmongocxx-static.pc"
endif
endif
ifeq (${MONGOCXXDRIRVER_VERSION_},r4.1.0)
	@#enable these lines to debug Patching
	@#cp "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/bson2-static.pc" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/bson2-static.pc".orig
	@#cp "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/mongoc2-static.pc" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/mongoc2-static.pc".orig
	@#cp "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/libmongocxx-static.pc" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/libmongocxx-static.pc".orig
	@#cp "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/libbsoncxx-static.pc" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/libbsoncxx-static.pc".orig
	@#NOTE: these patches could easily change in the future! with new releases of mongo-cxx-driver
	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "mongo-cxx-driver ${MONGOCXXDRIRVER_VERSION_} - Patching: ${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/{libmongocxx,libbsoncxx}.pc"
	@#libmongocxx-static.pc has Requires: libbsoncxx-static >= 3.12.9, mongoc2 >= 2.0.0, but the required mongoc2 must be mongoc2-static (BUG)
	@${SED} -i "s/\,\ mongoc2\ /\,\ mongoc2-static\ /g" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/libmongocxx-static.pc"
	@#libbsoncxx-static.pc has Requires: bson2 >= 2.0.0, but the required bson2 must be bson2-static (BUG)
	@${SED} -i "s/\:\ bson2\ /\:\ bson2-static\ /g" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/libbsoncxx-static.pc"
ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "mongo-cxx-driver ${MONGOCXXDRIRVER_VERSION_} - Patching: ${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/{bsonc2,mongoc2}-static.pc"
	@#.lib files ignored by pkg-config when using --msvc-syntax, so mapped back to -l (to be re-mapped back to .lib by pkgconfig)
	@${SED} -i "s/\$${libdir}\/bson2\.lib/-lbson2/g" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/bson2-static.pc"
	@${SED} -i "s/\$${libdir}\/mongoc2\.lib/-lmongoc2/g" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/mongoc2-static.pc"
	@${SED} -i "s/secur32\.lib/-lsecur32/g" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/mongoc2-static.pc"
	@${SED} -i "s/crypt32\.lib/-lcrypt32/g" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/mongoc2-static.pc"
	@${SED} -i "s/Bcrypt\.lib/-lBcrypt/g" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/mongoc2-static.pc"

	@#Copy PDB files - https://jira.mongodb.org/browse/CXX-3291 - to avoid warnings - bson2.lib(common-json.obj) : warning LNK4099: PDB 'bson2.pdb' was not found with 'bson2.lib(common-json.obj)'
	@-cp ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}_deps/mongo-c-driver-build/src/libbson/${CMAKE_BUILD_TYPE_}/bson2.pdb "${PER_CONFIGURATION_BUILDS_DIR_}lib/"
	@-cp ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}_deps/mongo-c-driver-build/src/libmongoc/${CMAKE_BUILD_TYPE_}/*.pdb "${PER_CONFIGURATION_BUILDS_DIR_}lib/"
	@-cp ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}_deps/mongo-c-driver-build/src/libmongoc/utf8proc_obj.dir/Debug/utf8proc_obj.pdb "${PER_CONFIGURATION_BUILDS_DIR_}lib/"
	@-cp ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}_deps/mongo-c-driver-build/src/libmongoc/zlib_obj.dir/${CMAKE_BUILD_TYPE_}/zlib_obj.pdb "${PER_CONFIGURATION_BUILDS_DIR_}lib/"
	@-cp ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}src/bsoncxx/${CMAKE_BUILD_TYPE_}/bsoncxx-*.pdb "${PER_CONFIGURATION_BUILDS_DIR_}lib/"
	@-cp ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}src/mongocxx/${CMAKE_BUILD_TYPE_}/mongocxx-*.pdb "${PER_CONFIGURATION_BUILDS_DIR_}lib/"
endif
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
	@rm -rf	${StroikaRoot}Builds/*/lib/{*bson*,*mongo*}
	@rm -rf	${StroikaRoot}Builds/*/lib/cmake/{*bson*,*mongoc*} 
	@rm -rf	${StroikaRoot}Builds/*/lib/pkgconfig/{*bson*,*mongo*}
	@rm -rf	${StroikaRoot}Builds/*/include/{*bson*,*mongoc*} 
	@rm -rf CURRENT
else
	@rm -rf ${PRODUCED_OUTPUT_ARTIFACTS_}
	@rm -rf	${StroikaPlatformTargetBuildDir}lib/{*bson*,*mongo*}
	@rm -rf	${StroikaPlatformTargetBuildDir}lib/cmake/{*bson*,*mongoc*} 
	@rm -rf	${StroikaPlatformTargetBuildDir}lib/pkgconfig/{*bson*,*mongo*}
	@rm -rf	${StroikaPlatformTargetBuildDir}include/{*bson*,*mongoc*} 
endif
