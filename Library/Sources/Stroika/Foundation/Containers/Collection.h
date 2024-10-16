/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Collection_h_
#define _Stroika_Foundation_Containers_Collection_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Compare.h"
#include "Stroika/Foundation/Configuration/Concepts.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Memory/SharedByValue.h"
#include "Stroika/Foundation/Traversal/Iterable.h"

/**
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 *      @todo   Have Difference/Union/Intersection??? methods/?? Do research....
 *
 *      @todo   Consider adding RetainAll (Set<T>) API - like in Collection.h, and Java. Key diff is was force
 *              use of SET as arg - not another Bag? Or maybe overload with different container types as args?
 *              COULD do 2 versions - one with Iterable<T> and one with Set<T>. trick is to get definition
 *              to work without untoward dependencies between set and bag code? I think that means
 *              most of the check impl needs to be in the envelope to avoid always building it through vtables.
 *
 *      @todo   Perhaps add a Shake() method that produces a NEW Collection of a possibly different backend TYPE!
 *              with a random ordering.
 *
 *              Or add Bag<> class that does basically this.
 *
 *      @todo   Document relationship with Java Collection<T> (http://docs.oracle.com/javase/7/docs/api/java/util/Collection.html)
 *              Similar but Stroika container interface/container connection, Equals handling, and other language specific
 *              issues. Java has more 'all' variants (and retainAll - see possible todo above). And java makes this
 *              strictly an interface - used by lots of other subtypes (like set<>) - which we may wish to do as
 *              well (not sure - sterl feels this is a very bad idea, and I'm ambivalent).
 */

namespace Stroika::Foundation::Containers {

    using Configuration::ArgByValueType;
    using Traversal::IInputIterator;
    using Traversal::IIterableOf;
    using Traversal::Iterable;
    using Traversal::Iterator;

    /**
     *  \brief  A Collection<T> is a container to manage an un-ordered collection of items, without equality defined for T
     *
     *  A Collection<T> is a container pattern to manage an un-ordered collection of items,
     *  without equality defined for T. This is both an abstract interface, but the Collection<T>
     *  class it actually concrete because it automatically binds to a default implementation.
     *
     *  \note   This means that the order in which items appear during iteration is <em>arbitrary</em> and
     *          may vary from collection to collection type.
     *
     *          Some sub-types of Collection<T> - like SortedCollection<> - do make specific promises
     *          about ordering.
     *
     *  A Collection<T> is the simplest kind of collection. It allows addition and
     *  removal of elements, but makes no guarantees about element ordering.
     *
     *  \em Performance
     *      Collections are typically designed to optimize item addition and iteration.
     *      They are fairly slow at item access (as they have no keys). Removing items
     *      is usually slow, except in the context of an Iterator<T>, where it is usually
     *      very fast.
     *
     *  \note   See <a href="./ReadMe.md">ReadMe.md</a> for common features of all Stroika containers (especially
     *          constructors, iterators, etc)
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  \em Concrete Implementations:
     *      o   @see Concrete::Collection_Array<>
     *      o   @see Concrete::Collection_LinkedList<>
     *      o   @see Concrete::Collection_stdforward_list<>
     *      o   @see Concrete::SortedCollection_stdmultiset<>
     *      o   @see Concrete::SortedCollection_SkipList<>
     *
     *  \note   Shake
     *      Considered adding a Shake() method (when this was called Bag<T>), but that would restrict
     *      the use to backends capable of this randomizing of order (eg. not hash tables
     *      or trees), and is incompatible with the idea of subtypes like  SortedCollection<T>. Also
     *      since a Collection<> intrinsically has no ordering, I'm not totally sure what it would
     *      mean to Shake/change its ordering?
     *
     *  \note   EQUALS_COMPARER (operator==, operator!=)
     *          We do not provide a notion of 'Equals' or operator==, operator!=, because
     *          its not clear how to compare collections (no ordering defined, sometimes sorted, no equals defined for ELTS etc)
     *
     *          The caller may use the inherited (from Iterable<>) SetEquals, MultiSetEquals, or SequenceEquals()
     *          as appropriate. Methods that require and equals comparer, take one as argument with appropriate defaulting.
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *      static_assert (not equality_comparable<Collection<...>>);
     * 
     *      o   No comparisons are provided, because there is no intrinsic way to compare collections for equality, less etc. (order not defined)
     *          See inherited Iterable<>::SequentialEquals, Iterable<>::MultiSetEquals, , Iterable<>::SetEquals.
     */
    template <typename T>
    class [[nodiscard]] Collection : public Iterable<T> {
    private:
        using inherited = Iterable<T>;

