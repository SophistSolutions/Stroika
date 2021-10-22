
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Debug_Cast_inl_
#define _Stroika_Foundation_Debug_Cast_inl_ 1

namespace Stroika::Foundation::Debug {

    template <typename T, typename T1>
    inline T UncheckedDynamicCast (T1&& arg)
    {
        static_assert (is_reference_v<T> or is_pointer_v<T>);
        #if qDebug
        using DECAYED_T = conditional_t<is_reference_v<T>, decay_t<T>, remove_pointer_t<decay_t<T>>>; // remove_reference_t
        #endif
        if constexpr (is_reference_v<T>) {
            // must special case here cuz if we compare two references wont compare POINTERS, but instead call operator== on the underlying T type
            Require (static_cast<const DECAYED_T*> (&arg) == dynamic_cast<const DECAYED_T*> (&arg));
            AssertMember (&arg, DECAYED_T);
        }
        else {
            Require (static_cast<T> (arg) == dynamic_cast<T> (arg));
            AssertMember (arg, DECAYED_T);
        }
        return static_cast<T> (arg);
    }

}

#endif /*_Stroika_Foundation_Debug_Cast_inl_*/
