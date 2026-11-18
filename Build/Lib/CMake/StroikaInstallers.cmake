#
# StroikaInstallers (<target> NAME <name> PACKAGE <package> VERSION_FILE <file> OUTPUT_DIRECTORY <directory>
#                    [PROGRAM_NAME <file name>] [FILES <file or directory>...] [DEPENDS <target or file>...]
#                    [WIX_PROGRAM_NAME <file name>] [WIX_SCRIPT <script>] [WIX_ARGS <-Dname=value>...])
#
# Installers for a program run as a service - the one <target> builds - from the files in the calling directory's
# Installers/ (as Samples/Service's): each into OUTPUT_DIRECTORY, built again when the program changes, and each where its
# tool is:
#   - on a UNIX host with dpkg-deb, <package>-<version>.<host>.<arch>.deb (InstallerDeb.cmake), and with rpmbuild,
#     <package>-<version>.<host>.<arch>.rpm (InstallerRPM.cmake), each installing
#         /opt/<name>/<PROGRAM_NAME>   the program (PROGRAM_NAME by default <name>) - stripped, unless the configuration
#                                      keeps executables' debug symbols
#         /opt/<name>/...              FILES, beside it: each a file, or a directory with all it holds (relative to the
#                                      calling directory)
#         /usr/lib/systemd/system/     Installers/<package>.service, the service that runs the program
#         /var/opt/<name>/             empty
#     The .deb's control file is Installers/<package>.control.static, given its version and architecture; its maintainer
#     scripts Installers/<package>.preinst, .postinst, .prerm and .postrm, each where there is one - else, the service
#     enabled and started once installed, and stopped and disabled before it is removed. The .rpm's spec is
#     Installers/<package>.rpm.spec, given its version: its scripts too, and the list of the files it installs.
#   - unless the configuration has no WIX (configure's --WIX no), <name>-Windows-<arch>-<configuration>.msi
#     (InstallerWIX.cmake), from Installers/Installer.wix: the program as WIX_PROGRAM_NAME (by default its own file name).
#     What else it installs (FILES, say) is WIX_SCRIPT's to add - an installer's own step, given WIX_ARGS - as
#     Samples/HTMLUI/Installers/AddWebAppToWIX.cmake adds HTMLUI's web app.
# The version is the version file's major.minor and stage (StroikaVersion.cmake) - 3.0d26x; <host> the host's name (Linux),
# <arch> the configuration's architecture. DEPENDS: what else the installers are built after, and again when it changes -
# what builds FILES, say.
#
include (StroikaVersion)

set (StroikaInstallersDir_ "${CMAKE_CURRENT_LIST_DIR}")

