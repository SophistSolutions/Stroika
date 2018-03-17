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

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Factory {

                /*
                 ********************************************************************************
                 *********** Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, TRAITS> *****************
                 ********************************************************************************
                 */
                template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                atomic<Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS> (*) ()> Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::sFactory_ (nullptr);
                template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                inline Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS> Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::operator() () const
                {
                    /*
                     *  Would have been more performant to just and assure always properly set, but to initialize
                     *  sFactory_ with a value other than nullptr requires waiting until after main() - so causes problems
                     *  with containers constructed before main.
                     *
                     *  This works more generally (and with hopefully modest enough performance impact).
                     */
                    if (auto f = sFactory_.load ()) {
                        return f ();
                    }
                    else {
                        return Default_ ();
                    }
                }
                template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                void Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Register (Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS> (*factory) ())
                {
                    sFactory_ = factory;
                }
                template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                inline Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS> Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Default_ ()
                {
                    /*
                     *  Use SFINAE to select best default implementation.
                     */
                    return Default_SFINAE_ (static_cast<DOMAIN_TYPE*> (nullptr));
                }
                template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                inline Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS> Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Default_SFINAE_ (...)
                {
                    /*
                     *  Note - though this is not an efficient implementation of Bijection<> for large sizes, its probably the most
                     *  efficeint representation which adds no requirements to DOMAIN_TYPE, such as operator< (or a traits less) or
                     *  a hash function. And its quite reasonable for small Bijection's - which are often the case.
                     *
                     *  Calls may use an explicit initializer of Bijection_xxx<> to get better performance for large sized
                     *  maps.
                     */
                    return Concrete::Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS> ();
                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Bijection_Factory_inl_ */
