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
                 ************************** Set_Factory<T, EQUALS_COMPARER> *********************
                 ********************************************************************************
                 */
                template <typename T, typename EQUALS_COMPARER>
                atomic<Set<T> (*) ()> Set_Factory<T, EQUALS_COMPARER>::sFactory_ (nullptr);
                template <typename T, typename EQUALS_COMPARER>
                inline Set<T> Set_Factory<T, EQUALS_COMPARER>::operator() () const
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
                        return Default_ (std::equal_to<T> ());
                    }
                }
                template <typename T, typename EQUALS_COMPARER>
                inline Set<T> Set_Factory<T, EQUALS_COMPARER>::operator() (const EQUALS_COMPARER& equalsComparer) const
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
                        return Default_ (equalsComparer);
                    }
                }
                template <typename T, typename EQUALS_COMPARER>
                void Set_Factory<T, EQUALS_COMPARER>::Register (Set<T> (*factory) ())
                {
                    sFactory_ = factory;
                }
                template <typename T, typename EQUALS_COMPARER>
                inline Set<T> Set_Factory<T, EQUALS_COMPARER>::Default_ (const EQUALS_COMPARER& equalsComparer)
                {
                    /*
                     *  Use SFINAE to select best default implementation.
                     */
                    return Default_SFINAE_ (equalsComparer, static_cast<T*> (nullptr));
                }
                template <typename T, typename EQUALS_COMPARER>
                template <typename CHECK_T>
                inline Set<T> Set_Factory<T, EQUALS_COMPARER>::Default_SFINAE_ (const EQUALS_COMPARER& equalsComparer, CHECK_T*, typename enable_if<Configuration::has_lt<CHECK_T>::value>::type*)
                {
                    if (typeid (EQUALS_COMPARER) == typeid (equal_to<T>)) {
                        return Concrete::Set_stdset<T> (); // OK to omit TRAITS (and not manually pass in equals) cuz checked this method using default traits so no need to specify traits here
                    }
                    else {
                        return Concrete::Set_LinkedList<T> (equalsComparer);
                    }
                }
                template <typename T, typename EQUALS_COMPARER>
                inline Set<T> Set_Factory<T, EQUALS_COMPARER>::Default_SFINAE_ (const EQUALS_COMPARER& equalsComparer, ...)
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
                    return Concrete::Set_LinkedList<T> (equalsComparer); // probably not right - probably have to specify ARG to CTOR with comparer
                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Set_Factory_inl_ */
