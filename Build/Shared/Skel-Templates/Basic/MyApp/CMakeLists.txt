#
# The program ${APP_NAME}, from Sources/, in ${APP_NAME}/ in the configuration's Builds directory - with its version (from
# VERSION) as AppVersion.h
#
StroikaMakeVersionFile ("${PROJECT_SOURCE_DIR}/VERSION" "${CMAKE_CURRENT_BINARY_DIR}/AppVersion.h")
add_executable (${APP_NAME} Sources/Main.cpp)
target_include_directories (${APP_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries (${APP_NAME} PRIVATE Stroika::Frameworks)
# a generator expression, though empty, keeps a Visual Studio generator from adding a directory for the build type
set_target_properties (${APP_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${STROIKA_BUILDS_DIR}${APP_NAME}$<0:>")
StroikaCopyFilesToExeDir (${APP_NAME})