    public:
        /**
         *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
         */
        using ArchetypeContainerType = Collection;

    public:
        /**
         *  @see inherited::value_type
         */
        using value_type = typename inherited::value_type;

    protected:
        class _IRep;

    public:
        /**
         *  For the CTOR overload with ITERABLE_OF_ADDABLE, its anything that supports c.begin(), c.end () to find
         *  all the elements, and which has elements (iterated) convertible to T.
         *
         *  \par Example Usage
         *      \code
         *          Sequence<int> s;
         *          std::vector<int> v;
         *
         *          Collection<int> c1  = {1, 2, 3};
         *          Collection<int> c2  = c1;
         *          Collection<int> c3{ c1 };
         *          Collection<int> c4{ c1.begin (), c1.end () };
         *          Collection<int> c5{ s };
         *          Collection<int> c6{ v };
         *          Collection<int> c7{ v.begin (), v.end () };
         *          Collection<int> c8{ move (c1) };
         *      \endcode
         *
         *  \note   Most other containers (e.g. Set<>, Sequence<>) have the 'ITERABLE_OF_ADDABLE&& src' CTOR be explicit, whereas Collection does not.
         *          This is because converting to a Set or Sequence has some semantics, and the programmer should be clear on this. But a Collection<>
         *          acts just like an Iterable<T> (except that its modifiable). So allow this case to be non-explicit.
         * 
         *  \note   <a href="ReadMe.md#Container Constructors">See general information about container constructors that applies here</a>
         */
        Collection ();
        Collection (Collection&&) noexcept      = default;
        Collection (const Collection&) noexcept = default;
        Collection (const initializer_list<value_type>& src);
        template <IIterableOf<T> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Collection<T>>)
        Collection (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : Collection{}
        {
            AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            _AssertRepValidType ();
        }
#endif
        ;

        template <IInputIterator<T> ITERATOR_OF_ADDABLE, sentinel_for<remove_cvref_t<ITERATOR_OF_ADDABLE>> ITERATOR_OF_ADDABLE2>
        Collection (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE2&& end);

    protected:
        explicit Collection (shared_ptr<_IRep>&& src) noexcept;
        explicit Collection (const shared_ptr<_IRep>& src) noexcept;

    public:
        nonvirtual Collection& operator= (Collection&&) noexcept = default;
        nonvirtual Collection& operator= (const Collection&)     = default;

    public:
        /**
         * \brief Compares items with TRAITS::EqualsCompareFunctionType::Equals, and returns true if any match.
         */
        template <Common::IEqualsComparer<T> EQUALS_COMPARER = equal_to<T>>
        nonvirtual bool Contains (ArgByValueType<value_type> item, EQUALS_COMPARER&& equalsComparer = {}) const;

    public:
        /**
         * Add the given item(s) to this Collection<T>. Note - if the given items are already present, another
         * copy will be added. No promises are made about where the added value will appear in iteration.
         *
         *  \note overload taking Iterator<T>* addedAt returns an iterator pointing at the added item.
         *
         *  \note mutates container
         */
        nonvirtual void Add (ArgByValueType<value_type> item);
        nonvirtual void Add (ArgByValueType<value_type> item, Iterator<T>* addedAt);

    public:
        /**
         *  \note   AddAll/2 is alias for .net AddRange ()
         * 
         *  \note mutates container
         */
        template <IInputIterator<T> ITERATOR_OF_ADDABLE, sentinel_for<remove_cvref_t<ITERATOR_OF_ADDABLE>> ITERATOR_OF_ADDABLE2>
        nonvirtual void AddAll (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE2&& end);
        template <IIterableOf<T> ITERABLE_OF_ADDABLE>
        nonvirtual void AddAll (ITERABLE_OF_ADDABLE&& items);

    public:
        /**
         * This function requires that the iterator 'i' came from this container.
         *
         * The value pointed to by 'i' is updated - replaced with the value 'newValue'.
         * 
         * \note if you update the container, and wish to continue iterating, you must pass in an &i to nextI,
         *       and continue using that iterator (i is invalid after this call). It is left undefined, whether or not you
         *       will encounter the item 'i' again in iteration (for example, if the container is sorted, and you change
         *       the associated value to larger, you probably will encounter it again).
         * 
         *  \note - this nextI value is therefore of very little value, since you cannot reliably continue iterating with it, knowing
         *        what you will get next. @todo consider if this should be deprecated.
         *        http://stroika-bugs.sophists.com/browse/STK-922
         * 
         *  This is basically the same as {Remove (i); Add (newValue);} except that it might perform better.
         *
         *  \note mutates container
         */
        nonvirtual void Update (const Iterator<value_type>& i, ArgByValueType<value_type> newValue, Iterator<value_type>* nextI = nullptr);

    public:
        /**
         *  \brief Remove () the argument value (which must exist)
         *
         *  \see RemoveIf() to remove an item that may not exist
         * 
         *  This will reduce the size of the container by one.
         *
         *  \param nextI - if provided (not null) - will be filled in with the next value after where iterator i
         *         is pointing - since i is invalidated by changing the container)
         *
         *   \note mutates container
         */
        template <Common::IPotentiallyComparer<T> EQUALS_COMPARER = equal_to<T>>
        nonvirtual void Remove (ArgByValueType<value_type> item, EQUALS_COMPARER&& equalsComparer = {});
        nonvirtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI = nullptr);

