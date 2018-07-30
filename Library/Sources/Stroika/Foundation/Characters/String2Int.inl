/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String2Int_inl_
#define _Stroika_Foundation_Characters_String2Int_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Configuration/Common.h"

namespace Stroika::Foundation::Characters {

    namespace Private_ {
        unsigned long long int String2UInt_ (const String& s);
        long long int          String2Int_ (const String& s);
        DISABLE_COMPILER_MSC_WARNING_START (4018)
        template <typename T>
        T String2IntOrUInt_ (const String& s)
        {
            if (numeric_limits<T>::is_signed) {
                long long int l = String2Int_ (s);
                if (l <= numeric_limits<T>::min ()) {
                    return numeric_limits<T>::min ();
                }
                if (l >= numeric_limits<T>::max ()) {
                    return numeric_limits<T>::max ();
                }
                return static_cast<T> (l);
            }
            else {
                unsigned long long int l = String2UInt_ (s);
                if (l >= numeric_limits<T>::max ()) {
                    return numeric_limits<T>::max ();
                }
                return static_cast<T> (l);
            }
        }
        DISABLE_COMPILER_MSC_WARNING_END (4018)
    }

    /*
     ********************************************************************************
     *********************************** String2Int *********************************
     ********************************************************************************
     */
    template <typename T>
    inline T String2Int (const String& s)
    {
        return Private_::String2IntOrUInt_<T> (s);
    }

}

#endif /*_Stroika_Foundation_Characters_String2Int_inl_*/
