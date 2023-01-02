/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_GUID_inl_
#define _Stroika_Foundation_Common_GUID_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Common {

    /*
     ********************************************************************************
     *********************************** Common::GUID *******************************
     ********************************************************************************
     */
#if qPlatform_Windows
    constexpr GUID::GUID (const ::GUID& src)  noexcept
        : Data1{src.Data1}
        , Data2{src.Data2}
        , Data3{src.Data3}
    {
        for (size_t i = 0; i < Memory::NEltsOf (Data4); ++i) {
            Data4[i] = src.Data4[i];
        }
    }
#endif
    inline Common::GUID::GUID (const array<uint8_t, 16>& src)  noexcept
    {
        ::memcpy (this, src.data (), 16);
    }
    inline const std::byte* Common::GUID::begin () const  noexcept
    {
        return reinterpret_cast<const std::byte*> (this);
    }
    inline const std::byte* Common::GUID::end () const noexcept
    {
        return reinterpret_cast<const std::byte*> (this) + 16;
    }
    constexpr size_t Common::GUID::size () const noexcept
    {
        return 16;
    }
    template <typename T>
    inline T Common::GUID::As () const
        requires (
                is_same_v<T,Characters::String> 
                    or is_same_v<T,std::string> 
                    or is_same_v<T, Memory::BLOB> 
                    or is_same_v<T, array<std::byte, 16>> 
                    or is_same_v<T, array<uint8_t, 16>> 
                    ) 
    {
        if constexpr (is_same_v<T, Characters::String>) {
            char buf[1024];
            Verify (::snprintf (buf, Memory::NEltsOf (buf), "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                             Data1, Data2, Data3,
                             Data4[0], Data4[1], Data4[2], Data4[3], Data4[4], Data4[5], Data4[6], Data4[7]) > 0);
            return Characters::String::FromASCII (buf);
        }
        else if constexpr (is_same_v<T, std::string>) {
            char buf[1024];
            Verify (::snprintf (buf, Memory::NEltsOf (buf), "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                       Data1, Data2, Data3,
                       Data4[0], Data4[1], Data4[2], Data4[3], Data4[4], Data4[5], Data4[6], Data4[7]) > 0);
             return  buf;
        }
        else if constexpr (is_same_v<T, Memory::BLOB>) {
            return Memory::BLOB{begin (), end ()};
        }
        else if constexpr (is_same_v<T, array<std::byte, 16>>) {
            return *reinterpret_cast<const array<std::byte, 16>*> (this);
        }
        else if constexpr (is_same_v<T, array<uint8_t, 16>>) {
            return *reinterpret_cast<const array<uint8_t, 16>*> (this);
        }
    }

}

#endif /*_Stroika_Foundation_Common_GUID_inl_*/