/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedAssociation_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_SortedAssociation_Factory_h_

#include "../../StroikaPreComp.h"

/**
 *  \file
 */

namespace Stroika::Foundation::Containers {
    template <typename KEY_TYPE, typename VALUE_TYPE>
    class SortedAssociation;
}

namespace Stroika::Foundation::Containers::Factory {

    /**
     *  \brief   Singleton factory object - Used to create the default backend implementation of a SortedAssociation<> container; typically not called directly
     *
     *  Note - you can override the underlying factory dynamically by calling SortedAssociation_Factory<T,TRAITS>::Register ().
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_INORDER_COMPARER = less<KEY_TYPE>>
    class SortedAssociation_Factory {
    public:
        static_assert (not is_reference_v<KEY_TYPE> and not is_reference_v<VALUE_TYPE> and not is_reference_v<KEY_INORDER_COMPARER>,
                       "typically if this fails its because a (possibly indirect) caller forgot to use forward<>(), or remove_cvref_t");
        static_assert (Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER> (), "StrictInOrder comparer required with SortedAssociation");

    public:
        /**
         */
        using ConstructedType = SortedAssociation<KEY_TYPE, VALUE_TYPE>;

    public:
        /**
         *  Function type to create an SortedAssociation object.
         */
        using FactoryFunctionType = function<ConstructedType (const KEY_INORDER_COMPARER& keyInOrderComparer)>;

    public:
        /**
         *  Hints can be used in factory constructor to guide the choice of the best container implementation/backend.
         */
        struct Hints {};

    public:
        /**
         *  Construct a factory for producing new SortedAssociations. The default is to use whatever was registered with 
         *  SortedAssociation_Factory::Register (), but a specific factory can easily be constructed with provided arguments.
         */
        constexpr SortedAssociation_Factory ();
        constexpr SortedAssociation_Factory (const Hints& hints);
        constexpr SortedAssociation_Factory (const FactoryFunctionType& f);
        constexpr SortedAssociation_Factory (const SortedAssociation_Factory&) = default;

    public:
        /**
         *  This can be called anytime, before main(), or after. BUT - beware, any calls to Register must
         *  be externally synchronized, meaning effectively that they must happen before the creation of any
         *  threads, to be safe. Also note, since this returns a const reference, any calls to Register() after
         *  a call to Default, even if synchronized, is suspect.
         */
        static const SortedAssociation_Factory& Default ();

    public:
        /**
         *  You can call this directly, but there is no need, as the Association<T,TRAITS> CTOR does so automatically.
         */
        nonvirtual ConstructedType operator() (const KEY_INORDER_COMPARER& keyInOrderComparer = {}) const;

    public:
        /**
         *  Register a default global factory for SortedAssociation objects (of the templated type/parameters).
         *  No need to call, typically, as the default factory is generally fine.
         * 
         *  \par Example Usage
         *      \code
         *          SortedAssociation_Factory::Register(SortedAssociation_Factory{SortedAssociation_Factory::Hints{.fOptimizeForLookupSpeedOverUpdateSpeed=true});
         *          SortedAssociation_Factory::Register();    // or use defaults
         *      \endcode
         *
         *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
         *          BUT - special note/restriction - must be called before any threads call Association_Factory::SortedAssociation_Factory() OR
         *          SortedAssociation_Factory::Default(), which effectively means must be called at the start of main, but before creating any threads
         *          which might use the factory).
         * 
         *  \NOTE this differs markedly from Stroika 2.1, where Register could be called anytime, and was internally synchronized.
         * 
         *  \note If you wanted a dynamically chanegable factory (change after main), you could write one yourself with its own internal syncrhonization,
         *        set the global one here, then perform the changes to its internal structure through another API.
         */
        static void Register (const optional<SortedAssociation_Factory>& f = nullopt);

    private:
        FactoryFunctionType fFactory_;

    private:
        // function to assure magically constructed even if called before main
        static SortedAssociation_Factory& AccessDefault_ ();
    };
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "SortedAssociation_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedAssociation_Factory_h_ */
