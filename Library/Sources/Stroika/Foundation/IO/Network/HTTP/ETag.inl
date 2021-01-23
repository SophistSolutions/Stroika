/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_ETag_inl_
#define _Stroika_Foundation_IO_Network_HTTP_ETag_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
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
        if (wireFormat.StartsWith (L"\\W\""sv)) {
            return ETag{wireFormat.SubString (3, -1), true};
        }
        return nullopt;
    }
    template <>
    inline Characters::String ETag::As () const
    {
        return (fWeak ? L"\\W\""sv : L"\"") + fValue + L"\""sv;
    }
#if __cpp_impl_three_way_comparison < 201907
    inline bool operator== (const ETag& lhs, const ETag& rhs)
    {
        return lhs.fWeak == rhs.fWeak and lhs.fValue == rhs.fValue;
    }
    inline bool operator!= (const ETag& lhs, const ETag& rhs)
    {
        return not(lhs == rhs);
    }
#endif

}

#endif /*_Stroika_Foundation_IO_Network_HTTP_ETag_inl_*/
