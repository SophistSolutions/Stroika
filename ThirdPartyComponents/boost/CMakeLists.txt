#
# boost, as its Makefile builds it: bootstrapped, then built and installed by its own b2, among its sources - with Visual
# Studio, in Visual Studio's environment
#
set (version_ 1_92_0)
string (REPLACE "_" "." versionWithDots_ "${version_}")

# The compiler and the configuration's flags, written into user-config.jam as its Makefile writes them (no link flags: it
# defines none) - but on UNIX with the platform's preprocessor definitions too, which its Makefile leaves out, as every
# other C++ component and Stroika's own code get them: with assertions, -D_GLIBCXX_DEBUG, which changes the standard
# library's types. With Visual Studio, as its Makefile's (without -DUNICODE ...).
if (STROIKA_BUILD_PLATFORM MATCHES "^VisualStudio")
    set (toolset_ msvc)
    set (cFlags_ "${STROIKA_PLATFORM_C_FLAGS}")
    set (cxxFlags_ "${STROIKA_PLATFORM_CXX_FLAGS}")
else ()
    if (CMAKE_HOST_APPLE)
        set (toolset_ clang-darwin)
    else ()
        set (toolset_ gcc)
    endif ()
    set (cFlags_ "${CMAKE_C_FLAGS}")
    set (cxxFlags_ "${CMAKE_CXX_FLAGS}")
endif ()
file (CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/user-config.jam" @ONLY
    CONTENT "using ${toolset_} : : \"${CMAKE_CXX_COMPILER}\" : <cflags>\"${cFlags_}\" <cxxflags>\"${cxxFlags_}\" <linkflags>\"\" ;\n")

# --hash: b2 names its build directories by a hash, not by their properties - else too long for Windows
set (b2Args_ toolset=${toolset_} link=static runtime-link=static threading=multi --layout=system
    --without-atomic --without-python --without-regex --without-wave)
if (NOT toolset_ STREQUAL "msvc")
    execute_process (COMMAND "${CMAKE_CXX_COMPILER}" -dumpversion OUTPUT_VARIABLE compilerVersion_ OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
    if (STROIKA_ARCH STREQUAL "armhf" AND NOT CMAKE_CXX_COMPILER MATCHES "clang" AND compilerVersion_ VERSION_LESS 12)
        list (APPEND b2Args_ --without-context context-impl=ucontext --without-cobalt --without-coroutine --without-fiber
            --without-graph_parallel --without-mpi)
    endif ()
    if (CMAKE_CXX_COMPILER MATCHES "clang" AND CMAKE_CXX_FLAGS MATCHES "-stdlib=libc\\+\\+")
        list (APPEND b2Args_ --without-cobalt)
    endif ()
endif ()
if (STROIKA_ASSERTIONS_ENABLED)
    list (APPEND b2Args_ variant=debug)
else ()
    list (APPEND b2Args_ variant=release)
endif ()
list (APPEND b2Args_ --hash -d2 "--prefix=${STROIKA_BUILDS_DIR}" "-sBOOST_BUILD_USER_CONFIG=${CMAKE_CURRENT_BINARY_DIR}/user-config.jam")

if (toolset_ STREQUAL "msvc")
    # Bootstrapped in tools/build, whose b2 also goes beside its sources - and, installed, in the configuration's Builds
    # directory, as its Makefile installs it. Then its build, for the configuration's architecture; b2's jobs its own
    # default, one per processor, as with make.
    if (STROIKA_ARCH STREQUAL "x86")
        list (APPEND b2Args_ address-model=32)
    else ()
        list (APPEND b2Args_ address-model=64)
    endif ()
    list (JOIN b2Args_ " " b2Args_)
    StroikaVisualStudioCommand (configure_ configure "cd tools\\build
call bootstrap.bat
if not exist ..\\..\\b2.exe copy src\\engine\\b2.exe ..\\..\\b2.exe
.\\b2 install --prefix=${STROIKA_BUILDS_DIR}")
    StroikaVisualStudioCommand (build_ build "if exist bin.v2 rmdir /s /q bin.v2\n.\\b2 ${b2Args_} install")
    set (pc_ boost-msvc.pc)
else ()
    # b2 takes no job slots from make (it is no jobserver client): the jobs to give it
    set (STROIKA_BOOST_JOBS 8 CACHE STRING "boost's b2 -j: how many compiles at once")
    set (configure_ ./bootstrap.sh "--prefix=${STROIKA_BUILDS_DIR}")
    set (build_ "${CMAKE_COMMAND}" -E rm -rf bin.v2 COMMAND ./b2 ${b2Args_} -j${STROIKA_BOOST_JOBS} install)
    set (pc_ boost.pc)
endif ()
# Stroika's boost.pc, and BOOST_AUTO_LINK_SYSTEM in boost/config/user.hpp: it is built with --layout=system
set (afterInstall_ "${CMAKE_COMMAND}" "-DBUILDS_DIR=${STROIKA_BUILDS_DIR}" "-DPC=${CMAKE_CURRENT_SOURCE_DIR}/${pc_}"
    -P "${CMAKE_CURRENT_SOURCE_DIR}/AfterInstall.cmake")

StroikaThirdPartyComponent (boost
    ARCHIVE boost_${version_}.tar.gz
    URLS https://archives.boost.io/release/${versionWithDots_}/source/boost_${version_}.tar.gz
         https://sourceforge.net/projects/boost/files/boost/${versionWithDots_}/boost_${version_}.tar.gz/download
    BUILD_IN_SOURCE
    CONFIGURE_COMMAND ${configure_}
    BUILD_COMMAND ${build_}
    INSTALL_COMMAND ${afterInstall_}
    DEPENDS zlib zstd
)
# Built again when its flags change - in user-config.jam, not its build's command line, which ExternalProject compares - and
# from the start (its build first removes b2's, bin.v2): b2 takes no object as out of date for flags changed there.
ExternalProject_Add_StepDependencies (boost build "${CMAKE_CURRENT_BINARY_DIR}/user-config.jam")
