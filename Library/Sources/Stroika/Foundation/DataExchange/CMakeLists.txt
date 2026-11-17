target_sources (stroika-foundation PRIVATE
    Atom.cpp
    BadFormatException.cpp
    InternetMediaType.cpp
    InternetMediaTypeRegistry.cpp
    ObjectVariantMapper.cpp
    OptionsFile.cpp
    StructFieldMetaInfo.cpp
    TypedBLOB.cpp
    VariantValue.cpp
)

add_subdirectory (Archive)
add_subdirectory (Compression)
add_subdirectory (JSON)
add_subdirectory (Variant)
add_subdirectory (StructuredStreamEvents)
add_subdirectory (XML)
