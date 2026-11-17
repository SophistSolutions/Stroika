target_sources (stroika-foundation PRIVATE
    Format.cpp
)

add_subdirectory (Digest)
add_subdirectory (Encoding)
add_subdirectory (Providers)
add_subdirectory (PKI)
add_subdirectory (SSL)
