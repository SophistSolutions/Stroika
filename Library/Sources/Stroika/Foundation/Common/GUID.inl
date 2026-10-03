/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::Common {

    /*
     ********************************************************************************
     ********************************* Common::GUID *********************************
     ********************************************************************************
     */
#if qStroika_Platform_MacOS
    inline GUID::GUID (const uuid_t& src) noexcept
        : GUID{FromRFC9562Bytes (as_bytes (span<const unsigned char, 16>{src}))}
    {
    }
#elif qStroika_Platform_Windows
    constexpr GUID::GUID (const ::GUID& src) noexcept
        : Data1{src.Data1}
        , Data2{src.Data2}
        , Data3{src.Data3}
    {
        for (size_t i = 0; i < std::size (Data4); ++i) {
            Data4[i] = src.Data4[i];
        }
    }
#endif
    constexpr GUID GUID::FromRFC9562Bytes (span<const byte, 16> bytes) noexcept
    {
        // by shifting, not copying - so whatever this machine's byte order
        auto b = [&] (size_t i) { return static_cast<uint32_t> (bytes[i]); };
        GUID r;
        r.Data1 = (b (0) << 24) | (b (1) << 16) | (b (2) << 8) | b (3);
        r.Data2 = static_cast<uint16_t> ((b (4) << 8) | b (5));
        r.Data3 = static_cast<uint16_t> ((b (6) << 8) | b (7));
        for (size_t i = 0; i < std::size (r.Data4); ++i) {
            r.Data4[i] = static_cast<uint8_t> (bytes[8 + i]);
        }
        return r;
    }
    constexpr array<byte, 16> GUID::AsRFC9562Bytes () const noexcept
    {
        array<byte, 16> r{};
        r[0] = static_cast<byte> (Data1 >> 24);
        r[1] = static_cast<byte> (Data1 >> 16);
        r[2] = static_cast<byte> (Data1 >> 8);
        r[3] = static_cast<byte> (Data1);
        r[4] = static_cast<byte> (Data2 >> 8);
        r[5] = static_cast<byte> (Data2);
        r[6] = static_cast<byte> (Data3 >> 8);
        r[7] = static_cast<byte> (Data3);
        for (size_t i = 0; i < std::size (Data4); ++i) {
            r[8 + i] = static_cast<byte> (Data4[i]);
        }
        return r;
    }
    namespace Private_ {
        template <Characters::IConvertibleToString STRISH_TYPE>
        inline string AsAscii (STRISH_TYPE&& src)
        {
            // @todo could be more efficient either using spans and passed in buffer arg for if needed (like with String Peek) but
            // also could just do additional template specializations in CPP file for cases like string_view or const char*
            if constexpr (same_as<STRISH_TYPE, Characters::String>) {
                return src.AsASCII ();
            }
            if constexpr (IAnyOf<STRISH_TYPE, const char*, string_view, string>) {
                return forward<STRISH_TYPE> (src);
            }
            return Characters::String{src}.AsASCII ();
        }
    }
    template <Characters::IConvertibleToString STRISH_TYPE>
    inline GUID::GUID (STRISH_TYPE&& src)
        : GUID{mk_ (Private_::AsAscii (src))}
    {
    }
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
    template <IAnyOf<Characters::String, std::string, Memory::BLOB, array<byte, 16>, array<uint8_t, 16>> T>
    inline T Common::GUID::As () const
    {
        if constexpr (same_as<T, Characters::String>) {
            char buf[1024];
            Verify (::snprintf (buf, std::size (buf), "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", Data1, Data2, Data3, Data4[0],
                                Data4[1], Data4[2], Data4[3], Data4[4], Data4[5], Data4[6], Data4[7]) > 0);
            return Characters::String{buf};
        }
        else if constexpr (same_as<T, std::string>) {
            char buf[1024];
            Verify (::snprintf (buf, std::size (buf), "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", Data1, Data2, Data3, Data4[0],
                                Data4[1], Data4[2], Data4[3], Data4[4], Data4[5], Data4[6], Data4[7]) > 0);
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

/*
 ********************************************************************************
 ********* formatter<Stroika::Foundation::Common::GUID, char/wchar_t> ***********
 ********************************************************************************
 */
template <class FmtContext>
inline typename FmtContext::iterator qStroika_Foundation_Characters_FMT_PREFIX_::formatter<Stroika::Foundation::Common::GUID, wchar_t>::format (
    const Stroika::Foundation::Common::GUID& s, FmtContext& ctx) const
{
    return inherited::format (s.As<Stroika::Foundation::Characters::String> ().As<std::wstring> (), ctx);
}
template <class FmtContext>
inline typename FmtContext::iterator
qStroika_Foundation_Characters_FMT_PREFIX_::formatter<Stroika::Foundation::Common::GUID, char>::format (const Stroika::Foundation::Common::GUID& s,
                                                                                                        FmtContext& ctx) const
{
    return inherited::format (s.As<std::string> (), ctx);
}
