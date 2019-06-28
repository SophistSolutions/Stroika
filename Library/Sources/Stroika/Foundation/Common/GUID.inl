/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
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
        : Data1 (src.Data1)
        , Data2 (src.Data2)
        , Data3 (src.Data3)
    {
        for (size_t i = 0; i < NEltsOf (Data4); ++i) {
            Data4[i] = src.Data4[i];
        }
    }
#endif
    inline Common::GUID::GUID (const array<uint8_t, 16>& src)
    {
        memcpy (this, src.data (), 16);
    }
    constexpr GUID GUID::Zero ()
    {
        return GUID{};
    }

    /*
     ********************************************************************************
     ************************* GUID::ThreeWayComparer *******************************
     ********************************************************************************
     */
    inline int GUID::ThreeWayComparer::operator() (const GUID& lhs, const GUID& rhs) const
    {
        static_assert (sizeof (GUID) == 16); // else cannot use memcmp this way
        return memcmp (&lhs, &rhs, sizeof (GUID));
    }

    /*
     ********************************************************************************
     ************************* Common::GUID operators *******************************
     ********************************************************************************
     */
    inline bool operator< (const GUID& lhs, const GUID& rhs)
    {
        return GUID::ThreeWayComparer{}(lhs, rhs) < 0;
    }
    inline bool operator<= (const GUID& lhs, const GUID& rhs)
    {
        return GUID::ThreeWayComparer{}(lhs, rhs) <= 0;
    }
    inline bool operator== (const GUID& lhs, const GUID& rhs)
    {
        return GUID::ThreeWayComparer{}(lhs, rhs) == 0;
    }
    inline bool operator!= (const GUID& lhs, const GUID& rhs)
    {
        return GUID::ThreeWayComparer{}(lhs, rhs) != 0;
    }
    inline bool operator>= (const GUID& lhs, const GUID& rhs)
    {
        return GUID::ThreeWayComparer{}(lhs, rhs) >= 0;
    }
    inline bool operator> (const GUID& lhs, const GUID& rhs)
    {
        return GUID::ThreeWayComparer{}(lhs, rhs) > 0;
    }

}

#endif /*_Stroika_Foundation_Common_GUID_inl_*/