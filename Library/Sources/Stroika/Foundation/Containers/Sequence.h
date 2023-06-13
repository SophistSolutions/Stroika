/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Sequence_h_
#define _Stroika_Foundation_Containers_Sequence_h_ 1

#include "../StroikaPreComp.h"

#include <compare>
#include <limits>

#include "../Common/Compare.h"
#include "../Configuration/Common.h"
#include "../Configuration/Concepts.h"
#include "../Memory/SharedByValue.h"
#include "../Traversal/Iterable.h"
#include "../Traversal/RandomAccessIterator.h"

#include "Common.h"

/*
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo       Add/FIX three-way-comparer support - operator<=> etc - for Sequence. I Think it can be
 *                  easily defined as operator<> on ELT, followed by operator<> on value and then as a string
 *                  of those operations.
 *
 *      @todo       Provide Slice () overload to mask inherited one from Iterable, but more efficient, and return
 *                  sequence. Mention alias 'SubSequence' from older todo.
 *
 *      @todo       Stroika v1 had REVERSE_ITERATORS - and so does STL. At least for sequences, we need reverse iterators!
 *                  NOTE - this is NOT a special TYPE of itearator (unlike STL). Its just iterator returned from rbegin(), rend().
 *
 *      @todo       Sequence<> must support RandomAccessIterator<>
 *
 *      @todo       Where() and probably other things should use new EmptyClone() strategy - so cheaper and
 *                  returns something of same underlying data structure  type.
 *
 *      @todo       Add insert(Iterator<T>,T) overload (so works with Mapping<..>::As<...> ()
 *
 *      @todo       Must support Iterator<T>::operator-(Iterator<T>) or some-such so that SequenceIterator must work with qsort().
 *                  In other words, must act as random-access iterator so it can be used in algorithjms that use STL
 *                  random-access iterators. (FOLLOW RULES OF RANDOM ACCESS ITERAOTRS)
 *
 *                  std::iterator<input_iterator_tag, T> versus ?? other iterator tag?
 *
 *      @todo       Maybe add (back) SequenceIterator - with support for operator- (difference), and UpdateCurrent, and GetIndex()
 *                  Maybe also AdvanceBy(), BackBy() methods. Though All these COULD be methods of the underlying Sequence object.
 *
 *      @todo       Implement stuff like Contains () using ApplyUnti.... and lambdas, so locking works cheaply, and
 *                  so no virtual references to operator== - so can always create Sequence<T> even if no operator== defined
 *                  for T.
 *
 *      @todo       Assure well documetned that Stroika 1.0 mutators are replaced with modifocations directly on the container,
 *                  taking the iterator as argument!
 *
 *      @todo       Document and Consider that though iterator compares with CONTAINER.end () work fine with Stroika iterators,
 *                  other comparisons fail. For example, i < it.end (); and more importantly, constructs like i-s.begin() fail.
 *
 *                  Consider  if we can make this work, or document why and that we cannot. Maybe we need a concept of
 *                  SequenceIterator (like we had in Stroika 1) - which adds operator-?
 *
 *      @todo       Add Sequence_SparseArray<T> - using btree implementation
 *                  it requires there is an empty T CTOR. But then uses btree to store values when they differ from T()
 *                  (implement using redback tree or using stl map<>)
 *
 *      @todo       Add backend implementation of Sequence<T> using and Sequence_stdlist<>
 *
 *      @todo       Make sure that Sequence<T> (vector<T>) CTOR reserves the appropriate size before appending,
 *                  by using type_traits logic to figure out of legal to compare - and see length. Same for
 *                  Sequence<T> (ITER iFrom, ITER iTo) - do re-allocate size if appropriate - can do diff
 *                  iTo-iFrom.
 */

namespace Stroika::Foundation::Containers {

    using Common::IPotentiallyComparer;
    using Configuration::ArgByValueType;
    using Configuration::ExtractValueType_t;
    using Traversal::Iterable;
    using Traversal::Iterator;

