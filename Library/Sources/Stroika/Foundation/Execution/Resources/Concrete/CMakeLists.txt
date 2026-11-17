if ("POSIX" IN_LIST STROIKA_TARGET_PLATFORMS)
    target_sources (stroika-foundation PRIVATE
        ElfResourceManager.cpp
    )
endif ()
if ("Windows" IN_LIST STROIKA_TARGET_PLATFORMS)
    target_sources (stroika-foundation PRIVATE
        WindowsResourceManager.cpp
    )
endif ()
