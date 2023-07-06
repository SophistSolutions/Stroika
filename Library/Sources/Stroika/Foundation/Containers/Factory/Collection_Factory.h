/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Collection_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_Collection_Factory_h_

#include "../../StroikaPreComp.h"

/**
 *  \file
 */

namespace Stroika::Foundation::Containers {
    template <typename T>
    class Collection;
}

namespace Stroika::Foundation::Containers::Factory {

    /**
     *  \brief   Singleton factory object - Used to create the default backend implementation of a Collection<> container; typically not called directly
     *
     *  Note - you can override the underlying factory dynamically by calling Collection_Factory<T>::Register ().
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class Collection_Factory {
    public:
        static_assert (not is_reference_v<T>,
                       "typically if this fails its because a (possibly indirect) caller forgot to use forward<TTT>(), or remove_cvref_t");

    public:
        /**
         *  The type of object produced by the factory.
         */
        using ConstructedType = Collection<T>;

    public:
        /**
         *  Function type to create an ConstructedType object.
         */
        using FactoryFunctionType = function<ConstructedType ()>;

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
         *  Collection_Factory::Register (), but a specific factory can easily be constructed with provided arguments.
         */
        constexpr Collection_Factory ();
        constexpr Collection_Factory (const Hints& hints);
        constexpr Collection_Factory (const FactoryFunctionType& f);
        constexpr Collection_Factory (const Collection_Factory&) = default;

    public:
        /**
         *  This can be called anytime, before main(), or after. BUT - beware, any calls to Register must
         *  be externally synchronized, meaning effectively that they must happen before the creation of any
         *  threads, to be safe. Also note, since this returns a const reference, any calls to Register() after
         *  a call to Default, even if synchronized, is suspect.
         */
        static const Collection_Factory& Default ();

    public:
        /**
         *  You can call this directly, but there is no need, as the Collection<T> CTOR does so automatically.
         */
        nonvirtual ConstructedType operator() () const;

    public:
        /**
         *  Register a default global factory for ConstructedType objects (of the templated type/parameters).
         *  No need to call, typically, as the default factory is generally fine.
         * 
         *  \par Example Usage
         *      \code
         *          Collection_Factory::Register(Collection_Factory{Collection_Factory::Hints{.fOptimizeForLookupSpeedOverUpdateSpeed=true});
         *          Collection_Factory::Register();    // or use defaults
         *      \endcode
         *
         *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
         *          BUT - special note/restriction - must be called before any threads call Association_Factory::Collection_Factory() OR
         *          Collection_Factory::Default(), which effectively means must be called at the start of main, but before creating any threads
         *          which might use the factory).
         * 
         *  \NOTE this differs markedly from Stroika 2.1, where Register could be called anytime, and was internally synchronized.
         * 
         *  \note If you wanted a dynamically chanegable factory (change after main), you could write one yourself with its own internal syncrhonization,
         *        set the global one here, then perform the changes to its internal structure through another API.
         */
        static void Register (const optional<Collection_Factory>& f = nullopt);

    private:
        const FactoryFunctionType fFactory_;
        const bool                fHints_OptimizeForLookupSpeedOverUpdateSpeed{true};

    private:
        // function to assure magically constructed even if called before main
        static Collection_Factory& AccessDefault_ ();
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "Collection_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Collection_Factory_h_ */
