if (STROIKA_FEATURE_ATLMFC STREQUAL "use-system")
    target_sources (stroika-frameworks PRIVATE
        MFC_WordProcessor.cpp
        MFC.cpp
    )
endif ()
if ("Windows" IN_LIST STROIKA_TARGET_PLATFORMS)
    target_sources (stroika-frameworks PRIVATE
        Windows.cpp
        Windows_FileRegistration.cpp
    )
    # as its Makefile: the Windows version, so the SDK headers need not default it ("_WIN32_WINNT not defined"); STRICT handle types
    set_source_files_properties (MFC_WordProcessor.cpp MFC.cpp Windows.cpp Windows_FileRegistration.cpp
        TARGET_DIRECTORY stroika-frameworks PROPERTIES COMPILE_DEFINITIONS "_WIN32_WINNT=0x0A00;STRICT"
    )
endif ()
