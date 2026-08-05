/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Sequence_h_
#define _Stroika_Foundation_Containers_Sequence_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <compare>
#include <limits>

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Compare.h"
#include "Stroika/Foundation/Common/Concepts.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Memory/SharedByValue.h"
#include "Stroika/Foundation/Traversal/Iterable.h"
#include "Stroika/Foundation/Traversal/RandomAccessIterator.h"

/*
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo       Provide Slice () overload to mask inherited one from Iterable, but more efficient, and returning
 *                  Sequence (not Iterable<T>). Mention alias 'SubSequence' from older todo. Now cheap to do
 *                  efficiently, given GetAt ()/MakeRandomAccessIterator (). Consider also supporting the negative
 *                  and optional index arguments that Iterable<T>::Slice () explicitly defers to this subclass.
 *
 *                  This subsumes the (much older) idea of being able to start a Sequence at an arbitrary point and
 *                  end it at an arbitrary point.
 *
 *      @todo       Stroika v1 had REVERSE_ITERATORS - and so does STL. At least for sequences, we need reverse iterators!
 *                  NOTE - this is NOT a special TYPE of iterator (unlike STL). Its just iterator returned from rbegin(), rend().
 *
 *                  Most of the work for this is now done - MakeBidirectionalIterator () exists - so what remains is
 *                  deciding what rbegin ()/rend () return, and how that interacts with AtEnd ()/sentinel comparison.
 *
 *      @todo       Add lowercase STL-style insert(Iterator<T>,T) overload, forwarding to Insert (Iterator<T>,T) (which
 *                  already handles the 'i == end ()' means append case). Needed so Mapping<>::As<Sequence<...>> () works
 *                  - see the note on Mapping<>::As<> () about requiring an insert(ITERATOR,Value) method.
 *
 *      @todo       Document that though comparing an iterator with CONTAINER.end () works fine with Stroika iterators,
 *                  other comparisons do not: begin ()/end () are forward-only Iterator<T>, so 'i < s.end ()' and -
 *                  more importantly - 'i - s.begin ()' don't compile. Document that MakeRandomAccessIterator () is how
 *                  you get an iterator supporting difference and ordering.
 *
 *      @todo       Add backend implementation of Sequence<T> using Sequence_stdlist<>. Low priority - Sequence_LinkedList
 *                  and Sequence_DoublyLinkedList already cover the linked-list shape.
 *
 *      @todo       Sequence<T> (ITERABLE_OF_ADDABLE), Sequence<T> (ITER iFrom, ITER iTo), and AppendAll () currently
 *                  append one element at a time (a span of size 1 per _IRep::Insert () call). But _IRep::Insert () takes
 *                  a span<const value_type>, and every backend implements the bulk case well (DataStructures::Array<T>
 *                  does a single ReserveAtLeast (); Sequence_stdvector uses ReserveTweaks::Reserve4AddN ()) - so for
 *                  contiguous or sized sources we should batch into one Insert () call rather than N.
 */

namespace Stroika::Foundation::Containers {

    using Common::ArgByValueType;
    using Common::IPotentiallyComparer;
    using Traversal::BidirectionalIterator;
    using Traversal::IInputIterator;
    using Traversal::IIterableOfTo;
    using Traversal::Iterable;
    using Traversal::Iterator;
    using Traversal::RandomAccessIterator;

