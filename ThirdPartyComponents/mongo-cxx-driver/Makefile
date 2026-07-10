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
#	NOTE: this makefile - to build - requires an internet connection, as the build process downloads files
#	todo: nice if we could fix that to use 'Origs-Cache' mike most of our other thirdpartycomponents.
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

# NOTE r4.3.1 incompatible with openssl 4.x - https://jira.mongodb.org/browse/CXX-3516

#
# CHECK FOR UPDATED VERSIONS https://github.com/mongodb/mongo-cxx-driver/tags or https://github.com/mongodb/mongo-cxx-driver/releases
#
#		(NOTE WHEN CHANGING THIS - REVIEW THE PATCHES THAT DEPEND ON THIS VERSION BELOW)
#
MONGOCXXDRIRVER_VERSION_=r4.4.0
# MONGOCXXDRIRVER_VERSION_=r4.3.1
# MONGOCXXDRIRVER_VERSION_=r4.3.0
# MONGOCXXDRIRVER_VERSION_=r4.2.0

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

# fails to compile with no openssl
ifeq (${qFeatureFlag_OpenSSL},no)
CMAKE_ARGS+= -DENABLE_SSL=OFF
endif

#
# If we have zlib, use it, instead of the one included in the mongo-cxx-driver
# makes sense todo, but triggered linker errors for me, so diasble for now...--LGP 2025-03-19
#
#	# WITH CLANG++-15 we get
# 		/usr/bin/ld: /Sandbox/Stroika-Dev/Builds/clang++-15-debug-libc++/lib/libz.a(trees.c.o): warning: relocation against `_length_code' in read-only section `.text'
# 		/usr/bin/ld: /Sandbox/Stroika-Dev/Builds/clang++-15-debug-libc++/lib/libz.a(trees.c.o): relocation R_X86_64_PC32 against symbol `_length_code' can not be used when making a shared object; recompile with -fPIC
# 		/usr/bin/ld: final link failed: bad value
# 	during mongo-cxx-driver build
# 
# And g++-15 too
# /usr/bin/gcc -fPIC -D_GLIBCXX_DEBUG -g -fsanitize=address,undefined -O3 -DNDEBUG -Wl,--version-script="/Sandbox/Stroika-Dev/IntermediateFiles/Debug/ThirdPartyComponents/mongo-cxx-driver/_deps/mongo-c-driver-src/build/cmake/libmongoc-hidden-symbols.map" -Wl,--dependency-file=CMakeFiles/mongoc_shared.dir/link.d -shared -Wl,-soname,libmongoc2.so.2 -o libmongoc2.so.2.1.2 "CMakeFiles/mongoc_shared.dir/src/mongoc/mcd-azure.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mcd-nsinfo.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mcd-rpc.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-aggregate.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-apm.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-array.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-async.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-async-cmd.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-buffer.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-bulk-operation.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-change-stream.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-client.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-client-pool.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-client-side-encryption.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-cluster.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-cluster-aws.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-cluster-sasl.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-collection.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-compression.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-counters.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-crypt.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-cursor-array.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-cursor.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-cursor-cmd.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-cursor-change-stream.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-cursor-cmd-deprecated.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-cursor-find.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-cursor-find-cmd.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-cursor-find-opquery.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-cursor-legacy.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-database.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-error.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-deprioritized-servers.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-flags.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-find-and-modify.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-generation-map.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-init.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-gridfs.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-gridfs-bucket.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-gridfs-bucket-file.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-gridfs-file.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-gridfs-file-list.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-gridfs-file-page.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-handshake.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-host-list.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-http.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-interrupt.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-list.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-linux-distro-scanner.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-log.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-log-and-monitor-private.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-memcmp.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-cmd.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-oidc-callback.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-oidc-env.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-opcode.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-optional.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-opts-helpers.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-opts.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-queue.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-read-concern.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-read-prefs.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-rpc.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-server-api.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-server-description.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-server-stream.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-client-session.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-server-monitor.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-set.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-shared.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-socket.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-stream-buffered.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-stream.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-stream-file.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-stream-gridfs.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-stream-gridfs-download.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-stream-gridfs-upload.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-stream-socket.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-structured-log.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-timeout.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-topology.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-topology-background-monitoring.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-topology-description.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-topology-description-apm.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-topology-scanner.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-ts-pool.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-uri.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-util.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-version-functions.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-write-command.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-write-concern.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/service-gcp.c.o" "CMakeFiles/mongoc_shared.dir/__/__/__/mongo-c-driver-src/src/common/src/common-atomic.c.o" "CMakeFiles/mongoc_shared.dir/__/__/__/mongo-c-driver-src/src/common/src/common-b64.c.o" "CMakeFiles/mongoc_shared.dir/__/__/__/mongo-c-driver-src/src/common/src/common-md5.c.o" "CMakeFiles/mongoc_shared.dir/__/__/__/mongo-c-driver-src/src/common/src/common-json.c.o" "CMakeFiles/mongoc_shared.dir/__/__/__/mongo-c-driver-src/src/common/src/common-string.c.o" "CMakeFiles/mongoc_shared.dir/__/__/__/mongo-c-driver-src/src/common/src/common-oid.c.o" "CMakeFiles/mongoc_shared.dir/__/__/__/mongo-c-driver-src/src/common/src/common-thread.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-crypto.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-scram.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-stream-tls.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-ssl.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-crypto-openssl.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-rand-openssl.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-crypto-cng.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-rand-cng.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-stream-tls-secure-channel.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-secure-channel.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-crypto-common-crypto.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-rand-common-crypto.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-stream-tls-secure-transport.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-secure-transport.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-stream-tls-openssl.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-stream-tls-openssl-bio.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-openssl.c.o" "CMakeFiles/mongoc_shared.dir/src/mongoc/mongoc-ocsp-cache.c.o" "CMakeFiles/mongoc/usr/bin/ld: /Sandbox/Stroika-Dev/Builds/Debug/lib/libz.a(trees.c.o): warning: relocation against `_dist_code' in read-only section `.text'
# /usr/bin/ld: /Sandbox/Stroika-Dev/Builds/Debug/lib/libz.a(compress.c.o): relocation R_X86_64_PC32 against symbol `__asan_option_detect_stack_use_after_return' can not be used when making a shared object; recompile with -fPIC
# /usr/bin/ld: final link failed: bad value

