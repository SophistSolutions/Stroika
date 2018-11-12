/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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

}

#endif /*_Stroika_Foundation_Common_GUID_inl_*/