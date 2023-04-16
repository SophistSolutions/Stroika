/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_MultiSet_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_MultiSet_Factory_h_

#include "../../StroikaPreComp.h"

#include <atomic>

/**
 *  \file
 */

namespace Stroika::Foundation::Containers {

    template <typename T, typename TRAITS>
    class MultiSet;
}

namespace Stroika::Foundation::Containers::Factory {

    /**
    *  \brief   Singleton factory object - Used to create the default backend implementation of a MultiSet<> container
    *
    *  Note - you can override the underlying factory dynamically by calling MultiSet_Factory<T,TRAITS>::Register (), or
    *  replace it statically by template-specializing MultiSet_Factory<T,TRAITS>::New () - though the later is trickier.
    *
    *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
    */
    template <typename T, typename TRAITS, typename EQUALS_COMPARER = equal_to<T>>
    class MultiSet_Factory {
    private:
        static inline atomic<MultiSet<T, TRAITS> (*) (const EQUALS_COMPARER&)> sFactory_{nullptr};

    public:
        static_assert (not is_reference_v<T> and not is_reference_v<EQUALS_COMPARER>,
                       "typically if this fails its because a (possibly indirect) caller forgot to use forward<>(), or remove_cvref_t");
        static_assert (Common::IsEqualsComparer<EQUALS_COMPARER> (), "Equals comparer required with MultiSet_Factory");

    public:
        /**
         *  Hints can be used in factory constructor to guide the choice of the best container implementation/backend.
         */
        struct Hints {
            optional<bool> fOptimizeForLookupSpeedOverUpdateSpeed;
        };

    public:
        constexpr MultiSet_Factory (const EQUALS_COMPARER& equalsComparer, const Hints& hints = {});

    public:
        /**
         *  You can call this directly, but there is no need, as the MultiSet<T,TRAITS> CTOR does so automatically.
         */
        nonvirtual MultiSet<T, TRAITS> operator() () const;

    public:
        /**
         *  Register a replacement creator/factory for the given MultiSet<T,TRAITS>. Note this is a global change.
         */
        static void Register (MultiSet<T, TRAITS> (*factory) (const EQUALS_COMPARER&) = nullptr);

    private:
        [[no_unique_address]] const EQUALS_COMPARER fEqualsComparer_;
        const Hints                                 fHints_;

    private:
        static MultiSet<T, TRAITS> Default_ (const EQUALS_COMPARER& equalsComparer, const Hints& hints);
    };
}

/*
 ********************************************************************************
 **************************** Implementation Details ****************************
 ********************************************************************************
 */
#include "MultiSet_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_MultiSet_Factory_h_ */
