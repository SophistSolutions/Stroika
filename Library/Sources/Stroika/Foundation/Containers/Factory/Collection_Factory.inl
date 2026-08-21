/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Containers/Concrete/Collection_Array.h"
#include "Stroika/Foundation/Containers/Concrete/Collection_LinkedList.h"
#include "Stroika/Foundation/Containers/Concrete/SortedCollection_stdmultiset.h"

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
                if (fHints_OptimizeForLookupSpeedOverUpdateSpeed) [[likely]] {
                    /*
                     *  The default. Chosen for LOOKUP/REMOVE speed - O(log n) Contains ()/Remove ()
                     *  instead of a linear scan - and NOT for add speed, which it is markedly worse at.
                     *  Measured 3.0d24 (Tests/52, Windows x86_64 release, 500 elements) against
                     *  Collection_Array<int>: adding one at a time costs ~8x more here, and AddAll () of
                     *  a contiguous range ~200x more, because every element pays a tree insertion that
                     *  no amount of batching removes.
                     */
                    static const auto kDefault_ = Concrete::SortedCollection_stdmultiset<T>{};
                    return kDefault_;
                }
                else {
                    /*
                     *  Asked for update speed, so give up the O(log n) lookup for the array: AddAll ()
                     *  of a contiguous range hands the whole span to Array::Insert (), which reserves
                     *  once - measured ~20-30x cheaper per element than the per-element path, and ~200x
                     *  cheaper than doing it through the sorted multiset above.
                     *
                     *  Before this, the totally_ordered branch ignored the hint entirely, so a caller
                     *  asking for update speed was silently given the sorted multiset anyway.
                     */
                    static const auto kDefault_ = Concrete::Collection_Array<T>{};
                    return kDefault_;
                }
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
