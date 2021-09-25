/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_KeyedCollection_h_
#define _Stroika_Foundation_Containers_KeyedCollection_h_ 1

#include "../StroikaPreComp.h"

#include "../Common/Compare.h"
#include "../Configuration/Common.h"
#include "../Configuration/Concepts.h"
#include "../Execution/Synchronized.h"
#include "../Traversal/Iterable.h"

#include "Collection.h"

/*
 *  \file
 *
 * 
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo   https://stroika.atlassian.net/browse/STK-133 - add regression tests for KeyedCollection/SortedKeyedCollection
 *
 */

namespace Stroika::Foundation::Containers {

    using Configuration::ArgByValueType;
    using Traversal::Iterable;
    using Traversal::Iterator;

    /**
     *  \note its OK if DEFAULT_KEY_EXTRACTOR is invalid, it just means you must specify the default-key-extractor in the 
     *        constructor for KeyedCollection<>
     */
    template <typename T, typename KEY_TYPE, typename DEFAULT_KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>, typename DEFAULT_KEY_EXTRACTOR = void>
    struct KeyedCollection_DefaultTraits {
        /**
         */
        using KeyType = KEY_TYPE;

        /**
         */
        using value_type = T;

        /**
         */
        using KeyExtractorType = function<KEY_TYPE (ArgByValueType<T>)>;

        /**
         *  This is the type returned by GetElementEqualsComparer () and CAN be used as the argument to a KeyedCollection<> as EqualityComparer, but
         *  we allow any template in the KeyedCollection<> CTOR for an equalityComparer that follows the Common::IsEqualsComparer () concept.
         *
         *  \note   @see also EqualsComparer{} to compare whole KeyedCollection<>s
         */
        using KeyEqualityComparerType = Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals, function<bool (ArgByValueType<KEY_TYPE>, ArgByValueType<KEY_TYPE>)>>;

        /**
         *  Default extractor if not specified in constructor (e.g. default-constructor KeyedCollection())
         */
        using DefaultKeyExtractor = DEFAULT_KEY_EXTRACTOR;
        static inline KeyExtractorType mk_ ()
        {
            if constexpr (is_same_v<DEFAULT_KEY_EXTRACTOR, void>) {
                return nullptr;
            }
            else {
                return DEFAULT_KEY_EXTRACTOR{};
            }
        }

        /**
         *  Default extractor if not specified in constructor (e.g. default-constructor KeyedCollection())
         */
        static const inline KeyExtractorType kDefaultKeyExtractor{mk_ ()};

        /**
         *  Default comparer if not specified in constructor (e.g. default-constructor KeyedCollection())
         */
        static const inline KeyEqualityComparerType kDefaultKeyEqualsComparer{DEFAULT_KEY_EQUALS_COMPARER{}};

        /**
         *  Define typedef for this KeyedCollection traits object (so other traits can generically allow recovery of the
         *  underlying KeyedCollection's TRAITS objects.
         */
        using KeyedCollectionTraitsType = KeyedCollection_DefaultTraits<T, KEY_TYPE, DEFAULT_KEY_EQUALS_COMPARER>;
    };

    /**
     *      KeyedCollection adds most access patterns used in Mapping to a Collection, but stores only a single
     *      object. The idea is to have the ID (from a Mapping<ID,Obj>) stored directly in the 'Obj', but still
     *      be able to do lookups based on the ID.
     * 
     *      This can also be thought of as much like a Set<T> - where the element T has a known KEY field.
     *      You could easily make a Set<T> like this, but then lookups would require you passed in a 'T' instead
     *      of just the Key. That lookup by key is basically the 'feature' that KeyedCollection provides.
     *
     *  \note   Considered using Set<T> (to avoid using this concept) with helper mkComparerByIDExtract.
     *          This is close (for adding/removing items works fine). But - you need to be able to lookup items by
     *          ID, and that required forcing the type T to be constructible just from an ID (awkward and potentially
     *          expensive).
     *
     *  \note   Not based on, but useful to refer to .Net KeyedColleciton<KEY,T>
     *          @see https://msdn.microsoft.com/en-us/library/ms132438%28v=vs.110%29.aspx?f=255&MSPPError=-2147217396
     *
     *  \note   Important that KeyedCollection<>::Add () will REPLACE the value, unlike with set.
     *
     *  \note Design Choice:
     *          Could either embed the 'extractor' function in the container type TRAITS or passed in as a
     *          constructor argument to the container. 
     *              As-part-of-TRAITS:  Logically cannot change for a given container type, so unambiguous how stuff pulled out of T (extractor etc)
     *                  THis makes the most sense, but its inconvenient in usage.
     *              As constructor argument to container: 
     *                  This might appear slightly more loosy goosy - but no more so than having differnt comparers for Set<T>. And its more
     *                  convenient in use/practice.
     *
     *  \note Note About Iterators
     *      o   Stroika container iterators must have shorter lifetime than the container they are iterating over.
     *
     *      o   Stroika container iterators are all automatically patched, so that if you change the underlying container
     *          the iterators are automatically updated internally to behave sensibly.
     *
     *  TODO:
     *      @todo   Consider adding methods from Set<T>, like Union, Interection, Difference etc. Or way to create
     *              Set easily (maybe more logical).
     */
    template <typename T, typename KEY_TYPE, typename TRAITS = KeyedCollection_DefaultTraits<T, KEY_TYPE>>
    class KeyedCollection : public Iterable<T> {

