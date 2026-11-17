if ("POSIX" IN_LIST STROIKA_TARGET_PLATFORMS)
    target_sources (stroika-foundation PRIVATE
        SemWaitableEvent.cpp
        SignalBlock.cpp
        Users.cpp
    )
endif ()
