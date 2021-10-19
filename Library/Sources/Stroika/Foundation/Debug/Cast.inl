
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Debug_Cast_inl_
#define _Stroika_Foundation_Debug_Cast_inl_ 1

namespace Stroika::Foundation::Debug {

    template <typename T, typename T1>
    T UncheckedDynamicCast (T1&& arg)
    {
        using DECAYED_T = decay_t<T>; // remove_reference_t
        if constexpr (is_reference_v<T>) {
            // must special case here cuz if we compare two references wont compare POINTERS, but instead call operator== on the underlying T type
            Require (static_cast<const DECAYED_T*> (&arg) == dynamic_cast<const DECAYED_T*> (&arg));
        }
        else {
            Require (static_cast<T> (arg) == dynamic_cast<T> (arg));
        }
        AssertMember (&arg, DECAYED_T);
        return static_cast<T> (arg);
    }

}

#endif /*_Stroika_Foundation_Debug_Cast_inl_*/