    private:
        using inherited = Iterable<T>;

    protected:
        class _IRep;

    protected:
#if qCompilerAndStdLib_TemplateTemplateWithTypeAlias_Buggy
        using _IRepSharedPtr = conditional_t<Stroika::Foundation::Traversal::kIterableUsesStroikaSharedPtr, Stroika::Foundation::Memory::SharedPtr<_IRep>, shared_ptr<_IRep>>;
#else
        using _IRepSharedPtr = typename inherited::template PtrImplementationTemplate<_IRep>;
#endif

    public:
        /**
         *  Just a short-hand for the 'TRAITS' part of KeyedCollection<T,KEY_TYPE,TRAITS>. This is often handy to use in
         *  building other templates.
         */
        using TraitsType = TRAITS;

    public:
        /**
         *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
         */
        using ArchetypeContainerType = KeyedCollection;

    public:
        /**
         */
        using KeyType = KEY_TYPE;

    public:
        /**
         */
        using KeyExtractorType = function<KEY_TYPE (ArgByValueType<T>)>;

    public:
        /**
         *  This is the type returned by GetElementEqualsComparer () and CAN be used as the argument to a KeyedCollection<> as EqualityComparer, but
         *  we allow any template in the KeyedCollection<> CTOR for an equalityComparer that follows the Common::IsEqualsComparer () concept.
         *
         *  \note   @see also EqualsComparer{} to compare whole KeyedCollection<>s
         */
        using KeyEqualityComparerType = Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals, function<bool (ArgByValueType<KEY_TYPE>, ArgByValueType<KEY_TYPE>)>>;

    public:
        /**
         */
        using value_type = typename inherited::value_type;

    public:
        /**
         *  \brief check if the argument type can be passed as argument to the arity/1 overload of Add ()
         *
         *  \todo https://stroika.atlassian.net/browse/STK-651 - Experimental feature which might be used as a concept check on various templates
         */
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable = is_convertible_v<POTENTIALLY_ADDABLE_T, T>;

    public:
        /**
         *  For the CTOR overload with CONTAINER_OF_ADDABLE, its anything that supports c.begin(), c.end () to find
         *  all the elements.
         *
         *  All constructors either copy their source comparer (copy/move CTOR), or use the provided argument comparer
         *  (which in turn defaults to equal_to<T>).
         *
         *  If TRAITS (TraitsType) has a valid default extractor, enable certain constructors.
         *
         *  \note For efficiency sake, the base constructor takes a templated EQUALS_COMPARER (avoiding translation to function<bool(T,T)>>, but
         *        for simplicity sake, many of the other constructors force that conversion.
         *
         * \req IsEqualsComparer<EQUALS_COMPARER> () - for constructors with that type parameter
         *
         *  \par Example Usage
         *      \code
         *      \endcode
         *
         *  \note Implementation note:
         *        Reason for the not is_base_of_v<> restriction on CTOR/1(CONTAINER_OF_ADDABLE&&) is to prevent compiler from
         *        instantiating that constructor template for argument subclasses of this container type, and having those take precedence over the
         *        default X(const X&) CTOR.
         * 
         *        And also careful not to apply to non-iterables.
         */
        template <typename KE = typename TraitsType::DefaultKeyExtractor, enable_if_t<Configuration::is_callable_v<KE>>* = nullptr>
        KeyedCollection (KeyEqualityComparerType keyComparer = TraitsType::kDefaultKeyEqualsComparer);
        KeyedCollection (KeyExtractorType keyExtractor, KeyEqualityComparerType keyComparer = TraitsType::kDefaultKeyEqualsComparer);
        KeyedCollection (const KeyedCollection& src) noexcept = default;
        template <typename CONTAINER_OF_ADDABLE, typename KE = typename TraitsType::DefaultKeyExtractor, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<KeyedCollection<T, KEY_TYPE, TRAITS>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>> and Configuration::is_callable_v<KE>>* = nullptr>
        KeyedCollection (CONTAINER_OF_ADDABLE&& src, KeyEqualityComparerType keyComparer = TraitsType::kDefaultKeyEqualsComparer);
        template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<KeyedCollection<T, KEY_TYPE, TRAITS>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>* = nullptr>
        KeyedCollection (CONTAINER_OF_ADDABLE&& src, KeyExtractorType keyExtractor, KeyEqualityComparerType keyComparer = TraitsType::kDefaultKeyEqualsComparer);

#if 0
        template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<KeyedCollection<T,KEY_TYPE,TRAITS>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>* = nullptr>
        explicit KeyedCollection (CONTAINER_OF_ADDABLE&& src, KeyExtractorType keyExtractor = typename TraitsType::DefaultKeyExtractor, KeyEqualityComparerType keyComparer = typename TraitsType::DefaultKeyEqualsComparer);
        template <typename COPY_FROM_ITERATOR_OF_ADDABLE, enable_if_t<Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE>>* = nullptr>
        KeyedCollection (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end, KeyExtractorType keyExtractor = typename TraitsType::DefaultKeyExtractor, KeyEqualityComparerType keyComparer = typename TraitsType::DefaultKeyEqualsComparer);
#endif
    protected:
        explicit KeyedCollection (const _IRepSharedPtr& rep) noexcept;
        explicit KeyedCollection (_IRepSharedPtr&& rep) noexcept;

#if qDebug
    public:
        ~KeyedCollection ();
#endif

