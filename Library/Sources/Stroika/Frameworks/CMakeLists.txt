#
# The Frameworks library. Each directory's CMakeLists.txt lists its sources, as its Makefile does - keep the two in step
# while make builds it too.
#
add_library (stroika-frameworks STATIC)
target_link_libraries (stroika-frameworks PUBLIC stroika-foundation)

# googletest - Frameworks/Test, which Stroika's regression tests are built on, uses it
if (NOT STROIKA_FEATURE_GoogleTest STREQUAL "no")
    find_package (GTest CONFIG REQUIRED)
    target_link_libraries (stroika-frameworks PUBLIC GTest::gtest)
endif ()

add_subdirectory (Auth)
add_subdirectory (Led)
add_subdirectory (Modbus)
add_subdirectory (NetworkMonitor)
add_subdirectory (Service)
add_subdirectory (SystemPerformance)
add_subdirectory (Test)
add_subdirectory (WebServer)
add_subdirectory (WebService)
add_subdirectory (UPnP)
