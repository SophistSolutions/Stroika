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
                 ****** Mapping_Factory<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER> **************
                 ********************************************************************************
                 */
#if qCompiler_cpp17ExplicitInlineStaticMemberOfTemplate_Buggy
                template <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER>
                atomic<Mapping<KEY_TYPE, VALUE_TYPE> (*) (const KEY_EQUALS_COMPARER&)> Mapping_Factory<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER>::sFactory_ (nullptr);
#endif
                template <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER>
                inline Mapping_Factory<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER>::Mapping_Factory (const KEY_EQUALS_COMPARER& keyEqualsComparer)
                    : fKeyEqualsComparer_ (keyEqualsComparer)
                {
                }
                template <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER>
                inline Mapping<KEY_TYPE, VALUE_TYPE> Mapping_Factory<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER>::operator() () const
                {
                    /*
                     *  Would have been more performant to just and assure always properly set, but to initialize
                     *  sFactory_ with a value other than nullptr requires waiting until after main() - so causes problems
                     *  with containers constructed before main.
                     *
                     *  This works more generally (and with hopefully modest enough performance impact).
                     */
                    if (auto f = sFactory_.load ()) {
                        return f (fKeyEqualsComparer_);
                    }
                    else {
                        return Default_ (fKeyEqualsComparer_);
                    }
                }
                template <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER>
                void Mapping_Factory<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER>::Register (Mapping<KEY_TYPE, VALUE_TYPE> (*factory) (const KEY_EQUALS_COMPARER&))
                {
                    sFactory_ = factory;
                }
                template <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER>
                inline Mapping<KEY_TYPE, VALUE_TYPE> Mapping_Factory<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER>::Default_ (const KEY_EQUALS_COMPARER& keyEqualsComparer)
                {
                    /*
                     *  Note - though this is not an efficient implementation of Mapping<> for large sizes, its probably the most
                     *  efficeint representation which adds no requirements to KEY_TYPE, such as operator< (or a traits less) or
                     *  a hash function. And its quite reasonable for small Mapping's - which are often the case.
                     *
                     *  Calls may use an explicit initializer of Mapping_xxx<> to get better performance for large sized
                     *  maps.
                     */
                    return Concrete::Mapping_LinkedList<KEY_TYPE, VALUE_TYPE> (keyEqualsComparer);
                }

#if qCompiler_cpp17ExplicitInlineStaticMemberOfTemplate_Buggy
                template <typename KEY_TYPE, typename VALUE_TYPE>
                atomic<Mapping<KEY_TYPE, VALUE_TYPE> (*) ()> Mapping_Factory<KEY_TYPE, VALUE_TYPE, void>::sFactory_ (nullptr);
#endif
                template <typename KEY_TYPE, typename VALUE_TYPE>
                inline Mapping<KEY_TYPE, VALUE_TYPE> Mapping_Factory<KEY_TYPE, VALUE_TYPE, void>::operator() () const
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
                template <typename KEY_TYPE, typename VALUE_TYPE>
                void Mapping_Factory<KEY_TYPE, VALUE_TYPE, void>::Register (Mapping<KEY_TYPE, VALUE_TYPE> (*factory) ())
                {
                    sFactory_ = factory;
                }
                template <typename KEY_TYPE, typename VALUE_TYPE>
                inline Mapping<KEY_TYPE, VALUE_TYPE> Mapping_Factory<KEY_TYPE, VALUE_TYPE, void>::Default_ ()
                {
                    /*
                    *  Use SFINAE to select best default implementation.
                    */
                    return Default_SFINAE_ (static_cast<KEY_TYPE*> (nullptr));
                }
                template <typename KEY_TYPE, typename VALUE_TYPE>
                template <typename CHECK_KEY>
                inline Mapping<KEY_TYPE, VALUE_TYPE> Mapping_Factory<KEY_TYPE, VALUE_TYPE, void>::Default_SFINAE_ (CHECK_KEY*, typename enable_if<Configuration::has_lt<CHECK_KEY>::value>::type*)
                {
                    return Concrete::Mapping_stdmap<KEY_TYPE, VALUE_TYPE> (); // OK to omit TRAITS (and not manually pass in equals) cuz checked using default traits so no need to specify traits here
                }
                template <typename KEY_TYPE, typename VALUE_TYPE>
                inline Mapping<KEY_TYPE, VALUE_TYPE> Mapping_Factory<KEY_TYPE, VALUE_TYPE, void>::Default_SFINAE_ (...)
                {
                    /*
                     *  Note - though this is not an efficient implementation of Mapping<> for large sizes, its probably the most
                     *  efficeint representation which adds no requirements to KEY_TYPE, such as operator< (or a traits less) or
                     *  a hash function. And its quite reasonable for small Mapping's - which are often the case.
                     *
                     *  Calls may use an explicit initializer of Mapping_xxx<> to get better performance for large sized
                     *  maps.
                     */
                    return Concrete::Mapping_LinkedList<KEY_TYPE, VALUE_TYPE> ();
                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Mapping_Factory_inl_ */
