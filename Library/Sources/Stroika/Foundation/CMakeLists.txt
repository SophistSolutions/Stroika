#
# The Foundation library. Each directory's CMakeLists.txt lists its sources, as its Makefile does - keep the two in step
# while make builds it too.
#
add_library (stroika-foundation STATIC)

target_compile_definitions (stroika-foundation PUBLIC ${STROIKA_FOUNDATION_DEFINITIONS})
target_compile_options (stroika-foundation PUBLIC ${STROIKA_FOUNDATION_COMPILE_OPTIONS})
# and Stroika/Current-Version.h, which Common/StroikaVersion.h includes - STROIKA_VERSION as macros - ahead of any other copy
StroikaMakeVersionFile ("${PROJECT_SOURCE_DIR}/../STROIKA_VERSION" "${CMAKE_CURRENT_BINARY_DIR}/include/Stroika/Current-Version.h" TYPE StroikaLibVersion)
target_include_directories (stroika-foundation PUBLIC ${STROIKA_FOUNDATION_INCLUDE_DIRECTORIES} "${CMAKE_CURRENT_BINARY_DIR}/include" ${STROIKA_PLATFORM_INCLUDE_DIRECTORIES})
# what links Foundation links with the configuration's link options and system libraries
target_link_options (stroika-foundation INTERFACE ${STROIKA_LINK_OPTIONS})
target_link_libraries (stroika-foundation INTERFACE ${STROIKA_LINK_LIBRARIES})

# the third-party components the configuration uses (StroikaDependencies: Library/CMakeLists.txt)
target_link_libraries (stroika-foundation PUBLIC ${STROIKA_FOUNDATION_DEPENDENCIES})
if (MSVC AND NOT STROIKA_FEATURE_boost STREQUAL "no")
    # Boost's headers also name the libraries they need, without a path (auto-linking) - so links search where they are,
    # as make's links do
    target_link_directories (stroika-foundation INTERFACE "${STROIKA_BUILDS_DIR}lib")
endif ()

add_subdirectory (Cache)
add_subdirectory (Characters)
add_subdirectory (Common)
add_subdirectory (Cryptography)
add_subdirectory (Database)
add_subdirectory (DataExchange)
add_subdirectory (Debug)
add_subdirectory (Execution)
add_subdirectory (IO)
add_subdirectory (Linguistics)
add_subdirectory (Math)
add_subdirectory (Memory)
add_subdirectory (Streams)
add_subdirectory (Time)
