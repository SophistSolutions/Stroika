/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_Immportalize_inl_
#define _Stroika_Foundation_Common_Immportalize_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <mutex>

namespace Stroika::Foundation::Common {

    /*
     ********************************************************************************
     ****************************** Common::Immortalize *****************************
     ********************************************************************************
     */
    template <class T>
    inline T& Immortalize ()
    {
        static once_flag             sFlag_{};
        static aligned_union_t<1, T> sStorage_{};
        std::call_once (sFlag_, [] () { ::new (&sStorage_) T (); });
        return reinterpret_cast<T&> (sStorage_);
    }

}

#endif /*_Stroika_Foundation_Common_Immportalize_inl_*/
