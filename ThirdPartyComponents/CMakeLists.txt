#
# The configuration's third-party components, built with CMake beside make (#1157). Each <name>/CMakeLists.txt builds
# one as its Makefile does: its source archive from Origs-Cache (downloaded there when missing), configured with the
# configuration's toolchain file, built, and installed into the configuration's Builds directory (STROIKA_BUILDS_DIR) -
# where Stroika's own CMake build finds it (the top CMakeLists.txt, which builds this as it is configured). For one
# configuration:
#     cmake -S ThirdPartyComponents -B <build directory> -DCMAKE_TOOLCHAIN_FILE=<build root>/<configuration>/Toolchain.cmake
#     CMAKE_BUILD_PARALLEL_LEVEL=8 cmake --build <build directory>
# The jobs: with a Makefiles generator, the components' own builds share make's job slots, as under make; with Ninja, they
# build one at a time, each with CMAKE_BUILD_PARALLEL_LEVEL's (make's -jN sets it). curl builds here on UNIX only, as
# Stroika uses it only there.
#
cmake_minimum_required (VERSION 3.22)
project (StroikaThirdPartyComponents LANGUAGES NONE)

# the configuration's settings for each component's build, and StroikaThirdPartyComponent, which each <name>/CMakeLists.txt
# calls
include ("${CMAKE_CURRENT_SOURCE_DIR}/StroikaThirdPartyComponent.cmake")

