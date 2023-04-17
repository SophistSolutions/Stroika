/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_Bijection_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_Bijection_Factory_inl_

#include "../Concrete/Bijection_LinkedList.h"

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     * Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, DOMAIN_EQUALS_COMPARER, RANGE_EQUALS_COMPARER> *
     ********************************************************************************
     */
    template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER>
    constexpr Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, DOMAIN_EQUALS_COMPARER, RANGE_EQUALS_COMPARER>::Bijection_Factory (
        const DOMAIN_EQUALS_COMPARER& domainEqualsComparer, const RANGE_EQUALS_COMPARER& rangeEqualsComparer, const Hints& hints)
        : Bijection_Factory{DataExchange::ValidationStrategy::eAssertion, domainEqualsComparer, rangeEqualsComparer, hints}
    {
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER>
    constexpr Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, DOMAIN_EQUALS_COMPARER, RANGE_EQUALS_COMPARER>::Bijection_Factory (
        DataExchange::ValidationStrategy injectivityCheckPolicy, const DOMAIN_EQUALS_COMPARER& domainEqualsComparer,
        const RANGE_EQUALS_COMPARER& rangeEqualsComparer, const Hints& hints)
    {
        if (auto f = sFactory_.load ()) {
            this->fFactory_ = [=] () { return f (injectivityCheckPolicy, domainEqualsComparer, rangeEqualsComparer); };
        }
        else {
            if (hints.fOptimizeForLookupSpeedOverUpdateSpeed.value_or (true)) {
                // array has better memory cache locality properties so lookups probably faster
                // @todo ARRAY
                this->fFactory_ = [=] () {
                    return Concrete::Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>{injectivityCheckPolicy, domainEqualsComparer, rangeEqualsComparer};
                };
            }
            else {
                this->fFactory_ = [=] () {
                    return Concrete::Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>{injectivityCheckPolicy, domainEqualsComparer, rangeEqualsComparer};
                };
            }
        }
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE> Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, DOMAIN_EQUALS_COMPARER, RANGE_EQUALS_COMPARER>::operator() () const
    {
        return this->fFactory_ ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER>
    void Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, DOMAIN_EQUALS_COMPARER, RANGE_EQUALS_COMPARER>::Register (Bijection<DOMAIN_TYPE, RANGE_TYPE> (
        *factory) (DataExchange::ValidationStrategy, const DOMAIN_EQUALS_COMPARER&, const RANGE_EQUALS_COMPARER&))
    {
        sFactory_ = factory;
    }

}
#endif /* _Stroika_Foundation_Containers_Concrete_Bijection_Factory_inl_ */