    public:
        /**
         *  \brief RemoveIf () the (the first matching) argument value, if present. Returns true if item removed.
         *
         * It is legal to remove something that is not there. This function removes the first instance of item
         * (or each item for the 'items' overload), meaning that another instance of item could still be in the
         * Collection<T> after the remove.
         *
         * The overload Remove(PREDICATE) applies the function p(T) -> bool and deletes the first entry (if any) that return true for the predicate.
         * 
         *  \note mutates container
         */
        template <Common::IEqualsComparer<T> EQUALS_COMPARER = equal_to<T>>
        nonvirtual bool RemoveIf (ArgByValueType<value_type> item, EQUALS_COMPARER&& equalsComparer = {});
        template <predicate<T> PREDICATE>
        nonvirtual bool RemoveIf (PREDICATE&& p);

    public:
        /**
         *  \brief RemoveAll removes all, or all matching (predicate, iterator range, equals comparer or whatever) items.
         * 
         *  The no-arg overload removes all (quickly).
         * 
         *  The overloads that remove some subset of the items returns the number of items so removed.
         * 
         *  The overload with Iterator<T> arguments (start/end) must be iterators from this container.
         *
         *  \note mutates container
         */
        nonvirtual void RemoveAll ();
        template <Common::IEqualsComparer<T> EQUALS_COMPARER = equal_to<T>>
        nonvirtual size_t RemoveAll (const Iterator<value_type>& start, const Iterator<value_type>& end, EQUALS_COMPARER&& equalsComparer = {});
        template <IIterableOf<T> ITERABLE_OF_ADDABLE, typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual size_t RemoveAll (const ITERABLE_OF_ADDABLE& c, EQUALS_COMPARER&& equalsComparer = {});
        template <predicate<T> PREDICATE>
        nonvirtual size_t RemoveAll (PREDICATE&& p);