    /**
     *  \brief A generalization of a vector: a container whose elements are keyed by the natural numbers.
     *
     *      SmallTalk book page 153
     *
     * TODO:
     *
     *  ->  Add SetLength() method. Make sure it is optimally efficient, but try
     *      to avoid introducing a virtual function. Probably overload, and 1 arg
     *      version will use T default CTOR. If done non-virtually with templates
     *      then we only require no arg CTOR when this function called - GOOD.
     *
     * Notes:
     *
     *      Note: the decision on arguments to a Sort() function was difficult.
     *  Making the arg default to op <= would not work since for type int it
     *  wouldn't be defined, and sometimes people define it as a member function,
     *  or taking const T& args. Thus the function pointer type would not match.
     *  The other alternative is to overload, and have the no arg function just
     *  have a static private CompareFunction that calls op<=. This does work
     *  pretty well, BUT it fails in cases like Sequence(Picture) where there
     *  is no op<= defined. Here, we could force the definition of this function,
     *  but that would be generally awkward and was judged not worth the trouble.
     *  Just define your own little compare function that does op <=. That simple.
     *
     *      The other approach sterl's been pushing is that of functional objects
     *  described in Coplain, and the latest Stroustrup book (Nov 91). I haven't
     *  looked closely enuf to decide.
     *
     *      Another important addition was the CurrentIndex method. This was
     *  decided since it allowed for easy filtering (like only third thru eight
     *  elements, or only odd elements) without keeping an extra index variable
     *  which was often very awkward. This feature will probably be seldom used,
     *  and is seldom needed, but is one of the few things that differentiate
     *  a SequenceForEach from a Sequence (ie SequenceIterator from
     *  CollectionIterator). This statement really comes down to our really only
     *  needing sequence iterators rarely, and mostly using CollectionIterators.
     *
     *  \note   \em Iterators
     *      Note that iterators always run in Sequence order, from smallest index
     *      to largest. Items inserted before the current iterator index will not
     *      be encountered, and items inserted after the current index will be encountered.
     *      Items inserted at the current index remain undefined if they will
     *      be encountered or not.
     *
     *  \em Concrete Implementations:
     *      o   @see Concrete::Sequence_Array<>
     *      o   @see Concrete::Sequence_DoublyLinkedList<>
     *      o   @see Concrete::Sequence_LinkedList<>
     *      o   @see Concrete::Sequence_stdvector<>
     * 
     *  \em Factory:
     *      @see Sequence_Factory<> to see default implementations.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  \note   Design Note - TRAITS for equals versus COMPARER template param to methods that need it
     *      We experimented (until Stroika 2.0a20 apx) with using TRAITS that were optional
     *      with Sequence<> - and had the equals comparer. This worked OK. The advantage of
     *      having the 'equals' method in the TRAITS was that it assured (for a given instance of Sequence)
     *      that all comparisons/notions of equality were tied to the instance.
     *
     *      The idea WAS that you could even have a comparer that stored data in the instance (we never implemented that).
     *
     *      But this notion of equals has problems for defining Sequence<>::Equals() - do you use the
     *      one from the RHS or LHS?
     *
     *      Plus - making it a template param just added to the syntactic garbage in the template
     *      names (like in the debugger how the names printed out). This is no biggie, but
     *      it wasn't a plus.
     *
     *      So now (as of v2.0a20) - we just have the EQUALS_COMPARER be a templated param to the
     *      methods that need it.
     * 
     *  \note   See <a href="./ReadMe.md">ReadMe.md</a> for common features of all Stroika containers (especially
     *          constructors, iterators, etc)
     *
     *  \note <a href="Design-Overview.md#Comparisons">Comparisons</a>:
     *        o static_assert (equality_comparable<T> ==> equality_comparable<Sequence<T>>);
     *        o static_assert (totally_ordered<T> ==> totally_ordered<Sequence<T>>);
     *        o using EqualsComparer = typename Iterable<T>::template SequentialEqualsComparer<T_EQUALS_COMPARER>;
     *        o using ThreeWayComparer = typename Iterable<T>::template SequentialThreeWayComparer<T_EQUALS_COMPARER>;
     */
    template <typename T>
    class Sequence : public Iterable<T> {
    private:
        using inherited = Iterable<T>;

    protected:
        class _IRep;

    public:
        /**
         *  @see inherited::value_type
         */
        using value_type = typename inherited::value_type;

    public:
        /**
         *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
         */
        using ArchetypeContainerType = Sequence;

