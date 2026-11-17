if ("Windows" IN_LIST STROIKA_TARGET_PLATFORMS)
    target_sources (stroika-foundation PRIVATE
        Exception.cpp
        HRESULTErrorException.cpp
        StructuredException.cpp
        Users.cpp
        WaitSupport.cpp
    )
endif ()
