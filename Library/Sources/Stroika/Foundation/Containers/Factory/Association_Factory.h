/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Association_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_Association_Factory_h_

#include "../../StroikaPreComp.h"

#include <atomic>
#include <type_traits>

/**
 *  \file
 *
 *  TODO:
 *      @todo   Extend this metaphor to have different kinds of factories, like mkAssociation_Fastest,
 *              mkAssociation_Smallest, mkAssociationWithHash_Fastest etc...
 *              Possibly extend to policy objects, and have properties for this stuff?
 *
 *      @todo   Rethink use of TRAITS as argument to factory. Probably can only do the SFINAE stuff
 *              when TRAITS EQUALS the default.
 */

namespace Stroika::Foundation::Containers {
    template <typename KEY_TYPE, typename VALUE_TYPE>
    class Association;
}

namespace Stroika::Foundation::Containers::Factory {

    /**
     *  \brief   Singleton factory object - Used to create the default backend implementation of a Association<> container
     *
     *  Note - you can override the underlying factory dynamically by calling Association_Factory<T,TRAITS>::Register (), or
     *  replace it statically by template-specializing Association_Factory<T,TRAITS>::New () - though the later is trickier.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  \note   Association_Factory<K,P> makes up its own default comparer, and so can use order Associations, like Association_stdmap, whereas
     *          Association_Factory<K,P,E> - since it takes an equals comparer - is restricted to backends that work with an equals comparere.
     */
    template <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
    class Association_Factory {
    private:
        static inline atomic<Association<KEY_TYPE, VALUE_TYPE> (*) (const KEY_EQUALS_COMPARER&)> sFactory_{nullptr};

    public:
        static_assert (not is_reference_v<KEY_TYPE> and not is_reference_v<VALUE_TYPE> and not is_reference_v<KEY_EQUALS_COMPARER>, "typically if this fails its because a (possibly indirect) caller forgot to use forward<TTT>(), or remove_cvref_t");
        static_assert (Common::IsEqualsComparer<KEY_EQUALS_COMPARER> (), "Equals comparer required with Association_Factory");

    public:
        Association_Factory (const KEY_EQUALS_COMPARER& equalsComparer = {});

    public:
        /**
         *  You can call this directly, but there is no need, as the Association<T,TRAITS> CTOR does so automatically.
         */
        nonvirtual Association<KEY_TYPE, VALUE_TYPE> operator() () const;

    public:
        /**
         *  Register a replacement creator/factory for the given Association<KEY_TYPE, VALUE_TYPE,TRAITS>. Note this is a global change.
         */
        static void Register (Association<KEY_TYPE, VALUE_TYPE> (*factory) (const KEY_EQUALS_COMPARER&) = nullptr);

    private:
        [[NO_UNIQUE_ADDRESS_ATTR]] const KEY_EQUALS_COMPARER fKeyEqualsComparer_;

    private:
        static Association<KEY_TYPE, VALUE_TYPE> Default_ (const KEY_EQUALS_COMPARER&);
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "Association_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Association_Factory_h_ */