    public:
        /**
         *  For the CTOR overload with ITERABLE_OF_ADDABLE, its anything that supports c.begin(), c.end () to find
         *  all the elements.
         *
         *  \note   <a href="ReadMe.md#Container Constructors">See general information about container constructors that applies here</a>
         *
         *  \par Example Usage
         *      \code
         *        Collection<int> c;
         *        std::vector<int> v;
         *
         *        Sequence<int> s1  = {1, 2, 3};
         *        Sequence<int> s2  = s1;
         *        Sequence<int> s3{ s1 };
         *        Sequence<int> s4{ s1.begin (), s1.end () };
         *        Sequence<int> s5{ c };
         *        Sequence<int> s6{ v };
         *        Sequence<int> s7{ v.begin (), v.end () };
         *        Sequence<int> s8{ move (s1) };
         *      \endcode
         */
        Sequence ();
        Sequence (Sequence&&) noexcept      = default;
        Sequence (const Sequence&) noexcept = default;
        Sequence (const initializer_list<value_type>& src);
        template <IIterableOfTo<T> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Sequence<T>>)
        explicit Sequence (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : Sequence{}
        {
            AppendAll (forward<ITERABLE_OF_ADDABLE> (src));
            _AssertRepValidType ();
        }
#endif
        ;
        template <IInputIterator<T> ITERATOR_OF_ADDABLE, sentinel_for<remove_cvref_t<ITERATOR_OF_ADDABLE>> ITERATOR_OF_ADDABLE2>
        Sequence (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE2&& end);

    protected:
        explicit Sequence (shared_ptr<_IRep>&& rep) noexcept;
        explicit Sequence (const shared_ptr<_IRep>& rep) noexcept;

    public:
        /**
         */
        nonvirtual Sequence& operator= (Sequence&&) noexcept = default;
        nonvirtual Sequence& operator= (const Sequence&)     = default;

    public:
        /**
         * \brief Create a BidirectionalIterator for this sequence (note inherited Iterable<T>::MakeIterator () returns a forward iterator, not a random access or bidirectional iterator).
         *
         *  \note that though this always 'works' - the returned iterator maybe quite inefficient
         *        depending on the backend implementation type.
         */
        nonvirtual BidirectionalIterator<T> MakeBidirectionalIterator () const;

    public:
        /**
         * \brief Create a RandomAccessIterator for this sequence (note inherited Iterable<T>::MakeIterator () returns a forward iterator, not a random access iterator).
         *
         *  \note that though this always 'works' - the returned iterator maybe quite inefficient
         *        depending on the backend implementation type (like GetAt ()).
         */
        nonvirtual RandomAccessIterator<T> MakeRandomAccessIterator () const;

    public:
        /**
         * \brief 'override' Iterable<>::Map () function so RESULT_CONTAINER defaults to Sequence, and improve that case to clone properties from this rep (such is rep type, etc).
         */
        template <typename RESULT_CONTAINER = Sequence<T>, invocable<T> ELEMENT_MAPPER>
        nonvirtual RESULT_CONTAINER Map (ELEMENT_MAPPER&& elementMapper) const
            requires (convertible_to<invoke_result_t<ELEMENT_MAPPER, T>, typename RESULT_CONTAINER::value_type> or
                      convertible_to<invoke_result_t<ELEMENT_MAPPER, T>, optional<typename RESULT_CONTAINER::value_type>>);

    public:
        /**
         *  Apply the function function to each element, and return all the ones for which it was true.
         *
         *  @see Iterable<T>::Where
         */
        template <derived_from<Iterable<T>> RESULT_CONTAINER = Sequence<T>, predicate<T> INCLUDE_PREDICATE>
        nonvirtual RESULT_CONTAINER Where (INCLUDE_PREDICATE&& includeIfTrue) const;

    public:
        /**
         *
         *  \par Example Usage
         *      \code
         *          Sequence<int> c { 3, 5, 9, 38, 3, 5 };
         *          EXPECT_TRUE (c.OrderBy () == Sequence<int> { 3, 3, 5, 5, 9, 38 });
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          Sequence<int> c { 3, 5, 9, 38, 3, 5 };
         *          EXPECT_TRUE (c.OrderBy ([](int lhs, int rhs) -> bool { return lhs < rhs; }) == Sequence<int> { 3, 3, 5, 5, 9, 38 });
         *      \endcode
         *
         *  \note hides Iterable<T>::OrderBy since provides more specific types
         *
         *  \note This performs a stable sort (preserving the relative order of items that compare equal).
         *        That maybe less performant than a regular (e.g. quicksort) but works better as a default, in most cases, as it allows combining multi-level sorts.
         *
         *  @aliases Sort ()
         *
         *  \note Should be of type IInOrderComparer, but not required - for convenience of use (so can be used with any lambda functor)
         */
        template <IPotentiallyComparer<T> INORDER_COMPARER_TYPE = less<T>>
        nonvirtual Sequence OrderBy (INORDER_COMPARER_TYPE&& inorderComparer = INORDER_COMPARER_TYPE{}) const;

    public:
        /**
         * simply indirect to @Iterable<T>::SequentialEqualsComparer
         *
         *  A Sequence<T> doesn't generally require a comparison for individual elements
         *  be be defined, but obviously to compare if the containers are equal, you must
         *  compare the individual elements (at least sometimes).
         *
         *  If operator==(T,T) is predefined, you can just call:
         *  \par Example Usage
         *      \code
         *          Sequence<int> a, b;
         *          if (a == b) {
         *          }
         *      \endcode
         *
         *  or
         *      \code
         *          Sequence<int> a, b;
         *          if (Sequence<int>::EqualsComparer{eltComparer} (a, b)) {
         *          }
         *      \endcode
         *
         *  to compare with an alternative comparer.
         */
        template <Common::IEqualsComparer<T> T_EQUALS_COMPARER = equal_to<T>>
        using EqualsComparer = typename Iterable<value_type>::template SequentialEqualsComparer<T_EQUALS_COMPARER>;

    public:
        /**
         */
        template <typename ELEMENT_COMPARER = compare_three_way>
        using ThreeWayComparer = typename Iterable<value_type>::template SequentialThreeWayComparer<ELEMENT_COMPARER>;

    public:
        /**
         * simply indirect to @Sequence<>::EqualsComparer
         */
        nonvirtual bool operator== (const Sequence& rhs) const
            requires (equality_comparable<T>);

    public:
        /**
         * simply indirect to @Sequence<>::operator
         */
        nonvirtual auto operator<=> (const Sequence& rhs) const
            requires (three_way_comparable<T>);

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
        nonvirtual void RemoveAll ();
        template <predicate<T> PREDICATE>
        nonvirtual size_t RemoveAll (PREDICATE&& p);

    public:
        /**
         *  \pre i < size ()
         */
        nonvirtual value_type GetAt (size_t i) const;

    public:
        /**
         *  \pre i < size ()
         *
         *  \note mutates container
         */
        nonvirtual void SetAt (size_t i, ArgByValueType<value_type> item);

    private:
        struct TemporaryElementReference_;

    public:
        /**
         *  \brief alias for GetAt (i) - but returning const T
         * 
         *  @aliases GetAt
         * 
         *  \pre i < size ().
         *
         *  \note this returns const value_type, so you cannot accidentally assign to the result
         *          a[3] = 4; // wont compile because return type const
         * 
         *  \note - we investigated having operator[] (int) return TemporaryElementReference_, but the trouble
         *        is that, though this almost works, the COST of TemporaryElementReference_ is much higher than
         *        returning the cost value_type, and its not super obvious at call point that you are getting the
         *        expensive or inexpensive version (depends on constness of this pointer).
         * 
         *        Also considered having this return T&, the way you would with std c++ vector (etc). This would avoid a lot
         *        of issues. BUT - it would BREAK the COW (copy-on-write) semantics. Consider if we had a single
         *        reference to a sequence. And we grab the value_type& (to update it; this doesn't increase refCnt for container). Then in another thread,
         *        we access the sequence (incrementing its reps ref count). We could be updating through that saved
         *        reference to T while the other thread is looking at the sequence - a dangerous race.
         * 
         *        So because all of this, use the syntax a(3) instead of a[3] if you want a modifiable reference
         *        (to call non-const methods on or to assign to).
         */
        nonvirtual const value_type operator[] (size_t i) const;

    public:
        /**
         *  \brief operator() is similar to operator[] - but returning TemporaryElementReference_, and so is updatable/writable
         * 
         * See the notes on operator[]. The semantics of this method are similar to operator[],
         * except that it returns a proxy object which allows (immediately) updating the element of the sequence.
         * 
         *  \par Example Usage
         *      \code
         *          Sequence<T> s = ...;
         *          s(2) = 4;       // s[2] = 4;    WONT COMPILE
         *      \endcode
         *      
         *      is equivalent to
         *      \code
         *          auto t = s.GetAt (2);
         *          t = 4;
         *          s.SetAt (2, i);     // note when using operator() - this SetAt() dont even if t not updated so wasteful unless updating
         *      \endcode
         * 
         *  \pre i < size ()
         *
         *  \note mutates container
         */
        nonvirtual TemporaryElementReference_ operator() (size_t i);

    public:
        /**
         *      Search the sequence and see if the given item is contained in
         *  it, and return the index of that item. Comparison is done with
         *  TRAITS::EqualsCompareFunctionType (which defaults to operator== (T, T))
         *  for first two overloads - third taking iterator always works)
         *
         *  Note that the IndexOf(Iterator<T>) overload ignores the EQUALS_COMPARER
         *  but still must be a template method because non-template methods
         *  cannot be overloaded with template members.
         *
         *  If not found for the by value overloads, IndexOf () return {};
         *  For the IndexOf(Iterator<T>) - \pre it is found/legal iterator 
         */
        template <Common::IEqualsComparer<T> EQUALS_COMPARER = equal_to<T>>
        nonvirtual optional<size_t> IndexOf (ArgByValueType<value_type> i, EQUALS_COMPARER&& equalsComparer = {}) const;
        template <Common::IEqualsComparer<T> EQUALS_COMPARER = equal_to<T>>
        nonvirtual optional<size_t> IndexOf (const Sequence& s, EQUALS_COMPARER&& equalsComparer = {}) const;
        template <typename IGNORED = void>
        nonvirtual size_t IndexOf (const Iterator<value_type>& i) const;

    public:
        /**
         *      Insert the given item into the sequence at the given index.
         *  Any active iterators will encounter the given item if their
         *  cursor encounters the new index in the course of iteration.
         *  Put another way, If you are iterating forwards, and you add an
         *  item after what you are up to you will hit it - if you are iterating
         *  backwards and you add an item before where you are, you will hit it -
         *  otherwise you will miss the added item during iteration.
         *
         *      NB: Adding an item at the CURRENT index has no effect on
         *  what the iterator says is the current item.
         *
         *  \note mutates container
         */
        nonvirtual void Insert (size_t i, ArgByValueType<value_type> item);
        nonvirtual void Insert (const Iterator<value_type>& i, ArgByValueType<value_type> item);

    public:
        /**
         *  \brief Insert all the given items into this sequence, starting at offset 'i'.
         *
         *  \pre IInputIterator<ITERATOR_OF_ADDABLE, T> or IIterableOfTo<ITERABLE_OF_ADDABLE, T>
         */
        template <IInputIterator<T> ITERATOR_OF_ADDABLE, sentinel_for<ITERATOR_OF_ADDABLE> ITERATOR_OF_ADDABLE2>
        nonvirtual void InsertAll (size_t i, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE2&& end);
        template <IIterableOfTo<T> ITERABLE_OF_ADDABLE>
        nonvirtual void InsertAll (size_t i, ITERABLE_OF_ADDABLE&& s);

    public:
        /**
         *  \note mutates container
         */
        nonvirtual void Prepend (ArgByValueType<value_type> item);

    public:
        /**
         *  \pre IInputIterator<ITERATOR_OF_ADDABLE, T> or IIterableOfTo<ITERABLE_OF_ADDABLE, T>
         *
         *  \note mutates container
         */
        template <IIterableOfTo<T> ITERABLE_OF_ADDABLE>
        nonvirtual void PrependAll (ITERABLE_OF_ADDABLE&& s);
        template <IInputIterator<T> ITERATOR_OF_ADDABLE>
        nonvirtual void PrependAll (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         *  This is roughly Insert (size(), item), except that there is a race after you call size, and before
         *  Insert, which calling Append () avoids.
         *
         *  \note mutates container
         */
        nonvirtual void Append (ArgByValueType<value_type> item);

    public:
        /**
         *  This is roughly AppendAll (size(), s), except that there is a race after you call size,
         *  and before Insert, which calling Append () avoids. Also note - if used in a multithreaded environment,
         *  the appended items wont necessarily all get appended at once, since other threads could make
         *  changes in between.
         *
         *  \note mutates container
         */
        template <IIterableOfTo<T> ITERABLE_OF_ADDABLE>
        nonvirtual void AppendAll (ITERABLE_OF_ADDABLE&& s);
        template <IInputIterator<T> ITERATOR_OF_ADDABLE, sentinel_for<remove_cvref_t<ITERATOR_OF_ADDABLE>> ITERATOR_OF_ADDABLE2>
        nonvirtual void AppendAll (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE2&& end);

    public:
        /**
         * This function requires that the iterator 'i' came from this container.
         *
         * The value pointed to by 'i' is updated - replaced with the value 'newValue'.
         *
         *  \param nextI - if provided (not null) - will be filled in with a valid iterator pointing where i is pointing - since i is invalidated by changing the container)
         * 
         *  \note - this differs from Collection::Update() (which advances *nextI); since for a sequence, there is no need to ever
         *          invalidate the current item on a removal (order doesnt change on update to a Sequence).
         *
         *  \note mutates container
         */
        nonvirtual void Update (const Iterator<value_type>& i, ArgByValueType<value_type> newValue, Iterator<value_type>* nextI = nullptr);

    public:
        /**
         *  This function requires that the iterator 'i' came from this container.
         *
         *  The value pointed to by 'i' is removed.
         *
         *  Remove the item at the given position of the sequence. Make sure
         *  that iteration is not disturbed by this removal. In particular, any
         *  items (other than the one at index) that would have been seen, will
         *  still be, and no new items will be seen that wouldn't have been.
         *
         *  \note mutates container
         */
        nonvirtual void Remove (size_t i);
        nonvirtual void Remove (size_t start, size_t end);
        nonvirtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI = nullptr);

    public:
        /*
         *  Convert Sequence<T> losslessly into a standard supported C++ type.
         *  Supported types include:
         *      o   vector<T>
         *      o   list<T>
         *      (maybe any container that takes CTOR (IT BEGIN, IT END) - but don't count on that yet...
         */
        template <typename CONTAINER_OF_ADDABLE>
        nonvirtual CONTAINER_OF_ADDABLE As () const;
        template <typename CONTAINER_OF_ADDABLE>
        nonvirtual void As (CONTAINER_OF_ADDABLE* into) const;

    public:
        /**
         *  @see Iterable<T>::First ()
         */
        nonvirtual optional<value_type> First () const;
        nonvirtual optional<value_type> First (const function<bool (ArgByValueType<value_type>)>& that) const;

    public:
        /**
         *  @see Iterable<T>::FirstValue ()
         */
        nonvirtual value_type FirstValue (ArgByValueType<value_type> defaultValue = {}) const;

    public:
        /**
         *  @see Iterable<T>::Last ()
         */
        nonvirtual optional<value_type> Last () const;
        nonvirtual optional<value_type> Last (const function<bool (ArgByValueType<value_type>)>& that) const;

    public:
        /**
         *  @see Iterable<T>::LastValue ()
         */
        nonvirtual value_type LastValue (ArgByValueType<value_type> defaultValue = {}) const;

    public:
        /**
         *  @aliases Append
         * 
         *  \note mutates container
         */
        nonvirtual void push_back (ArgByValueType<value_type> item);

    public:
        /**
         *  Read the last element (GetLast()). Requires not empty.
         */
        nonvirtual value_type back () const;

    public:
        /**
         */
        nonvirtual value_type front () const;

    public:
        /**
         * @aliases RemoveAll ().
         */
        nonvirtual void clear ();

    public:
        /**
         * @aliases Remove ().
         *
         *  \note mutates container
         */
        nonvirtual void erase (size_t i);
        nonvirtual Iterator<value_type> erase (const Iterator<value_type>& i);

    public:
        /**
         * \brief Alias for Append/AppendAll ().
         */
        nonvirtual Sequence& operator+= (ArgByValueType<value_type> item);
        nonvirtual Sequence& operator+= (const Sequence& items);

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
     *  \brief  Implementation detail for Sequence<T> implementors.
     *
     *  Protected abstract interface to support concrete implementations of
     *  the Sequence<T> container API.
     */
    template <typename T>
    class Sequence<T>::_IRep : public Iterable<T>::_IRep {
    private:
        using inherited = typename Iterable<T>::_IRep;

    protected:
        _IRep () = default;

    public:
        virtual ~_IRep () = default;

    protected:
        static constexpr size_t _kSentinelLastItemIndex = numeric_limits<size_t>::max ();

    public:
        virtual shared_ptr<_IRep> CloneEmpty () const                                   = 0;
        virtual shared_ptr<_IRep> CloneAndPatchIterator (Iterator<value_type>* i) const = 0;
        // 'i' argument to GetAt MAYBE kBadSequenceIndex - indicating last element
        virtual value_type GetAt (size_t i) const                                                                                   = 0;
        virtual void       SetAt (size_t i, ArgByValueType<value_type> item)                                                        = 0;
        virtual size_t     IndexOf (const Iterator<value_type>& i) const                                                            = 0;
        virtual void       Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI)                                      = 0;
        virtual void       Update (const Iterator<value_type>& i, ArgByValueType<value_type> newValue, Iterator<value_type>* nextI) = 0;
        // 'at' argument to Insert MAYBE kBadSequenceIndex - indicating append
        virtual void Insert (size_t at, const span<const value_type>& copyFrom) = 0;
        virtual void Remove (size_t from, size_t to)                            = 0;
        // implementors without a more efficient backend-specific iterator available can just return
        // _MakeBidirectionalIterator_ViaGetAt () / _MakeRandomAccessIterator_ViaGetAt ()
        virtual BidirectionalIterator<value_type> GetBidirectionalIterator () const = 0;
        virtual RandomAccessIterator<value_type>  GetRandomAccessIterator () const  = 0;

    protected:
        /**
         *  \brief Generic (backend-independent) implementations of GetBidirectionalIterator (),
         *         implemented purely in terms of GetAt ()/size ().
         *
         *  Intended to be called from a concrete Sequence_* backend's GetBidirectionalIterator ()
         *  override, for backends that don't (yet) have a more efficient backend-specific implementation available.
         */
        nonvirtual BidirectionalIterator<T> _MakeBidirectionalIterator_ViaGetAt () const;

    protected:
        /**
         *  \brief Generic (backend-independent) implementations of GetRandomAccessIterator (),
         *         implemented purely in terms of GetAt ()/size ().
         *
         *  Intended to be called from a concrete Sequence_* backend's GetRandomAccessIterator ()
         *  override, for backends that don't (yet) have a more efficient backend-specific implementation available.
         */
        nonvirtual RandomAccessIterator<T> _MakeRandomAccessIterator_ViaGetAt () const;

    private:
        /**
         *  \brief Generic RandomAccessIterator<T>::IRep implementation, usable with ANY Sequence<T> backend,
         *         implemented purely in terms of GetAt ()/size (). Backs _MakeBidirectionalIterator_ViaGetAt ()/_MakeRandomAccessIterator_ViaGetAt ().
         */
        class IndexBasedRandomAccessIteratorRep_;

    private:
        friend Sequence<T>;
    };

    /**
     *  Basic operator overload with the obvious meaning (Sequence<T> copy and Sequence<T>::AppendAll)
     */
    template <typename T>
    Sequence<T> operator+ (const Iterable<T>& lhs, const Sequence<T>& rhs);
    template <typename T>
    Sequence<T> operator+ (const Sequence<T>& lhs, const Iterable<T>& rhs);
    template <typename T>
    Sequence<T> operator+ (const Sequence<T>& lhs, const Sequence<T>& rhs);

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "Sequence.inl"

#endif /*_Stroika_Foundation_Containers_Sequence_h_ */