    /**
     *  Having Stroika_Foundation_Containers_Sequence_SupportProxyModifiableOperatorBracket work would be nice. For POD
     *  types, this isn't hard to do well. But for things like Sequence<String> x;, where we want x[1].c_str () to work,
     *  the only way I've found is to use a temp proxy subclassing from T. And that has costs over constant usage.
     *
     *  So unless I can find a better way, leave this off -- LGP 2017-02-22
     *
     *  Made some progress on this, but now the problem is that when you assign, like as:
     *
     *  Sequene<String> s;
     *
     *  String a = s[0];    // eror at runtime
     *
     *  cuz String&& CTOR gets called with arg TemporaryReference<String>, and so but the time TemporaryReference DTOR called, value has
     *  gone away. No obvious way to tell udnerlying value 'stolen' so maybe the sucblassing trick wont work after all.
     *
     *  @see https://stroika.atlassian.net/browse/STK-582
     */
#ifndef Stroika_Foundation_Containers_Sequence_SupportProxyModifiableOperatorBracket
#define Stroika_Foundation_Containers_Sequence_SupportProxyModifiableOperatorBracket 0
#endif
#ifndef Stroika_Foundation_Containers_Sequence_SupportProxyModifiableOperatorOpenCloseParens
#define Stroika_Foundation_Containers_Sequence_SupportProxyModifiableOperatorOpenCloseParens 1
#endif

    /**
     *  \brief A generalization of a vector: a container whose elements are keyed by the natural numbers.
     *
     *      SmallTalk book page 153
     *
     *
     * TODO:
     *
     *  ->  At some point in the near future we may add the ability to start at an
     *      arbitrary point in a sequence, and end at an arbitrary point. This
     *      requires more thought though. That functionality is probably not too
     *      important in light of being able to compute the current index easily
     *      in an iteration. Also, it requires more thought how to fit in with
     *      the sequenceDirection. Do we have a seperate constructor speciing
     *      two start and endpoints and use their relative order to decide a
     *      direction? Do we just add the two start and end values to the end of
     *      the param list? How hard is this todo with Sequence_DLL?? If this
     *      functionality is subsumed by smart-iterators, does it make sense to
     *      wait to we provide that functionality?
     *
     *  ->  Figure out exactly what we will do about sorting/lookup function
     *      specification. Stroustrup like class param with somehow defaulting
     *      to op==????
     *
     *  ->  Add SetLength() method. Make sure it is optimally efficient, but try
     *      to avoid introducing a virtual function. Probably overload, and 1 arg
     *      version will use T default CTOR. If done nonvirtually with templates
     *      then we only require no arg CTOR when this function called - GOOD.
     *      Cannot really do with GenClass (would need to compile in seperate .o,
     *      even that wont work - need to not compile except when called).
     *
     *  ->  Consider patching iterators on insertions??? If not, document more
     *      clearly why not. Document exact details of patching in SEQUENCE as
     *      part of API!!!!
     *
     * Notes:
     *
     *      Note: the decsion on arguments to a Sort() function was difficult.
     *  Making the arg default to op <= would not work since for type int it
     *  wouldnt be defined, and sometimes people define it as a member function,
     *  or taking const T& args. Thus the function pointer type would not match.
     *  The other alternative is to overload, and have the no arg function just
     *  have a static private CompareFunction that calls op<=. This does work
     *  pretty well, BUT it fails in cases like Sequence(Picture) where there
     *  is no op<= defined. Here, we could force the definition of this function,
     *  but that would be genrally awkward and was jugdged not worth the trouble.
     *  Just define your own little compare function that does op <=. Thats simple.
     *
     *      The other approach sterl's been pushing is that of functional objects
     *  described in Coplain, and the latest Stroustrup book (Nov 91). I haven't
     *  looked closely enuf to decide.
     *
     *      Another imporant addition was the CurrentIndex method. This was
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
     *      it wasnt a plus.
     *
     *      So now (as of v2.0a20) - we just have the EQUALS_COMPARER be a templated param to the
     *      methods that need it.
     * 
     *  \note   See <a href="./ReadMe.md">ReadMe.md</a> for common features of all Stroika containers (especially
     *          constructors, iterators, etc)
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *        o Standard Stroika Comparison equality (==, <=> etc) - IFF equal_to (for ==/!=) and three_way_comparer<> (for the rest are defined
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
        using ArchetypeContainerType = Sequence<T>;

    public:
        /**
         *  \brief check if the argument type can be passed as argument to the arity/1 overload of Add (prepend/append)
         */
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v = is_convertible_v<POTENTIALLY_ADDABLE_T, T>;

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
         *        Sequence<int> s3  { s1 };
         *        Sequence<int> s4  { s1.begin (), s1.end () };
         *        Sequence<int> s5  { c };
         *        Sequence<int> s6  { v };
         *        Sequence<int> s7  { v.begin (), v.end () };
         *        Sequence<int> s8  { move (s1) };
         *      \endcode
         */
        Sequence ();
        Sequence (Sequence&& src) noexcept      = default;
        Sequence (const Sequence& src) noexcept = default;
        Sequence (const initializer_list<value_type>& src);

