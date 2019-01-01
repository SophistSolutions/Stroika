/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_SortedAssociation_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedAssociation_Factory_inl_

#include "../Concrete/SortedAssociation_stdmultimap.h"

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     ************ SortedAssociation_Factory<KEY_TYPE, VALUE_TYPE, TRAITS> ***********
     ********************************************************************************
     */
#if qCompiler_cpp17ExplicitInlineStaticMemberOfTemplate_Buggy
    template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
    atomic<SortedAssociation<KEY_TYPE, VALUE_TYPE> (*) ()> SortedAssociation_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::sFactory_ (nullptr);
#endif
    template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
    inline SortedAssociation<KEY_TYPE, VALUE_TYPE> SortedAssociation_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::operator() () const
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
    void SortedAssociation_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::Register (SortedAssociation<KEY_TYPE, VALUE_TYPE> (*factory) ())
    {
        sFactory_ = factory;
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
    inline SortedAssociation<KEY_TYPE, VALUE_TYPE> SortedAssociation_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::Default_ ()
    {
        return Concrete::SortedAssociation_stdmultimap<KEY_TYPE, VALUE_TYPE> ();
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_SortedAssociation_Factory_inl_ */
