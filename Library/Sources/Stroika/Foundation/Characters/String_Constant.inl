/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_Constant_inl_
#define _Stroika_Foundation_Characters_String_Constant_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Characters {

    /*
     ********************************************************************************
     ******************************** operator"" _k *********************************
     ********************************************************************************
     */
    inline String_Constant operator"" _k (const wchar_t* s, size_t len)
    {
        return String_Constant{s, s + len};
    }

}
#endif // _Stroika_Foundation_Characters_String_Constant_inl_
