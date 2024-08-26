/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::Debug {

    /*
     ********************************************************************************
     ************************** Debug::UncheckedDynamicCast *************************
     ********************************************************************************
     */
    template <typename T, typename T1>
    inline T UncheckedDynamicCast (T1&& arg) noexcept
    {
        static_assert (is_reference_v<T> or is_pointer_v<T>);
        if constexpr (is_pointer_v<T>) {
            Require (arg != nullptr);
        }
        if constexpr (qDebug) {
            DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wunused-local-typedefs\"");
            using DECAYED_T = conditional_t<is_reference_v<T>, remove_cvref_t<T>, remove_pointer_t<remove_cvref_t<T>>>; // remove_reference_t
            DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wunused-local-typedefs\"");
            if constexpr (is_reference_v<T>) {
                // must special case here cuz if we compare two references wont compare POINTERS, but instead call operator== on the underlying T type
                Require (static_cast<const DECAYED_T*> (&arg) == dynamic_cast<const DECAYED_T*> (&arg));
                AssertMember (&arg, DECAYED_T);
            }
            else {
                Require (static_cast<T> (arg) == dynamic_cast<T> (arg));
                AssertMember (arg, DECAYED_T);
                Ensure (arg != nullptr);
            }
        }
        return static_cast<T> (arg);
    }

    /*
     ********************************************************************************
     ************************* Debug::UncheckedDynamicPointerCast *******************
     ********************************************************************************
     */
    template <typename T, typename T1>
    inline std::shared_ptr<T> UncheckedDynamicPointerCast (const std::shared_ptr<T1>& arg) noexcept
    {
        //Require (dynamic_pointer_cast<T> (arg) != nullptr);   -- checked redundantly in UncheckedDynamicCast
        if (const auto p = UncheckedDynamicCast<T*> (arg.get ())) {
            return shared_ptr<T>{std::move (arg), p};
        }
        return {};
    }

}
