/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Bijection_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_Bijection_Factory_h_

#include "../../StroikaPreComp.h"

#include <atomic>
#include <type_traits>

/**
 *  \file
 */

namespace Stroika::Foundation::Containers {
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    class Bijection;
}

namespace Stroika::Foundation::Containers::Factory {

    /**
     *  \brief   Singleton factory object - Used to create the default backend implementation of a Bijection<> container; typically not used directly
     *
     *  Note - you can override the underlying factory dynamically by calling Bijection_Factory<T,TRAITS>::Register ().
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER>
    class Bijection_Factory {
    public:
        static_assert (not is_reference_v<DOMAIN_TYPE>,
                       "typically if this fails its because a (possibly indirect) caller forgot to use forward<TTT>(), or remove_cvref_t");
        static_assert (not is_reference_v<RANGE_TYPE>,
                       "typically if this fails its because a (possibly indirect) caller forgot to use forward<TTT>(), or remove_cvref_t");
        static_assert (not is_reference_v<DOMAIN_EQUALS_COMPARER>,
                       "typically if this fails its because a (possibly indirect) caller forgot to use forward<TTT>(), or remove_cvref_t");
        static_assert (not is_reference_v<RANGE_EQUALS_COMPARER>,
                       "typically if this fails its because a (possibly indirect) caller forgot to use forward<TTT>(), or remove_cvref_t");
        static_assert (Common::IsEqualsComparer<DOMAIN_EQUALS_COMPARER> (), "Domain Equals comparer required with Bijection_Factory");
        static_assert (Common::IsEqualsComparer<RANGE_EQUALS_COMPARER> (), "Range Equals comparer required with Bijection_Factory");

    public:
        /**
         */
        using ConstructedType = Bijection<DOMAIN_TYPE, RANGE_TYPE>;

    public:
        /**
         *  Function type to create an Association object.
         */
        using FactoryFunctionType =
            function<ConstructedType (DataExchange::ValidationStrategy injectivityCheckPolicy,
                                      const DOMAIN_EQUALS_COMPARER& domainEqualsComparer, const RANGE_EQUALS_COMPARER& rangeEqualsComparer)>;

    public:
        /**
         *  Hints can be used in factory constructor to guide the choice of the best container implementation/backend.
         */
        struct Hints {
            optional<bool> fOptimizeForLookupSpeedOverUpdateSpeed;
        };

    public:
        /**
         *  Construct a factory for producing new bijections. The default is to use whatever was registered with 
         *  Bijection_Factory::Register (), but a specific factory can easily be constructed with provided arguments.
         */
        constexpr Bijection_Factory ();
        constexpr Bijection_Factory (const Hints& hints);
        constexpr Bijection_Factory (const FactoryFunctionType& f);
        constexpr Bijection_Factory (const Bijection_Factory&) = default;

    public:
        /**
         *  This can be called anytime, before main(), or after. BUT - beware, any calls to Register must
         *  be externally synchronized, meaning effectively that they must happen before the creation of any
         *  threads, to be safe. Also note, since this returns a const reference, any calls to Register() after
         *  a call to Default, even if synchronized, is suspect.
         */
        static const Bijection_Factory& Default ();

    public:
        /**
         *  You can call this directly, but there is no need, as the Bijection<DOMAIN_TYPE, RANGE_TYPE> CTOR does so automatically.
         */
        nonvirtual ConstructedType operator() (const DOMAIN_EQUALS_COMPARER& domainEqualsComparer = {},
                                               const RANGE_EQUALS_COMPARER&  rangeEqualsComparer  = {}) const;
        nonvirtual ConstructedType operator() (DataExchange::ValidationStrategy injectivityCheckPolicy,
                                               const DOMAIN_EQUALS_COMPARER&    domainEqualsComparer = {},
                                               const RANGE_EQUALS_COMPARER&     rangeEqualsComparer  = {}) const;

    public:
        /**
         *  Register a default global factory for Bijection objects (of the templated type/parameters).
         *  No need to call, typically, as the default factory is generally fine.
         * 
         *  \par Example Usage
         *      \code
         *          Bijection_Factory::Register(Bijection_Factory{Bijection_Factory::Hints{.fOptimizeForLookupSpeedOverUpdateSpeed=true});
         *          Bijection_Factory::Register();    // or use defaults
         *      \endcode
         *
         *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
         *          BUT - special note/restriction - must be called before any threads call Association_Factory::Bijection_Factory() OR
         *          Bijection_Factory::Default(), which effectively means must be called at the start of main, but before creating any threads
         *          which might use the factory).
         * 
         *  \NOTE this differs markedly from Stroika 2.1, where Register could be called anytime, and was internally synchronized.
         */
        static void Register (const optional<Bijection_Factory>& f = nullopt);

    private:
        FactoryFunctionType fFactory_;

    private:
        // function to assure magically constructed even if called before main
        static Bijection_Factory& AccessDefault_ ();
    };

}

/*
 ********************************************************************************
 **************************** Implementation Details ****************************
 ********************************************************************************
 */
#include "Bijection_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Bijection_Factory_h_ */
