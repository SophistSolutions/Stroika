/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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

    using Common::IEqualsComparer;
    using Configuration::ArgByValueType;
    using Traversal::IInputIterator;
    using Traversal::IIterableOf;
    using Traversal::Iterable;
    using Traversal::Iterator;

    /**
     */
    template <typename POTENTIAL_KEY_EXTRACTOR, typename T, typename KEY_TYPE>
    concept IKeyedCollection_KeyExctractor =
        is_invocable_v<POTENTIAL_KEY_EXTRACTOR, T> and is_convertible_v<std::invoke_result_t<POTENTIAL_KEY_EXTRACTOR, T>, KEY_TYPE>;

    /**
     *  \note KEY_EXTRACTOR_TYPE defaults in such a way that you can specify a function (std::function or lambda) in the constructor.
     *        Specify a specific function object to allow for slightly more efficient, but less flexible use.
     * 
     *  \par Example Usage
     *      \code
     *          namespace Private_ {
     *              struct Obj_ {
     *                  type_index fTypeIndex;  // KEY
     *                  int otherData{};        // 
     *              };
     *              struct My_Extractor_ {
     *                  auto operator() (const Obj_& t) const -> type_index { return t.fTypeIndex; };
     *              };
     *              using My_Traits_ = Containers::KeyedCollection_DefaultTraits<Obj_, type_index, My_Extractor_>;
     *          }
     *          void RunAll ()
     *          {
     *              using namespace Private_;
     *              {
     *                  KeyedCollection<Obj_, type_index, My_Traits_>                  s2;
     *                  s2.Add (Obj_{typeid (int)});
     *                  s2.Add (Obj_{typeid (long int)});
     *              }
     *              {
     *                  // Or slighltly more flexiblely, but less efficiently
     *                  KeyedCollection<Obj_, type_index>                  s2{My_Extractor_{}};
     *                  s2.Add (Obj_{typeid (int)});
     *                  s2.Add (Obj_{typeid (long int)});
     *              }
     *          }
     *      \endcode
     */
    template <typename T, typename KEY_TYPE, typename KEY_EXTRACTOR_TYPE = function<KEY_TYPE (ArgByValueType<T>)>>
    struct KeyedCollection_DefaultTraits {
        /**
         *  Default extractor if not specified in constructor (e.g. default-constructor KeyedCollection())
         */
        using KeyExtractorType = KEY_EXTRACTOR_TYPE;
    };

    /**
     *  KeyedCollection can be templated with a KeyExtractorType that allows it to be used with default construction
     *  and no keyextractor specfied.
     * 
     *  But the default definition - using std::function - requires the constructor to provide an extractor function
     *  since the default for this std::function is not callable.
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    concept IKeyedCollection_ExtractorCanBeDefaulted = is_invocable_v<typename TRAITS::KeyExtractorType, T> and
                                                       std::is_convertible_v<std::invoke_result_t<typename TRAITS::KeyExtractorType, T>, KEY_TYPE> and
                                                       is_default_constructible_v<typename TRAITS::KeyExtractorType> and
                                                       not is_same_v<typename TRAITS::KeyExtractorType, function<KEY_TYPE (T)>> and
                                                       not is_same_v<typename TRAITS::KeyExtractorType, function<KEY_TYPE (const T&)>>;

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
     *  \note   you can only Add a T/value_type, not a KEY_TYPE, but 'lookup-style' operations can operate on KEY_TYPE.
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
    class [[nodiscard]] KeyedCollection : public Iterable<T> {

    private:
        using inherited = Iterable<T>;

    protected:
        class _IRep;

    public:
        /**
         *  @see inherited::value_type - this is 'T' (not KEY_TYPE)
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
         *  This is the type of the 'extractor' function, which can be static (so occupy no space) but defaults to
         *  function<KEY_TYPE (ArgByValueType<T>)>> - specified in the TRAITS argument to KeyedCollection<>
         */
        using KeyExtractorType = typename TRAITS::KeyExtractorType;
        static_assert (IKeyedCollection_KeyExctractor<KeyExtractorType, T, KEY_TYPE>);

    public:
        /**
         *  This is the type returned by GetElementEqualsComparer () and CAN be used as the argument to a KeyedCollection<> as EqualityComparer, but
         *  we allow any template in the KeyedCollection<> CTOR for an equalityComparer that follows the IEqualsComparer concept.
         *
         *  \note   @see also EqualsComparer{} to compare whole KeyedCollection<>s
         */
        using KeyEqualityComparerType =
            Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals, function<bool (ArgByValueType<KEY_TYPE>, ArgByValueType<KEY_TYPE>)>>;
        static_assert (IEqualsComparer<KeyEqualityComparerType, key_type>);

    public:
        /**
         *  For the CTOR overload with ITERABLE_OF_ADDABLE, its anything that supports c.begin(), c.end () to find
         *  all the elements and for which the result of c.begin ().
         *
         *  All constructors either copy their source comparer (copy/move CTOR), or use the provided argument comparer
         *  (which in turn defaults to equal_to<T>).
         *
         *  If TRAITS (TraitsType) has a valid default extractor, enable certain constructors.
         *
         *  \note For efficiency sake, the base constructor takes a templated EQUALS_COMPARER (avoiding translation to function<bool(T,T)>>
         *        so the REP can see the actual type, but the container API itself erases this specific type using std::function.
         *
         *  \req EqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER - for constructors with that type parameter
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
         *  \note   <a href="ReadMe.md#Container Constructors">See general information about container constructors that applies here</a>
         */
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
        KeyedCollection (KEY_EQUALS_COMPARER&& keyComparer = KEY_EQUALS_COMPARER{})
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>);
        KeyedCollection (KeyedCollection&& src) noexcept      = default;
        KeyedCollection (const KeyedCollection& src) noexcept = default;
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
        KeyedCollection (const KeyExtractorType& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer = KEY_EQUALS_COMPARER{});
        template <IIterableOf<T> ITERABLE_OF_ADDABLE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
        KeyedCollection (ITERABLE_OF_ADDABLE&& src)
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS> and
                      not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, KeyedCollection<T, KEY_TYPE, TRAITS>>)
