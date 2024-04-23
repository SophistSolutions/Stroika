/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    /*
     ********************************************************************************
     ************************************ HTTP::ETag ********************************
     ********************************************************************************
     */
    inline ETag::ETag (const String& value, bool weak)
        : fValue{value}
        , fWeak{weak}
    {
    }
    inline optional<ETag> ETag::Parse (const String& wireFormat)
    {
        if (not wireFormat.EndsWith ('\"')) {
            return nullopt;
        }
        if (wireFormat.StartsWith ('\"')) {
            return ETag{wireFormat.SubString (1, -1)};
        }
        if (wireFormat.StartsWith ("\\W\""sv)) {
            return ETag{wireFormat.SubString (3, -1), true};
        }
        return nullopt;
    }
    template <>
    inline Characters::String ETag::As () const
    {
        return (fWeak ? "\\W\""sv : "\""sv) + fValue + "\""sv;
    }
    inline Characters::String ETag::ToString () const
    {
        return As<String> ();
    }

}
