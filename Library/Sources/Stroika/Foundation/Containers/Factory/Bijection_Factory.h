/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Bijection_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_Bijection_Factory_h_

#include "../../StroikaPreComp.h"

#include <atomic>
#include <type_traits>

/**
 *  \file
 *
 *  TODO:
 *      @todo   Extend this metaphor to have different kinds of factories, like mkBijection_Fastest,
 *              mkBijection_Smallest, mkBijectionWithHash_Fastest etc...
 *              Possibly extend to policy objects, and have properties for this stuff?
 *
 *      @todo   Rethink use of TRAITS as argument to factory. Probably can only do the SFINAE stuff
 *              when TRAITS EQUALS the default.
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            template <typename DOMAIN_TYPE, typename RANGE_TYPE>
            class Bijection;

            namespace Factory {

                /**
                 *  \brief   Singleton factory object - Used to create the default backend implementation of a Bijection<> container
                 *
                 *  Note - you can override the underlying factory dynamically by calling Bijection_Factory<T,TRAITS>::Register (), or
                 *  replace it statically by template-specailizing Bijection_Factory<T,TRAITS>::New () - though the later is trickier.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 */
                template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER>
                class Bijection_Factory {
                private:
                    static atomic<Bijection<DOMAIN_TYPE, RANGE_TYPE> (*) (const DOMAIN_EQUALS_COMPARER&, const RANGE_EQUALS_COMPARER&)> sFactory_;

                public:
                    Bijection_Factory (const DOMAIN_EQUALS_COMPARER& domainEqualsComparer, const RANGE_EQUALS_COMPARER& rangeEqualsComparer);

                public:
                    /**
                     *  You can call this directly, but there is no need, as the Bijection<T,TRAITS> CTOR does so automatically.
                     */
                    nonvirtual Bijection<DOMAIN_TYPE, RANGE_TYPE> operator() () const;

                public:
                    /**
                     *  Register a replacement creator/factory for the given Bijection<DOMAIN_TYPE, RANGE_TYPE,TRAITS>. Note this is a global change.
                     */
                    static void Register (Bijection<DOMAIN_TYPE, RANGE_TYPE> (*factory) (const DOMAIN_EQUALS_COMPARER&, const RANGE_EQUALS_COMPARER&) = nullptr);

                private:
                    const DOMAIN_EQUALS_COMPARER fDomainEqualsComparer_;
                    const RANGE_EQUALS_COMPARER  fRangeEqualsComparer_;

                private:
                    static Bijection<DOMAIN_TYPE, RANGE_TYPE> Default_ (const DOMAIN_EQUALS_COMPARER&, const RANGE_EQUALS_COMPARER&);
                };
            }
        }
    }
}

/*
 ********************************************************************************
 **************************** Implementation Details ****************************
 ********************************************************************************
 */
#include "Bijection_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Bijection_Factory_h_ */
