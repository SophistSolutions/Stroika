#
# ${APP_NAME}, built on Stroika - its CMake package (find_package (Stroika)), for one of Stroika's configurations, by that
# configuration's preset (CMakeUserPresets.json, beside this file, which Stroika writes as it applies the configuration):
#     cmake --preset <configuration>
#     cmake --build --preset <configuration>
# make CONFIGURATION=<configuration> all does that, once it has built Stroika's libraries for the configuration.
#
cmake_minimum_required (VERSION 3.22)
project (${APP_NAME} LANGUAGES C CXX)

find_package (Stroika REQUIRED)

add_subdirectory (${APP_NAME})