function (StroikaInstallers target)
    cmake_parse_arguments (PARSE_ARGV 1 arg "" "NAME;PACKAGE;VERSION_FILE;OUTPUT_DIRECTORY;PROGRAM_NAME;WIX_PROGRAM_NAME;WIX_SCRIPT"
                           "FILES;DEPENDS;WIX_ARGS")
    if (arg_UNPARSED_ARGUMENTS)
        message (FATAL_ERROR "StroikaInstallers (${target}): unknown arguments ${arg_UNPARSED_ARGUMENTS}")
    endif ()
    set (sourceDir_ "${CMAKE_CURRENT_SOURCE_DIR}/Installers")
    StroikaReadVersion ("${arg_VERSION_FILE}" v_)
    set (version_ "${v__MAJOR}.${v__MINOR}${v__DECORATED_STAGE}")
    set (strip_)
    if (NOT STROIKA_INCLUDE_DEBUG_SYMBOLS_IN_EXECUTABLES)
        set (strip_ "-DSTRIP=${CMAKE_STRIP}")
    endif ()
    set (common_ "-DEXE=$<TARGET_FILE:${target}>" "-DNAME=${arg_NAME}" "-DPACKAGE=${arg_PACKAGE}" "-DSOURCE_DIR=${sourceDir_}")
    # each built again when this call, or this function, changes: a Makefile generator does not compare a custom command's
    # command line with the last one's
    set (definedBy_ "${CMAKE_CURRENT_LIST_FILE}" "${CMAKE_CURRENT_FUNCTION_LIST_FILE}")
    set (outputs_)

    # the .deb's and .rpm's: the program's name in them, and FILES, absolute - given to their scripts as one argument,
    # "-DFILES=<list>" (a custom command's quoted argument stays one, its ';'s and all)
    set (programName_ "${arg_PROGRAM_NAME}")
    if (NOT programName_)
        set (programName_ "${arg_NAME}")
    endif ()
    set (files_)
    foreach (file_ IN LISTS arg_FILES)
        cmake_path (ABSOLUTE_PATH file_ BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" NORMALIZE)
        list (APPEND files_ "${file_}")
    endforeach ()
    if (NOT CMAKE_HOST_WIN32)
        find_program (STROIKA_DPKG_DEB dpkg-deb)
        find_program (STROIKA_RPMBUILD rpmbuild)
    endif ()
    if (STROIKA_DPKG_DEB)
        # the architecture, as Debian names it (as Build/Scripts/MapArch2DebFileArchitecture)
        set (debArch_ "${STROIKA_ARCH}")
        if (STROIKA_ARCH STREQUAL "x86_64")
            set (debArch_ amd64)
        elseif (STROIKA_ARCH STREQUAL "x86")
            set (debArch_ i386)
        elseif (STROIKA_ARCH STREQUAL "aarch64")
            set (debArch_ arm64)
        endif ()
        # its maintainer scripts, those the installer has (cmake runs again when it gains or loses one)
        file (GLOB debScripts_ CONFIGURE_DEPENDS "${sourceDir_}/${arg_PACKAGE}.preinst" "${sourceDir_}/${arg_PACKAGE}.postinst"
              "${sourceDir_}/${arg_PACKAGE}.prerm" "${sourceDir_}/${arg_PACKAGE}.postrm")
        set (deb_ "${arg_OUTPUT_DIRECTORY}/${arg_PACKAGE}-${version_}.${CMAKE_HOST_SYSTEM_NAME}.${STROIKA_ARCH}.deb")
        add_custom_command (
            OUTPUT "${deb_}"
            COMMAND "${CMAKE_COMMAND}" ${common_} "-DPROGRAM_NAME=${programName_}" "-DFILES=${files_}" "-DVERSION=${version_}"
                    "-DARCH=${debArch_}" ${strip_} "-DLOG=${CMAKE_CURRENT_BINARY_DIR}/DEB-Installer-Build-Output.txt" "-DOUTPUT=${deb_}"
                    -P "${StroikaInstallersDir_}/InstallerDeb.cmake"
            DEPENDS ${target} ${arg_DEPENDS} "${sourceDir_}/${arg_PACKAGE}.control.static" "${sourceDir_}/${arg_PACKAGE}.service"
                    ${debScripts_} "${StroikaInstallersDir_}/InstallerDeb.cmake" ${definedBy_}
            COMMENT "Building DEB installer ${deb_}"
            VERBATIM
        )
        list (APPEND outputs_ "${deb_}")
    endif ()
    if (STROIKA_RPMBUILD)
        set (rpm_ "${arg_OUTPUT_DIRECTORY}/${arg_PACKAGE}-${version_}.${CMAKE_HOST_SYSTEM_NAME}.${STROIKA_ARCH}.rpm")
        add_custom_command (
            OUTPUT "${rpm_}"
            COMMAND "${CMAKE_COMMAND}" ${common_} "-DPROGRAM_NAME=${programName_}" "-DFILES=${files_}" "-DVERSION=${v__MAJOR}.${v__MINOR}"
                    "-DRELEASE=${v__DECORATED_STAGE}" ${strip_} "-DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/RPM_TMP"
                    "-DLOG=${CMAKE_CURRENT_BINARY_DIR}/RPM-Installer-Build-Output.txt" "-DOUTPUT=${rpm_}"
                    -P "${StroikaInstallersDir_}/InstallerRPM.cmake"
            DEPENDS ${target} ${arg_DEPENDS} "${sourceDir_}/${arg_PACKAGE}.rpm.spec" "${sourceDir_}/${arg_PACKAGE}.service"
                    "${StroikaInstallersDir_}/InstallerRPM.cmake" ${definedBy_}
            COMMENT "Building RPM installer ${rpm_}"
            VERBATIM
        )
        list (APPEND outputs_ "${rpm_}")
    endif ()
    if (STROIKA_FEATURE_WIX AND NOT STROIKA_FEATURE_WIX STREQUAL "no")
        # WIX's programs: with --WIX use, those the third-party build extracted (ThirdPartyComponents/WIX/CMakeLists.txt; its
        # build directory beside this one, as the top CMakeLists.txt builds them) or make did (ThirdPartyComponents/WIX/CURRENT)
        # - else, or if neither has them, the PATH's
        set (hints_)
        if (STROIKA_FEATURE_WIX STREQUAL "use")
            set (hints_ HINTS "${CMAKE_BINARY_DIR}/../ThirdParty/WIX/src" "${StroikaInstallersDir_}/../../../ThirdPartyComponents/WIX/CURRENT")
        endif ()
        foreach (tool_ candle light heat)
            string (TOUPPER "${tool_}" toolVariable_)
            find_program (STROIKA_WIX_${toolVariable_} ${tool_}.exe ${hints_} REQUIRED)
        endforeach ()
        set (exeName_ "${arg_WIX_PROGRAM_NAME}")
        if (NOT exeName_)
            set (exeName_ "$<TARGET_FILE_NAME:${target}>")
        endif ()
        set (script_)
        if (arg_WIX_SCRIPT)
            cmake_path (ABSOLUTE_PATH arg_WIX_SCRIPT BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" NORMALIZE)
            set (script_ "-DSCRIPT=${arg_WIX_SCRIPT}")
        endif ()
        set (msi_ "${arg_OUTPUT_DIRECTORY}/${arg_NAME}-Windows-${STROIKA_ARCH}-${STROIKA_CONFIGURATION}.msi")
        cmake_path (REPLACE_EXTENSION msi_ LAST_ONLY .wixpdb OUTPUT_VARIABLE wixpdb_) # light writes it beside the .msi
        add_custom_command (
            OUTPUT "${msi_}"
            BYPRODUCTS "${wixpdb_}"
            COMMAND "${CMAKE_COMMAND}" ${common_} "-DEXE_NAME=${exeName_}" "-DARCH=${STROIKA_ARCH}"
                    "-DCONFIGURATION=${STROIKA_CONFIGURATION}" "-DVERSION=${version_}" "-DFOUR_DOT=${v__WIN32_FOUR_DOT}"
                    "-DCANDLE=${STROIKA_WIX_CANDLE}" "-DLIGHT=${STROIKA_WIX_LIGHT}" "-DHEAT=${STROIKA_WIX_HEAT}"
                    "-DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/WIX_TMP" "-DLOG=${CMAKE_CURRENT_BINARY_DIR}/WIX-Installer-Build-Output.txt"
                    "-DOUTPUT=${msi_}" ${script_} ${arg_WIX_ARGS} -P "${StroikaInstallersDir_}/InstallerWIX.cmake"
            DEPENDS ${target} ${arg_DEPENDS} "${sourceDir_}/Installer.wix" "${sourceDir_}/Installer_en-us.wxl" ${arg_WIX_SCRIPT}
                    "${StroikaInstallersDir_}/InstallerWIX.cmake" ${definedBy_}
            COMMENT "Building WIX installer ${msi_}"
            VERBATIM
        )
        list (APPEND outputs_ "${msi_}")
    endif ()
    if (outputs_)
        add_custom_target ("${target}-installers" ALL DEPENDS ${outputs_})
    endif ()
endfunction ()
