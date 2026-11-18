#
# mongo-cxx-driver - and the mongo-c-driver its own configure fetches - as its Makefile builds it
#
set (version_ r4.6.1)

# Its version (r4.6.1 is 4.6.1), which it must be given: GitHub's tag archive has no build/VERSION_CURRENT, so it would
# otherwise compute one from git history it does not have
string (REGEX REPLACE "^r" "" buildVersion_ "${version_}")
set (args_ "-DBUILD_VERSION=${buildVersion_}" -DBUILD_SHARED_LIBS:BOOL=OFF -DENABLE_SHARED:BOOL=OFF)
if (STROIKA_FEATURE_OpenSSL STREQUAL "no")
    list (APPEND args_ -DENABLE_SSL=OFF)
endif ()

if (STROIKA_BUILD_PLATFORM MATCHES "^VisualStudio")
    # the configuration's flags for its build type, as its Makefile adjusts them: two files at once in each compile (-MP2 -
    # msbuild compiles a project's files one at a time, and mongo builds largely alone, after openssl), the warnings it
    # would give where Stroika links it with link-time code generation silenced, and no AddressSanitizer in its C
    # (CDRIVER-6346: a global-buffer-overflow in _mongoc_socket_setkeepalive_windows)
    string (TOUPPER "${STROIKA_THIRD_PARTY_BUILD_TYPE}" buildType_)
    set (cFlags_ "${CMAKE_C_FLAGS_${buildType_}}")
    set (cxxFlags_ "${CMAKE_CXX_FLAGS_${buildType_}}")
    if (STROIKA_LINK_OPTIONS MATCHES "-LTCG")
        string (APPEND cFlags_ " -wd4701 -wd4702 -wd4724")
    endif ()
    string (APPEND cFlags_ " -MP2")
    string (APPEND cxxFlags_ " -MP2")
    string (REGEX REPLACE "(^| )-fsanitize=address( |$)" "\\1" cFlags_ "${cFlags_}")
    list (APPEND args_ "-DCMAKE_C_FLAGS_${buildType_}=${cFlags_}" "-DCMAKE_CXX_FLAGS_${buildType_}=${cxxFlags_}"
        "-DCMAKE_CONFIGURATION_TYPES=${STROIKA_THIRD_PARTY_BUILD_TYPE}")
    # and its mongoc2-static.pc fixed after its install (WindowsPc.cmake)
    set (install_ INSTALL_COMMAND "${CMAKE_COMMAND}" --install <BINARY_DIR> --config ${STROIKA_THIRD_PARTY_BUILD_TYPE}
        COMMAND "${CMAKE_COMMAND}" "-DPC=${STROIKA_BUILDS_DIR}lib/pkgconfig/mongoc2-static.pc" -P "${CMAKE_CURRENT_SOURCE_DIR}/WindowsPc.cmake")
else ()
    # the configuration's flags, as its Makefile adjusts them: no -freflection (it is built as an older C++); with g++, no
    # -flto, no -fsanitize=undefined before g++ 12, and -O1 under ThreadSanitizer at -O0 (GCC bug 59922: mongo-c-driver's
    # -Werror=return-type fails its build)
    set (cFlags_ "${STROIKA_THIRD_PARTY_C_FLAGS}")
    string (REGEX REPLACE "(^| )-freflection( |$)" "\\1" cxxFlags_ "${STROIKA_THIRD_PARTY_CXX_FLAGS}")
    execute_process (COMMAND "${CMAKE_C_COMPILER}" --version OUTPUT_VARIABLE compilerBanner_ ERROR_QUIET)
    execute_process (COMMAND "${CMAKE_C_COMPILER}" -dumpfullversion OUTPUT_VARIABLE compilerVersion_ OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
    if (NOT compilerBanner_ MATCHES "clang")
        foreach (flags_ IN ITEMS cFlags_ cxxFlags_)
            string (REGEX REPLACE "(^| )-flto( |$)" "\\1" ${flags_} "${${flags_}}")
            if (compilerVersion_ VERSION_LESS 12)
                string (REGEX REPLACE "(^| )-fsanitize=undefined( |$)" "\\1" ${flags_} "${${flags_}}")
            endif ()
        endforeach ()
        if (cFlags_ MATCHES "-fsanitize=thread" AND NOT cFlags_ MATCHES "(^| )-O[1-9sgz]")
            string (APPEND cFlags_ " -O1")
            string (APPEND cxxFlags_ " -O1")
        endif ()
    endif ()
    list (APPEND args_ "-DCMAKE_C_FLAGS=${cFlags_}" "-DCMAKE_CXX_FLAGS=${cxxFlags_}")
    set (install_)
endif ()

# configured twice, as its Makefile does (Configure.cmake): its arguments, each bracket-quoted, in a file for that script
set (arguments_ ${STROIKA_THIRD_PARTY_GENERATOR_ARGS} ${STROIKA_THIRD_PARTY_CMAKE_ARGS} ${args_})
set (text_ "set (arguments_")
foreach (argument_ IN LISTS arguments_)
    string (APPEND text_ "\n    [==[${argument_}]==]")
endforeach ()
file (WRITE "${CMAKE_CURRENT_BINARY_DIR}/configure-arguments.cmake" "${text_}\n)\n")

StroikaThirdPartyComponent (mongo-cxx-driver
    ARCHIVE mongo-cxx-driver-${version_}.tar.gz
    URLS https://github.com/mongodb/mongo-cxx-driver/archive/refs/tags/${version_}.tar.gz
    CONFIGURE_COMMAND "${CMAKE_COMMAND}" -E env "PKG_CONFIG_PATH=${STROIKA_PKG_CONFIG_PATH}"
                      "${CMAKE_COMMAND}" "-DARGUMENTS_FILE=${CMAKE_CURRENT_BINARY_DIR}/configure-arguments.cmake" -DSOURCE_DIR=<SOURCE_DIR>
                      -DBINARY_DIR=<BINARY_DIR> -P "${CMAKE_CURRENT_SOURCE_DIR}/Configure.cmake"
    ${install_}
    DEPENDS openssl zlib zstd # zstd: mongo-c-driver finds it, for its compression
)
