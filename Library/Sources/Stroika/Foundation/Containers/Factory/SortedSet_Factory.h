/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedSet_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_SortedSet_Factory_h_

#include "../../StroikaPreComp.h"

/**
 *  \file
 */

namespace Stroika::Foundation::Containers {
    template <typename T>
    class SortedSet;
}

namespace Stroika::Foundation::Containers::Factory {

    /**
     *  \brief   Singleton factory object - Used to create the default backend implementation of a SortedSet<> container; typically not called directly
     *
     *  Note - you can override the underlying factory dynamically by calling SortedSet_Factory<T,INORDER_COMPARER>::Register ().
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T, typename INORDER_COMPARER = less<T>>
    class SortedSet_Factory {
    public:
        static_assert (not is_reference_v<T> and not is_reference_v<INORDER_COMPARER>,
                       "typically if this fails its because a (possibly indirect) caller forgot to use forward<TTT>(), or remove_cvref_t");
        static_assert (Common::IsStrictInOrderComparer<INORDER_COMPARER> (), "StrictInOrder comparer required with SortedSet");

    public:
        /**
         *  The type of object produced by the factory.
         */
        using ConstructedType = SortedSet<T>;

    public:
        /**
         *  Function type to create an ConstructedType object.
         */
        using FactoryFunctionType = function<ConstructedType (const INORDER_COMPARER& inorderComparer)>;

    public:
        /**
         *  Hints can be used in factory constructor to guide the choice of the best container implementation/backend.
         */
        struct Hints {};

    public:
        /**
         *  Construct a factory for producing new ConstructedType objects. The default is to use whatever was registered with 
         *  SortedSet_Factory::Register (), but a specific factory can easily be constructed with provided arguments.
         */
        constexpr SortedSet_Factory ();
        constexpr SortedSet_Factory (const Hints& hints);
        constexpr SortedSet_Factory (const FactoryFunctionType& f);
        constexpr SortedSet_Factory (const SortedSet_Factory&) = default;

    public:
        /**
         *  This can be called anytime, before main(), or after. BUT - beware, any calls to Register must
         *  be externally synchronized, meaning effectively that they must happen before the creation of any
         *  threads, to be safe. Also note, since this returns a const reference, any calls to Register() after
         *  a call to Default, even if synchronized, is suspect.
         */
        static const SortedSet_Factory& Default ();

    public:
        /**
         *  You can call this directly, but there is no need, as the SortedSet<T,TRAITS> CTOR does so automatically.
         */
        nonvirtual ConstructedType operator() (const INORDER_COMPARER& inorderComparer = {}) const;

    public:
        /**
         *  Register a default global factory for ConstructedType objects (of the templated type/parameters).
         *  No need to call, typically, as the default factory is generally fine.
         * 
         *  \par Example Usage
         *      \code
         *          SortedSet_Factory::Register(SortedSet_Factory{SortedSet_Factory::Hints{.fOptimizeForLookupSpeedOverUpdateSpeed=true});
         *          SortedSet_Factory::Register();    // or use defaults
         *      \endcode
         *
         *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
         *          BUT - special note/restriction - must be called before any threads call Association_Factory::SortedSet_Factory() OR
         *          SortedSet_Factory::Default(), which effectively means must be called at the start of main, but before creating any threads
         *          which might use the factory).
         * 
         *  \NOTE this differs markedly from Stroika 2.1, where Register could be called anytime, and was internally synchronized.
         * 
         *  \note If you wanted a dynamically chanegable factory (change after main), you could write one yourself with its own internal syncrhonization,
         *        set the global one here, then perform the changes to its internal structure through another API.
         */
        static void Register (const optional<SortedSet_Factory>& f = nullopt);

    private:
        FactoryFunctionType fFactory_;

    private:
        // function to assure magically constructed even if called before main
        static SortedSet_Factory& AccessDefault_ ();
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "SortedSet_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedSet_Factory_h_ */