    public:
        /**
         * \brief 'override' Iterable<>::Map () function so RESULT_CONTAINER defaults to Collection, and improve that case to clone properties from this rep (such is rep type, comparisons etc).
         */
        template <typename RESULT_CONTAINER = Collection<T>, invocable<T> ELEMENT_MAPPER>
        nonvirtual RESULT_CONTAINER Map (ELEMENT_MAPPER&& elementMapper) const
            requires (convertible_to<invoke_result_t<ELEMENT_MAPPER, T>, typename RESULT_CONTAINER::value_type> or
                      convertible_to<invoke_result_t<ELEMENT_MAPPER, T>, optional<typename RESULT_CONTAINER::value_type>>);

    public:
        /**
         *  \brief return a subset of the Collection for which includeIfTrue returns true.
         * 
         *  Defaults to returning a Collection<>, but pass in Iterable<T> type argument to get a lazy-evaluated iterable result.
         *
         *  \par Example Usage
         *      \code
         *          Collection<int> c { 1, 2, 3, 4, 5, 6 };
         *          EXPECT_TRUE (c.Where ([] (int i) { return i % 2 == 0; }).SetEquals (Iterable<int> { 2, 4, 6 }));
         *          EXPECT_TRUE (c.Where<Iterable<int>> ([] (int i) { return i % 2 == 0; }).SetEquals (Iterable<int> { 2, 4, 6 })); // to get lazy evaluation
         *      \endcode
         *
         */
        template <derived_from<Iterable<T>> RESULT_CONTAINER = Collection<T>, predicate<T> INCLUDE_PREDICATE>
        nonvirtual RESULT_CONTAINER Where (INCLUDE_PREDICATE&& includeIfTrue) const;

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
        template <Common::IEqualsComparer<T> EQUALS_COMPARER = equal_to<T>>
        nonvirtual void erase (ArgByValueType<value_type> item, EQUALS_COMPARER&& equalsComparer = {});
        nonvirtual Iterator<value_type> erase (const Iterator<value_type>& i);

    public:
        /**
         *  operator+ is syntactic sugar on Add() or AddAll() - depending on the overload.
         *
         *  *DEVELOPER NOTE*
         *      Note - we use an overload
         *      of Collection<T> for the container case instead of a template, because I'm not sure how to use specializations
         *      to distinguish the two cases. If I can figure that out, this can transparently be
         *      replaced with operator+= (X), with appropriate specializations.
         *
         *  \note mutates container
         */
        nonvirtual Collection& operator+= (ArgByValueType<value_type> item);
        nonvirtual Collection& operator+= (const Iterable<value_type>& items);

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
     *  \brief  Implementation detail for Collection<T> implementors.
     *
     *  Protected abstract interface to support concrete implementations of
     *  the Collection<T> container API.
     */
    template <typename T>
    class Collection<T>::_IRep : public Iterable<T>::_IRep {
    private:
        using inherited = typename Iterable<T>::_IRep;

    protected:
        _IRep () = default;

    public:
        virtual ~_IRep () = default;

    public:
        virtual shared_ptr<_IRep> CloneEmpty () const                                   = 0;
        virtual shared_ptr<_IRep> CloneAndPatchIterator (Iterator<value_type>* i) const = 0;
        // if oAddedI != nullptr, on output, its filled in with iterator pointing to added item
        virtual void Add (ArgByValueType<value_type> item, Iterator<value_type>* oAddedI)                            = 0;
        virtual void Update (const Iterator<value_type>& i, ArgByValueType<T> newValue, Iterator<value_type>* nextI) = 0;
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI)                             = 0;
    };

    /**
     *  Basic operator overload with the obvious meaning (Collection<T> copy and Collection<T>::AddAll)
     */
    template <typename T>
    Collection<T> operator+ (const Iterable<T>& lhs, const Collection<T>& rhs);
    template <typename T>
    Collection<T> operator+ (const Collection<T>& lhs, const Iterable<T>& rhs);
    template <typename T>
    Collection<T> operator+ (const Collection<T>& lhs, const Collection<T>& rhs);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Collection.inl"

#endif /*_Stroika_Foundation_Containers_Collection_h_ */
