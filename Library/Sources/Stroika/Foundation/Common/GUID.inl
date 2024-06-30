/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::Common {

    /*
     ********************************************************************************
     *********************************** Common::GUID *******************************
     ********************************************************************************
     */
#if qPlatform_Windows
    constexpr GUID::GUID (const ::GUID& src) noexcept
        : Data1{src.Data1}
        , Data2{src.Data2}
        , Data3{src.Data3}
    {
        for (size_t i = 0; i < Memory::NEltsOf (Data4); ++i) {
            Data4[i] = src.Data4[i];
        }
    }
#endif
    inline Common::GUID::GUID (const array<uint8_t, 16>& src) noexcept
    {
        ::memcpy (this, src.data (), 16);
    }
    inline const byte* GUID::begin () const noexcept
    {
        return reinterpret_cast<const byte*> (this);
    }
    inline const byte* GUID::end () const noexcept
    {
        return reinterpret_cast<const byte*> (this) + 16;
    }
    constexpr size_t GUID::size () const noexcept
    {
        return 16;
    }
    inline const uint8_t* GUID::data () const noexcept
    {
        return reinterpret_cast<const uint8_t*> (this);
    }
    template <typename T>
    inline T Common::GUID::As () const
        requires (same_as<T, Characters::String> or same_as<T, std::string> or same_as<T, Memory::BLOB> or same_as<T, array<byte, 16>> or
                  same_as<T, array<uint8_t, 16>>)
    {
        if constexpr (same_as<T, Characters::String>) {
            char buf[1024];
            Verify (::snprintf (buf, Memory::NEltsOf (buf), "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", Data1, Data2, Data3,
                                Data4[0], Data4[1], Data4[2], Data4[3], Data4[4], Data4[5], Data4[6], Data4[7]) > 0);
            return Characters::String{buf};
        }
        else if constexpr (same_as<T, std::string>) {
            char buf[1024];
            Verify (::snprintf (buf, Memory::NEltsOf (buf), "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", Data1, Data2, Data3,
                                Data4[0], Data4[1], Data4[2], Data4[3], Data4[4], Data4[5], Data4[6], Data4[7]) > 0);
            return buf;
        }
        else if constexpr (same_as<T, array<byte, 16>> or same_as<T, array<uint8_t, 16>>) {
            return *reinterpret_cast<const T*> (this);
        }
        else if constexpr (same_as<T, Memory::BLOB>) {
            return T{begin (), end ()}; // tricky case cuz BLOB forward declared, not defined when this procedure definition first seen
        }
    }

}
