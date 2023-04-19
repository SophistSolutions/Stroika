/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_DenseDataHyperRectangle_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_DenseDataHyperRectangle_Factory_h_

#include "../../StroikaPreComp.h"

#include <atomic>

/**
 *  \file
 */

namespace Stroika::Foundation::Containers {
    template <typename T, typename... INDEXES>
    class DenseDataHyperRectangle;
}

namespace Stroika::Foundation::Containers::Factory {

    /**
     *  \brief   Singleton factory object - Used to create the default backend implementation of a DenseDataHyperRectangle<> container
     *
     *  Note - you can override the underlying factory dynamically by calling DenseDataHyperRectangle_Factory<T>::Register (), or
     *  replace it statically by template-specializing DenseDataHyperRectangle_Factory<T>::New () - though the later is trickier.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T, typename... INDEXES>
    class DenseDataHyperRectangle_Factory {
    private:
        static inline atomic<DenseDataHyperRectangle<T, INDEXES...> (*) (INDEXES...)> sFactory_{nullptr};

    public:
        /**
         */
        using ConstructedType = DenseDataHyperRectangle<T, INDEXES...>;

    public:
        /**
         *  Function type to create a ConstructedType object.
         */
        using FactoryFunctionType = function<ConstructedType (INDEXES... dimensions)>;

    public:
        /**
         *  Hints can be used in factory constructor to guide the choice of the best container implementation/backend.
         */
        struct Hints {};

    public:
        /**
         *  Construct a factory for producing new Collections. The default is to use whatever was registered with 
         *  Collection_Factory::Register (), but a specific factory can easily be constructed with provided arguments.
         */
        constexpr DenseDataHyperRectangle_Factory ();
        constexpr DenseDataHyperRectangle_Factory (const Hints& hints);
        constexpr DenseDataHyperRectangle_Factory (const FactoryFunctionType& f);
        constexpr DenseDataHyperRectangle_Factory (const DenseDataHyperRectangle_Factory&) = default;

    public:
        /**
         *  This can be called anytime, before main(), or after. BUT - beware, any calls to Register must
         *  be externally synchronized, meaning effectively that they must happen before the creation of any
         *  threads, to be safe. Also note, since this returns a const reference, any calls to Register() after
         *  a call to Default, even if synchronized, is suspect.
         */
        static const DenseDataHyperRectangle_Factory& Default ();

    public:
        /**
         *  You can call this directly, but there is no need, as the Collection<T> CTOR does so automatically.
         */
        nonvirtual ConstructedType operator() (INDEXES... dimensions) const;

    public:
        /**
         *  Register a default global factory for DenseDataHyperRectangle objects (of the templated type/parameters).
         *  No need to call, typically, as the default factory is generally fine.
         * 
         *  \par Example Usage
         *      \code
         *          DenseDataHyperRectangle_Factory::Register(DenseDataHyperRectangle_Factory{DenseDataHyperRectangle_Factory::Hints{.fOptimizeForLookupSpeedOverUpdateSpeed=true});
         *          DenseDataHyperRectangle_Factory::Register();    // or use defaults
         *      \endcode
         *
         *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
         *          BUT - special note/restriction - must be called before any threads call Association_Factory::DenseDataHyperRectangle_Factory() OR
         *          DenseDataHyperRectangle_Factory::Default(), which effectively means must be called at the start of main, but before creating any threads
         *          which might use the factory).
         * 
         *  \NOTE this differs markedly from Stroika 2.1, where Register could be called anytime, and was internally synchronized.
         */
        static void Register (const optional<DenseDataHyperRectangle_Factory>& f = nullopt);

    private:
        FactoryFunctionType fFactory_;

    private:
        // function to assure magically constructed even if called before main
        static DenseDataHyperRectangle_Factory& AccessDefault_ ();
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "DenseDataHyperRectangle_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_DenseDataHyperRectangle_Factory_h_ */