#if qCompilerAndStdLib_template_Requires_templateDeclarationMatchesOutOfLine2_Buggy
            : KeyedCollection{}
        {
            AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            _AssertRepValidType ();
        }
#endif
        ;
        template <IIterableOf<T> ITERABLE_OF_ADDABLE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
        KeyedCollection (KEY_EQUALS_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src)
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS> and
                      not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, KeyedCollection<T, KEY_TYPE, TRAITS>>)
#if qCompilerAndStdLib_template_Requires_templateDeclarationMatchesOutOfLine2_Buggy
            : KeyedCollection{KeyExtractorType{}, forward<KEY_EQUALS_COMPARER> (keyComparer)}
        {
            AddAll (src);
            _AssertRepValidType ();
        }
#endif
        ;
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, IIterableOf<T> ITERABLE_OF_ADDABLE>
        KeyedCollection (const KeyExtractorType& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src);
        template <IIterableOf<T> ITERABLE_OF_ADDABLE>
        KeyedCollection (const KeyExtractorType& keyExtractor, ITERABLE_OF_ADDABLE&& src)
            requires (IEqualsComparer<KEY_TYPE, equal_to<KEY_TYPE>>);
        template <IInputIterator<T> ITERATOR_OF_ADDABLE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
        KeyedCollection (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>);
        template <IInputIterator<T> ITERATOR_OF_ADDABLE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
        KeyedCollection (KEY_EQUALS_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>);
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, IInputIterator<T> ITERATOR_OF_ADDABLE>
        KeyedCollection (const KeyExtractorType& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    protected:
        explicit KeyedCollection (shared_ptr<_IRep>&& rep) noexcept;
        explicit KeyedCollection (const shared_ptr<_IRep>& rep) noexcept;

    public:
        /**
         */
        nonvirtual KeyedCollection& operator= (KeyedCollection&& rhs)      = default;
        nonvirtual KeyedCollection& operator= (const KeyedCollection& rhs) = default;

    public:
        /**
         *  Returns the extractor.
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
         *  Keys () returns an Iterable object with just the key part of the KeyedCollection.
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
        nonvirtual value_type LookupChecked (ArgByValueType<KeyType> key, THROW_IF_MISSING&& throwIfMissing) const;

    public:
        /**
         *  Always safe to call. If result of Lookup () !has_value, returns argument 'default' or 'sentinel' value.
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
         *  it is left undefined whether or not the new (not included) attributes associated with the added
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
         *  \note mutates container
         */
        template <IInputIterator<T> ITERATOR_OF_ADDABLE>
        nonvirtual unsigned int AddAll (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <IIterableOf<T> ITERABLE_OF_ADDABLE>
        nonvirtual unsigned int AddAll (ITERABLE_OF_ADDABLE&& items);

    public:
        /**
         *  \brief Remove the argument value (which must exist) from the KeyedCollection.
         * 
         *  @see RemoveIf () for when the argument might not exist
         *
         *  \note mutates container
         *
         *  \param nextI - if provided (not null) - will be filled in with the next value after where iterator i is pointing - since i is invalidated by changing the container)
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
        template <IInputIterator<T> ITERATOR_OF_ADDABLE>
        nonvirtual size_t RemoveAll (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <IIterableOf<T> ITERABLE_OF_ADDABLE>
        nonvirtual size_t RemoveAll (const ITERABLE_OF_ADDABLE& s);
        nonvirtual void   RemoveAll ();
        template <predicate<T> PREDICATE>
        nonvirtual size_t RemoveAll (PREDICATE&& p);

    public:
        /**
         * \brief 'override' Iterable<>::Map () function so RESULT_CONTAINER defaults to KeyedCollection, and improve that case to clone properties from this rep (such is rep type, extractor etc).
         */
        template <typename RESULT_CONTAINER = KeyedCollection<T, KEY_TYPE, TRAITS>, invocable<T> ELEMENT_MAPPER>
        nonvirtual RESULT_CONTAINER Map (ELEMENT_MAPPER&& elementMapper) const
            requires (convertible_to<invoke_result_t<ELEMENT_MAPPER, T>, typename RESULT_CONTAINER::value_type> or
                      convertible_to<invoke_result_t<ELEMENT_MAPPER, T>, optional<typename RESULT_CONTAINER::value_type>>);

    public:
        /**
         *  Apply the function function to each element, and return all the ones for which it was true.
         *
         *  \note   Alias - this could have been called 'Subset' - as it constructs a subset.
         *
         *  @see Iterable<T>::Where
         */
        template <derived_from<Iterable<T>> RESULT_CONTAINER = KeyedCollection<T, KEY_TYPE, TRAITS>, predicate<T> INCLUDE_PREDICATE>
        nonvirtual RESULT_CONTAINER Where (INCLUDE_PREDICATE&& includeIfTrue) const;

    public:
        struct EqualsComparer;

    public:
        /**
         * Simply indirect to KeyedCollection<>::EqualsComparer
         */
        nonvirtual bool operator== (const KeyedCollection& rhs) const;
        nonvirtual bool operator== (const Iterable<value_type>& rhs) const;

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
    class KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep : public Iterable<T>::_IRep {

    private:
        using inherited = typename Iterable<T>::_IRep;

    public:
        virtual KeyExtractorType        GetKeyExtractor () const                              = 0;
        virtual KeyEqualityComparerType GetKeyEqualityComparer () const                       = 0;
        virtual shared_ptr<_IRep>       CloneEmpty () const                                   = 0;
        virtual shared_ptr<_IRep>       CloneAndPatchIterator (Iterator<value_type>* i) const = 0;
        // always clear/set item, and ensure return value == item->IsValidItem());
        // 'item' arg CAN be nullptr
        virtual bool Lookup (ArgByValueType<KeyType> key, optional<value_type>* item) const = 0;
        // return true if new item, and false if simply updated
        virtual bool Add (ArgByValueType<value_type> item)                               = 0;
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) = 0;
        // returns true iff a change made, false if elt was not present
        virtual bool RemoveIf (ArgByValueType<KEY_TYPE> key) = 0;
    };

    /**
     * logically indirect to @Set<KeyType>::EqualsComparer (using this->GetEqualsComparer ()) to compare the keys
     *  Order is meaningless in comparing KeyedCollection - this compares just by Set equality of the keys.
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    struct KeyedCollection<T, KEY_TYPE, TRAITS>::EqualsComparer
        : Common::ComparisonRelationDeclarationBase<Common::ComparisonRelationType::eEquals> {
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
