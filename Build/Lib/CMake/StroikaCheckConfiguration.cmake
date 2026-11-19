#
# StroikaCheckConfiguration (<toolchain file>)
#
# Stops the build, saying how to fix it, unless the configuration as applied (Build/Scripts/ApplyConfiguration) is
# current - its toolchain file, and its Stroika settings (Stroika-Configuration.cmake, included already): no older than
# its configuration file, or than ApplyConfiguration, and that configuration made by this Stroika (STROIKA_VERSION no
# newer). make sees to that before it runs cmake, as its target apply-configuration-if-stale_ does; without make (cmake
# --preset, an IDE), the build stops here. Each of those files is a configure dependency, so a build after one changes
# stops too. Stroika's own build checks so (CMakeLists.txt), and a program built on Stroika's package (StroikaConfig.cmake),
# whose libraries were built as the configuration was applied. (A IS_NEWER_THAN B holds for files of the same time too: so
# each test asks whether the later one is NOT newer than the earlier.)
#
function (StroikaCheckConfiguration toolchainFile)
    if (NOT DEFINED STROIKA_CONFIGURATION_FILE)
        message (FATAL_ERROR "Configuration ${STROIKA_CONFIGURATION} was applied by an older Stroika: apply it again - make CONFIGURATION=${STROIKA_CONFIGURATION} apply-configuration")
    endif ()
    get_filename_component (sourceRoot "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../../.." ABSOLUTE)
    set (applyConfiguration "${sourceRoot}/Build/Scripts/ApplyConfiguration")
    set (stroikaVersion "${sourceRoot}/STROIKA_VERSION")
    set_property (DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${STROIKA_CONFIGURATION_FILE}" "${applyConfiguration}" "${stroikaVersion}")
    if (NOT EXISTS "${STROIKA_CONFIGURATION_FILE}")
        message (FATAL_ERROR "Configuration ${STROIKA_CONFIGURATION} is gone: there is no ${STROIKA_CONFIGURATION_FILE}")
    elseif (NOT "${STROIKA_CONFIGURATION_FILE}" IS_NEWER_THAN "${stroikaVersion}")
        message (FATAL_ERROR "Configuration ${STROIKA_CONFIGURATION} was made by an older Stroika (STROIKA_VERSION is newer than its ${STROIKA_CONFIGURATION_FILE}): make it again, then apply it - make CONFIGURATION=${STROIKA_CONFIGURATION} reconfigure, then make CONFIGURATION=${STROIKA_CONFIGURATION} apply-configuration")
    elseif (NOT "${toolchainFile}" IS_NEWER_THAN "${STROIKA_CONFIGURATION_FILE}")
        message (FATAL_ERROR "Configuration ${STROIKA_CONFIGURATION} has changed since it was applied (its ${STROIKA_CONFIGURATION_FILE} is newer than ${toolchainFile}): apply it again - make CONFIGURATION=${STROIKA_CONFIGURATION} apply-configuration")
    elseif (NOT "${toolchainFile}" IS_NEWER_THAN "${applyConfiguration}")
        message (FATAL_ERROR "Stroika's Build/Scripts/ApplyConfiguration has changed since configuration ${STROIKA_CONFIGURATION} was applied: apply it again - make CONFIGURATION=${STROIKA_CONFIGURATION} apply-configuration")
    endif ()
endfunction ()
