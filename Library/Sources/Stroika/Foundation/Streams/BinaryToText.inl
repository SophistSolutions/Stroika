/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Streams/InternallySynchronizedInputStream.h"

namespace Stroika::Foundation::Streams::BinaryToText::Reader {

    /*
     ********************************************************************************
     **************************** BinaryToText::Reader::New *******************************
     ********************************************************************************
     */
    inline auto New (const InputStream::Ptr<Character>& src) -> InputStream::Ptr<Character>
    {
        return src;
    }
    template <typename... ARGS>
    inline InputStream::Ptr<Character> New (Execution::InternallySynchronized internallySynchronized, ARGS... args)
    {
        switch (internallySynchronized) {
            case Execution::InternallySynchronized::eNotKnownInternallySynchronized:
                return New (forward<ARGS...> (args...));
            case Execution::eInternallySynchronized:
                // @todo could explicitly specialize more cases and handle more efficiently, but using the REP overload of InternallySynchronizedInputStream
                return InternallySynchronizedInputStream::New ({}, New (forward<ARGS...> (args...)));
        }
    }

}

/*
 ********************************************************************************
 ******************************** DEPRECATED ************************************
 ********************************************************************************
 *
 *  Kept only so code written for older Stroika versions keeps compiling - each says what replaces it.
 */
namespace Stroika::Foundation::Streams::BinaryToText {
    DISABLE_COMPILER_MSC_WARNING_START (4996);
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    enum class [[deprecated ("Since Stroika v3.0d25 - use Reader::New (src) or Convert (src) - a byte order mark, else UTF-8 - or "
                             "ByteOrderMarkOr{codeCvt}")]] AutomaticCodeCvtFlags {
        eReadBOMAndIfNotPresentUseUTF8,
        eReadBOMAndIfNotPresentUseCurrentLocale,

        eDEFAULT = eReadBOMAndIfNotPresentUseUTF8
    };
    using AutomaticCodeCvtFlags::eReadBOMAndIfNotPresentUseCurrentLocale;
    using AutomaticCodeCvtFlags::eReadBOMAndIfNotPresentUseUTF8;
    [[deprecated ("Since Stroika v3.0d25 - use Convert (src) - a byte order mark, else UTF-8 - or Convert (src, "
                  "ByteOrderMarkOr{codeCvt})")]] Characters::String
    Convert (const Memory::BLOB& src, optional<AutomaticCodeCvtFlags> codeCvtFlags);
    DISABLE_COMPILER_MSC_WARNING_END (4996);
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
}
namespace Stroika::Foundation::Streams::BinaryToText::Reader {
    DISABLE_COMPILER_MSC_WARNING_START (4996);
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    [[deprecated ("Since Stroika v3.0d25 - use New (src, seekable, readAhead) - a byte order mark, else UTF-8 - or New (src, "
                  "ByteOrderMarkOr{codeCvt}, ...)")]] Ptr
    New (const InputStream::Ptr<byte>& src, optional<AutomaticCodeCvtFlags> codeCvtFlags, optional<SeekableFlag> seekable = {},
         ReadAhead readAhead = eReadAheadAllowed);
    DISABLE_COMPILER_MSC_WARNING_END (4996);
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
}
