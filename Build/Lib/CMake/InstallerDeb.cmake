#
# cmake -DEXE=<program> -DNAME=<name> -DPROGRAM_NAME=<file name> -DPACKAGE=<package> -DVERSION=<version>
#       -DARCH=<Debian architecture> -DSOURCE_DIR=<Installers directory> [-DFILES=<file or directory>...]
#       [-DSTRIP=<strip program>] -DLOG=<log> -DOUTPUT=<.deb> -P InstallerDeb.cmake
#
# A .deb installing the program as a systemd service (StroikaInstallers.cmake), as Samples/Service/Installers/Makefile
# builds one: the program as /opt/NAME/PROGRAM_NAME (stripped by STRIP, if given) and FILES beside it, SOURCE_DIR's
# PACKAGE.service in /usr/lib/systemd/system, and an empty /var/opt/NAME; its control file SOURCE_DIR's
# PACKAGE.control.static with VERSION and ARCH. Put together in a new temporary directory (mktemp -d), as dpkg-deb needs
# permissions that a WSL /mnt filesystem does not keep; dpkg-deb's output goes to LOG.
#
execute_process (COMMAND mktemp -d OUTPUT_VARIABLE workDir_ OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ERROR_IS_FATAL ANY)
set (root_ "${workDir_}/${PACKAGE}-${VERSION}")
file (MAKE_DIRECTORY "${root_}/DEBIAN" "${root_}/opt/${NAME}" "${root_}/usr/lib/systemd/system" "${root_}/var/opt/${NAME}")
execute_process (COMMAND chmod -R 755 "${root_}" COMMAND_ERROR_IS_FATAL ANY)

file (READ "${SOURCE_DIR}/${PACKAGE}.control.static" control_)
file (WRITE "${root_}/DEBIAN/control" "Version: ${VERSION}\n${control_}Architecture: ${ARCH}\n")
# its maintainer scripts: SOURCE_DIR's PACKAGE.preinst, .postinst, .prerm and .postrm, where it has one - else, for postinst
# and prerm, the service enabled and started once installed, and stopped and disabled before it is removed, where systemd is
# running (/run/systemd/system): elsewhere, as in a container, systemctl fails - and a failed postinst leaves the package
# half-configured
foreach (script_ preinst postinst prerm postrm)
    if (EXISTS "${SOURCE_DIR}/${PACKAGE}.${script_}")
        file (COPY_FILE "${SOURCE_DIR}/${PACKAGE}.${script_}" "${root_}/DEBIAN/${script_}")
    elseif (script_ STREQUAL "postinst")
        file (WRITE "${root_}/DEBIAN/postinst" "#!/bin/sh\n" "if [ -d /run/systemd/system ]; then\n" "    systemctl enable ${PACKAGE}\n"
              "    systemctl start ${PACKAGE}\n" "fi\n")
    elseif (script_ STREQUAL "prerm")
        file (WRITE "${root_}/DEBIAN/prerm" "#!/bin/sh\n" "if [ -d /run/systemd/system ]; then\n" "    systemctl stop ${PACKAGE} || true\n"
              "    systemctl disable ${PACKAGE} || true\n" "fi\n")
    else ()
        continue ()
    endif ()
    execute_process (COMMAND chmod 755 "${root_}/DEBIAN/${script_}" COMMAND_ERROR_IS_FATAL ANY)
endforeach ()

set (strip_)
if (STRIP)
    set (strip_ --strip "--strip-program=${STRIP}")
endif ()
execute_process (COMMAND install -m 755 ${strip_} "${EXE}" "${root_}/opt/${NAME}/${PROGRAM_NAME}" COMMAND_ERROR_IS_FATAL ANY)
foreach (file_ IN LISTS FILES)
    execute_process (COMMAND cp -r "${file_}" "${root_}/opt/${NAME}/" COMMAND_ERROR_IS_FATAL ANY)
endforeach ()
execute_process (COMMAND install -m 644 "${SOURCE_DIR}/${PACKAGE}.service" "${root_}/usr/lib/systemd/system/${PACKAGE}.service"
    COMMAND_ERROR_IS_FATAL ANY)

execute_process (COMMAND dpkg-deb --root-owner-group --build "${root_}" OUTPUT_FILE "${LOG}" ERROR_FILE "${LOG}" RESULT_VARIABLE result_)
if (NOT result_ EQUAL 0)
    file (READ "${LOG}" log_)
    message (FATAL_ERROR "dpkg-deb failed (${result_}), building ${OUTPUT} in ${workDir_} - its output (${LOG}):\n${log_}")
endif ()
get_filename_component (outputDir_ "${OUTPUT}" DIRECTORY)
file (MAKE_DIRECTORY "${outputDir_}")
file (COPY_FILE "${root_}.deb" "${OUTPUT}")
file (REMOVE_RECURSE "${workDir_}")
