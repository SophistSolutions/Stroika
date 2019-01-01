/*
* Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
*/

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_Association_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_Association_Factory_inl_

#include "../Concrete/Association_LinkedList.h"
#include "../Concrete/Association_stdmultimap.h"

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     ********* Association_Factory<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER> *******
     ********************************************************************************
     */
#if qCompiler_cpp17ExplicitInlineStaticMemberOfTemplate_Buggy
    template <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER>
    atomic<Association<KEY_TYPE, VALUE_TYPE> (*) ()> Association_Factory<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER>::sFactory_ (nullptr);
#endif
    template <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER>
    inline Association<KEY_TYPE, VALUE_TYPE> Association_Factory<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER>::operator() () const
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
    template <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER>
    void Association_Factory<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER>::Register (Association<KEY_TYPE, VALUE_TYPE> (*factory) ())
    {
        sFactory_ = factory;
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER>
    inline Association<KEY_TYPE, VALUE_TYPE> Association_Factory<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER>::Default_ ()
    {
        /*
         *  Note - though this is not an efficient implementation of Association<> for large sizes, its probably the most
         *  efficeint representation which adds no requirements to KEY_TYPE, such as operator< (or a traits less) or
         *  a hash function. And its quite reasonable for small Association's - which are often the case.
         *
         *  Calls may use an explicit initializer of Association_xxx<> to get better performance for large sized
         *  maps.
         */
        return Concrete::Association_LinkedList<KEY_TYPE, VALUE_TYPE> ();
    }

    /*
     ********************************************************************************
     ********** Association_Factory<KEY_TYPE, VALUE_TYPE, false_type> ***************
     ********************************************************************************
     */
#if qCompiler_cpp17ExplicitInlineStaticMemberOfTemplate_Buggy
    template <typename KEY_TYPE, typename VALUE_TYPE>
    atomic<Association<KEY_TYPE, VALUE_TYPE> (*) ()> Association_Factory<KEY_TYPE, VALUE_TYPE, false_type>::sFactory_ (nullptr);
#endif
    template <typename KEY_TYPE, typename VALUE_TYPE>
    inline Association<KEY_TYPE, VALUE_TYPE> Association_Factory<KEY_TYPE, VALUE_TYPE, false_type>::operator() () const
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
    void Association_Factory<KEY_TYPE, VALUE_TYPE, false_type>::Register (Association<KEY_TYPE, VALUE_TYPE> (*factory) ())
    {
        sFactory_ = factory;
    }
    template <typename KEY_TYPE, typename VALUE_TYPE>
    inline Association<KEY_TYPE, VALUE_TYPE> Association_Factory<KEY_TYPE, VALUE_TYPE, false_type>::Default_ ()
    {
        /*
         *  Use SFINAE to select best default implementation.
         */
        return Default_SFINAE_ (static_cast<KEY_TYPE*> (nullptr));
    }
    template <typename KEY_TYPE, typename VALUE_TYPE>
    template <typename CHECK_KEY>
    inline Association<KEY_TYPE, VALUE_TYPE> Association_Factory<KEY_TYPE, VALUE_TYPE, false_type>::Default_SFINAE_ (CHECK_KEY*, enable_if_t<Configuration::has_lt<CHECK_KEY>::value>*)
    {
        return Concrete::Association_stdmultimap<KEY_TYPE, VALUE_TYPE> (); // OK to omit TRAITS (and not manually pass in equals) cuz checked using default traits so no need to specify traits here
    }
    template <typename KEY_TYPE, typename VALUE_TYPE>
    inline Association<KEY_TYPE, VALUE_TYPE> Association_Factory<KEY_TYPE, VALUE_TYPE, false_type>::Default_SFINAE_ (...)
    {
        /*
         *  Note - though this is not an efficient implementation of Association<> for large sizes, its probably the most
         *  efficeint representation which adds no requirements to KEY_TYPE, such as operator< (or a traits less) or
         *  a hash function. And its quite reasonable for small Association's - which are often the case.
         *
         *  Calls may use an explicit initializer of Association_xxx<> to get better performance for large sized
         *  maps.
         */
        return Concrete::Association_LinkedList<KEY_TYPE, VALUE_TYPE> ();
    }

}
#endif /* _Stroika_Foundation_Containers_Concrete_Association_Factory_inl_ */
