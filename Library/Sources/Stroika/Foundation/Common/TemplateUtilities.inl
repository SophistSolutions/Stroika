/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
    template <typename T, typename... ARGS>
    inline T& Immortalize (ARGS... args)
    {
        struct StorageImpl_ {
            union {
                T _Storage;
            };
            constexpr StorageImpl_ () noexcept
                : _Storage{}
            {
            }
            StorageImpl_ (const StorageImpl_&)            = delete;
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
        call_once (sFlag_, [&] () { ::new (&sStorage_) T{args...}; });
        return reinterpret_cast<T&> (sStorage_);
    }

    namespace Private_ {
        template <typename VariantType, typename T, std::size_t index = 0>
        constexpr std::size_t variant_index ()
        {
            // From https://stackoverflow.com/questions/52303316/get-index-by-type-in-stdvariant/66386518#66386518
            static_assert (std::variant_size_v<VariantType> > index, "Type not found in variant");
            if constexpr (index == std::variant_size_v<VariantType>) {
                return index;
            }
            else if constexpr (std::is_same_v<std::variant_alternative_t<index, VariantType>, T>) {
                return index;
            }
            else {
                return variant_index<VariantType, T, index + 1> ();
            }
        }
    }
    template < typename VARIANT_VALUE, typename T>
    constexpr size_t VariantIndex = Private_::variant_index<VARIANT_VALUE, T> ();

}

#endif /*_Stroika_Foundation_Common_TemplateUtilities_inl_*/
