target_sources (stroika-foundation PRIVATE
    Character.cpp
    CharacterEncodingException.cpp
    Charset.cpp
    CodeCvt.cpp
    CodePage.cpp
    Format.cpp
    Number.cpp
    RegularExpression.cpp
    SDKString.cpp
    String.cpp
    ToString.cpp
    UTFConvert.cpp
)

add_subdirectory (CString)
add_subdirectory (Platform)
