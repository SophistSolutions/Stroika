target_sources (stroika-foundation PRIVATE
    Reader.cpp
    Writer.cpp
)

add_subdirectory (CharacterDelimitedLines)
add_subdirectory (FormURLEncoded)
add_subdirectory (INI)
add_subdirectory (JSON)
add_subdirectory (XML)
