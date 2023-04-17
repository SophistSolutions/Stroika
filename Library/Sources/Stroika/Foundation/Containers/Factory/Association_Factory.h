/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Association_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_Association_Factory_h_

#include "../../StroikaPreComp.h"

#include <atomic>
#include <type_traits>

/**
 *  \file
 */

namespace Stroika::Foundation::Containers {
    template <typename KEY_TYPE, typename VALUE_TYPE>
    class Association;
}

namespace Stroika::Foundation::Containers::Factory {

    /**
     *  \brief   Singleton factory object - Used to create the default backend implementation of a Association<> container
     *
     *  Note - you can override the underlying factory dynamically by calling Association_Factory<T,TRAITS>::Register (), or
     *  replace it statically by template-specializing Association_Factory<T,TRAITS>::New () - though the later is trickier.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  \note   Association_Factory<K,P> makes up its own default comparer, and so can use order Associations, like Association_stdmap, whereas
     *          Association_Factory<K,P,E> - since it takes an equals comparer - is restricted to backends that work with an equals comparere.
     */
    template <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
    class Association_Factory {
    public:
        static_assert (not is_reference_v<KEY_TYPE> and not is_reference_v<VALUE_TYPE> and not is_reference_v<KEY_EQUALS_COMPARER>,
                       "typically if this fails its because a (possibly indirect) caller forgot to use forward<TTT>(), or remove_cvref_t");
        static_assert (Common::IsEqualsComparer<KEY_EQUALS_COMPARER> (), "Equals comparer required with Association_Factory");
#if 0
        
    public:
        /** 
         */
        using BareFactoryFunctionType = Association<KEY_TYPE, VALUE_TYPE> (*) (const KEY_EQUALS_COMPARER& keyEqualsComparer);
#endif

    public:
        /** 
         */
        using FactoryFunctionType = function<Association<KEY_TYPE, VALUE_TYPE> (const KEY_EQUALS_COMPARER& keyEqualsComparer)>;

    public:
        /**
         *  Hints can be used in factory constructor to guide the choice of the best container implementation/backend.
         */
        struct Hints {
            optional<bool> fOptimizeForLookupSpeedOverUpdateSpeed;
        };

    public:
        constexpr Association_Factory (const Hints& hints = {});
        constexpr Association_Factory (const FactoryFunctionType& f);
        constexpr Association_Factory (const Association_Factory&) = default;

    public:
        /**
         *  You can call this directly, but there is no need, as the Association<T,TRAITS> CTOR does so automatically.
         */
        nonvirtual Association<KEY_TYPE, VALUE_TYPE> operator() (const KEY_EQUALS_COMPARER& keyEqualsComparer = {}) const;

    public:
        /**
         *  Register a default global factory for Association objects (of the templated type/parameters).
         *  No need to call, typically, as the default factory is generally fine.
         * 
         *  \par Example Usage
         *      \code
         *          Association_Factory::Register(Association_Factory{Association_Factory::Hints{.fOptimizeForLookupSpeedOverUpdateSpeed=true});
         *          Association_Factory::Register();    // or use defaults
         *      \endcode
         *
         *  \note   \em Thread-Safety   <a href='#Internally-Synchronized-Thread-Safety'>Internally-Synchronized-Thread-Safety</a>
         */
        static void Register (optional<Association_Factory> f = nullopt);

    private:
        FactoryFunctionType                                   fFactory_;
        static inline atomic<shared_ptr<Association_Factory>> sDefaultFactory_;
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "Association_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Association_Factory_h_ */