        template <ranges::range ITERABLE_OF_ADDABLE>
        explicit Sequence (ITERABLE_OF_ADDABLE&& src)
            requires (not derived_from<decay_t<ITERABLE_OF_ADDABLE>, Sequence<T>>)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : Sequence{}
        {
            static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
            AppendAll (forward<ITERABLE_OF_ADDABLE> (src));
            _AssertRepValidType ();
        }
#endif
        ;
        template <input_iterator ITERATOR_OF_ADDABLE>
        Sequence (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    protected:
        explicit Sequence (shared_ptr<_IRep>&& rep) noexcept;
        explicit Sequence (const shared_ptr<_IRep>& rep) noexcept;

    public:
        /**
         */
        nonvirtual Sequence& operator= (Sequence&& rhs) noexcept = default;
        nonvirtual Sequence& operator= (const Sequence& rhs)     = default;

    public:
        /**
         *  Apply the function function to each element, and return all the ones for which it was true.
         *
         *  @see Iterable<T>::Where
         */
        nonvirtual Sequence Where (const function<bool (ArgByValueType<value_type>)>& includeIfTrue) const;

    public:
        /**
         *
         *  \par Example Usage
         *      \code
         *          Sequence<int> c { 3, 5, 9, 38, 3, 5 };
         *          VerifyTestResult (c.OrderBy () == Sequence<int> { 3, 3, 5, 5, 9, 38 });
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          Sequence<int> c { 3, 5, 9, 38, 3, 5 };
         *          VerifyTestResult (c.OrderBy ([](int lhs, int rhs) -> bool { return lhs < rhs; }) == Sequence<int> { 3, 3, 5, 5, 9, 38 });
         *      \endcode
         *
         *  \note hides Iterable<T>::OrderBy since provides more specific types
         *
         *  \note This performs a stable sort (preserving the relative order of items that compare equal).
         *        That maybe less performant than a regular (e.g. quicksort) but works better as a default, in most cases, as it allows combining multi-level sorts.
         *
         *  \note alias for Sort ()
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
         * simply indirect to @Sequence<>::EqualsComparer (only defined if equal_to<T> is defined)
         */
        nonvirtual bool operator== (const Sequence& rhs) const;

    public:
        /**
         * simply indirect to @Sequence<>::operator (only defined if ???comparethreeway?<T> is defined)
         */
        nonvirtual auto operator<=> (const Sequence& rhs) const;

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
         *  \req i < size ()
         */
        nonvirtual value_type GetAt (size_t i) const;

    public:
        /**
         *  \req i < size ()
         *
         *  \note mutates container
         */
        nonvirtual void SetAt (size_t i, ArgByValueType<value_type> item);

    private:
        template <typename X, typename ENABLE = void>
        struct TemporaryElementReference_;

    public:
        /**
         *  \req i < size ().
         *
         *  \note - when using the non-const operator[] overload with a type T which is a struct/class, this may be significantly less efficient than
         *          just directly calling 'GetAt'.
         */
        nonvirtual value_type operator[] (size_t i) const;
#if Stroika_Foundation_Containers_Sequence_SupportProxyModifiableOperatorBracket
        nonvirtual TemporaryElementReference_<T> operator[] (size_t i);
#endif

#if Stroika_Foundation_Containers_Sequence_SupportProxyModifiableOperatorOpenCloseParens
    public:
        /**
         *  \req i < size ()
         *
         *  \note - variant returning TemporaryElementReference_ is EXPERIMENTAL as of 2017-02-21 - if Stroika_Foundation_Containers_Sequence_SupportProxyModifiableOperatorBracket
         *
         *  \note mutates container
         */
        nonvirtual TemporaryElementReference_<value_type> operator() (size_t i);
#endif

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
         *  For the IndexOf(Iterator<T>) - \req it is found/legal iterator 
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
         *  \req IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>
         *  \req IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>
         */
        template <input_iterator ITERATOR_OF_ADDABLE>
        nonvirtual void InsertAll (size_t i, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <ranges::range ITERABLE_OF_ADDABLE>
        nonvirtual void InsertAll (size_t i, ITERABLE_OF_ADDABLE&& s);

    public:
        /**
         *  \note mutates container
         */
        nonvirtual void Prepend (ArgByValueType<value_type> item);

    public:
        /**
         *  \req IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>
         *  \req IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>
         *
         *  \note mutates container
         */
        template <ranges::range ITERABLE_OF_ADDABLE>
        nonvirtual void PrependAll (ITERABLE_OF_ADDABLE&& s);
        template <input_iterator ITERATOR_OF_ADDABLE>
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
         *  and before Insert, which calling Append () avoids. Also note - if used in a multithreaded enivonment,
         *  the appended items wont necesarily all get appended at once, since other threads could make
         *  changes in between.
         *
         *  \req IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>
         *  \req IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>
         *
         *  \note mutates container
         */
        template <ranges::range ITERABLE_OF_ADDABLE>
        nonvirtual void AppendAll (ITERABLE_OF_ADDABLE&& s);
        template <input_iterator ITERATOR_OF_ADDABLE>
        nonvirtual void AppendAll (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         * This function requires that the iterator 'i' came from this container.
         *
         * The value pointed to by 'i' is updated - replaced with the value 'newValue'.
         *
         *  \param nextI - if provided (not null) - will be filled in with a valid iterator pointing where i is pointing - since i is invalidated by changing the container)
         * 
         *  \note - this differers from Collection::Update() (which adnvances *nextI); since for a sequence, there is no need to ever
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
         * \brief STL-ish alias for RemoveAll ().
         */
        nonvirtual void clear ();

    public:
        /**
         * \brief STL-ish alias for Remove ().
         *
         *  \note mutates container
         */
        nonvirtual void erase (size_t i);
        nonvirtual Iterator<value_type> erase (const Iterator<value_type>& i);

    public:
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
        static constexpr size_t _kSentinalLastItemIndex = numeric_limits<size_t>::max ();

    public:
        virtual shared_ptr<_IRep> CloneEmpty () const                                   = 0;
        virtual shared_ptr<_IRep> CloneAndPatchIterator (Iterator<value_type>* i) const = 0;
        // 'i' argument to GetAt MAYBE kBadSequenceIndex - indictating last element
        virtual value_type GetAt (size_t i) const                                                                                   = 0;
        virtual void       SetAt (size_t i, ArgByValueType<value_type> item)                                                        = 0;
        virtual size_t     IndexOf (const Iterator<value_type>& i) const                                                            = 0;
        virtual void       Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI)                                      = 0;
        virtual void       Update (const Iterator<value_type>& i, ArgByValueType<value_type> newValue, Iterator<value_type>* nextI) = 0;
        // 'at' argument to Insert MAYBE kBadSequenceIndex - indictating append
        virtual void Insert (size_t at, const value_type* from, const value_type* to) = 0;
        virtual void Remove (size_t from, size_t to)                                  = 0;

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
