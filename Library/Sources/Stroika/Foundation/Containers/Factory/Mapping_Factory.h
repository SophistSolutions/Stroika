/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_Mapping_Factory_h_

#include "../../StroikaPreComp.h"

#include <type_traits>

/**
 *  \file
 */

namespace Stroika::Foundation::Containers {
    template <typename KEY_TYPE, typename VALUE_TYPE>
    class Mapping;
}

namespace Stroika::Foundation::Containers::Factory {

    /**
     *  \brief   Singleton factory object - Used to create the default backend implementation of a Mapping<> container; typically not called directly
     *
     *  Note - you can override the underlying factory dynamically by calling Mapping_Factory<T,TRAITS>::Register ().
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  \note   Mapping_Factory<K,P> makes up its own default comparer, and so can use order mappings, like Mapping_stdmap, whereas
     *          Mapping_Factory<K,P,E> - since it takes an equals comparer - is restricted to backends that work with an equals comparere.
     */
    template <typename KEY_TYPE, typename VALUE_TYPE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
    class Mapping_Factory {
    private:
        static inline atomic<Mapping<KEY_TYPE, VALUE_TYPE> (*) (const KEY_EQUALS_COMPARER&)> sFactory_{nullptr};

    public:
        static_assert (not is_reference_v<KEY_TYPE> and not is_reference_v<VALUE_TYPE> and not is_reference_v<KEY_EQUALS_COMPARER>,
                       "typically if this fails its because a (possibly indirect) caller forgot to use forward<TTT>(), or remove_cvref_t");

    public:
        /**
         *  The type of object produced by the factory.
         */
        using ConstructedType = Mapping<KEY_TYPE, VALUE_TYPE>;

    public:
        /**
         *  Function type to create an Mapping object.
         */
        using FactoryFunctionType = function<ConstructedType (const KEY_EQUALS_COMPARER& keyEqualsComparer)>;

    public:
        /**
         *  Hints can be used in factory constructor to guide the choice of the best container implementation/backend.
         */
        struct Hints {
            optional<bool> fOptimizeForLookupSpeedOverUpdateSpeed;
        };

    public:
        /**
         *  Construct a factory for producing new ConstructedType ivhects. The default is to use whatever was registered with 
         *  Mapping_Factory::Register (), but a specific factory can easily be constructed with provided arguments.
         */
        constexpr Mapping_Factory ();
        constexpr Mapping_Factory (const Hints& hints);
        constexpr Mapping_Factory (const FactoryFunctionType& f);
        constexpr Mapping_Factory (const Mapping_Factory&) = default;

    public:
        /**
         *  This can be called anytime, before main(), or after. BUT - beware, any calls to Register must
         *  be externally synchronized, meaning effectively that they must happen before the creation of any
         *  threads, to be safe. Also note, since this returns a const reference, any calls to Register() after
         *  a call to Default, even if synchronized, is suspect.
         */
        static const Mapping_Factory& Default ();

    public:
        /**
         *  You can call this directly, but there is no need, as the Mapping<T> CTOR does so automatically.
         */
        nonvirtual ConstructedType operator() (const KEY_EQUALS_COMPARER& keyEqualsComparer) const;

    public:
        /**
         *  Register a default global factory for Mapping objects (of the templated type/parameters).
         *  No need to call, typically, as the default factory is generally fine.
         * 
         *  \par Example Usage
         *      \code
         *          Mapping_Factory::Register(Mapping_Factory{Mapping_Factory::Hints{.fOptimizeForLookupSpeedOverUpdateSpeed=true});
         *          Mapping_Factory::Register();    // or use defaults
         *      \endcode
         *
         *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
         *          BUT - special note/restriction - must be called before any threads call Association_Factory::Mapping_Factory() OR
         *          Mapping_Factory::Default(), which effectively means must be called at the start of main, but before creating any threads
         *          which might use the factory).
         * 
         *  \NOTE this differs markedly from Stroika 2.1, where Register could be called anytime, and was internally synchronized.
         * 
         *  \note If you wanted a dynamically chanegable factory (change after main), you could write one yourself with its own internal syncrhonization,
         *        set the global one here, then perform the changes to its internal structure through another API.
         */
        static void Register (const optional<Mapping_Factory>& f = nullopt);

    private:
        const FactoryFunctionType fFactory_;
        const bool                fHints_OptimizeForLookupSpeedOverUpdateSpeed{true};

    private:
        // function to assure magically constructed even if called before main
        static Mapping_Factory& AccessDefault_ ();
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "Mapping_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Mapping_Factory_h_ */
