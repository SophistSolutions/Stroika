#
# cmake -DEXE=<program> -DNAME=<name> -DPROGRAM_NAME=<file name> -DPACKAGE=<package> -DVERSION=<major.minor>
#       -DRELEASE=<release> -DSOURCE_DIR=<Installers directory> [-DFILES=<file or directory>...] [-DSTRIP=<strip program>]
#       -DWORK_DIR=<directory> -DLOG=<log> -DOUTPUT=<.rpm> -P InstallerRPM.cmake
#
# A .rpm installing the program as a systemd service (StroikaInstallers.cmake), as Samples/Service/Installers/Makefile
# builds one: the files the .deb installs (InstallerDeb.cmake), as the source archive PACKAGE-VERSION.tar.gz, built by
# rpmbuild from SOURCE_DIR's PACKAGE.rpm.spec - given its version (VERSION, RELEASE) and WORK_DIR as its top directory.
# rpmbuild's output goes to LOG.
#
file (REMOVE_RECURSE "${WORK_DIR}")
set (tree_ "${PACKAGE}-${VERSION}")
foreach (directory_ RPMS SRPMS BUILD SOURCES SPECS tmp "${tree_}/opt/${NAME}" "${tree_}/usr/lib/systemd/system" "${tree_}/var/opt/${NAME}")
    file (MAKE_DIRECTORY "${WORK_DIR}/${directory_}")
endforeach ()

set (strip_)
if (STRIP)
    set (strip_ --strip "--strip-program=${STRIP}")
endif ()
execute_process (COMMAND install -m 755 ${strip_} "${EXE}" "${WORK_DIR}/${tree_}/opt/${NAME}/${PROGRAM_NAME}" COMMAND_ERROR_IS_FATAL ANY)
foreach (file_ IN LISTS FILES)
    execute_process (COMMAND cp -r "${file_}" "${WORK_DIR}/${tree_}/opt/${NAME}/" COMMAND_ERROR_IS_FATAL ANY)
endforeach ()
execute_process (COMMAND install -m 644 "${SOURCE_DIR}/${PACKAGE}.service" "${WORK_DIR}/${tree_}/usr/lib/systemd/system/${PACKAGE}.service"
    COMMAND_ERROR_IS_FATAL ANY)
execute_process (COMMAND tar -zcf "${tree_}.tar.gz" "${tree_}/" WORKING_DIRECTORY "${WORK_DIR}" COMMAND_ERROR_IS_FATAL ANY)
file (COPY "${WORK_DIR}/${tree_}.tar.gz" DESTINATION "${WORK_DIR}/SOURCES")

file (READ "${SOURCE_DIR}/${PACKAGE}.rpm.spec" spec_)
file (WRITE "${WORK_DIR}/SPECS/${PACKAGE}.spec" "%define _topdir ${WORK_DIR}/\nVersion: ${VERSION}\nRelease: ${RELEASE}\n${spec_}")
execute_process (COMMAND rpmbuild -ba "SPECS/${PACKAGE}.spec" WORKING_DIRECTORY "${WORK_DIR}" OUTPUT_FILE "${LOG}" ERROR_FILE "${LOG}"
    RESULT_VARIABLE result_)
file (GLOB rpms_ "${WORK_DIR}/RPMS/*/*.rpm")
list (LENGTH rpms_ count_)
if (NOT result_ EQUAL 0 OR NOT count_ EQUAL 1)
    file (READ "${LOG}" log_)
    message (FATAL_ERROR "rpmbuild failed (${result_}; RPMS: ${rpms_}), building ${OUTPUT} in ${WORK_DIR} - its output (${LOG}):\n${log_}")
endif ()
get_filename_component (outputDir_ "${OUTPUT}" DIRECTORY)
file (MAKE_DIRECTORY "${outputDir_}")
file (COPY_FILE "${rpms_}" "${OUTPUT}")
