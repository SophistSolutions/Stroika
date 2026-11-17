if ("Windows" IN_LIST STROIKA_TARGET_PLATFORMS)
    target_sources (stroika-foundation PRIVATE
        CodePage.cpp
    )
endif ()
