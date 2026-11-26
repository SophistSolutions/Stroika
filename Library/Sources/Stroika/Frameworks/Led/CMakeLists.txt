target_sources (stroika-frameworks PRIVATE
    BiDiLayoutEngine.cpp
    ChunkedArrayTextStore.cpp
    Command.cpp
    FlavorPackage.cpp
    GDI.cpp
    HandySimple.cpp
    HiddenText.cpp
    IdleManager.cpp
    LineBasedPartition.cpp
    Marker.cpp
    MultiRowTextImager.cpp
    PartitioningTextImager.cpp
)
if ("Windows" IN_LIST STROIKA_TARGET_PLATFORMS)
    target_sources (stroika-frameworks PRIVATE
        SimpleLed.cpp
    )
endif ()
target_sources (stroika-frameworks PRIVATE
    SimpleTextImager.cpp
    SimpleTextInteractor.cpp
    SimpleTextStore.cpp
    SpellCheckEngine_Basic.cpp
    StandardStyledTextImager.cpp
    StandardStyledTextInteractor.cpp
    StdDialogs.cpp
    StyledTextEmbeddedObjects.cpp
    StyledTextImager.cpp
    Support.cpp
    SyntaxColoring.cpp
    TextBreaks.cpp
    TextImager.cpp
    TextInteractor.cpp
    TextInteractorMixins.cpp
    TextStore.cpp
    WordProcessor.cpp
    WordWrappedTextImager.cpp
    WordWrappedTextInteractor.cpp
)

add_subdirectory (Platform)
add_subdirectory (StyledTextIO)
