/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Containers/Concrete/Bijection_LinkedList.h"

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     * Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, DOMAIN_EQUALS_COMPARER, RANGE_EQUALS_COMPARER> *
     ********************************************************************************
     */
    template <typename DOMAIN_TYPE, typename RANGE_TYPE, IEqualsComparer<DOMAIN_TYPE> DOMAIN_EQUALS_COMPARER, IEqualsComparer<RANGE_TYPE> RANGE_EQUALS_COMPARER>
    constexpr Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, DOMAIN_EQUALS_COMPARER, RANGE_EQUALS_COMPARER>::Bijection_Factory (const FactoryFunctionType& f)
        : fFactory_{f}
    {
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE, IEqualsComparer<DOMAIN_TYPE> DOMAIN_EQUALS_COMPARER, IEqualsComparer<RANGE_TYPE> RANGE_EQUALS_COMPARER>
    constexpr Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, DOMAIN_EQUALS_COMPARER, RANGE_EQUALS_COMPARER>::Bijection_Factory ()
        : Bijection_Factory{AccessDefault_ ()}
    {
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE, IEqualsComparer<DOMAIN_TYPE> DOMAIN_EQUALS_COMPARER, IEqualsComparer<RANGE_TYPE> RANGE_EQUALS_COMPARER>
    constexpr Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, DOMAIN_EQUALS_COMPARER, RANGE_EQUALS_COMPARER>::Bijection_Factory ([[maybe_unused]] const Hints& hints)
        : Bijection_Factory{[] () -> FactoryFunctionType {
            // @todo add more backends, and pay attention to hints
            // especially easy to add Bijection_Array, and probably strictly better - since add needs to effectively walk whole list to see if new or update.
            return [] (DataExchange::ValidationStrategy injectivityCheckPolicy, const DOMAIN_EQUALS_COMPARER& domainEqualsComparer,
                       const RANGE_EQUALS_COMPARER& rangeEqualsComparer) {
                return Concrete::Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>{injectivityCheckPolicy, domainEqualsComparer, rangeEqualsComparer};
            };
        }()}
    {
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE, IEqualsComparer<DOMAIN_TYPE> DOMAIN_EQUALS_COMPARER, IEqualsComparer<RANGE_TYPE> RANGE_EQUALS_COMPARER>
    inline auto Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, DOMAIN_EQUALS_COMPARER, RANGE_EQUALS_COMPARER>::Default () -> const Bijection_Factory&
    {
        return AccessDefault_ ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE, IEqualsComparer<DOMAIN_TYPE> DOMAIN_EQUALS_COMPARER, IEqualsComparer<RANGE_TYPE> RANGE_EQUALS_COMPARER>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE> Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, DOMAIN_EQUALS_COMPARER, RANGE_EQUALS_COMPARER>::operator() (
        const DOMAIN_EQUALS_COMPARER& domainEqualsComparer, const RANGE_EQUALS_COMPARER& rangeEqualsComparer) const
    {
        return this->operator() (DataExchange::ValidationStrategy::eAssertion, domainEqualsComparer, rangeEqualsComparer);
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE, IEqualsComparer<DOMAIN_TYPE> DOMAIN_EQUALS_COMPARER, IEqualsComparer<RANGE_TYPE> RANGE_EQUALS_COMPARER>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE> Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, DOMAIN_EQUALS_COMPARER, RANGE_EQUALS_COMPARER>::operator() (
        DataExchange::ValidationStrategy injectivityCheckPolicy, const DOMAIN_EQUALS_COMPARER& domainEqualsComparer,
        const RANGE_EQUALS_COMPARER& rangeEqualsComparer) const
    {
        return this->fFactory_ (injectivityCheckPolicy, domainEqualsComparer, rangeEqualsComparer);
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE, IEqualsComparer<DOMAIN_TYPE> DOMAIN_EQUALS_COMPARER, IEqualsComparer<RANGE_TYPE> RANGE_EQUALS_COMPARER>
    void Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, DOMAIN_EQUALS_COMPARER, RANGE_EQUALS_COMPARER>::Register (const optional<Bijection_Factory>& f)
    {
        AccessDefault_ () = f.has_value () ? *f : Bijection_Factory{Hints{}};
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE, IEqualsComparer<DOMAIN_TYPE> DOMAIN_EQUALS_COMPARER, IEqualsComparer<RANGE_TYPE> RANGE_EQUALS_COMPARER>
    inline auto Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, DOMAIN_EQUALS_COMPARER, RANGE_EQUALS_COMPARER>::AccessDefault_ () -> Bijection_Factory&
    {
        static Bijection_Factory sDefault_{Hints{}};
        return sDefault_;
    }

}
