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
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers {

    using Configuration::ArgByValueType;
    using Configuration::ExtractValueType_t;
    using Traversal::Iterable;
    using Traversal::Iterator;

    /**
     *  @todo SB concept
     */
    template <typename T, typename KEY_TYPE, typename POTENTIAL_KEY_EXTRACTOR>
    constexpr bool KeyedCollection_IsKeyExctractor ()
    {
        if constexpr (is_invocable_v<POTENTIAL_KEY_EXTRACTOR, T>) {
            return std::is_convertible_v<std::invoke_result_t<POTENTIAL_KEY_EXTRACTOR, T>, KEY_TYPE>;
        }
        return false;
    }

    /**
     *  \note its OK if DEFAULT_KEY_EXTRACTOR is invalid, it just means you must specify the default-key-extractor in the 
     *        constructor for KeyedCollection<>
     * 
     *  \par Example Usage
     *      \code
     *          struct Device_Key_Extractor_ {
     *              GUID operator() (const DiscoveryInfo_& t) const { return t.fGUID; };
     *          };
     *          using DiscoveryDeviceCollection_ = KeyedCollection<DiscoveryInfo_, GUID, KeyedCollection_DefaultTraits<DiscoveryInfo_, GUID, Device_Key_Extractor_>>;
     *          
     *          // one way
     *          DiscoveryDeviceCollection_ sDiscoveredDevices_;
     *          sDiscoveredDevices_.Add (DiscoveryInfo_ {...});
     * 
     *          // another way
     *          namespace Stroika::Foundation::Containers {
     *              template <>
     *              struct KeyedCollection_DefaultTraits<DiscoveryInfo_, GUID, void> : DiscoveryDeviceCollection_ {
     *              };
     *           }
     *          KeyedCollection<DiscoveryInfo_, GUID> sDiscoveredDevices2_;
     *          sDiscoveredDevices_.Add (DiscoveryInfo_ {...});
     *      \endcode
     */
    template <typename T, typename KEY_TYPE, typename DEFAULT_KEY_EXTRACTOR = void>
    struct KeyedCollection_DefaultTraits {
        /**
         *  Default extractor if not specified in constructor (e.g. default-constructor KeyedCollection())
         */
        using DefaultKeyExtractor = DEFAULT_KEY_EXTRACTOR;
    };

    /**
     *  \brief a cross between Mapping<KEY, T> and Collection<T> and Set<T>
     * 
     *      KeyedCollection adds most access patterns used in Mapping to a Collection, but stores only a single
     *      object. The idea is to have the ID (from a Mapping<ID,Obj>) stored directly in the 'Obj', but still
     *      be able to do lookups based on the ID.
     * 
     *      This can also be thought of as much like a Set<T> - where the element T has a known KEY field.
     *      You could easily make a Set<T> like this, but then lookups would require you passed in a 'T' instead
     *      of just the Key. That lookup by key is basically the 'feature' that KeyedCollection provides.
     *
     *  \note   Considered using Set<T> (to avoid using this concept) with helper mkComparerByIDExtract.
     *              auto Set<T>::mkComparerByIDExtract (const function<ID (T)>& idExtractor) -> ElementEqualityComparerType
     *              {
     *                  return [idExtractor] (const T& lhs, const T& rhs) { return idExtractor (lhs) == idExtractor (rhs); };
     *              }
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
     *  \note   See <a href="./ReadMe.md">ReadMe.md</a> for common features of all Stroika containers (especially
     *          constructors, iterators, etc)
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
         *  @see inherited::value_type
         */
        using value_type = typename inherited::value_type;

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
        using key_type = KEY_TYPE;

    public:
        /**
         */
        using KeyExtractorType = function<KEY_TYPE (ArgByValueType<value_type>)>;

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
         *  \brief check if the argument type can be passed as argument to the arity/1 overload of Add ()
         * 
         *  NOTE - you can only Add a T/value_type, not a KEY_TYPE, but 'lookup-style' operations can operate on KEY_TYPE.
         */
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v = is_convertible_v<POTENTIALLY_ADDABLE_T, value_type>;

    public:
        /**
         *  For the CTOR overload with ITERABLE_OF_ADDABLE, its anything that supports c.begin(), c.end () to find
         *  all the elements and for which the result of c.begin () IsAddable_v().
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
         *          struct T1 {
         *              int key;
         *              int value;
         *          };
         *          struct T1_Key_Extractor {
         *              int operator() (const T1& t) const { return t.key; };
         *          };
         *          using T1_Traits = KeyedCollection_DefaultTraits<T1, int, T1_Key_Extractor>;
         *
         *          KeyedCollection<T1, int> kc1 {[] (T1 e) { return e.key; }};     // specify extractor explicitly
         *          KeyedCollection<T1, int, T1_Traits> kc2 {[] (T1 e) { return e.key; }};
         *          KeyedCollection<T1, int, T1_Traits> kc3{};                      // get the extractor from the TRAITS
         *      \endcode
         *
         *  \note The constructor argument KEY_EQUALS_COMPARER must be declared to be an equals_comparers to avoid
         *        ambiguity/accidental mixups between inorder and equals (or three way) comparers. Consider wrapping lambdas with Common::DeclareEqualsComparer
         *
         *  \par Example Usage
         *      \code
         *          KeyedCollection<DiskInfoType, filesystem::path> result{[] (DiskInfoType e) { return e.fDeviceName; }};
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          KeyedCollection<DiskInfoType, filesystem::path> result{[] (DiskInfoType e) { return e.fDeviceName; }};
         *      \endcode
         *
         *  \note   <a href="ReadMe.md#Container Constructors">See general information about container constructors that applies here</a>
         */
        template <typename KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>,
                  typename KEY_EXTRACTOR       = typename TraitsType::DefaultKeyExtractor,
                  enable_if_t<
                      Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>* = nullptr>
        KeyedCollection (KEY_EQUALS_COMPARER&& keyComparer = KEY_EQUALS_COMPARER{});
        KeyedCollection (KeyedCollection&& src) noexcept      = default;
        KeyedCollection (const KeyedCollection& src) noexcept = default;
        template <typename KEY_EXTRACTOR,
                  typename KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>,
                  enable_if_t<
                      Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>* = nullptr>
        KeyedCollection (KEY_EXTRACTOR&& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer = KEY_EQUALS_COMPARER{});
        template <typename ITERABLE_OF_ADDABLE,
                  typename KEY_EXTRACTOR       = typename TraitsType::DefaultKeyExtractor,
                  typename KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>,
                  enable_if_t<
                      Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<KeyedCollection<T, KEY_TYPE, TRAITS>, decay_t<ITERABLE_OF_ADDABLE>> and Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>* = nullptr>
        KeyedCollection (ITERABLE_OF_ADDABLE&& src);
        template <typename ITERABLE_OF_ADDABLE,
                  typename KEY_EXTRACTOR       = typename TraitsType::DefaultKeyExtractor,
                  typename KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>,
                  enable_if_t<
                      Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<KeyedCollection<T, KEY_TYPE, TRAITS>, decay_t<ITERABLE_OF_ADDABLE>> and Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>* = nullptr>
        KeyedCollection (KEY_EQUALS_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src);
        template <typename KEY_EXTRACTOR, typename KEY_EQUALS_COMPARER, typename ITERABLE_OF_ADDABLE,
                  enable_if_t<
                      KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> () and Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>* = nullptr>
        KeyedCollection (KEY_EXTRACTOR&& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src);
        template <typename ITERATOR_OF_ADDABLE,
                  typename KEY_EXTRACTOR       = typename TraitsType::DefaultKeyExtractor,
                  typename KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>,
                  enable_if_t<
                      Configuration::IsIterator_v<ITERATOR_OF_ADDABLE> and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> ()>* = nullptr>
        KeyedCollection (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <typename ITERATOR_OF_ADDABLE,
                  typename KEY_EXTRACTOR       = typename TraitsType::DefaultKeyExtractor,
                  typename KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>,
                  enable_if_t<
                      Configuration::IsIterator_v<ITERATOR_OF_ADDABLE> and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> ()>* = nullptr>
        KeyedCollection (KEY_EQUALS_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <typename KEY_EXTRACTOR, typename KEY_EQUALS_COMPARER, typename ITERATOR_OF_ADDABLE,
                  enable_if_t<
                      KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> () and Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>* = nullptr>
        KeyedCollection (KEY_EXTRACTOR&& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    protected:
        explicit KeyedCollection (_IRepSharedPtr&& rep) noexcept;
        explicit KeyedCollection (const _IRepSharedPtr& rep) noexcept;

    public:
        /**
         */
        nonvirtual KeyedCollection& operator= (KeyedCollection&& rhs) = default;
        nonvirtual KeyedCollection& operator= (const KeyedCollection& rhs) = default;

    public:
        /**
         *  Returns the extractor (but wrapped as a function<> object, so perhaps slightly less efficient
         *  to use than the type passed in via CTOR).
         */
        nonvirtual KeyExtractorType GetKeyExtractor () const;

    public:
        /**
         *  Returns the key equality comparer (but wrapped as a function<> object, so perhaps slightly less efficient
         *  to use than the type passed in via CTOR). Actually - could be MUCH less efficient if what was passed in was less<T> for example,
         *  an ordered comparer, and that gets mapped to an equality comparer. But this will work.
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
        nonvirtual bool Contains (ArgByValueType<value_type> item) const;

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
         *  \note Alias LookupOrException
         */
        template <typename THROW_IF_MISSING>
        nonvirtual value_type LookupChecked (ArgByValueType<KeyType> key, const THROW_IF_MISSING& throwIfMissing) const;

    public:
        /**
         *  Always safe to call. If result of Lookup () !has_value, returns argument 'default' or 'sentinal' value.
         *
         *  \note Alias LookupOrDefault
         */
        nonvirtual value_type LookupValue (ArgByValueType<KeyType> key, ArgByValueType<value_type> defaultValue = value_type{}) const;

    public:
        /**
         *  Add when T is already present has may have no effect (logically has no effect) on the
         *  container (not an error or exception) (except that if T contains fields not part of comparison, those will be updated).
         *
         *  So for a user-defined type T (or any type where the caller specifies the compare function)
         *  it is left undefined whether or not the new (not included) attributes assocaited with the added
         *  item make it into the Set.
         *
         *  return true if new item, and false if simply updated
         * 
         *  \note - because Add and Update logic are identical, KeyedCollection<> has no explicit Update (iterator) method.
         *          it COULD someday add an overload with optional Iterator<> hint argument, like in STL
         *
         *  \note mutates container
         */
        nonvirtual bool Add (ArgByValueType<value_type> item);

    public:
        /**
         *  \note   AddAll/2 is alias for .net AddRange ()
         *
         *  Returns the number if items actually added (not necessarily same as end-start)
         * 
         *  \req IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>
         *  \req IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>
         * 
         *  \note mutates container
         */
        template <typename ITERATOR_OF_ADDABLE>
        nonvirtual unsigned int AddAll (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>* = nullptr>
        nonvirtual unsigned int AddAll (ITERABLE_OF_ADDABLE&& items);

    public:
        /**
         *  \brief Remove the argument value (which must exist) from the KeyedCollection.
         * 
         *  @see RemoveIf () for when the argument might not exist
         *
         *  \note mutates container
         */
        nonvirtual void Remove (const Iterator<value_type>& i, const Iterator<value_type>* nextI = nullptr);
        nonvirtual void Remove (ArgByValueType<KeyType> item);
        nonvirtual void Remove (ArgByValueType<value_type> item);

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
        nonvirtual bool RemoveIf (ArgByValueType<value_type> item);

    public:
        /**
         *  \brief RemoveAll removes all, or all matching (predicate, iterator range, equals comparer or whatever) items.
         * 
         *  The no-arg overload removes all (quickly).
         * 
         *  The overloads that remove some subset of the items returns the number of items so removed.
         * 
         *  \note mutates container
         */
        template <typename ITERATOR_OF_ADDABLE>
        nonvirtual size_t RemoveAll (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <typename ITERABLE_OF_ADDABLE>
        nonvirtual size_t RemoveAll (const ITERABLE_OF_ADDABLE& s);
        nonvirtual void   RemoveAll ();
        template <typename PREDICATE, enable_if_t<Configuration::IsTPredicate<T, PREDICATE> ()>* = nullptr>
        nonvirtual size_t RemoveAll (const PREDICATE& p);

    public:
        /**
         *  Apply the function function to each element, and return all the ones for which it was true.
         *
         *  \note   Alias - this could have been called 'Subset' - as it constructs a subset.
         *
         *  @see Iterable<T>::Where
         */
        nonvirtual KeyedCollection Where (const function<bool (ArgByValueType<value_type>)>& includeIfTrue) const;

    public:
        struct EqualsComparer;

#if __cpp_impl_three_way_comparison >= 201907
    public:
        /**
         * Simply intdirect to KeyedCollection<>::EqualsComparer
         */
        nonvirtual bool operator== (const KeyedCollection& rhs) const;
        nonvirtual bool operator== (const Iterable<value_type>& rhs) const;
#endif

    public:
        /**
         *      Synonym for Add/AddAll.
         *
         *  Design note  use Addll/RemoveAll() for CONTAINER variant - since can easily lead to ambiguity/confusion
         *
         *  \note mutates container
         */
        nonvirtual KeyedCollection& operator+= (ArgByValueType<value_type> item);
        nonvirtual KeyedCollection& operator+= (const Iterable<value_type>& items);

    public:
        /**
         *      Synonym for Remove/RemoveAll.
         *
         *  Design note  use Addll/RemoveAll() for CONTAINER variant - since can easily lead to ambiguity/confusion
         *
         *  \note mutates container
         */
        nonvirtual KeyedCollection& operator-= (ArgByValueType<value_type> item);
        nonvirtual KeyedCollection& operator-= (const Iterable<value_type>& items);

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
        nonvirtual void erase (ArgByValueType<value_type> item);
        nonvirtual Iterator<value_type> erase (const Iterator<value_type>& i);

    protected:
        /**
         *  \brief Utility to get WRITABLE underlying shared_ptr (replacement for what we normally do - _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ())
         *         but where we also handle the cloning/patching of the associated iterator
         * 
         *  When you have a non-const operation (such as Remove) with an argument of an Iterator<>, then due to COW,
         *  you may end up cloning the container rep, and yet the Iterator<> contains a pointer to the earlier rep (and so maybe unusable).
         * 
         *  Prior to Stroika 2.1b14, this was handled elegantly, and automatically, by the iterator patching mechanism. But that was deprecated (due to cost, and
         *  rarity of use), in favor of this more restricted feature, where we just patch the iterators on an as-needed basis.
         * 
         *  \todo @todo - could be smarter about moves and avoid some copies here - I think, and this maybe performance sensitive enough to look into that... (esp for COMMON case where no COW needed)
         */
        nonvirtual tuple<_IRep*, Iterator<value_type>> _GetWritableRepAndPatchAssociatedIterator (const Iterator<value_type>& i);

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
        virtual KeyExtractorType        GetKeyExtractor () const                              = 0;
        virtual KeyEqualityComparerType GetKeyEqualityComparer () const                       = 0;
        virtual _IRepSharedPtr          CloneEmpty () const                                   = 0;
        virtual _IRepSharedPtr          CloneAndPatchIterator (Iterator<value_type>* i) const = 0;
        virtual Iterable<KEY_TYPE>      Keys () const                                         = 0;
        // always clear/set item, and ensure return value == item->IsValidItem());
        // 'item' arg CAN be nullptr
        virtual bool Lookup (ArgByValueType<KeyType> key, optional<value_type>* item) const = 0;
        // return true if new item, and false if simply updated
        virtual bool Add (ArgByValueType<value_type> item)                               = 0;
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) = 0;
        // returns true iff a change made, false if elt was not present
        virtual bool RemoveIf (ArgByValueType<KEY_TYPE> key) = 0;

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
        nonvirtual bool operator() (const KeyedCollection& lhs, const Iterable<value_type>& rhs) const;
        nonvirtual bool operator() (const Iterable<value_type>& lhs, const KeyedCollection& rhs) const;
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "KeyedCollection.inl"

#endif /*_Stroika_Foundation_Containers_KeyedCollection_h_ */
