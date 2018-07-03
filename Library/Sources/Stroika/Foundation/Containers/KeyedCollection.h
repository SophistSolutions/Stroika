/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_KeyedCollection_h_
#define _Stroika_Foundation_Containers_KeyedCollection_h_ 1

#include "../StroikaPreComp.h"

#include "../Common/Compare.h"
#include "../Common/KeyValuePair.h"
#include "../Configuration/Common.h"
#include "../Configuration/Concepts.h"
#include "../Execution/Synchronized.h"
#include "../Traversal/Iterable.h"

#include "Collection.h"

/*
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a> --- NOFUNCTIONAL DRAFT
 *
 *  TODO:
 *      @todo   https://stroika.atlassian.net/browse/STK-133 - add regression tests for KeyedCollection/SortedKeyedCollection
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-117 KeyedCollection needs get extractor method either const or static based on type
 *
 *              Add DEFUALT_EXTRACTOR to traits (optional arg to traits type, or new traits type)
 *              and mmove arg for extractor to END so it can be defualted or overloaded.
 *
 *              UNCLEAR if this is what we want or always parameter. Not clear how to provide both
 *              sensibly.
 *
 *              Probably the answer is some SFINAE "ISDEFINED(TRAITS::EXTRACTOR)" then have one set of constructors
 *              and otherwise another set.
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            using Configuration::ArgByValueType;
            using Traversal::Iterable;
            using Traversal::Iterator;

            using Common::KeyValuePair;

            /**
             */
            template <typename KEY_TYPE, typename T, typename KEY_EQUALS_COMPARER = std::equal_to<KEY_TYPE>>
            struct KeyedCollection_DefaultTraits {
                /**
                 */
                using KeyEqualsCompareFunctionType = KEY_EQUALS_COMPARER;

                /**
                 *  Define typedef for this KeyedCollection traits object (so other traits can generically allow recovery of the
                 *  underlying KeyedCollection's TRAITS objects.
                 */
                using KeyedCollectionTraitsType = KeyedCollection_DefaultTraits<KEY_TYPE, T, KEY_EQUALS_COMPARER>;

                /**
                 */
                using KeyExtractorFunctionType = function<KEY_TYPE (T)>;

                using KeyExtractor = void; // so not 'isfunction'
            };

            /**
             */
            template <typename KEY_TYPE, typename T, typename EXTRACTOR, typename KEY_EQUALS_COMPARER = std::equal_to<KEY_TYPE>>
            struct KeyedCollection_DefaultTraitsWithExtractor : KeyedCollection_DefaultTraits<KEY_TYPE, T, KEY_EQUALS_COMPARER> {
                /**
                 */
                using KeyExtractor = EXTRACTOR;
            };

            /**
             *      KeyedCollection adds most access patterns used in Mapping to a Collection, but stores only a single
             *      object. It takes a parameter (??? TBD but including constructor based) to extract the KEY from
             *      the collection item value type.
             *
             *  \note   Not based on, but useful to refer to .Net KeyedColleciton<KEY,T>
             *          @see https://msdn.microsoft.com/en-us/library/ms132438%28v=vs.110%29.aspx?f=255&MSPPError=-2147217396
             *
             *  \note   Important that KeyedCollection<>::Add () will REPLACE the value, unlike with set.
             *
             *  \note Note About Iterators
             *      o   Stroika container iterators must have shorter lifetime than the container they are iterating over.
             *
             *      o   Stroika container iterators are all automatically patched, so that if you change the underlying container
             *          the iterators are automatically updated internally to behave sensibly.
             *
             */
            template <typename KEY_TYPE, typename T, typename TRAITS = KeyedCollection_DefaultTraits<KEY_TYPE, T>>
            class KeyedCollection : public Collection<T> {
            private:
                using inherited = Collection<T>;

            protected:
                class _IRep;

            protected:
                using _KeyedCollectionRepSharedPtr = typename inherited::template SharedPtrImplementationTemplate<_IRep>;

            public:
                /**
                 *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
                 */
                using ArchetypeContainerType = KeyedCollection<KEY_TYPE, T, TRAITS>;

            public:
                /**
                 *  Just a short-hand for the 'TRAITS' part of KeyedCollection<T,TRAITS>. This is often handy to use in
                 *  building other templates.
                 */
                using TraitsType = TRAITS;

            public:
                /**
                 */
                using KeyType = KEY_TYPE;

            public:
                /**
                 */
                using value_type = typename inherited::value_type;

            public:
                /**
                 *  Just a short-hand for the WellOrderCompareFunctionType specified through traits. This is often handy to use in
                 *  building other templates.
                 */
                using WellOrderCompareFunctionType = typename TraitsType::WellOrderCompareFunctionType;

            public:
                /**
                 */
                using KeyExtractorFunctionType = typename TraitsType::KeyExtractorFunctionType;

            public:
                /**
                 *  All constructors must agree about what extractor to use: either one from the traits
                 *  or one provided in all constructors (that dont get it indirectly from another place).
                 */
                static constexpr bool kConstructorsHaveExtractorArgument = is_function<typename TraitsType::KeyExtractor>::value;

            public:
                /**
                 *  All constructors must agree about what extractor to use: either one from the traits
                 *  or one provided in all constructors (that dont get it indirectly from another place).
                 *
                 *  @todo use kConstructorsHaveExtractorArgument in SFINAE_ENABLE_IF_HAS_KEY_EXTRACTOR but harder with MSVC2k13
                 */
                template <typename SFINAE_ENABLE_IF_HAS_KEY_EXTRACTOR = typename enable_if<is_function<typename TraitsType::KeyExtractor>::value>::type>
                KeyedCollection ();
                template <typename SFINAE_ENABLE_IF_HAS_KEY_EXTRACTOR = typename enable_if<not is_function<typename TraitsType::KeyExtractor>::value>::type>
                KeyedCollection (KeyExtractorFunctionType keyExtractor);
                KeyedCollection (const KeyedCollection<KEY_TYPE, T, TRAITS>& src);
                KeyedCollection (KeyedCollection<KEY_TYPE, T, TRAITS>&& src);
                template <typename SFINAE_ENABLE_IF_HAS_KEY_EXTRACTOR = typename enable_if<is_function<typename TraitsType::KeyExtractor>::value>::type>
                KeyedCollection (const std::initializer_list<T>& src);
                template <typename SFINAE_ENABLE_IF_HAS_KEY_EXTRACTOR = typename enable_if<not is_function<typename TraitsType::KeyExtractor>::value>::type>
                KeyedCollection (const std::initializer_list<T>& src, KeyExtractorFunctionType keyExtractor);
                template <typename CONTAINER_OF_ADDABLE, typename ENABLE_IF = typename enable_if<Configuration::has_beginend<CONTAINER_OF_ADDABLE>::value && !std::is_convertible<const CONTAINER_OF_ADDABLE*, const KeyedCollection<KEY_TYPE, T, TRAITS>*>::value>::type>
                explicit KeyedCollection (const CONTAINER_OF_ADDABLE& src, KeyExtractorFunctionType keyExtractor);
                template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
                KeyedCollection (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end, KeyExtractorFunctionType keyExtractor);

            protected:
                explicit KeyedCollection (const _KeyedCollectionRepSharedPtr& src);
                explicit KeyedCollection (_KeyedCollectionRepSharedPtr&& src);

            public:
                /**
                 */
                nonvirtual KeyedCollection<KEY_TYPE, T, TRAITS>& operator= (const KeyedCollection<KEY_TYPE, T, TRAITS>& rhs);
                nonvirtual KeyedCollection<KEY_TYPE, T, TRAITS>& operator= (KeyedCollection<KEY_TYPE, T, TRAITS>&& rhs) = default;

            public:
                /**
                 *  Keys () returns an Iterable object with just the key part of the Mapping.
                 *
                 *  Note this method may not return a collection which is sorted. Note also, the
                 *  returned value is a copy of the keys (by value) - at least logically (implementations
                 *  maybe smart enough to use lazy copying).
                 *
                 *  \em Design Note:
                 *      The analagous method in C#.net - Dictionary<TKey, TValue>.KeyCollection
                 *      (http://msdn.microsoft.com/en-us/library/yt2fy5zk(v=vs.110).aspx) returns a live reference
                 *      to the underlying keys. We could have (fairly easily) done that, but I didn't see the point.
                 *
                 *      In .net, the typical model is that you have a pointer to an object, and pass around that
                 *      pointer (so by reference semantics) - so this returning a live reference makes more sense there.
                 *
                 *      Since Stroika containers are logically copy-by-value (even though lazy-copied), it made more
                 *      sense to apply that lazy-copy (copy-on-write) paradigm here, and make the returned set of
                 *      keys a logical copy at the point 'keys' is called.
                 *
                 *  See:
                 *      @see Values ()
                 */
                nonvirtual Iterable<KeyType> Keys () const;

            public:
                /**
                 *  Note - as since Lookup/1 returns an optional<T> - it can be used very easily to provide
                 *  a default value on Lookup (so for case where not present) - as in:
                 *      returm m.Lookup (key).Value (putDefaultValueHere);
                 *
                 *  Note - for both overloads taking an item pointer, the pointer may be nullptr (in which case not assigned to).
                 *  But if present, will always be assigned to if Lookup returns true (found). And for the optional overload
                 *      \req    Ensure (item == nullptr or returnValue == item->has_value());
                 *
                 *  \note   Alias - Lookup (key, value_type* value) - is equivilent to .Net TryGetValue ()
                 */
                nonvirtual optional<value_type> Lookup (ArgByValueType<KeyType> key) const;
                nonvirtual bool                 Lookup (ArgByValueType<KeyType> key, optional<value_type>* item) const;
                nonvirtual bool                 Lookup (ArgByValueType<KeyType> key, value_type* item) const;
                nonvirtual bool                 Lookup (ArgByValueType<KeyType> key, nullptr_t) const;

            public:
                /**
                 *  Always safe to call. If result of Lookup () '!has_value', returns argument 'default' or 'sentinal' value.
                 */
                nonvirtual value_type LookupValue (ArgByValueType<KeyType> key, ArgByValueType<value_type> defaultValue = value_type{}) const;

            public:
                /**
                 *  \req ContainsKey (key);
                 */
                nonvirtual value_type operator[] (ArgByValueType<KeyType> key) const;

            public:
                /**
                 *  Synonym for Lookup (key).has_value ()
                 */
                nonvirtual bool ContainsKey (ArgByValueType<KeyType> key) const;

            public:
                /**
                 * \brief Compares items with TRAITS::EqualsCompareFunctionType::Equals, and returns true if any match.
                 */
                template <typename EQUALS_COMPARER = std::equal_to<T>>
                nonvirtual bool Contains (T item) const;
                nonvirtual bool Contains (ArgByValueType<KeyType> item) const;

            public:
                /**
                 * It is legal to remove something that is not there. This function removes the first instance of item
                 * (or each item for the 'items' overload), meaning that another instance of item could still be in the
                 * Collection<T> after the remove. Thus function just reduces the MultiSet() by one (or zero if item wasn't found).
                 *
                 * SECOND OVERLOAD:
                 * This function requires that the iterator 'i' came from this container.
                 *
                 * The value pointed to by 'i' is removed.
                 */
                template <typename EQUALS_COMPARER = std::equal_to<T>>
                nonvirtual void Remove (T item);
                nonvirtual void Remove (const Iterator<T>& i);
                nonvirtual void Remove (ArgByValueType<KeyType> item);

            protected:
                nonvirtual void _AssertRepValidType () const;
            };

            /**
             *  \brief  Implementation detail for KeyedCollection<KEY_TYPE, T, TRAITS> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the KeyedCollection<KEY_TYPE, T, TRAITS> container API.
             */
            template <typename KEY_TYPE, typename T, typename TRAITS>
            class KeyedCollection<KEY_TYPE, T, TRAITS>::_IRep : public Collection<T>::_IRep {
            public:
                virtual Iterable<KEY_TYPE> Keys () const = 0;
                // always clear/set item, and ensure return value == item->IsValidItem());
                // 'item' arg CAN be nullptr
                virtual bool Lookup (ArgByValueType<KeyType> key, optional<value_type>* item) const = 0;
                virtual void Remove (ArgByValueType<KEY_TYPE> key)                                  = 0;
            };
        }
    }
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "KeyedCollection.inl"

#endif /*_Stroika_Foundation_Containers_KeyedCollection_h_ */
