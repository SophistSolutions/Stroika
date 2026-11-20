#
# The Frameworks library. Each directory's CMakeLists.txt lists its sources, as its Makefile does - keep the two in step
# while make builds it too.
#
add_library (stroika-frameworks STATIC)
target_link_libraries (stroika-frameworks PUBLIC stroika-foundation)

# and googletest, which Frameworks/Test - Stroika's regression tests are built on it - uses (StroikaDependencies:
# the top CMakeLists.txt)
target_link_libraries (stroika-frameworks PUBLIC ${STROIKA_FRAMEWORKS_DEPENDENCIES})

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
