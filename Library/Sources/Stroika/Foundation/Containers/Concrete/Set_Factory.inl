/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_Set_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_Set_Factory_inl_

#include    "Set_LinkedList.h"
#include    "Set_stdset.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                 ********************************************************************************
                 ************************** Set_Factory<T, TRAITS> ******************************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                atomic<Set<T, TRAITS> (*) ()>   Set_Factory<T, TRAITS>::sFactory_ (nullptr);
                template    <typename T, typename TRAITS>
                inline  Set<T, TRAITS>  Set_Factory<T, TRAITS>::mk ()
                {
                    /*
                     *  Would have been more performant to just and assure always properly set, but to initialize
                     *  sFactory_ with a value other than nullptr requires waiting until after main() - so causes problems
                     *  with containers constructed before main.
                     *
                     *  This works more generally (and with hopefully modest enough performance impact).
                     */
                    auto f = sFactory_.load ();
                    if (f == nullptr) {
                        f = &Default_;
                    }
                    return f ();
                }
                template    <typename T, typename TRAITS>
                void    Set_Factory<T, TRAITS>::Register (Set<T, TRAITS> (*factory) ())
                {
                    sFactory_ = factory;
                }
                template    <typename T, typename TRAITS>
                Set<T, TRAITS>  Set_Factory<T, TRAITS>::Default_ ()
                {
                    /*
                     *  Use SFINAE to select best default implementation.
                     */
                    return Default_SFINAE_<T> ();
                }
                template    <typename T, typename TRAITS>
                template    <typename CHECK_T>
                inline  Set<T, TRAITS>  Set_Factory<T, TRAITS>::Default_SFINAE_ (typename enable_if <Common::Has_Operator_LessThan<CHECK_T>::value>::type*)
                {
                    return Set_stdset<T> ();
                }
                template    <typename T, typename TRAITS>
                template    <typename CHECK_T>
                inline  Set<T, TRAITS>  Set_Factory<T, TRAITS>::Default_SFINAE_ (typename enable_if < !Common::Has_Operator_LessThan<CHECK_T>::value >::type*)
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
                    return Set_LinkedList<T, TRAITS> ();
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Set_Factory_inl_ */
