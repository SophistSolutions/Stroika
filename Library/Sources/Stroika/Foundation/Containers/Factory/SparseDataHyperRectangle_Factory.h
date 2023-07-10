/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SparseDataHyperRectangle_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_SparseDataHyperRectangle_Factory_h_

#include "../../StroikaPreComp.h"

/**
 *  \file
 */

namespace Stroika::Foundation::Containers {
    template <typename T, typename... INDEXES>
    class SparseDataHyperRectangle;
}

namespace Stroika::Foundation::Containers::Factory {

    /**
     *  \brief   Singleton factory object - Used to create the default backend implementation of a SparseDataHyperRectangle<> container; typically not called directly
     *
     *  Note - you can override the underlying factory dynamically by calling SparseDataHyperRectangle_Factory<T>::Register ().
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T, typename... INDEXES>
    class SparseDataHyperRectangle_Factory {
    public:
        /**
         *  The type of object produced by the factory.
         */
        using ConstructedType = SparseDataHyperRectangle<T, INDEXES...>;

    public:
        /**
         *  Function type to create an ConstructedType object.
         */
        using FactoryFunctionType = function<ConstructedType (Configuration::ArgByValueType<T> defaultItem)>;

    public:
        /**
         */
        struct Hints {};

    public:
        /**
         *  Construct a factory for producing new ConstructedType objects. The default is to use whatever was registered with 
         *  SparseDataHyperRectangle_Factory::Register (), but a specific factory can easily be constructed with provided arguments.
         */
        constexpr SparseDataHyperRectangle_Factory ();
        constexpr SparseDataHyperRectangle_Factory (const Hints& hints);
        constexpr SparseDataHyperRectangle_Factory (const FactoryFunctionType& f);
        constexpr SparseDataHyperRectangle_Factory (const SparseDataHyperRectangle_Factory&) = default;

    public:
        /**
         *  This can be called anytime, before main(), or after. BUT - beware, any calls to Register must
         *  be externally synchronized, meaning effectively that they must happen before the creation of any
         *  threads, to be safe. Also note, since this returns a const reference, any calls to Register() after
         *  a call to Default, even if synchronized, is suspect.
         */
        static const SparseDataHyperRectangle_Factory& Default ();

    public:
        /**
         *  You can call this directly, but there is no need, as the SparseDataHyperRectangle<T, INDEXES...> CTOR does so automatically.
         */
        nonvirtual ConstructedType operator() (Configuration::ArgByValueType<T> defaultItem = {}) const;

    public:
        /**
         *  Register a default global factory for ConstructedType objects (of the templated type/parameters).
         *  No need to call, typically, as the default factory is generally fine.
         * 
         *  \par Example Usage
         *      \code
         *          SparseDataHyperRectangle_Factory::Register(SparseDataHyperRectangle_Factory{SparseDataHyperRectangle_Factory::Hints{.fOptimizeForLookupSpeedOverUpdateSpeed=true});
         *          SparseDataHyperRectangle_Factory::Register();    // or use defaults
         *      \endcode
         *
         *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
         *          BUT - special note/restriction - must be called before any threads call Association_Factory::SparseDataHyperRectangle_Factory() OR
         *          SparseDataHyperRectangle_Factory::Default(), which effectively means must be called at the start of main, but before creating any threads
         *          which might use the factory).
         * 
         *  \NOTE this differs markedly from Stroika 2.1, where Register could be called anytime, and was internally synchronized.
         * 
         *  \note If you wanted a dynamically chanegable factory (change after main), you could write one yourself with its own internal syncrhonization,
         *        set the global one here, then perform the changes to its internal structure through another API.
         */
        static void Register (const optional<SparseDataHyperRectangle_Factory>& f = nullopt);

    private:
        FactoryFunctionType fFactory_;

    private:
        // function to assure magically constructed even if called before main
        static SparseDataHyperRectangle_Factory& AccessDefault_ ();
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "SparseDataHyperRectangle_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SparseDataHyperRectangle_Factory_h_ */