# BROKEN ON WINDOWS TOO (for other reasons - .pc file I need to fix - but just disable for now)--LGP 2025-12-16
# ifneq (${BuildPlatform}, Unix)
# ifneq (${qFeatureFlag_ZLib}, no)
# CMAKE_ARGS+= -DENABLE_ZLIB=SYSTEM
# endif
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

#
#https://jira.mongodb.org/browse/CDRIVER-6346
#
#==47280==ERROR: AddressSanitizer: global-buffer-overflow on address 0x7ff61bbd7fee at pc 0x7ffcd9d24ec6 bp 0x00fe51cf9d60 sp 0x00fe51cf9510
#READ of size 16 at 0x7ff61bbd7fee thread T0
#    #0 0x7ffcd9d24ec5 in wcslen D:\a_work\1\s\src\vctools\asan\llvm\compiler-rt\lib\sanitizer_common\sanitizer_common_interceptors.inc:7199
#    #1 0x7ffdf99e2660 in RtlInitUnicodeStringEx+0x20 (C:\WINDOWS\SYSTEM32\ntdll.dll+0x180082660)
#    #2 0x7ffdf6c51f90 in RegQueryValueExW+0x90 (C:\WINDOWS\System32\KERNELBASE.dll+0x180021f90)
#    #3 0x7ff618cfefee in _mongoc_socket_setkeepalive_windows C:\Sandbox\Stroika\DevRoot\IntermediateFiles\Debug\ThirdPartyComponents\mongo-cxx-driver_deps\mongo-c-driver-src\src\libmongoc\src\mongoc\mongoc-socket.c:399
#
ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
CFLAGS:=$(filter-out -fsanitize=address, $(CFLAGS))
# CXXFLAGS:=$(filter-out -fsanitize=address, $(CXXFLAGS))
endif
# @todo RETEST!, and if needed, better document why/what fails...
# NOTE getting asan errors on windows - til we have basic function working - try disabling
## LGP NEED BETTER DOCS ON THIS - AND NOW THIS MAYBE CAUSING FAILURE WITH VC2k2026-- 
# CFLAGS:=$(filter-out -fsanitize=address, $(CFLAGS))
# CXXFLAGS:=$(filter-out -fsanitize=address, $(CXXFLAGS))


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


ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
CMAKE_ARGS +=  -DCMAKE_CONFIGURATION_TYPES="${CMAKE_BUILD_TYPE}"
# CMAKE_ARGS +=  -DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
endif



ifeq ($(findstring $(DETECTED_HOST_OS),MSYS-Cygwin),)
LIBPREFIX_=lib
endif