#
# The components Conan keeps (StroikaComponentFromConan): built by their recipes here the first time a configuration needs
# one, and found in Conan's cache after that - by any build tree on the machine. One left out of this list is built here, in
# this build tree.
#
set (STROIKA_THIRD_PARTY_FROM_CONAN zlib zstd StrawberryPerl openssl curl boost mongo-cxx-driver libxml2 Xerces sqlite lzma fmtlib GoogleTest WIX)
if (STROIKA_THIRD_PARTY_FROM_CONAN)
    find_program (STROIKA_CONAN conan REQUIRED)
    # The configuration, to Conan (conanfile.py's option "configuration"): a digest of its toolchain file and Stroika settings,
    # less what names this build tree or the configuration - their paths, its name and file - so the same configuration in
    # another tree finds its components in Conan's cache. And of its generator, which can change what a component installs:
    # mongo-cxx-driver's library names carry the Visual Studio toolset only under a Visual Studio generator.
    file (READ "${CMAKE_TOOLCHAIN_FILE}" toolchain_)
    file (READ "${configurationDir_}/Stroika-Configuration.cmake" settings_)
    string (REGEX REPLACE "#[^\n]*\n" "" toolchain_ "${toolchain_}") # their comments name the configuration
    string (REGEX REPLACE "#[^\n]*\n" "" settings_ "${settings_}")
    string (REGEX REPLACE "set\\(STROIKA_CONFIGURATION(_FILE)? [^\n]*\n" "" settings_ "${settings_}")
    cmake_path (GET configurationDir_ PARENT_PATH buildRoot_)
    cmake_path (GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH sourceRoot_)
    foreach (path_ IN ITEMS "${STROIKA_BUILDS_DIR}" "${buildRoot_}" "${sourceRoot_}")
        string (REPLACE "${path_}" "<tree>" toolchain_ "${toolchain_}")
        string (REPLACE "${path_}" "<tree>" settings_ "${settings_}")
    endforeach ()
    string (SHA256 STROIKA_CONAN_CONFIGURATION_DIGEST "${toolchain_}${settings_}${CMAKE_GENERATOR}|${CMAKE_GENERATOR_PLATFORM}|${CMAKE_GENERATOR_TOOLSET}")
    # its profile: just the OS building - which Conan needs, to write its scripts for its shell - as the components declare no
    # settings (so it is no part of a package's identity)
    set (os_ "${CMAKE_HOST_SYSTEM_NAME}")
    if (os_ STREQUAL "Darwin")
        set (os_ Macos)
    endif ()
    set (STROIKA_CONAN_PROFILE "${CMAKE_CURRENT_BINARY_DIR}/conan-profile")
    file (WRITE "${STROIKA_CONAN_PROFILE}" "# Stroika's third-party components declare no settings: the configuration is an option (conanfile.py)\n[settings]\nos=${os_}\n")
    # What a component's own build, in Conan's cache, uses as this one does (conanfile.py): cmake, its generator - and Visual
    # Studio's platform, toolset and instance, or the make program (Ninja's) - and, with a multi-configuration generator
    # (Visual Studio's), the build type
    set (STROIKA_CONAN_CONF "user.stroika:cmake=${CMAKE_COMMAND}" "user.stroika:generator=${CMAKE_GENERATOR}")
    foreach (setting_ IN ITEMS GENERATOR_PLATFORM GENERATOR_TOOLSET GENERATOR_INSTANCE MAKE_PROGRAM)
        if (CMAKE_${setting_} AND NOT (setting_ STREQUAL "MAKE_PROGRAM" AND CMAKE_GENERATOR MATCHES "^Visual Studio"))
            string (TOLOWER "${setting_}" name_)
            list (APPEND STROIKA_CONAN_CONF "user.stroika:${name_}=${CMAKE_${setting_}}")
        endif ()
    endforeach ()
    get_property (multiConfig_ GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if (multiConfig_)
        list (APPEND STROIKA_CONAN_CONF "user.stroika:build_config=${STROIKA_THIRD_PARTY_BUILD_TYPE}")
    endif ()
    # the lock each component's Conan steps take, in Conan's home: so builds sharing its cache - which is not safe for two
    # Conan commands at once - take turns
    execute_process (COMMAND "${STROIKA_CONAN}" config home OUTPUT_VARIABLE STROIKA_CONAN_HOME OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ERROR_IS_FATAL ANY)
    file (TO_CMAKE_PATH "${STROIKA_CONAN_HOME}" STROIKA_CONAN_HOME)
    set (STROIKA_CONAN_LOCK "${STROIKA_CONAN_HOME}/stroika-third-party.lock")
endif ()

#
# StroikaConanPackage_ (<variable> <directory>)
#
# The component in <directory> as Conan names its package (conanfile.py): stroika-<directory> in lower case, as Conan requires
# of a name, and its recipe's version (set (version_ ...)) - <variable>_PACKAGE, <variable>_VERSION, and <variable>, the two
# as a reference
#
function (StroikaConanPackage_ variable directory)
    file (STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/${directory}/CMakeLists.txt" version_ REGEX "^set \\(version_ ")
    string (REGEX REPLACE "^set \\(version_ ([^) ]+)\\).*" "\\1" version_ "${version_}")
    string (TOLOWER "stroika-${directory}" package_)
    set (${variable}_PACKAGE ${package_} PARENT_SCOPE)
    set (${variable}_VERSION ${version_} PARENT_SCOPE)
    set (${variable} ${package_}/${version_} PARENT_SCOPE)
endfunction ()

#
# StroikaComponentFromConan (<directory> [DEPENDS <component>...] [TOOLS <component>...])
#
# The component in <directory>, from Conan's cache, copied into the configuration's Builds directory (StroikaDeploy.py) -
# built into the cache first, by its recipe here (conanfile.py), when the configuration has no package of it there yet, on the
# components it DEPENDS on, with the TOOLS its build runs (each one Conan keeps). A target of its name, as
# StroikaThirdPartyComponent makes, for the components built after it; it runs Conan again only once what builds the
# component, or one it depends on, changes. Conan's commands are in a script (conan.cmake, in the component's binary
# directory), which takes Conan's lock first.
#
function (StroikaComponentFromConan directory)
    cmake_parse_arguments (PARSE_ARGV 1 arg "" "" "DEPENDS;TOOLS")
    StroikaConanPackage_ (this ${directory})
    # what builds it, as conanfile.py exports it
    file (GLOB_RECURSE recipe_ LIST_DIRECTORIES false "${CMAKE_CURRENT_SOURCE_DIR}/${directory}/*")
    list (FILTER recipe_ EXCLUDE REGEX "/${directory}/(Makefile$|CURRENT|ORIGS|ORIGs)")
    set (binaryDir_ "${CMAKE_CURRENT_BINARY_DIR}/${directory}")
    # its dependencies and tools, as options (conanfile.py) - its own, and those of each component in its graph (added
    # before it): each Conan command is given those of all added so far. Its tools' recipes it exports, as each component
    # exports its own.
    set (depends_)
    set (dependsStamps_)
    foreach (component_ IN LISTS arg_DEPENDS)
        StroikaConanPackage_ (dependency ${component_})
        list (APPEND depends_ ${dependency})
        list (APPEND dependsStamps_ "${CMAKE_CURRENT_BINARY_DIR}/${component_}/conan-stamp")
    endforeach ()
    set (tools_)
    set (exports_)
    foreach (component_ IN LISTS arg_TOOLS ITEMS ${directory})
        StroikaConanPackage_ (package ${component_})
        if (NOT component_ STREQUAL directory)
            list (APPEND tools_ ${package})
        endif ()
        string (APPEND exports_ "execute_process (COMMAND [==[${STROIKA_CONAN}]==] export [==[${CMAKE_CURRENT_SOURCE_DIR}]==] --name ${package_PACKAGE} --version ${package_VERSION} COMMAND_ERROR_IS_FATAL ANY)\n")
    endforeach ()
    list (JOIN depends_ "," depends_)
    list (JOIN tools_ "," tools_)
    set_property (GLOBAL APPEND PROPERTY STROIKA_CONAN_OPTIONS_ -o:a "${this_PACKAGE}/*:depends=${depends_}" -o:a "${this_PACKAGE}/*:tools=${tools_}")
    get_property (options_ GLOBAL PROPERTY STROIKA_CONAN_OPTIONS_)
    # options and conf values for both contexts (:a) - the tools' (build) too, not just what is built into Stroika (host)
    set (install_ "${STROIKA_CONAN}" install --requires ${this} --build missing
        -o:a "stroika-*/*:configuration=${STROIKA_CONAN_CONFIGURATION_DIGEST}" ${options_} -pr:a "${STROIKA_CONAN_PROFILE}"
        -c:a "user.stroika:toolchain_file=${CMAKE_TOOLCHAIN_FILE}" -c:a "user.stroika:origs_cache=${STROIKA_ORIGS_CACHE}")
    foreach (conf_ IN LISTS STROIKA_CONAN_CONF)
        list (APPEND install_ -c:a "${conf_}")
    endforeach ()
    list (APPEND install_ --deployer "${CMAKE_CURRENT_SOURCE_DIR}/StroikaDeploy.py" --deployer-folder "${STROIKA_BUILDS_DIR}"
        --output-folder "${binaryDir_}")
    # each argument a bracket argument in the script, so none needs quoting
    list (JOIN install_ "]==] [==[" install_)
    # written only when changed (file (CONFIGURE)), so the script's time says when it last did
    file (CONFIGURE OUTPUT "${binaryDir_}/conan.cmake" @ONLY CONTENT "\
# ${this_PACKAGE} from Conan's cache - built there first, if the configuration has no package of it yet - copied into the\n\
# configuration's Builds directory: cmake -P, by ThirdPartyComponents/CMakeLists.txt's StroikaComponentFromConan, which wrote it.\n\
# MAKEFLAGS unset: Conan does not pass on make's job slots, so the component's own build takes CMAKE_BUILD_PARALLEL_LEVEL's jobs\n\
unset (ENV{MAKEFLAGS})\n\
unset (ENV{MFLAGS})\n\
file (LOCK [==[${STROIKA_CONAN_LOCK}]==] GUARD PROCESS TIMEOUT 0 RESULT_VARIABLE locked_)\n\
if (NOT locked_ EQUAL 0)\n\
    message (STATUS [==[${this_PACKAGE}: waiting for another build's Conan steps (${STROIKA_CONAN_LOCK})]==])\n\
    file (LOCK [==[${STROIKA_CONAN_LOCK}]==] GUARD PROCESS)\n\
endif ()\n\
${exports_}\
execute_process (COMMAND [==[${install_}]==] COMMAND_ERROR_IS_FATAL ANY)\n\
# Conan's cache keeps the components' packages, not their builds (as make clean removes a component's own): their build\n\
# folders, many GB for boost, removed once packaged - each package's b, beside its p, in a folder Conan names for the first\n\
# five letters of its name. By CMake, not conan cache clean, whose python cannot remove a path longer than Windows'\n\
# MAX_PATH (boost's documentation's, as of 1.92.0).\n\
file (GLOB buildFolders_ LIST_DIRECTORIES true [==[${STROIKA_CONAN_HOME}/p/b/stroi*/b]==])\n\
# none when nothing was built: file (REMOVE_RECURSE) with no paths is an error in CMake 3.28.3 (not 3.31.6)\n\
if (buildFolders_)\n\
    file (REMOVE_RECURSE \${buildFolders_})\n\
endif ()\n\
file (TOUCH [==[${binaryDir_}/conan-stamp]==])\n\
")
    # run again when one it depends on was: its package is then another
    add_custom_command (OUTPUT "${binaryDir_}/conan-stamp"
        COMMAND "${CMAKE_COMMAND}" -P "${binaryDir_}/conan.cmake"
        DEPENDS "${binaryDir_}/conan.cmake" ${recipe_} ${dependsStamps_} "${CMAKE_CURRENT_SOURCE_DIR}/StroikaThirdPartyComponent.cmake"
                "${CMAKE_CURRENT_SOURCE_DIR}/RemoveFiles.cmake" "${CMAKE_CURRENT_SOURCE_DIR}/conanfile.py"
                "${CMAKE_CURRENT_SOURCE_DIR}/StroikaDeploy.py" "${CMAKE_TOOLCHAIN_FILE}" "${configurationDir_}/Stroika-Configuration.cmake"
        USES_TERMINAL
        VERBATIM
    )
    add_custom_target (${directory} ALL DEPENDS "${binaryDir_}/conan-stamp")
    if (arg_DEPENDS)
        add_dependencies (${directory} ${arg_DEPENDS})
    endif ()
endfunction ()

#
# StroikaComponent (<directory> <feature> [TOOL] [DEPENDS <directory>...] [TOOLS <directory>...])
#
# The component in <directory>, where the configuration uses it (configure's --<feature> use): from Conan's cache, if it is
# one Conan keeps (STROIKA_THIRD_PARTY_FROM_CONAN), else built here. What it DEPENDS on - as its recipe's own DEPENDS
# (StroikaThirdPartyComponent), the components its build finds installed - and the TOOLS its build runs are those the
# configuration uses; a TOOL - a program another's build runs, not built into Stroika - Conan builds only as that one's
# tool, which is not copied into the configuration's Builds directory.
#
function (StroikaComponent directory feature)
    cmake_parse_arguments (PARSE_ARGV 2 arg "TOOL" "" "DEPENDS;TOOLS")
    if (NOT STROIKA_FEATURE_${feature} STREQUAL "use")
        return ()
    endif ()
    if (NOT directory IN_LIST STROIKA_THIRD_PARTY_FROM_CONAN)
        add_subdirectory (${directory})
        return ()
    endif ()
    set_property (GLOBAL APPEND PROPERTY STROIKA_CONAN_COMPONENTS_ ${directory})
    if (arg_TOOL)
        return ()
    endif ()
    get_property (used_ GLOBAL PROPERTY STROIKA_CONAN_COMPONENTS_)
    set (depends_)
    foreach (component_ IN LISTS arg_DEPENDS)
        if (component_ IN_LIST used_)
            list (APPEND depends_ ${component_})
        endif ()
    endforeach ()
    set (tools_)
    foreach (component_ IN LISTS arg_TOOLS)
        if (component_ IN_LIST used_)
            list (APPEND tools_ ${component_})
        endif ()
    endforeach ()
    StroikaComponentFromConan (${directory} DEPENDS ${depends_} TOOLS ${tools_})
endfunction ()

# the components the configuration uses - each after those it depends on
StroikaComponent (zlib ZLib)
StroikaComponent (zstd zstd)
StroikaComponent (StrawberryPerl StrawberryPerl TOOL)
StroikaComponent (openssl OpenSSL DEPENDS zlib TOOLS StrawberryPerl)
StroikaComponent (curl LibCurl DEPENDS openssl zlib zstd)
StroikaComponent (boost boost DEPENDS zlib zstd)
StroikaComponent (mongo-cxx-driver mongocxxdriver DEPENDS openssl zlib zstd)
StroikaComponent (libxml2 libxml2 DEPENDS zlib)
StroikaComponent (Xerces Xerces)
StroikaComponent (sqlite sqlite)
StroikaComponent (lzma LZMA)
StroikaComponent (fmtlib fmtlib)
StroikaComponent (GoogleTest GoogleTest)
StroikaComponent (WIX WIX)
