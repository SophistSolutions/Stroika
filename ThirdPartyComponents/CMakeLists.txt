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
# one, and found in Conan's cache after that - by any build tree on the machine. For now these; in time, all of them.
#
set (STROIKA_THIRD_PARTY_FROM_CONAN zlib)
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
    execute_process (COMMAND "${STROIKA_CONAN}" config home OUTPUT_VARIABLE conanHome_ OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ERROR_IS_FATAL ANY)
    file (TO_CMAKE_PATH "${conanHome_}" conanHome_)
    set (STROIKA_CONAN_LOCK "${conanHome_}/stroika-third-party.lock")
endif ()

#
# StroikaComponentFromConan (<directory>)
#
# The component in <directory>, from Conan's cache, copied into the configuration's Builds directory (StroikaDeploy.py) -
# built into the cache first, by its recipe here (conanfile.py), when the configuration has no package of it there yet. A
# target of its name, as StroikaThirdPartyComponent makes, for the components built after it (DEPENDS); it runs Conan again
# only once what builds the component changes. Conan's commands are in a script (conan.cmake, in the component's binary
# directory), which takes Conan's lock first.
#
function (StroikaComponentFromConan directory)
    file (STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/${directory}/CMakeLists.txt" version_ REGEX "^set \\(version_ ")
    string (REGEX REPLACE "^set \\(version_ ([^) ]+)\\).*" "\\1" version_ "${version_}")
    set (package_ stroika-${directory})
    # what builds it, as conanfile.py exports it
    file (GLOB_RECURSE recipe_ LIST_DIRECTORIES false "${CMAKE_CURRENT_SOURCE_DIR}/${directory}/*")
    list (FILTER recipe_ EXCLUDE REGEX "/${directory}/(Makefile$|CURRENT|ORIGS|ORIGs)")
    set (binaryDir_ "${CMAKE_CURRENT_BINARY_DIR}/${directory}")
    set (export_ "${STROIKA_CONAN}" export "${CMAKE_CURRENT_SOURCE_DIR}" --name ${package_} --version ${version_})
    set (install_ "${STROIKA_CONAN}" install --requires ${package_}/${version_} --build missing
        -o "${package_}/*:configuration=${STROIKA_CONAN_CONFIGURATION_DIGEST}" -pr:a "${STROIKA_CONAN_PROFILE}"
        -c "user.stroika:toolchain_file=${CMAKE_TOOLCHAIN_FILE}" -c "user.stroika:origs_cache=${STROIKA_ORIGS_CACHE}")
    foreach (conf_ IN LISTS STROIKA_CONAN_CONF)
        list (APPEND install_ -c "${conf_}")
    endforeach ()
    list (APPEND install_ --deployer "${CMAKE_CURRENT_SOURCE_DIR}/StroikaDeploy.py" --deployer-folder "${STROIKA_BUILDS_DIR}"
        --output-folder "${binaryDir_}")
    # each argument a bracket argument in the script, so none needs quoting
    list (JOIN export_ "]==] [==[" export_)
    list (JOIN install_ "]==] [==[" install_)
    # written only when changed (file (CONFIGURE)), so the script's time says when it last did
    file (CONFIGURE OUTPUT "${binaryDir_}/conan.cmake" @ONLY CONTENT "\
# ${package_} from Conan's cache - built there first, if the configuration has no package of it yet - copied into the\n\
# configuration's Builds directory: cmake -P, by ThirdPartyComponents/CMakeLists.txt's StroikaComponentFromConan, which wrote it.\n\
# MAKEFLAGS unset: Conan does not pass on make's job slots, so the component's own build takes CMAKE_BUILD_PARALLEL_LEVEL's jobs\n\
unset (ENV{MAKEFLAGS})\n\
unset (ENV{MFLAGS})\n\
file (LOCK [==[${STROIKA_CONAN_LOCK}]==] GUARD PROCESS TIMEOUT 0 RESULT_VARIABLE locked_)\n\
if (NOT locked_ EQUAL 0)\n\
    message (STATUS [==[${package_}: waiting for another build's Conan steps (${STROIKA_CONAN_LOCK})]==])\n\
    file (LOCK [==[${STROIKA_CONAN_LOCK}]==] GUARD PROCESS)\n\
endif ()\n\
execute_process (COMMAND [==[${export_}]==] COMMAND_ERROR_IS_FATAL ANY)\n\
execute_process (COMMAND [==[${install_}]==] COMMAND_ERROR_IS_FATAL ANY)\n\
file (TOUCH [==[${binaryDir_}/conan-stamp]==])\n\
")
    add_custom_command (OUTPUT "${binaryDir_}/conan-stamp"
        COMMAND "${CMAKE_COMMAND}" -P "${binaryDir_}/conan.cmake"
        DEPENDS "${binaryDir_}/conan.cmake" ${recipe_} "${CMAKE_CURRENT_SOURCE_DIR}/StroikaThirdPartyComponent.cmake"
                "${CMAKE_CURRENT_SOURCE_DIR}/RemoveFiles.cmake" "${CMAKE_CURRENT_SOURCE_DIR}/conanfile.py"
                "${CMAKE_CURRENT_SOURCE_DIR}/StroikaDeploy.py" "${CMAKE_TOOLCHAIN_FILE}" "${configurationDir_}/Stroika-Configuration.cmake"
        USES_TERMINAL
        VERBATIM
    )
    add_custom_target (${directory} ALL DEPENDS "${binaryDir_}/conan-stamp")
endfunction ()

#
# StroikaComponent (<directory> <feature>)
#
# The component in <directory>, where the configuration uses it (configure's --<feature> use): from Conan's cache, if it is
# one Conan keeps (STROIKA_THIRD_PARTY_FROM_CONAN), else built here.
#
function (StroikaComponent directory feature)
    if (NOT STROIKA_FEATURE_${feature} STREQUAL "use")
        return ()
    endif ()
    if (directory IN_LIST STROIKA_THIRD_PARTY_FROM_CONAN)
        StroikaComponentFromConan (${directory})
    else ()
        add_subdirectory (${directory})
    endif ()
endfunction ()

# the components the configuration uses - each after those it depends on
StroikaComponent (zlib ZLib)
StroikaComponent (zstd zstd)
StroikaComponent (StrawberryPerl StrawberryPerl)
StroikaComponent (openssl OpenSSL)
StroikaComponent (curl LibCurl)
StroikaComponent (boost boost)
StroikaComponent (mongo-cxx-driver mongocxxdriver)
StroikaComponent (libxml2 libxml2)
StroikaComponent (Xerces Xerces)
StroikaComponent (sqlite sqlite)
StroikaComponent (lzma LZMA)
StroikaComponent (fmtlib fmtlib)
StroikaComponent (GoogleTest GoogleTest)
StroikaComponent (WIX WIX)