ifeq (${MONGOCXXDRIRVER_VERSION_},r4.4.0)
LIBABIVER_=1
else
LIBABIVER_=
endif



PRODUCED_OUTPUT_ARTIFACTS_=	\
	${PER_CONFIGURATION_BUILDS_DIR_}include/bsoncxx/v_noabi/bsoncxx/json.hpp			\
	${PER_CONFIGURATION_BUILDS_DIR_}lib/pkgconfig/libbsoncxx${LIBABIVER_}-static.pc					\
	${PER_CONFIGURATION_BUILDS_DIR_}lib/pkgconfig/libmongocxx${LIBABIVER_}-static.pc

#todo add more
PRODUCED_OUTPUT_ARTIFACTS_+= \
	${PER_CONFIGURATION_BUILDS_DIR_}lib/${LIBPREFIX_}mongoc2${LIB_SUFFIX}				\
	${PER_CONFIGURATION_BUILDS_DIR_}lib/pkgconfig/bson2-static.pc						\
	${PER_CONFIGURATION_BUILDS_DIR_}lib/pkgconfig/mongoc2-static.pc


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
	@${ECHO} "${CMAKE}" ${CMAKE_ARGS} "${COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT_}" > "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}CONFIGURE-OUT.txt" 2>&1
	@${ECHO} "SEE ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}CMakeCache.txt" >> "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}CONFIGURE-OUT.txt" 2>&1
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && "${CMAKE}" ${CMAKE_ARGS} ${COMPILER_NATIVE_FILENAME_FMT_SRC_ROOT_} >> CONFIGURE-OUT.txt 2>&1
	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "mongo-cxx-driver ${MONGOCXXDRIRVER_VERSION_} - Build (see ${OUTPUT_WORKDIR_PRETTYNAME_}BUILD-OUT.txt) ..."
ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && ${ECHO} "${CMAKE} --build . --config=${CMAKE_BUILD_TYPE}" > BUILD-OUT.txt
	@+cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && "${CMAKE}"  --build . --config=${CMAKE_BUILD_TYPE} >> BUILD-OUT.txt 2>&1
else
	@cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && ${ECHO} "${CMAKE} --build ."  > BUILD-OUT.txt 2>&1
	@+cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && "${CMAKE}"  --build .  >> BUILD-OUT.txt 2>&1
endif
	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "mongo-cxx-driver ${MONGOCXXDRIRVER_VERSION_} - Install (see ${OUTPUT_WORKDIR_PRETTYNAME_}INSTALL-OUT.txt) ..."
ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
	@+cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && "${CMAKE}" --install . --config=${CMAKE_BUILD_TYPE}  > INSTALL-OUT.txt 2>&1
else
	@+cd "${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}" && "${CMAKE}" --install . > INSTALL-OUT.txt 2>&1
endif
ifeq (${MONGOCXXDRIRVER_VERSION_},r4.2.0)
ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "mongo-cxx-driver ${MONGOCXXDRIRVER_VERSION_} - Patching: ${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/mongoc2-static.pc"
	@if grep -q "\-l[^ ]*zstd\.lib" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/mongoc2-static.pc"; then\
		${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+2)) -n '+';\
		${SED} -i "s/\-l[^ ]*\/zstd\.lib//" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/mongoc2-static.pc";\
		${ECHO} '+';\
		${SED} -i "s/Requires: .*/&, libzstd/" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/mongoc2-static.pc";\
	fi
