/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_Set_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_Set_Factory_inl_

#include "../Concrete/Set_LinkedList.h"
#include "../Concrete/Set_stdset.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Factory {

                /*
                 ********************************************************************************
                 ************************** Set_Factory<T, TRAITS> ******************************
                 ********************************************************************************
                 */
                template <typename T, typename TRAITS>
                atomic<Set<T, TRAITS> (*) ()> Set_Factory<T, TRAITS>::sFactory_ (nullptr);
                template <typename T, typename TRAITS>
                inline Set<T, TRAITS> Set_Factory<T, TRAITS>::operator() () const
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
                template <typename T, typename TRAITS>
                void Set_Factory<T, TRAITS>::Register (Set<T, TRAITS> (*factory) ())
                {
                    sFactory_ = factory;
                }
                template <typename T, typename TRAITS>
                inline Set<T, TRAITS> Set_Factory<T, TRAITS>::Default_ ()
                {
                    /*
                     *  Use SFINAE to select best default implementation.
                     */
                    return Default_SFINAE_ (static_cast<T*> (nullptr));
                }
                template <typename T, typename TRAITS>
                template <typename CHECK_T>
                inline Set<T, TRAITS> Set_Factory<T, TRAITS>::Default_SFINAE_ (CHECK_T*, typename enable_if<Configuration::has_lt<CHECK_T>::value and is_same<TRAITS, DefaultTraits::Set<CHECK_T>>::value>::type*)
                {
                    return Concrete::Set_stdset<T> (); // OK to omit TRAITS (and not manually pass in equals) cuz checked this method using default traits so no need to specify traits here
                }
                template <typename T, typename TRAITS>
                inline Set<T, TRAITS> Set_Factory<T, TRAITS>::Default_SFINAE_ (...)
                {
                    /*
                     *  Note - though this is not an efficient implementation of Set<> for large sizes,
                     *  its probably the most efficeint representation which adds no requirements to KEY_TYPE,
                     *  such as operator< (or a traits less) or a hash function. And its quite reasonable for
                     *  small Sets's - which are often the case.
                     *
                     *  Calls may use an explicit initializer of Set_xxx<> to get better performance for large sized
                     *  sets.
                     */
                    return Concrete::Set_LinkedList<T, TRAITS> ();
                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Set_Factory_inl_ */
