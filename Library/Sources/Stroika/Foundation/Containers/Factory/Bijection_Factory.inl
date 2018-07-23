/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
#if qCompiler_cpp17ExplicitInlineStaticMemberOfTemplate_Buggy
    template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER>
    atomic<Bijection<DOMAIN_TYPE, RANGE_TYPE> (*) (Bijection_Base::InjectivityViolationPolicy, const DOMAIN_EQUALS_COMPARER&, const RANGE_EQUALS_COMPARER&)> Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, DOMAIN_EQUALS_COMPARER, RANGE_EQUALS_COMPARER>::sFactory_ (nullptr);
#endif
    template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER>
    inline Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, DOMAIN_EQUALS_COMPARER, RANGE_EQUALS_COMPARER>::Bijection_Factory (const DOMAIN_EQUALS_COMPARER& domainEqualsComparer, const RANGE_EQUALS_COMPARER& rangeEqualsComparer)
        : Bijection_Factory (InjectivityViolationPolicy::eDEFAULT, domainEqualsComparer, rangeEqualsComparer)
    {
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER>
    inline Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, DOMAIN_EQUALS_COMPARER, RANGE_EQUALS_COMPARER>::Bijection_Factory (InjectivityViolationPolicy injectivityCheckPolicy, const DOMAIN_EQUALS_COMPARER& domainEqualsComparer, const RANGE_EQUALS_COMPARER& rangeEqualsComparer)
        : fInjectivityViolationPolicy_ (injectivityCheckPolicy)
        , fDomainEqualsComparer_ (domainEqualsComparer)
        , fRangeEqualsComparer_ (rangeEqualsComparer)
    {
    }

    template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE> Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, DOMAIN_EQUALS_COMPARER, RANGE_EQUALS_COMPARER>::operator() () const
    {
        /*
         *  Would have been more performant to just and assure always properly set, but to initialize
         *  sFactory_ with a value other than nullptr requires waiting until after main() - so causes problems
         *  with containers constructed before main.
         *
         *  This works more generally (and with hopefully modest enough performance impact).
         */
        if (auto f = sFactory_.load ()) {
            return f (fInjectivityViolationPolicy_, fDomainEqualsComparer_, fRangeEqualsComparer_);
        }
        else {
            return Default_ (fInjectivityViolationPolicy_, fDomainEqualsComparer_, fRangeEqualsComparer_);
        }
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER>
    void Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, DOMAIN_EQUALS_COMPARER, RANGE_EQUALS_COMPARER>::Register (Bijection<DOMAIN_TYPE, RANGE_TYPE> (*factory) (InjectivityViolationPolicy, const DOMAIN_EQUALS_COMPARER&, const RANGE_EQUALS_COMPARER&))
    {
        sFactory_ = factory;
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE> Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, DOMAIN_EQUALS_COMPARER, RANGE_EQUALS_COMPARER>::Default_ (InjectivityViolationPolicy injectivityViolationPolicy, const DOMAIN_EQUALS_COMPARER& domainEqualsComparer, const RANGE_EQUALS_COMPARER& rangeEqualsComparer)
    {
        /*
            *  Note - though this is not an efficient implementation of Bijection<> for large sizes, its probably the most
            *  efficeint representation which adds no requirements to DOMAIN_TYPE, such as operator< (or a traits less) or
            *  a hash function. And its quite reasonable for small Bijection's - which are often the case.
            *
            *  Calls may use an explicit initializer of Bijection_xxx<> to get better performance for large sized
            *  maps.
            */
        return Concrete::Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE> (injectivityViolationPolicy, domainEqualsComparer, rangeEqualsComparer);
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Bijection_Factory_inl_ */
