/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::IO::FileSystem {

    [[deprecated ("Since Stroika v3.0d12 - use String::As<filesystem::path>")]] inline filesystem::path ToPath (const String& p)
    {
        return p.As<filesystem::path> ();
    }
    [[deprecated ("Since Stroika v3.0d12 - use String::As<filesystem::path>")]] inline optional<filesystem::path> ToPath (const optional<String>& p)
    {
        if (p) {
            return p->As<filesystem::path> ();
        }
        return nullopt;
    }
    [[deprecated ("Since Stroika v3.0d12 - use String CTOR")]] inline String FromPath (const filesystem::path& p)
    {
        return String{p};
    }
    [[deprecated ("Since Stroika v3.0d12 - use String CTOR??? revfisit if used...")]] inline optional<String> FromPath (const optional<filesystem::path>& p)
    {
        // unsure can deprecate???
        if (p) {
            return String{*p};
        }
        return nullopt;
    }

}

/*
 ********************************************************************************
 ******************************** DEPRECATED ************************************
 ********************************************************************************
 *
 *  Kept only so code written for older Stroika versions keeps compiling - each says what replaces it.
 */
namespace Stroika::Foundation::IO::FileSystem {
    [[deprecated ("Since Stroika v3.0d12 use path::preferred_separator ")]] constexpr wchar_t kPathComponentSeperator = filesystem::path::preferred_separator;
}