endif
	ln -s ${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/libbsoncxx-static.pc ${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/libmongocxx1-static.pc
else ifeq (${MONGOCXXDRIRVER_VERSION_},r4.3.0)
ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "mongo-cxx-driver ${MONGOCXXDRIRVER_VERSION_} - Patching: ${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/mongoc2-static.pc"
	@if grep -q "\-l[^ ]*zstd\.lib" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/mongoc2-static.pc"; then\
		${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+2)) -n '+';\
		${SED} -i "s/\-l[^ ]*\/zstd\.lib//" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/mongoc2-static.pc";\
		${ECHO} '+';\
		${SED} -i "s/Requires: .*/&, libzstd/" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/mongoc2-static.pc";\
	fi
endif
	ln -s ${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/libbsoncxx-static.pc ${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/libmongocxx1-static.pc
else ifeq (${MONGOCXXDRIRVER_VERSION_},r4.3.1)
ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "mongo-cxx-driver ${MONGOCXXDRIRVER_VERSION_} - Patching: ${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/mongoc2-static.pc"
	@if grep -q "\-l[^ ]*zstd\.lib" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/mongoc2-static.pc"; then\
		${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+2)) -n '+';\
		${SED} -i "s/\-l[^ ]*\/zstd\.lib//" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/mongoc2-static.pc";\
		${ECHO} '+';\
		${SED} -i "s/Requires: .*/&, libzstd/" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/mongoc2-static.pc";\
	fi
endif
else ifeq (${MONGOCXXDRIRVER_VERSION_},r4.4.0)
ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
	@# @todo test if still needed
	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "mongo-cxx-driver ${MONGOCXXDRIRVER_VERSION_} - Patching: ${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/mongoc2-static.pc"
	@if grep -q "\-l[^ ]*zstd\.lib" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/mongoc2-static.pc"; then\
		${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+2)) -n '+';\
		${SED} -i "s/\-l[^ ]*\/zstd\.lib//" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/mongoc2-static.pc";\
		${ECHO} '+';\
		${SED} -i "s/Requires: .*/&, libzstd/" "${PER_CONFIGURATION_BUILDS_DIR_}/lib/pkgconfig/mongoc2-static.pc";\
	fi
endif
endif
ifeq (VisualStudio,$(findstring VisualStudio,$(BuildPlatform)))
	@# Avoid warnings:
	@#		mongoc2.lib(mongoc-find-and-modify.obj) : warning LNK4099: PDB 'mongoc2.pdb' was not found with 'mongoc2.lib(mongoc-find-and-modify.obj)' or at 'C:\Sandbox\Stroika\DevRoot\Builds\Debug\Tests\mongoc2.pdb'; linking object as if no debug info
	@#		...
	@#		bson2.lib(common-b64.obj) : warning LNK4099: PDB 'bson2.pdb' was not found with 'bson2.lib(common-b64.obj)' or at 'C:\Sandbox\Stroika\DevRoot\Builds\Debug\Tests\bson2.pdb'; linking object as if no debug info
	@#Copy PDB files - https://jira.mongodb.org/browse/CXX-3291 - to avoid warnings - bson2.lib(common-json.obj) : warning LNK4099: PDB 'bson2.pdb' was not found with 'bson2.lib(common-json.obj)'
	@#	(which occur after a make clean on ThirdPartyComponents)
	@-cp ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}_deps/mongo-c-driver-build/src/libbson/${CMAKE_BUILD_TYPE}/bson2.pdb "${PER_CONFIGURATION_BUILDS_DIR_}lib/" 2> /dev/null
	@-cp ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}_deps/mongo-c-driver-build/src/libmongoc/${CMAKE_BUILD_TYPE}/*.pdb "${PER_CONFIGURATION_BUILDS_DIR_}lib/" 2> /dev/null
	@-cp ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}_deps/mongo-c-driver-build/src/libmongoc/utf8proc_obj.dir/Debug/utf8proc_obj.pdb "${PER_CONFIGURATION_BUILDS_DIR_}lib/" 2> /dev/null
	@-cp ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}_deps/mongo-c-driver-build/src/libmongoc/zlib_obj.dir/${CMAKE_BUILD_TYPE}/zlib_obj.pdb "${PER_CONFIGURATION_BUILDS_DIR_}lib/" 2> /dev/null
	@-cp ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}src/bsoncxx/${CMAKE_BUILD_TYPE}/bsoncxx-*.pdb "${PER_CONFIGURATION_BUILDS_DIR_}lib/" 2> /dev/null
	@-cp ${PER_CONFIGURATION_THIS_INTERMEDIATEFILES_DIR_}src/mongocxx/${CMAKE_BUILD_TYPE}/mongocxx-*.pdb "${PER_CONFIGURATION_BUILDS_DIR_}lib/" 2> /dev/null
endif



${ZIPPED_SOURCES_FILE_}:
	@MAKE_INDENT_LEVEL=$$((${MAKE_INDENT_LEVEL}+1)) ${StroikaRoot}ScriptsLib/WebGet ${ZIPPED_SOURCES_FILE_} ${FETCHURLS_}


ORIGs CURRENT:	${ZIPPED_SOURCES_FILE_}
	@${StroikaRoot}ScriptsLib/PrintProgressLine $$((${MAKE_INDENT_LEVEL}+1)) "mongo-cxx-driver ${MONGOCXXDRIRVER_VERSION_} - Extracting ${ZIPPED_SOURCES_FILE_} to" `$(StroikaRoot)ScriptsLib/SubstituteBackVariables $@` "... "
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
