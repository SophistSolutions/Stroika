/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Containers/Concrete/Collection_Array.h"
#include "Stroika/Foundation/Containers/Concrete/Collection_LinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/Collection_stdmultiset.h"

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     ****************************** Collection_Factory<T> ***************************
     ********************************************************************************
     */
    template <typename T>
    constexpr Collection_Factory<T>::Collection_Factory (const FactoryFunctionType& f)
        : fFactory_{f}
    {
    }
    template <typename T>
    constexpr Collection_Factory<T>::Collection_Factory ()
        : Collection_Factory{AccessDefault_ ()}
    {
    }
    template <typename T>
    constexpr Collection_Factory<T>::Collection_Factory (const Hints& hints)
        : fFactory_{nullptr}
        , fHints_OptimizeForLookupSpeedOverUpdateSpeed{hints.fOptimizeForLookupSpeedOverUpdateSpeed.value_or (true)}
    {
    }
    template <typename T>
    inline auto Collection_Factory<T>::Default () -> const Collection_Factory&
    {
        return AccessDefault_ ();
    }
    template <typename T>
    inline auto Collection_Factory<T>::operator() () const -> ConstructedType
    {
        if (this->fFactory_ == nullptr) [[likely]] {
            if constexpr (totally_ordered<T>) {
                // faster adds/removes - same size - so better if possible to use (unless very small collections maybe)
                static const auto kDefault_ = Concrete::Collection_stdmultiset<T>{};
                return kDefault_;
            }
            else {
                if (fHints_OptimizeForLookupSpeedOverUpdateSpeed) [[likely]] {
                    // questionable choice. For smaller sizes, probably faster, due to better locality.
                    // but adds can occasionally be slow (realloc/O(N)) instead of O(1).
                    static const auto kDefault_ = Concrete::Collection_Array<T>{};
                    return kDefault_;
                }
                else {
                    // This generally performs well, so long as you don't call 'size'
                    static const auto kDefault_ = Concrete::Collection_LinkedList<T>{};
                    return kDefault_;
                }
            }
        }
        else {
            return this->fFactory_ ();
        }
    }
    template <typename T>
    void Collection_Factory<T>::Register (const optional<Collection_Factory>& f)
    {
        AccessDefault_ () = f.has_value () ? *f : Collection_Factory{Hints{}};
    }
    template <typename T>
    inline auto Collection_Factory<T>::AccessDefault_ () -> Collection_Factory&
    {
        static Collection_Factory sDefault_{Hints{}};
        return sDefault_;
    }

}
