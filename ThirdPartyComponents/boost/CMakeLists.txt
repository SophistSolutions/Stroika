#
# boost, as its Makefile builds it: bootstrapped, then built and installed by its own b2, among its sources
#
set (version_ 1_92_0)
string (REPLACE "_" "." versionWithDots_ "${version_}")

if (STROIKA_BUILD_PLATFORM MATCHES "^VisualStudio")
    message (FATAL_ERROR "ThirdPartyComponents/boost: no Visual Studio build here (bootstrap.bat, vcvars) - its Makefile builds it")
endif ()

# b2 takes no job slots from make (it is no jobserver client): the jobs to give it
set (STROIKA_BOOST_JOBS 8 CACHE STRING "boost's b2 -j: how many compiles at once")

# the compiler and the configuration's flags, as its Makefile writes them into user-config.jam (no link flags: it defines none)
if (CMAKE_HOST_APPLE)
    set (toolset_ clang-darwin)
else ()
    set (toolset_ gcc)
endif ()
file (WRITE "${CMAKE_CURRENT_BINARY_DIR}/user-config.jam"
    "using ${toolset_} : : \"${CMAKE_CXX_COMPILER}\" : <cflags>\"${CMAKE_C_FLAGS}\" <cxxflags>\"${CMAKE_CXX_FLAGS}\" <linkflags>\"\" ;\n")

set (b2Args_ toolset=${toolset_} link=static runtime-link=static threading=multi --layout=system
    --without-atomic --without-python --without-regex --without-wave)
execute_process (COMMAND "${CMAKE_CXX_COMPILER}" -dumpversion OUTPUT_VARIABLE compilerVersion_ OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
if (STROIKA_ARCH STREQUAL "armhf" AND NOT CMAKE_CXX_COMPILER MATCHES "clang" AND compilerVersion_ VERSION_LESS 12)
    list (APPEND b2Args_ --without-context context-impl=ucontext --without-cobalt --without-coroutine --without-fiber
        --without-graph_parallel --without-mpi)
endif ()
if (CMAKE_CXX_COMPILER MATCHES "clang" AND CMAKE_CXX_FLAGS MATCHES "-stdlib=libc\\+\\+")
    list (APPEND b2Args_ --without-cobalt)
endif ()
if (STROIKA_ASSERTIONS_ENABLED)
    list (APPEND b2Args_ variant=debug)
else ()
    list (APPEND b2Args_ variant=release)
endif ()
list (APPEND b2Args_ --hash -d2 "--prefix=${STROIKA_BUILDS_DIR}" "-sBOOST_BUILD_USER_CONFIG=${CMAKE_CURRENT_BINARY_DIR}/user-config.jam"
    -j${STROIKA_BOOST_JOBS})

StroikaThirdPartyComponent (boost
    ARCHIVE boost_${version_}.tar.gz
    URLS https://archives.boost.io/release/${versionWithDots_}/source/boost_${version_}.tar.gz
         https://sourceforge.net/projects/boost/files/boost/${versionWithDots_}/boost_${version_}.tar.gz/download
    BUILD_IN_SOURCE
    CONFIGURE_COMMAND ./bootstrap.sh "--prefix=${STROIKA_BUILDS_DIR}"
    BUILD_COMMAND ./b2 ${b2Args_} install
    # Stroika's boost.pc, and BOOST_AUTO_LINK_SYSTEM in boost/config/user.hpp: it is built with --layout=system
    INSTALL_COMMAND "${CMAKE_COMMAND}" "-DBUILDS_DIR=${STROIKA_BUILDS_DIR}" "-DPC=${CMAKE_CURRENT_SOURCE_DIR}/boost.pc"
                    -P "${CMAKE_CURRENT_SOURCE_DIR}/AfterInstall.cmake"
    DEPENDS zlib zstd
)
