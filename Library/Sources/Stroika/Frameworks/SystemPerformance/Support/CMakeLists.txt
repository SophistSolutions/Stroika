if ("Windows" IN_LIST STROIKA_TARGET_PLATFORMS)
    target_sources (stroika-frameworks PRIVATE
        WMICollector.cpp
    )
endif ()
