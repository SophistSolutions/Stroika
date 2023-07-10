/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_MultiSet_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_MultiSet_Factory_h_

#include "../../StroikaPreComp.h"

/**
 *  \file
 */

namespace Stroika::Foundation::Containers {
    template <typename T, typename TRAITS>
    class MultiSet;
}

namespace Stroika::Foundation::Containers::Factory {

    /**
     *  \brief   Singleton factory object - Used to create the default backend implementation of a MultiSet<> container; typically not called directly
     *
     *  Note - you can override the underlying factory dynamically by calling MultiSet_Factory<T,TRAITS>::Register ().
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T, typename TRAITS, IEqualsComparer<T> EQUALS_COMPARER = equal_to<T>>
    class MultiSet_Factory {
    public:
        static_assert (not is_reference_v<T> and not is_reference_v<EQUALS_COMPARER>,
                       "typically if this fails its because a (possibly indirect) caller forgot to use forward<>(), or remove_cvref_t");

    public:
        /**
         *  The type of object produced by the factory.
         */
        using ConstructedType = MultiSet<T, TRAITS>;

    public:
        /**
         *  Function type to create an ConstructedType object.
         */
        using FactoryFunctionType = function<ConstructedType (const EQUALS_COMPARER& equalsComparer)>;

    public:
        /**
         *  Hints can be used in factory constructor to guide the choice of the best container implementation/backend.
         */
        struct Hints {
            optional<bool> fOptimizeForLookupSpeedOverUpdateSpeed;
        };

    public:
        /**
         *  Construct a factory for producing new ConstructedType objects. The default is to use whatever was registered with 
         *  MultiSet_Factory::Register (), but a specific factory can easily be constructed with provided arguments.
         */
        constexpr MultiSet_Factory ();
        constexpr MultiSet_Factory (const Hints& hints);
        constexpr MultiSet_Factory (const FactoryFunctionType& f);
        constexpr MultiSet_Factory (const MultiSet_Factory&) = default;

    public:
        /**
         *  This can be called anytime, before main(), or after. BUT - beware, any calls to Register must
         *  be externally synchronized, meaning effectively that they must happen before the creation of any
         *  threads, to be safe. Also note, since this returns a const reference, any calls to Register() after
         *  a call to Default, even if synchronized, is suspect.
         */
        static const MultiSet_Factory& Default ();

    public:
        /**
         *  You can call this directly, but there is no need, as the MultiSet<T,TRAITS> CTOR does so automatically.
         */
        nonvirtual ConstructedType operator() (const EQUALS_COMPARER& equalsComparer = {}) const;

    public:
        /**
         *  Register a default global factory for ConstructedType objects (of the templated type/parameters).
         *  No need to call, typically, as the default factory is generally fine.
         * 
         *  \par Example Usage
         *      \code
         *          MultiSet_Factory::Register(MultiSet_Factory{MultiSet_Factory::Hints{.fOptimizeForLookupSpeedOverUpdateSpeed=true});
         *          MultiSet_Factory::Register();    // or use defaults
         *      \endcode
         *
         *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
         *          BUT - special note/restriction - must be called before any threads call Association_Factory::MultiSet_Factory() OR
         *          MultiSet_Factory::Default(), which effectively means must be called at the start of main, but before creating any threads
         *          which might use the factory).
         * 
         *  \NOTE this differs markedly from Stroika 2.1, where Register could be called anytime, and was internally synchronized.
         * 
         *  \note If you wanted a dynamically chanegable factory (change after main), you could write one yourself with its own internal syncrhonization,
         *        set the global one here, then perform the changes to its internal structure through another API.
         */
        static void Register (const optional<MultiSet_Factory>& f = nullopt);

    private:
        FactoryFunctionType fFactory_;

    private:
        // function to assure magically constructed even if called before main
        static MultiSet_Factory& AccessDefault_ ();
    };

}

/*
 ********************************************************************************
 **************************** Implementation Details ****************************
 ********************************************************************************
 */
#include "MultiSet_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_MultiSet_Factory_h_ */