    public:
        /**
         */
        nonvirtual KeyedCollection& operator= (const KeyedCollection& rhs) = default;

    public:
        /**
         */
        nonvirtual KeyExtractorType GetKeyExtractor () const;

    public:
        /**
         */
        nonvirtual KeyEqualityComparerType GetKeyEqualityComparer () const;

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
         *  \note Contains (T) is same as Contains (KeyExtractor(t)) - it only looks at the key, not the rest of the value
         */
        nonvirtual bool Contains (ArgByValueType<KeyType> item) const;
        nonvirtual bool Contains (ArgByValueType<T> item) const;

    public:
        /**
         *  Like Contains - but a Set<> can use a comparison that only examines a part of T,
         *  making it useful to be able to return the rest of T.
         */
        nonvirtual optional<value_type> Lookup (ArgByValueType<KeyType> key) const;
        nonvirtual bool                 Lookup (ArgByValueType<KeyType> key, optional<value_type>* item) const;
        nonvirtual bool                 Lookup (ArgByValueType<KeyType> key, value_type* item) const;
        nonvirtual bool                 Lookup (ArgByValueType<KeyType> key, nullptr_t) const;

    public:
        /**
         *  Add when T is already present has may have no effect (logically has no effect) on the
         *  container (not an error or exception).
         *
         *  So for a user-defined type T (or any type where the caller specifies the compare function)
         *  it is left undefined whether or not the new (not included) attributes assocaited with the added
         *  item make it into the Set.
         *
         *  return true if new item, and false if simply updated
         *
         *  \note mutates container
         */
        nonvirtual bool Add (ArgByValueType<T> item);

    public:
        /**
         *  \note   AddAll/2 is alias for .net AddRange ()
         *
         *  Returns the number if items actually changed (not necessarily same as end-start)
         * 
         *  \note mutates container
         */
        template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
        nonvirtual unsigned int AddAll (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);
        template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T>>* = nullptr>
        nonvirtual unsigned int AddAll (CONTAINER_OF_ADDABLE&& items);
        nonvirtual unsigned int AddAll (const KeyedCollection& items);

    public:
        /**
         *  The overload taking an item doesn't require the value exists, but removes it if it does.
         *  The overload taking an iterator requires the iterator is valid.
         *
         *  @see RemoveIf ()
         *
         *  \note mutates container
         */
        nonvirtual void Remove (const Iterator<T>& i);
        nonvirtual void Remove (ArgByValueType<KeyType> item);
        nonvirtual void Remove (ArgByValueType<T> item);

    public:
        /**
         *  RemoveIf item if present. Whether present or not it does the same thing and
         *  assures the item is no longer present, but returns true iff the call made a change (removed
         *  the item).
         *
         *  Note - we chose to return true in the case of removal because this is the case most likely
         *  when a caller would want to take action.
         *
         *  \par Example Usage
         *      \code
         *          if (s.RemoveIf (n)) {
         *              write_to_disk(n);
         *          }
         *      \endcode
         *
         *  @see Remove ()
         *
         *  \note mutates container
         */
        nonvirtual bool RemoveIf (ArgByValueType<KeyType> item);
        nonvirtual bool RemoveIf (ArgByValueType<T> item);

    public:
        /**
         *  \note mutates container
         */
        template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
        nonvirtual void RemoveAll (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);
        template <typename CONTAINER_OF_ADDABLE>
        nonvirtual void RemoveAll (const CONTAINER_OF_ADDABLE& s);
        nonvirtual void RemoveAll ();

