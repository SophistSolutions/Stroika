/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_Association_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_Association_Factory_inl_

#include    "../Concrete/Association_LinkedList.h"
#include    "../Concrete/Association_stdmultimap.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                 ********************************************************************************
                 ********** Association_Factory<KEY_TYPE, VALUE_TYPE, TRAITS> *******************
                 ********************************************************************************
                 */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                atomic<Association<KEY_TYPE, VALUE_TYPE, TRAITS> (*) ()>    Association_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::sFactory_ (nullptr);
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  Association<KEY_TYPE, VALUE_TYPE, TRAITS>  Association_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()
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
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                void    Association_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::Register (Association<KEY_TYPE, VALUE_TYPE, TRAITS> (*factory) ())
                {
                    sFactory_ = factory;
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                Association<KEY_TYPE, VALUE_TYPE, TRAITS>  Association_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::Default_ ()
                {
                    /*
                     *  Use SFINAE to select best default implementation.
                     */
                    return Default_SFINAE_ (static_cast<KEY_TYPE*> (nullptr));
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                template    <typename CHECK_KEY>
                inline  Association<KEY_TYPE, VALUE_TYPE, TRAITS>  Association_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::Default_SFINAE_ (CHECK_KEY*, typename enable_if <Configuration::has_lt<CHECK_KEY>::value and is_same<TRAITS, DefaultTraits::Association<CHECK_KEY, VALUE_TYPE>>::value>::type*)
                {
                    return Association_stdmultimap<KEY_TYPE, VALUE_TYPE> ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  Association<KEY_TYPE, VALUE_TYPE, TRAITS>  Association_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::Default_SFINAE_ (...)
                {
                    /*
                     *  Note - though this is not an efficient implementation of Association<> for large sizes, its probably the most
                     *  efficeint representation which adds no requirements to KEY_TYPE, such as operator< (or a traits less) or
                     *  a hash function. And its quite reasonable for small Association's - which are often the case.
                     *
                     *  Calls may use an explicit initializer of Association_xxx<> to get better performance for large sized
                     *  maps.
                     */
                    return Association_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS> ();
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Association_Factory_inl_ */
