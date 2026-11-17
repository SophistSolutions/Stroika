target_sources (stroika-foundation PRIVATE
    IProvider.cpp
)
if (NOT STROIKA_FEATURE_Xerces STREQUAL "no")
    target_sources (stroika-foundation PRIVATE
        Xerces.cpp
    )
endif ()
if (NOT STROIKA_FEATURE_libxml2 STREQUAL "no")
    target_sources (stroika-foundation PRIVATE
        LibXML2.cpp
    )
endif ()
