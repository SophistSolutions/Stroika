/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_TemplateUtilities_inl_
#define _Stroika_Foundation_Common_TemplateUtilities_inl_ 1

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
        struct StorageImpl_ {
            union {
                T _Storage;
            };
            constexpr StorageImpl_ () noexcept
                : _Storage{}
            {
            }
            StorageImpl_ (const StorageImpl_&) = delete;
            StorageImpl_& operator= (const StorageImpl_&) = delete;
#if __has_cpp_attribute(msvc::noop_dtor)
            [[msvc::noop_dtor]]
#endif
            ~StorageImpl_ ()
            {
            }
        };
        static once_flag    sFlag_{};
        static StorageImpl_ sStorage_{};
        call_once (sFlag_, [] () { ::new (&sStorage_) T{}; });
        return reinterpret_cast<T&> (sStorage_);
    }

}

#endif /*_Stroika_Foundation_Common_TemplateUtilities_inl_*/