    public:
        /**
         *  Apply the function function to each element, and return all the ones for which it was true.
         *
         *  \note   Alias - this could have been called 'Subset' - as it constructs a subset.
         *
         *  @see Iterable<T>::Where
         */
        nonvirtual KeyedCollection Where (const function<bool (ArgByValueType<T>)>& includeIfTrue) const;

    public:
        struct EqualsComparer;

#if __cpp_impl_three_way_comparison >= 201907
    public:
        /**
         * Simply intdirect to KeyedCollection<>::EqualsComparer
         */
        nonvirtual bool operator== (const KeyedCollection& rhs) const;
        nonvirtual bool operator== (const Iterable<T>& rhs) const;
#endif

    public:
        /**
         *      Synonym for Add/AddAll.
         *
         *  Design note  use Addll/RemoveAll() for CONTAINER variant - since can easily lead to ambiguity/confusion
         *
         *  \note mutates container
         */
        nonvirtual KeyedCollection& operator+= (ArgByValueType<T> item);
        nonvirtual KeyedCollection& operator+= (const Iterable<T>& items);

    public:
        /**
         *      Synonym for Remove/RemoveAll.
         *
         *  Design note  use Addll/RemoveAll() for CONTAINER variant - since can easily lead to ambiguity/confusion
         *
         *  \note mutates container
         */
        nonvirtual KeyedCollection& operator-= (ArgByValueType<T> item);
        nonvirtual KeyedCollection& operator-= (const Iterable<T>& items);

    public:
        /**
         * \brief STL-ish alias for RemoveAll ().
         *
         *  \note mutates container
         */
        nonvirtual void clear ();

    public:
        /**
         * \brief STL-ish alias for Remove ().
         *
         *  \note mutates container
         */
        nonvirtual void erase (ArgByValueType<T> item);
        nonvirtual void erase (const Iterator<T>& i);

    protected:
        /**
         */
        template <typename T2>
        using _SafeReadRepAccessor = typename inherited::template _SafeReadRepAccessor<T2>;

    protected:
        /**
         */
        template <typename T2>
        using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<T2>;

    protected:
        nonvirtual void _AssertRepValidType () const;
    };

    /**
     *  \brief  Implementation detail for KeyedCollection<KEY_TYPE, T, TRAITS> implementors.
     *
     *  Protected abstract interface to support concrete implementations of
     *  the KeyedCollection<KEY_TYPE, T, TRAITS> container API.
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    class KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep : public Iterable<T>::_IRep
#if !qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
        ,
                                                        public Traversal::IterableBase::enable_shared_from_this_PtrImplementationTemplate<typename KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep>
#endif
    {
    protected:
        using _IRepSharedPtr = typename KeyedCollection<T, KEY_TYPE, TRAITS>::_IRepSharedPtr;

    public:
        virtual KeyExtractorType        GetKeyExtractor () const                               = 0;
        virtual KeyEqualityComparerType GetKeyEqualityComparer () const                        = 0;
        virtual _IRepSharedPtr          CloneEmpty (IteratorOwnerID forIterableEnvelope) const = 0;
        virtual Iterable<KEY_TYPE>      Keys () const                                          = 0;
        // always clear/set item, and ensure return value == item->IsValidItem());
        // 'item' arg CAN be nullptr
        virtual bool Lookup (ArgByValueType<KeyType> key, optional<value_type>* item) const = 0;
        // return true if new item, and false if simply updated
        virtual bool Add (ArgByValueType<T> item)                              = 0;
        virtual void Update (const Iterator<T>& i, ArgByValueType<T> newValue) = 0;
        virtual void Remove (const Iterator<T>& i)                             = 0;
        // returns true iff a change made, false if elt was not present
        virtual bool Remove (ArgByValueType<KEY_TYPE> key) = 0;
#if qDebug
        virtual void AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const = 0;
#endif
    protected:
        nonvirtual Iterable<KEY_TYPE> _Keys_Reference_Implementation () const;
    };

    /**
     * logically indirect to @Set<KeyType>::EqualsComparer (using this->GetEqualsComparer ()) to compare the keys
     *  Order is meaningless in comparing KeyedCollection - this compares just by Set equality of the keys.
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    struct KeyedCollection<T, KEY_TYPE, TRAITS>::EqualsComparer : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals> {
        constexpr EqualsComparer () = default;
        nonvirtual bool operator() (const KeyedCollection& lhs, const KeyedCollection& rhs) const;
        nonvirtual bool operator() (const KeyedCollection& lhs, const Iterable<T>& rhs) const;
        nonvirtual bool operator() (const Iterable<T>& lhs, const KeyedCollection& rhs) const;
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "KeyedCollection.inl"

#endif /*_Stroika_Foundation_Containers_KeyedCollection_h_ */
