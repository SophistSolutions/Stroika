/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_KeyedCollection_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_KeyedCollection_Factory_h_

#include "../../StroikaPreComp.h"

#include <type_traits>

/**
 *  \file
 */

namespace Stroika::Foundation::Containers {
    template <typename T, typename KEY_TYPE, typename TRAITS>
    class KeyedCollection;
}

namespace Stroika::Foundation::Containers::Factory {

    /**
     *  \brief   Singleton factory object - Used to create the default backend implementation of a KeyedCollection<> container; typically not called directly
     *
     *  Note - you can override the underlying factory dynamically by calling KeyedCollection_Factory<T,KEY_TYPE,TRAITS>::Register ().
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T, typename KEY_TYPE, typename TRAITS, typename KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
    class KeyedCollection_Factory {
    public:
        static_assert (not is_reference_v<T> and not is_reference_v<KEY_TYPE> and not is_reference_v<KEY_EQUALS_COMPARER>,
                       "typically if this fails its because a (possibly indirect) caller forgot to use forward<TTT>(), or remove_cvref_t");

    public:
        /**
         *  The type of object produced by the factory.
         */
        using ConstructedType = KeyedCollection<T, KEY_TYPE, TRAITS>;

    public:
        using KeyExtractorType = typename ConstructedType::KeyExtractorType;

    public:
        /**
         *  Function type to create an ConstructedType object.
         */
        using FactoryFunctionType = function<ConstructedType (const KeyExtractorType& keyExtractor, const KEY_EQUALS_COMPARER& keyComparer)>;

    public:
        /**
         *  Hints can be used in factory constructor to guide the choice of the best container implementation/backend.
         */
        struct Hints {
            /**
             */
            optional<bool> fOptimizeForLookupSpeedOverUpdateSpeed;
        };

    public:
        /**
         *  Construct a factory for producing new ConstructedType objects. The default is to use whatever was registered with 
         *  KeyedCollection_Factory::Register (), but a specific factory can easily be constructed with provided arguments.
         */
        constexpr KeyedCollection_Factory ();
        constexpr KeyedCollection_Factory (const Hints& hints);
        constexpr KeyedCollection_Factory (const FactoryFunctionType& f);
        constexpr KeyedCollection_Factory (const KeyedCollection_Factory&) = default;

    public:
        /**
         *  This can be called anytime, before main(), or after. BUT - beware, any calls to Register must
         *  be externally synchronized, meaning effectively that they must happen before the creation of any
         *  threads, to be safe. Also note, since this returns a const reference, any calls to Register() after
         *  a call to Default, even if synchronized, is suspect.
         */
        static const KeyedCollection_Factory& Default ();

    public:
        /**
         *  You can call this directly, but there is no need, as the KeyedCollection<T,TRAITS> CTOR does so automatically.
         */
        nonvirtual ConstructedType operator() (const KeyExtractorType& keyExtractor, const KEY_EQUALS_COMPARER& keyComparer) const;

    public:
        /**
         *  Register a default global factory for ConstructedType objects (of the templated type/parameters).
         *  No need to call, typically, as the default factory is generally fine.
         * 
         *  \par Example Usage
         *      \code
         *          KeyedCollection_Factory::Register(KeyedCollection_Factory{KeyedCollection_Factory::Hints{.fOptimizeForLookupSpeedOverUpdateSpeed=true});
         *          KeyedCollection_Factory::Register();    // or use defaults
         *      \endcode
         *
         *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
         *          BUT - special note/restriction - must be called before any threads call Association_Factory::KeyedCollection_Factory() OR
         *          KeyedCollection_Factory::Default(), which effectively means must be called at the start of main, but before creating any threads
         *          which might use the factory).
         * 
         *  \NOTE this differs markedly from Stroika 2.1, where Register could be called anytime, and was internally synchronized.
         * 
         *  \note If you wanted a dynamically chanegable factory (change after main), you could write one yourself with its own internal syncrhonization,
         *        set the global one here, then perform the changes to its internal structure through another API.
         */
        static void Register (const optional<KeyedCollection_Factory>& f = nullopt);

    private:
        const FactoryFunctionType fFactory_;

    private:
        // function to assure magically constructed even if called before main
        static KeyedCollection_Factory& AccessDefault_ ();
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "KeyedCollection_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_KeyedCollection_Factory_h_ */
