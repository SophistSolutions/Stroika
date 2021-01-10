/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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
    constexpr GUID::GUID (const ::GUID& src)
        : Data1{src.Data1}
        , Data2{src.Data2}
        , Data3{src.Data3}
    {
        for (size_t i = 0; i < NEltsOf (Data4); ++i) {
            Data4[i] = src.Data4[i];
        }
    }
#endif
    inline Common::GUID::GUID (const array<uint8_t, 16>& src)
    {
        ::memcpy (this, src.data (), 16);
    }
    inline std::byte* Common::GUID::begin ()
    {
        return reinterpret_cast<std::byte*> (this);
    }
    inline const std::byte* Common::GUID::begin () const
    {
        return reinterpret_cast<const std::byte*> (this);
    }
    inline std::byte* Common::GUID::end ()
    {
        return reinterpret_cast<std::byte*> (this) + 16;
    }
    inline const std::byte* Common::GUID::end () const
    {
        return reinterpret_cast<const std::byte*> (this) + 16;
    }
    constexpr GUID GUID::Zero ()
    {
        return GUID{};
    }

#if __cpp_impl_three_way_comparison < 201907 or qCompilerAndStdLib_SpaceshipAutoGenForOpEqualsForCommonGUID_Buggy
    /*
     ********************************************************************************
     ************************* Common::GUID operators *******************************
     ********************************************************************************
     */
    inline bool operator< (const GUID& lhs, const GUID& rhs)
    {
        return memcmp (&lhs, &rhs, sizeof (GUID)) < 0;
    }
    inline bool operator<= (const GUID& lhs, const GUID& rhs)
    {
        return memcmp (&lhs, &rhs, sizeof (GUID)) <= 0;
    }
    inline bool operator== (const GUID& lhs, const GUID& rhs)
    {
        return memcmp (&lhs, &rhs, sizeof (GUID)) == 0;
    }
    inline bool operator!= (const GUID& lhs, const GUID& rhs)
    {
        return memcmp (&lhs, &rhs, sizeof (GUID)) != 0;
    }
    inline bool operator>= (const GUID& lhs, const GUID& rhs)
    {
        return memcmp (&lhs, &rhs, sizeof (GUID)) >= 0;
    }
    inline bool operator> (const GUID& lhs, const GUID& rhs)
    {
        return memcmp (&lhs, &rhs, sizeof (GUID)) > 0;
    }
#endif

}

#endif /*_Stroika_Foundation_Common_GUID_inl_*/