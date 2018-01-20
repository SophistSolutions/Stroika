/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_Mapping_Factory_inl_

#include "../Concrete/Mapping_LinkedList.h"
#include "../Concrete/Mapping_stdmap.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Factory {

                /*
                 ********************************************************************************
                 ************** Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS> *******************
                 ********************************************************************************
                 */
                template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                atomic<Mapping<KEY_TYPE, VALUE_TYPE, TRAITS> (*) ()> Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::sFactory_ (nullptr);
                template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline Mapping<KEY_TYPE, VALUE_TYPE, TRAITS> Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::New ()
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
                template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                void Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::Register (Mapping<KEY_TYPE, VALUE_TYPE, TRAITS> (*factory) ())
                {
                    sFactory_ = factory;
                }
                template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline Mapping<KEY_TYPE, VALUE_TYPE, TRAITS> Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::Default_ ()
                {
                    /*
                     *  Use SFINAE to select best default implementation.
                     */
                    return Default_SFINAE_ (static_cast<KEY_TYPE*> (nullptr));
                }
                template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                template <typename CHECK_KEY>
                inline Mapping<KEY_TYPE, VALUE_TYPE, TRAITS> Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::Default_SFINAE_ (CHECK_KEY*, typename enable_if<Configuration::has_lt<CHECK_KEY>::value and is_same<TRAITS, DefaultTraits::Mapping<CHECK_KEY, VALUE_TYPE>>::value>::type*)
                {
                    return Concrete::Mapping_stdmap<KEY_TYPE, VALUE_TYPE> (); // OK to omit TRAITS (and not manually pass in equals) cuz checked using default traits so no need to specify traits here
                }
                template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline Mapping<KEY_TYPE, VALUE_TYPE, TRAITS> Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::Default_SFINAE_ (...)
                {
                    /*
                     *  Note - though this is not an efficient implementation of Mapping<> for large sizes, its probably the most
                     *  efficeint representation which adds no requirements to KEY_TYPE, such as operator< (or a traits less) or
                     *  a hash function. And its quite reasonable for small Mapping's - which are often the case.
                     *
                     *  Calls may use an explicit initializer of Mapping_xxx<> to get better performance for large sized
                     *  maps.
                     */
                    return Concrete::Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS> ();
                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Mapping_Factory_inl_ */
