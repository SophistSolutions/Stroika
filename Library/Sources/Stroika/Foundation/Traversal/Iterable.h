/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Iterable_h_
#define _Stroika_Foundation_Traversal_Iterable_h_ 1

#include "../StroikaPreComp.h"

#include <compare>
#include <functional>
#include <ranges>
#include <vector>

#include "../Common/Compare.h"
#include "../Configuration/Common.h"
#include "../Configuration/Concepts.h"
#include "../Configuration/TypeHints.h"
#include "../Debug/AssertExternallySynchronizedMutex.h"
#include "../Memory/SharedByValue.h"

#include "Iterator.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo   For methods similar to Iterable<T>::Where() (did for where),
 *              consider a TEMPLATED PARAMETER for the resulting Container type, so you can create a "Set" or whatever by doing
 *              Where... But tricky to unformly add to different container types. Maybe only ones you can say add, or the adder is
 *              a template paraM?
 *              (AND even there for Where - use AddableAdapter!!!)
 *
 *      @todo   SUBCLASSES of Iterable<> need to overload/replace several of these functions taking
 *              into account (by default) their comparers... Eg. Set<> should overload Distinct to do nothing by
 *              default.
 *
 *      @todo   max should take lambda that folds in the select,
 *              and Distinct and take lambda
 *
 *      @todo   Document (which -not all) Linq-like functions only pull as needed from the
 *              original source, and which force a pull (like where doesn't but max does).
 *
 *      @todo   Consider having Linq-like functions do DELAYED EVALUATION, so the computation only
 *              happens when you iterate. Maybe to some degree this already happens, but could do
 *              more (as MSFT does).
 *
 *      @todo   Ordering of parameters to SetEquals() etc templates? Type deduction versus
 *              default parameter?
 *
 *      @todo   REDO DOCS FOR ITERABLE - SO CLEAR ITS ALSO THE BASIS OF "GENERATORS". IT COULD  BE RENAMED
 *              GENERATOR (though don't)
 *
 *      @todo   since Iterator<T> now uses iterator<> traits stuff, so should Iterable<T>?
 */

namespace Stroika::Foundation::Characters {
    class String;
}

namespace Stroika::Foundation::Traversal {

    using Configuration::ArgByValueType;

    /**
     *  \brief  Iterable<T> is a base class for containers which easily produce an Iterator<T>
     *          to traverse them.
     *
     *  The Stroika iterators can be used either directly (similar to std::range), or in the STL begin/end style -
     *  and this class supports both styles of usage.
     * 
     *  \todo https://stroika.atlassian.net/browse/STK-975 - work with std::range
     * 
     *  Iterable<T> also supports read-only applicative operations on the contained data.
     *
     *  Iterable<T> is much like idea of 'abstract readonly container', but which only supports an
     *  exceedingly simplistic pattern of access.
     *
     *  *Important Design Note* (lifetime of iterators):
     *      The Lifetime of Iterator<T> objects created by an Iterable<T> instance must always be less
     *      than the creating Iterable's lifetime.
     *
     *      This may not be enforced by implementations (but generally will be in debug builds). But
     *      it is a rule!
     *
     *      The reason for this is that the underlying memory referenced by the iterator may be going away.
     *      We could avoid this by adding a shared_ptr<> reference count into each iterator, but that
     *      would make iterator objects significantly more expensive, and with little apparent value added.
     *      Similarly for weak_ptr<> references.
     *
     *  *Important Design Note* (construct with rep, no setrep):
     *      We have no:
     *          nonvirtual  void    _SetRep (IterableRepSharedPtr rep);
     *
     *      because allowing a _SetRep() method would complicate the efforts of subclasses of Iterable<T>
     *      to assure that the underlying type is of the appropriate subtype.
     *
     *      For example - see Bag_Array<T>::GetRep_().
     *
     *      Note - instead - you can 'assign' (operator=) to replace the value (and dynamic type) of
     *      an Iterable<> (or subclass) instance.
     *
     *  *Important Design Note* (copy on write/COW):
     *      Iterable uses 'SharedByValue', so that subclasses of Iterable (especially containers) CAN implement
     *      Copy-On-Write (COW). However, not ALL Iterables implement COW. In fact, not all Iterables are mutable!
     * 
     *      Iterable's data can come from arbitrary, programatic sources (like a sequence of uncomputed random numbers).
     *      If you wish to capture something like an Iterable for later use, but don't want its value to change once you've captured it,
     *      consider using Collection<T> or Sequence<> which is almost the same, but will make a copy of the data, and not allow it to
     *      change without preserve COW semantics.
     *
     *  *Design Note*:
     *      Why does Iterable<T> contain a size () method?
     *
     *          o   It’s always well defined what size() means (what you would get if you called
     *              MakeIterable() and iterated a bunch of times til the end).
     *
     *          o   Its almost always (and trivial) to perform that computation more efficiently than the
     *              iterate over each element apporach.
     *
     *          The gist of these two consideration means that if you need to find the length of
     *          an Iterable<T>, if it was defined as a method, you can access the trivial implemeantion,
     *          and if it was not defined, you would be forced into the costly implementation.
     *
     *      Adding size () adds no conceptual cost – because its already so well and clearly defined
     *      in terms of its basic operation (iteration). And it provides value (maybe just modest value).
     *
     *  *Design Note*:
     *      Order of Iteration.
     *
     *      Iterables<T> provide no promises about the order of iteration. Specific subclasses (like SortedSet<>)
     *      often will make specific guarantees about order of iteration.
     *
     *      We do NOT even promise you will see the same items, or seem them in the same order as you iterate
     *      (so for example, you can have a "RandomSequence<>" subclass from Iterable<> and return a different
     *      sequence of numbers each time you make an iterate and run.
     *
     *  *Design Note*:
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *          Chose NOT to include an equal_to<Iterable<T>> partial specialization here, but instead duplicatively in
     *          each subclass, so that it could more easily be implemented efficiently (not a biggie), but more
     *          importantly because it doesn't appear to me to make sense so say that a Stack<T> == Set<T>, even if
     *          their values were the same. In other words, the meaning of 'equals' varies between different kinds of
     *          iterables (over the same type).
     *
     *          We DO have methods SetEquals/MultiSetEquals/SequentialEquals (see below), as well as SequentialThreeWayComparer<> etc.
     *
     *  \em Design Note
     *      Methods like Min/Max/Median/Sum make little senese on empty Iterables. There were several choices
     *      available to deal with this:
     *          >   Assertion
     *          >   throw range_error()
     *          >   return a sensible default value (e.g. 0) for empty lists
     *          >   overloads to let callers select the desired behavior
     *
     *      Because I wanted these methods to be useful in scenarios like with database queries (inspired by Linq/ORMs)
     *      assertions seemed a poor choice.
     *
     *      throw range_error makes sense, but then requires lots of checking when used for throws, and that makes use needlessly complex.
     *
     *      So we eventually decided to use the return optional and have a variant named XXXValue () that returns the plain T with a default - since
     *      we use that pattern in so many places.
     *
     *  *Design Note* - Microsoft Linq:
     *      This API implements some of the Microsoft Linq API.
     *          https://msdn.microsoft.com/en-us/library/system.linq.enumerable_methods(v=vs.100).aspx
     *
     *      For example, we implement:
     *          o   Map         **most important**
     *          o   Reduce      **important - aka accumulate**
     *          o   Where
     *          o   Take
     *          o   Skip
     *          o   OrderBy
     *          o   First/Last/FirstValue/LastValue (though semantics and later names differ somewhat from .net FirstOrDefault)
     *
     *      We choose explicitly not to implement
     *          o   ToList/ToArray, no need because we have As<>, plus no List/Array classes (exactly).
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     */
    template <typename T>
    class Iterable {
    public:
        static_assert (is_copy_constructible_v<Iterator<T>>, "Must be able to create Iterator<T> to use Iterable<T>");

    public:
        /**
         * \brief value_type is an alias for the type iterated over - like vector<T>::value_type
         */
        using value_type = T;

    protected:
        class _IRep;

    public:
        /**
         *  \brief  Iterable are safely copyable (by value). Since Iterable uses COW, this just copies the underlying pointer and increments the reference count.
         */
        Iterable (const Iterable& src) noexcept = default;

    public:
        /**
         *  \brief  Iterable are safely moveable.
         */
        Iterable (Iterable&& src) noexcept = default;

    public:
        /**
         *  Make a copy of the given argument, and treat it as an iterable.
         *
         *  \par Example Usage
         *      \code
         *          Iterable<int> aa6{3, 4, 6};
         *      \endcode
         *
         *  \note Don't apply this constructor to non-containers (non-iterables), 
         *        and don't allow it to apply to SUBCLASSES of Iterable (since then we want to select the Iterable (const Iterable& from) constructor)
         */
        template <typename CONTAINER_OF_T, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_T> and not is_base_of_v<Iterable<T>, decay_t<CONTAINER_OF_T>>>* = nullptr>
        explicit Iterable (CONTAINER_OF_T&& from);

    public:
        /**
         *  \note   Use of initializer_list<T> (@see https://stroika.atlassian.net/browse/STK-739)
         *          Because of quirks of C++ overload resolution (https://en.cppreference.com/w/cpp/language/list_initialization)
         *          use of mem-initializers with Iterable<T> constructor calls have the unintuitive behavior of
         *          invoking the initializer_list<T> constructor preferatially (see docs above and 'Otherwise, the constructors of T are considered, in two phases'
         */
        Iterable (const initializer_list<T>& from);

    protected:
        /**
         *  \brief  Iterable's are typically constructed as concrete subtype objects, 
         *          whose CTOR passed in a shared copyable rep.
         *
         *  \note - the repPtr in construction can be, so that we don't
         *          need to increment its reference count as we pass it though the call chain to where it will be finally
         *          stored.
         */
        explicit Iterable (const shared_ptr<_IRep>& rep) noexcept;
        explicit Iterable (shared_ptr<_IRep>&& rep) noexcept;

    public:
        ~Iterable () = default;

    public:
        /**
         */
        nonvirtual Iterable& operator= (Iterable&& rhs) noexcept      = default;
        nonvirtual Iterable& operator= (const Iterable& rhs) noexcept = default;

    public:
        /**
         *  Often handy short-hand (punning) for a container to see if zero elts, then if on it returns false.
         */
        nonvirtual explicit operator bool () const;

    public:
        /**
         * \brief Create an iterator object which can be used to traverse the 'Iterable'.
         *
         * Create an iterator object which can be used to traverse the 'Iterable' - this object -
         * and visit each element.
         * 
         * \note LIFETIME NOTE:
         *      Iterators created this way, become invalidated (generally detected in debug builds), and cannot be used
         *      after the underlying Iterable is modified.
         */
        nonvirtual Iterator<T> MakeIterator () const;

    public:
        /**
         * \brief Returns the number of items contained.
         *
         * size () returns the number of elements in this 'Iterable' object. Its defined to be
         * the same number of elements you would visit if you created an iterator (MakeIterator())
         * and visited all items. In practice, as the actual number might vary as the underlying
         * iterable could change while being iterated over.
         *
         *  For example, a filesystem directory iterable could return a different length each time it was
         *  called, as files are added and removed from the filesystem.
         *
         *  Also note that size () can return a ridiculous number - like numeric_limits<size_t>::max () -
         *  for logically infinite sequences... like a sequence of random numbers.
         *
         *  \note Alias GetLength () - in fact in Stroika before 2.1b14, this was called GetLength ()
         *
         *  \note Design Note: noexcept
         *      We chose to allow the empty () method to allow exceptions, since an Iterable<T>
         *      is general enough (say externally network data sourced) - it could be temporily or otherwise unavailable.
         * 
         *  \em Performance:
         *      The performance of size() may vary wildly. It could be anywhere from O(1) to O(N)
         *      depending on the underlying type of Iterable<T>.
         */
        nonvirtual size_t size () const;

    public:
        /**
         * \brief Returns true iff size() == 0
         *
         *  \note Alias IsEmpty () - called IsEmpty () in Stroika 2.1b13 and prior
         *
         *  \note Design Note: noexcept
         *      We chose to allow the empty () method to allow exceptions, since an Iterable<T>
         *      is general enough (say externally network data sourced) - it could be temporily or otherwise unavailable.
         * 
         *  \em Performance:
         *      The performance of empty() may vary wildly (@see size) but will nearly always be O(1).
         */
        nonvirtual bool empty () const;

    public:
        /**
         *  Apply the (template argument) EQUALS_COMPARER to each element in the Iterable<T> and
         *  return true iff found. This invokes no virtual methods dependent (except MakeIterable or some such)
         *  and so gains no performance benefits from the organizaiton of the underlying Iterable<T>. This
         *  is just a short hand for the direct iteration one would trivially code by hand. Still - its
         *  easier to call Contains() that to code that loop!
         *
         *  And note - subclasses (like Containers::Set<T>) will hide this implementation with a more
         *  efficient one (that does indirect to the backend).
         *
         *  \em Performance:
         *      This algorithm is O(N).
         *
         */
        template <typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual bool Contains (ArgByValueType<T> element, EQUALS_COMPARER&& equalsComparer = EQUALS_COMPARER{}) const;

    public:
        /**
         *  SetEquals () - very inefficiently - but with constant small memory overhead - returns true if
         *  each element in the each iterable is contained in the other. The lengths CAN be different
         *  and the two Iterables<> be SetEquals().
         *
         *  \em Performance:
         *      This algorithm is O(N) * O(M) where N and M are the length of the two respective iterables.
         *
         *  \note \todo - consider alternative implementation where we accumulate into std::set<>. 
         *                Assume without loss of generality that N is the smaller side (can be determinted in O(M)).
         *                Accumulate into set would take N*log (N).
         *                Then we would iterate over M (O(M)), and each time check log(N)). So time would be sum of
         *                N*log (N) + M*(log(N)) or (N + M)*log(N).
         *                That's a little better (but at the cost of more RAM usage).
         *                NOTE ALSO - that 'trick' assumes T has a valid less<T>, which it may not!
         *
         */
        template <typename LHS_CONTAINER_TYPE, typename RHS_CONTAINER_TYPE, Common::IEqualsComparer<T> EQUALS_COMPARER = equal_to<T>,
                  enable_if_t<Configuration::IsIterable_v<RHS_CONTAINER_TYPE>>* = nullptr>
        static bool SetEquals (const LHS_CONTAINER_TYPE& lhs, const RHS_CONTAINER_TYPE& rhs, EQUALS_COMPARER&& equalsComparer = EQUALS_COMPARER{});
        template <typename RHS_CONTAINER_TYPE = initializer_list<T>, Common::IEqualsComparer<T> EQUALS_COMPARER = equal_to<T>,
                  enable_if_t<Configuration::IsIterable_v<RHS_CONTAINER_TYPE>>* = nullptr>
        nonvirtual bool SetEquals (const RHS_CONTAINER_TYPE& rhs, EQUALS_COMPARER&& equalsComparer = EQUALS_COMPARER{}) const;

    public:
        /**
         *  MultiSetEquals () - very inefficiently - but with constant small memory overhead - returns true if
         *  each element in the each iterable is contained in the other. They lengths CAN be different
         *  and the two Iterables<> be SetEquals().
         *
         *  \em Performance:
         *      This algorithm is O(N^^3)
         */
        template <typename LHS_CONTAINER_TYPE, typename RHS_CONTAINER_TYPE, Common::IEqualsComparer<T> EQUALS_COMPARER = equal_to<T>,
                  enable_if_t<Configuration::IsIterable_v<RHS_CONTAINER_TYPE>>* = nullptr>
        static bool MultiSetEquals (const LHS_CONTAINER_TYPE& lhs, const RHS_CONTAINER_TYPE& rhs, EQUALS_COMPARER&& equalsComparer = EQUALS_COMPARER{});
        template <typename RHS_CONTAINER_TYPE = initializer_list<T>, Common::IEqualsComparer<T> EQUALS_COMPARER = equal_to<T>,
                  enable_if_t<Configuration::IsIterable_v<RHS_CONTAINER_TYPE>>* = nullptr>
        nonvirtual bool MultiSetEquals (const RHS_CONTAINER_TYPE& rhs, EQUALS_COMPARER&& equalsComparer = EQUALS_COMPARER{}) const;

    public:
        /**
         *  SequentialEquals () - measures if iteration over the two containers produces identical sequences
         *  of elements (identical by compare with EQUALS_COMPARER). It does not call 'size' - by default - but just iterates (unless the paraemter useIterableSize)
         *
         *  \note - RHS_CONTAINER_TYPE can be any iterable, including an STL container like vector or initializer_list
         *
         *  \note If useIterableSize == true (Defaults false), size() method must be quick, and unchanged during the lifetime of the the comparison.
         *
         *  \em Performance:
         *      This algorithm is O(N)
         */
        template <typename LHS_CONTAINER_TYPE, typename RHS_CONTAINER_TYPE, Common::IEqualsComparer<T> EQUALS_COMPARER = equal_to<T>,
                  enable_if_t<Configuration::IsIterable_v<RHS_CONTAINER_TYPE>>* = nullptr>
        static bool SequentialEquals (const LHS_CONTAINER_TYPE& lhs, const RHS_CONTAINER_TYPE& rhs,
                                      EQUALS_COMPARER&& equalsComparer = EQUALS_COMPARER{}, bool useIterableSize = false);
        template <typename RHS_CONTAINER_TYPE = initializer_list<T>, Common::IEqualsComparer<T> EQUALS_COMPARER = equal_to<T>,
                  enable_if_t<Configuration::IsIterable_v<RHS_CONTAINER_TYPE>>* = nullptr>
        nonvirtual bool SequentialEquals (const RHS_CONTAINER_TYPE& rhs, EQUALS_COMPARER&& equalsComparer = EQUALS_COMPARER{},
                                          bool useIterableSize = false) const;

    public:
        template <
#if qCompilerAndStdLib_template_ForwardDeclareWithConceptsInTypenameCrasher_Buggy
            typename
#else
            Common::IEqualsComparer<T>
#endif
            T_EQUALS_COMPARER = equal_to<T>>
        struct SequentialEqualsComparer;

    public:
        template <typename T_THREEWAY_COMPARER = compare_three_way>
        struct SequentialThreeWayComparer;

    public:
        /**
         * \brief Support for ranged for, and STL syntax in general
         *
         *  begin ()/end() are similar to MakeIterator(), except that they allow for iterators to be
         *  used in an STL-style, which is critical for using C++ ranged iteration.
         *
         *  \par Example Usage
         *      \code
         *          for (Iterator<T> i = c.begin (); i != c.end (); ++i) {
         *              if (*i = T{}) {
         *                  break;
         *              }
         *          }
         *      \endcode
         *
         * OR
         *      \code
         *          for (const T& i : c) {
         *              if (*i = T{}) {
         *                  break;
         *              }
         *          }
         *      \endcode
         *
         */
        nonvirtual Iterator<T> begin () const;

    public:
        /**
         * \brief Support for ranged for, and STL syntax in general
         * 
         *  \note in INCOMPATIBLE change in Stroika v3.0d1 - from v2.1 - making this instance method instead of static method (needed for 'std::ranges' concept compatability).
         */
        nonvirtual constexpr Iterator<T> end () const noexcept;

    public:
        /**
         *  \brief  Run the argument function (or lambda) on each element of the container.
         *
         *  Take the given function argument, and call it for each element of the container. This
         *  is equivalent to:
         *
         *      for (Iterator<T> i = begin (); i != end (); ++i) {
         *          (doToElement) (*i);
         *      }
         *
         *  However, Apply () MAY perform the entire iteration more quickly (depending on the
         *  kind of the container).
         *
         *  Apply () also MAY be much faster than normal iteration (some simple tests
         *  - around 2015-02-15 - suggest Apply  () is perhaps 10x faster than using an iterator).
         *
         *  \par Example Usage
         *      \code
         *          unsigned int cnt { 0 };
         *          s.Apply ([&cnt] (int i) {
         *              cnt += i;
         *          });
         *          DbgTrace ("cnt=%d", cnt);
         *      \endcode
         *
         *  \note on 'seq' parameter, if you pass anything but (the default) eSeq value, be sure to check
         *        function argument is threadsafe.
         * 
         *  \note   Aliases:
         *      o   Apply could have logically been called ForEach, and is nearly identical to
         *          std::for_each (), except for not taking iterators as arguments, and not having
         *          any return value.
         * 
         *  \note   Why Apply takes std::function argument instead of templated FUNCTION parameter?
         *          Because Stroika iterables use a 'virtual' APPLY, you cannot pass arbitrary templated
         *          function calls passed that boundary. That choice ALLOWS traversal to be implemented
         *          quickly, and without and subsequent (virtual) calls on the container side, but one
         *          - CALL per iteration to the function itself.
         *
         *  \note   \em Thread-Safety   The argument function (lambda) may
         *              directly (or indirectly) access the Iterable<> being iterated over.
         */
        nonvirtual void Apply (const function<void (ArgByValueType<T> item)>& doToElement,
                               Execution::SequencePolicy                      seq = Execution::SequencePolicy::eDefault) const;

    public:
        /**
         *  \brief  Run the argument bool-returning function (or lambda) on each element of the
         *          container, and return an iterator pointing at the first element (depending on seq) found true.
         *          (or use First() to do same thing but return optional<>)
         *
         *  Take the given function argument, and call it for each element of the container. This is
         *  equivalent to:
         *
         *      for (Iterator<T> i = begin (); i != end (); ++i) {
         *          if (that (*i)) {
         *              return it;
         *          }
         *      }
         *      return end();
         *
         *  This function returns an iterator pointing to the element that triggered the abrupt loop
         *  end (for example the element you were searching for?). It returns the special iterator
         *  end() to indicate no doToElement() functions returned true.
         *
         *  Also, note that this function does NOT change any elements of the Iterable.
         * 
         *  \note about seq - eSeq - then the item returned will be first in
         *        iteration order. But if you pass in some other Execution::SequencePolicy 'seq', the algorithm
         *        will return the 'first it finds'.
         * 
         *        If you really care that the result is first, probably better to call Iterable<>::First (). Though
         *        it amounts to the same thing (setting SequencePolicy::eSeq) - its better documenting.
         * 
         *  Note that this used to be called 'ContainsWith' - because it can act the same way (due to
         *  operator bool () method of Iterator<T>).
         * 
         *  \note This is much like First(), except that it optional takes a different starting point, and 
         *        it returns an Iterator<T> intead of an optional<T>
         *        First () - often more handy.
         * 
         *  \note though semantically similar to iterating, it maybe faster, due to delegating 'search' to backend container
         *        implementation (though then call to lambda/checker maybe indirected countering this performance benefit).
         *
         *  @see Apply
         *
         *  \note   \em Thread-Safety   The argument function (lambda) may
         *              directly (or indirectly) access the Iterable<> being iterated over.
         *
         *  \par Example Usage
         *      \code
         *          bool IsAllWhitespace (String s) const
         *          {
         *              return not s.Find ([] (Character c) -> bool { return not c.IsWhitespace (); });
         *          }
         *      \endcode
         *
         *  \note - because the lifetime of the iterable must exceed that of the iterator, its generally unsafe to use Find()
         *          on a temporary (except with the trick if (auto i = x().Find(...)) { ok to access i here cuz x() temporary
         *          not destroyed yet).
         * 
         *  \note THAT_FUNTION type used to be hardwired to function<bool (ArgByValueType<T> item)>&, but now use template argument
         *     and (SOON concept) but for now enable_if_t
         * 
         *  \note desipte the name EQUALS_COMPARER, we allow EQUALS_COMPARER to just be IPotentiallyComparer<> and don't require
         *        EqualsComparer, just to simplify use, and because we cannot anticipate any real ambiguity or confusion resulting from this loose restriction.
         */
        template <predicate<T> THAT_FUNCTION>
        nonvirtual Iterator<T> Find (THAT_FUNCTION&& that, Execution::SequencePolicy seq = Execution::SequencePolicy::eDefault) const;
        template <Common::IPotentiallyComparer<T> EQUALS_COMPARER>
        nonvirtual Iterator<T> Find (Configuration::ArgByValueType<T> v, EQUALS_COMPARER&& equalsComparer = {},
                                     Execution::SequencePolicy seq = Execution::SequencePolicy::eDefault) const;
        template <typename THAT_FUNCTION, enable_if_t<Configuration::IsTPredicate<T, THAT_FUNCTION> ()>* = nullptr>
        nonvirtual Iterator<T> Find (const Iterator<T>& startAt, THAT_FUNCTION&& that,
                                     Execution::SequencePolicy seq = Execution::SequencePolicy::eDefault) const;
        template <typename EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<EQUALS_COMPARER, T> ()>* = nullptr>
        nonvirtual Iterator<T> Find (const Iterator<T>& startAt, Configuration::ArgByValueType<T> v, EQUALS_COMPARER&& equalsComparer = {},
                                     Execution::SequencePolicy seq = Execution::SequencePolicy::eDefault) const;

    public:
        /**
         *  As<CONTAINER_OF_T> () can be used to easily map an iterable to another container
         *  (for example STL container) which supports begin/end iterator constructor. This is
         *  really just a shorthand for
         *      CONTAINER_OF_T{this->begin (), this->end ()};
         *
         *  Note - this also works with (nearly all) of the Stroika containers as well
         *  (e.g. Set<T> x; x.As<Sequence<T>> ());
         *
         *  \em Design Note:
         *      We chose NOT to include an overload taking iterators because there was no connection between
         *      'this' and the used iterators, so you may as well just directly call CONTAINER_OF_T{it1, it2}.
         */
        template <typename CONTAINER_OF_T>
        nonvirtual CONTAINER_OF_T As () const;

    public:
        /**
        *  \brief Find the Nth element of the Iterable<>
        *
        *  \par Example Usage
        *       \code
        *           Iterable<int> c { 1, 2, 3, 4, 5, 6 };
        *           VerifyTestResult (c.Nth (1) == 2);
        *       \endcode
        *
        *  \req n < size ()
        */
        nonvirtual T Nth (size_t n) const;

    public:
        /**
         *  \brief Find the Nth element of the Iterable<>, but allow for n to be out of range, and just return argument default-value
         *
         *  \par Example Usage
         *      \code
         *          Iterable<int> c { 1, 2, 3, 4, 5, 6 };
         *          VerifyTestResult (c.NthValue (1) == 2);
         *          VerifyTestResult (c.NthValue (99) == int{});
         *      \endcode
         *
         */
        nonvirtual T NthValue (size_t n, ArgByValueType<T> defaultValue = {}) const;

    public:
        /**
         *  \brief Alias 'Filter' - produce a subset of this iterable where argument function returns true
         *
         *  BASED ON Microsoft .net Linq.
         *
         *  This returns an Iterable<T> with a subset of data - including only the items that pass the argument filter funtion.
         *
         *  The variants that take a template argument allow constructing a specific container type instead of an iterable.
         *  And the variant that takes an optional empty result container allows constructing a specific subtype of container (backend).
         *  Mostly ignore these overloads.
         *
         *  \par Example Usage
         *      \code
         *          Iterable<int> c { 1, 2, 3, 4, 5, 6 };
         *          VerifyTestResult (c.Where ([] (int i) { return i % 2 == 0; }).SequentialEquals (Iterable<int> { 2, 4, 6 }));
         *      \endcode
         *
         *  \note   Could have been called EachWith, EachWhere, EachThat (), AllThat, AllWhere, Filter, or SubsetWhere.
         * 
         *  \note   This is NEARLY IDENTICAL to the Map<RESULT,RESULT_CONTAINER> function - where it uses its optional returning filter function.
         *          But for this use case, its perhaps a bit terser, so maybe still useful --LGP 2022-11-15
         */
        nonvirtual Iterable<T> Where (const function<bool (ArgByValueType<T>)>& includeIfTrue) const;
        template <typename RESULT_CONTAINER>
        nonvirtual RESULT_CONTAINER Where (const function<bool (ArgByValueType<T>)>& includeIfTrue) const;
        template <typename RESULT_CONTAINER>
        nonvirtual RESULT_CONTAINER Where (const function<bool (ArgByValueType<T>)>& includeIfTrue, RESULT_CONTAINER&& emptyResult) const;

    public:
        /**
         *  BASED ON Microsoft .net Linq.
         *
         *  This returns an Iterable<T> that contains just the subset of the items which are distinct (equality comparer)
         *
         *  \par Example Usage
         *      \code
         *          Iterable<int> c { 1, 2, 2, 5, 9, 4, 5, 6 };
         *          VerifyTestResult (c.Distinct ().SetEquals (Iterable<int> { 1, 2, 4, 5, 6, 9 }));
         *      \endcode
         *
         *  @todo need overloads taking lambda that projects
         *  @todo for now use builtin stl set to accumulate, but need flexability on where compare and maybe also redo with hash?
         */
        template <typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual Iterable<T> Distinct (EQUALS_COMPARER&& equalsComparer = EQUALS_COMPARER{}) const;
        template <typename RESULT, typename EQUALS_COMPARER = equal_to<RESULT>>
        nonvirtual Iterable<RESULT> Distinct (const function<RESULT (ArgByValueType<T>)>& extractElt,
                                              EQUALS_COMPARER&&                           equalsComparer = EQUALS_COMPARER{}) const;

    public:
        /**
         *  \brief  Take transformation (e.g. projection) of the given type T to some derived value, and apply that element transformation
         *          to the entire iterable, producing a new iterable (or a new container).
         * 
         *  \note - @see https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/map
         *          as it does essentially the same thing. It can be used to completely transform a container of one thing
         *          into a (possibly smaller) container of something else by iterating over all the members, applying a function, and
         *          (optionally) appending the result of that function to the new container.
         * 
         *  \note   Prior to Stroika v2.1.10, this was called Select()
         * 
         *  \note - The overloads returning Iterable<RESULT> do NOT IMMEDIATELY traverse its argument, but uses @see CreateGenerator - to create a new iterable that dymanically pulls
         *          from 'this' Iterable<>'.
         * 
         *          The overloads returning RESULT_CONTAINER DO however immediately construct RESULT_CONTAINER, and fill it in the the result
         *          of traversal before Select() returns.
         * 
         *  \note   Alias Filter ()
         *          If the argument function returns optional<THE RETURN TYPE> - then only accomulate those that are returned 
         *          with has_value () (so also can be used to filter).
         *
         *  \par Example Usage
         *      \code
         *          Iterable<pair<int,char>> c { {1, 'a'}, {2, 'b'}, {3, 'c'} };
         *          VerifyTestResult (c.Map<int> ([] (pair<int,char> p) { return p.first; }).SequentialEquals (Iterable<int> { 1, 2, 3 }));
         *      \endcode
         *
         *  This can also easily be used to TRANSFORM an iterable.
         *  \par Example Usage
         *      \code
         *          Iterable<int> c { 3, 4, 7 };
         *          VerifyTestResult (c.Map<String> ([] (int i) { return Characters::Format (L"%d", i); }).SequentialEquals (Iterable<String> { "3", "4", "7" }));
         *      \endcode
         *
         *  \par Example Usage
         *      or transform into another container type
         *      \code
         *          Iterable<int> c { 3, 4, 7 };
         *          VerifyTestResult ((c.Map<String,vector<String>> ([] (int i) { return Characters::Format (L"%d", i); }) == vector<String>{L"3", L"4", L"7"}));
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          void ExpectedMethod (const Request* request, const Set<String>& methods, const optional<String>& fromInMessage)
         *          {
         *              String method{request->GetHTTPMethod ()};
         *              Set<String> lcMethods = methods.Map<String> ([](const String& s) { return s.ToLowerCase ();  });
         *              if (not methods.Contains (method.ToLowerCase ())) {
         *                  ...
         *      \endcode
         *
         *  Overload which returns optional<RESULT> and nullopt interpretted as skipping that element
         *
         *  \par Example Usage
         *      Filtering a list example:
         *      \code
         *          // GetAssociatedContentType -> optional<String> - skip items that are 'missing'
         *          possibleFileSuffixes.Map<InternetMediaType> ([&] (String suffix) { return r.GetAssociatedContentType (suffix); }).As<Set<InternetMediaType>> ())
         *          // OR
         *          possibleFileSuffixes.Map<InternetMediaType,Set<InternetMediaType>> ([&] (String suffix) { return r.GetAssociatedContentType (suffix); })
         *      \endcode
         */
        template <typename RESULT>
        nonvirtual Iterable<RESULT> Map (const function<RESULT (const T&)>& extract) const;
        template <typename RESULT>
        nonvirtual Iterable<RESULT> Map (const function<optional<RESULT> (const T&)>& extract) const;
        template <typename RESULT, typename RESULT_CONTAINER>
        nonvirtual RESULT_CONTAINER Map (const function<RESULT (const T&)>& extract) const;
        template <typename RESULT, typename RESULT_CONTAINER>
        nonvirtual RESULT_CONTAINER Map (const function<optional<RESULT> (const T&)>& extract) const;

    public:
        /**
         *  \brief Walk the entire list of items, and use the argument 'op' to combine (reduce) items to a resulting single item.
         * 
         *  \see https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/reduce
         *  \see https://learn.microsoft.com/en-us/dotnet/api/system.linq.enumerable.aggregate?redirectedfrom=MSDN&view=net-7.0#overloads
         * 
         *  \note   Alias - Accumulate
         * 
         *  \note   This was called Accumulate in Stroika up until 2.1.10
         *
         *  \par Example Usage
         *      \code
         *          Iterable<int> c { 1, 2, 3, 4, 5, 9 };
         *          VerifyTestResult (c.Reduce ([] (T lhs, T rhs) { return lhs + rhs; }) == 24);
         *      \endcode
         * 
         *  \par Implementation As if:
         *      \code
         *          optional<RESULT_TYPE> result;
         *          for (const auto& i : *this) {
         *              if (result) {
         *                  result = op (i, *result);
         *              }
         *              else {
         *                  result = i;
         *              }
         *          }
         *      \endcode
         *
         *  \note   returns nullopt if empty list
         *
         *  See:
         *      @see ReduceValue
         *      @see Join
         *      @see Sum
         *      @see SumValue
         */
        template <typename RESULT_TYPE = T>
        nonvirtual optional<RESULT_TYPE> Reduce (const function<RESULT_TYPE (ArgByValueType<T>, ArgByValueType<T>)>& op) const;

    public:
        /**
         *  @see @Reduce, but if value is missing, returns defaultValue arg or {}
         */
        template <typename RESULT_TYPE = T>
        nonvirtual RESULT_TYPE ReduceValue (const function<RESULT_TYPE (ArgByValueType<T>, ArgByValueType<T>)>& op,
                                            ArgByValueType<RESULT_TYPE>                                         defaultValue = {}) const;

    public:
        /**
         * \brief ape the javascript/python 'join' function - take the parts of 'this' iterable and combine them into a new object (typically a string)
         *
         *  This Join () API - if you use the template, is fairly generic and lets the caller iterate over subelements of this iterable, and
         *  combine them into a new thing (@see Reduce - it is similar but more general).
         * 
         *  For the very common case of accumulating objects into a String, there are additional (stringish) overloads that more closely mimic
         *  what you can do in javascript/python.
         * 
         *  \note The String returning overload converts to String with ((soon will use Characters::ToString)) i.ToString (), so this may not be
         *        a suitable conversion in all cases (mostly intended for debugging or quick cheap display)
         * 
         *  \par Example Usage
         *      \code
         *          Iterable<InternetAddress> c{IO::Network::V4::kLocalhost, IO::Network::V4::kAddrAny};
         *          VerifyTestResult (c.Join () == "localhost, INADDR_ANY");
         *          VerifyTestResult (c.Join ("; ") == "localhost, INADDR_ANY");
         *      \endcode
         *
         *  See:
         *      @see Accumulate
         */
        template <typename RESULT>
        nonvirtual RESULT Join (const function<RESULT (const T&)>& convertToT, const function<RESULT (const RESULT&, const RESULT&)>& combine) const;
        nonvirtual Characters::String Join (const function<Characters::String (const T&)>& convertToT,
                                            const function<Characters::String (const Characters::String&, const Characters::String&)>& combine) const;
        nonvirtual Characters::String Join (const function<Characters::String (const T&)>& convertToT, const Characters::String& separator) const;
        nonvirtual Characters::String Join (const function<Characters::String (const T&)>& convertToT) const;
        nonvirtual Characters::String Join (const Characters::String& separator) const;
        nonvirtual Characters::String Join () const;

    public:
        /**
         *  BASED ON Microsoft .net Linq.
         *
         *  This returns an Iterable<T> with a subset of data after skipping the argument number of items.
         *  If the number of items skipped is greater or equal to the length of the original Iterable, then
         *  an empty Iterable is returned.
         *
         *  \par Example Usage
         *      \code
         *          Iterable<int> c { 1, 2, 3, 4, 5, 6 };
         *          VerifyTestResult (c.Skip (3).SequentialEquals (Iterable<int> { 4, 5, 6 }));
         *      \endcode
         *
         *  @see https://msdn.microsoft.com/en-us/library/bb358985%28v=vs.100%29.aspx?f=255&MSPPError=-2147217396
         *      @Take
         */
        nonvirtual Iterable<T> Skip (size_t nItems) const;

    public:
        /**
         *  BASED ON Microsoft .net Linq.
         *
         *  This returns an Iterable<T> with up to nItems taken from the front of this starting iterable. If this Iterable
         *  is shorter, Take () returns just the original Iterable
         *
         *  \par Example Usage
         *      \code
         *          Iterable<int> c { 1, 2, 3, 4, 5, 6 };
         *          VerifyTestResult (c.Take (3).SequentialEquals (Iterable<int> { 1, 2, 3 }));
         *      \endcode
         *
         *  @see    https://msdn.microsoft.com/en-us/library/bb503062(v=vs.110).aspx
         *      @Skip
         */
        nonvirtual Iterable<T> Take (size_t nItems) const;

    public:
        /**
         *  This returns an Iterable<T> based on the current iterable, with the subset from position from to to.
         *  If some items don't exist, the resulting list is shortened (not an assertion error).
         *  Item at from is included in the output, but item 'to' is not included.
         *
         *  \par Example Usage
         *      \code
         *          Iterable<int> c { 1, 2, 3, 4, 5, 6 };
         *          VerifyTestResult (c.Slice (3, 5).SequentialEquals ({ 4, 5 }));
         *      \endcode
         *
         *  \req from <= to
         * 
         *  \note equivilent to Skip (from).Take (to-from)
         *
         *  @see https://www.w3schools.com/jsref/jsref_slice_array.asp  (EXCEPT FOR NOW - we don't support negative indexes or optional args; maybe do that for SEQUENCE subclass?)
         *  @see Take
         *  @see Slice
         */
        nonvirtual Iterable<T> Slice (size_t from, size_t to) const;

    public:
        /**
         *  \brief return the top/largest (possibly just top N) values from this Iterable<T>
         * 
         *  Provide a function object that says how you want to compare the 'T' elements.
         *  OPTIONALLY, provide a number N, saying to return the top N results (if N < size, just return size elements).
         * 
         *  \em Performance:
         *      let S = this->size();
         *      O(S) * ln (N)  ; so S*log(S) if you get all of them, but if you just need the top three, its O(S)
         *
         *  \par Example Usage
         *      \code
         *          Iterable<int> c{ 3, 5, 9, 38, 3, 5 };
         *          VerifyTestResult (c.Top ().SequentialEquals (c.OrderBy (std::greater<int>{})));
         *          VerifyTestResult (c.Top (2).SequentialEquals ({38, 9}));
         *          VerifyTestResult (c.Top (2, std::greater<int>{}).SequentialEquals ({38, 9}));   // same as previous line
         *          VerifyTestResult (c.Top (3, std::less<int>{}).SequentialEquals ({3, 3, 5}));
         *      \endcode
         * 
         *  \note Uses PossiblyInOrderComparer instead of InOrderComparer since from context, if you pass in a lambda, it
         *        should be clear about intent.
         */
        nonvirtual Iterable<T> Top () const;
        nonvirtual Iterable<T> Top (size_t n) const;
        template <Common::PossiblyInOrderComparer<T> COMPARER>
        nonvirtual Iterable<T> Top (COMPARER&& cmp) const;
        template <Common::PossiblyInOrderComparer<T> COMPARER>
        nonvirtual Iterable<T> Top (size_t n, COMPARER&& cmp) const;

    public:
        /**
         *  BASED ON Microsoft .net Linq.
         *
         *  \par Example Usage
         *      \code
         *          Iterable<int> c{ 3, 5, 9, 38, 3, 5 };
         *          VerifyTestResult (c.OrderBy ().SequentialEquals ({ 3, 3, 5, 5, 9, 38 }));
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          Iterable<int> c{ 3, 5, 9, 38, 3, 5 };
         *          VerifyTestResult (c.OrderBy ([](int lhs, int rhs) -> bool { return lhs < rhs; }).SequentialEquals ({ 3, 3, 5, 5, 9, 38 }));
         *      \endcode
         *
         *  \note This defaults to using seq=Execution::SequencePolicy::ePar, parallel sort, so be careful if your compare function doesn't support this - pass in 
         *        SequencePolicy::eSeq
         *
         *  \note This performs a stable sort (preserving the relative order of items that compare equal).
         *        That maybe less performant than a regular (e.g. quicksort) but works better as a default, in most cases, as it allows combining multi-level sorts.
         *
         *  \note alias for Sort ()
         *
         *  \note Should be of type IsStrictInOrderComparer (), but not required - for convenience of use (so can be used with any lambda functor)
         *
         *  See:
         *      @see https://msdn.microsoft.com/en-us/library/system.linq.enumerable.orderby(v=vs.110).aspx
         */
        template <typename INORDER_COMPARER_TYPE = less<T>>
        nonvirtual Iterable<T> OrderBy (INORDER_COMPARER_TYPE&&   inorderComparer = INORDER_COMPARER_TYPE{},
                                        Execution::SequencePolicy seq             = Execution::SequencePolicy::ePar) const;

    public:
        /**
         *  \brief  return first element in iterable, or if 'that' specified, first where 'that' is true, (or return missing)
         *
         *  \par Example Usage
         *      \code
         *          Iterable<int> c { 3, 5, 9, 38, 3, 5 };
         *          VerifyTestResult (*c.First () == 3);
         *          VerifyTestResult (*c.First ([](int i){ return i % 2 == 0;}) == 38);
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          Collection<SomeStruct> c;
         *          if (optional<SomeStruct> o = c.First ([=](SomeStruct smi) { return smi.fID == substanceId; })) {
         *              somthing_with_o (o);
         *          }
         *      \endcode
         *
         *  \note
         *      BASED ON Microsoft .net Linq.
         *      @see https://msdn.microsoft.com/en-us/library/system.linq.enumerable.first(v=vs.110).aspx
         */
        nonvirtual optional<T> First () const;
        nonvirtual optional<T> First (const function<bool (ArgByValueType<T>)>& that) const;
        template <typename RESULT_T = T>
        nonvirtual optional<RESULT_T> First (const function<optional<RESULT_T> (ArgByValueType<T>)>& that) const;

    public:
        /**
         *  \brief  return first element in iterable provided default
         *
         *  \par Example Usage
         *      \code
         *          Iterable<int> c { 3, 5, 9, 38, 3, 5 };
         *          VerifyTestResult (c.FirstValue () == 3);
         *      \endcode
         *
         *  \note
         *      BASED ON Microsoft .net Linq. (FirstOrDefault)
         *      @see https://msdn.microsoft.com/en-us/library/system.linq.enumerable.firstordefault(v=vs.110).aspx
         */
        nonvirtual T FirstValue (ArgByValueType<T> defaultValue = {}) const;

    public:
        /**
         *  \brief  return last element in iterable, or if 'that' specified, last where 'that' is true, (or return missing)
         *
         *  \par Example Usage
         *      \code
         *          Iterable<int> c { 3, 5, 9, 38, 3, 5 };
         *          VerifyTestResult (*c.Last () == 5);
         *          VerifyTestResult (*c.Last ([](int i){ return i % 2 == 0;}) == 38);
         *      \endcode
         *
         *  \note
         *      BASED ON Microsoft .net Linq. (Last)
         *      @see https://msdn.microsoft.com/en-us/library/system.linq.enumerable.last(v=vs.110).aspx
         */
        nonvirtual optional<T> Last () const;
        nonvirtual optional<T> Last (const function<bool (ArgByValueType<T>)>& that) const;
        template <typename RESULT_T = T>
        nonvirtual optional<RESULT_T> Last (const function<optional<RESULT_T> (ArgByValueType<T>)>& that) const;

    public:
        /**
         *  BASED ON Microsoft .net Linq. (LastOrDefault)
         *
         *  \par Example Usage
         *      \code
         *      Iterable<int> c { 3, 5, 9, 38, 3, 5 };
         *      VerifyTestResult (c.LastValue () == 5);
         *      \endcode
         *
         *  See:
         *      @see https://msdn.microsoft.com/en-us/library/system.linq.enumerable.lastordefault(v=vs.110).aspx
         */
        nonvirtual T LastValue (ArgByValueType<T> defaultValue = {}) const;

    public:
        /**
         *  \brief  return true iff argument predciate returns true for each element of the iterable
         *
         *  \par Example Usage
         *      \code
         *          Iterable<int> c { 3, 5, 9, 3, 5 };
         *          VerifyTestResult (c.All ([](int i){ return i % 2 == 1;}));
         *      \endcode
         *
         *  \note
         *      BASED ON Microsoft .net Linq. (Last)
         *      @see https://docs.microsoft.com/en-us/dotnet/api/system.linq.enumerable.all?view=netframework-4.7.2
         * 
         *  @see also Iterable<T>::Where ()
         */
        nonvirtual bool All (const function<bool (ArgByValueType<T>)>& testEachElt) const;

    public:
        /**
         *  BASED ON Microsoft .net Linq.
         *
         *  \par Example Usage
         *      \code
         *          Iterable<int> c { 1, 2, 3, 4, 5, 6 };
         *          VerifyTestResult (c.Min () == 1);
         *      \endcode
         *
         *  \note   returns nullopt if empty list
         *
         *  \note   Equivalent to Reduce ([] (T lhs, T rhs) { return min (lhs, rhs); })
         *
         *  See:
         *      https://msdn.microsoft.com/en-us/library/bb503062%28v=vs.100%29.aspx?f=255&MSPPError=-2147217396
         *      @Max
         */
        nonvirtual optional<T> Min () const;

    public:
        /**
         *  @see @Max
         */
        template <typename RESULT_TYPE = T>
        nonvirtual RESULT_TYPE MinValue (ArgByValueType<RESULT_TYPE> defaultValue = {}) const;

    public:
        /**
         *  BASED ON Microsoft .net Linq.
         *
         *  EXAMPLE:
         *      \code
         *          Iterable<int> c { 1, 2, 3, 4, 5, 6 };
         *          VerifyTestResult (c.Max () == 6);
         *      \endcode
         *
         *  \note   returns nullopt if empty list
         *
         *  \note   Equivalent to Reduce ([] (T lhs, T rhs) { return max (lhs, rhs); })
         *
         *  See:
         *      https://msdn.microsoft.com/en-us/library/bb503062%28v=vs.100%29.aspx?f=255&MSPPError=-2147217396
         *      @Min
         */
        nonvirtual optional<T> Max () const;

    public:
        /**
         *  @see @Max
         */
        template <typename RESULT_TYPE = T>
        nonvirtual RESULT_TYPE MaxValue (ArgByValueType<RESULT_TYPE> defaultValue = {}) const;

    public:
        /**
         *  BASED ON Microsoft .net Linq.
         *
         *  \par Example Usage
         *      \code
         *          Iterable<int> c { 1, 2, 3, 4, 5, 9 };
         *          VerifyTestResult (c.Mean () == 4);
         *      \endcode
         *
         *  \note   returns nullopt if empty list
         *
         *  AKA "Average"
         *
         *  See:
         *      https://msdn.microsoft.com/en-us/library/bb548647(v=vs.100).aspx
         */
        template <typename RESULT_TYPE = T>
        nonvirtual optional<RESULT_TYPE> Mean () const;

    public:
        /**
         *  @see @Mean
         */
        template <typename RESULT_TYPE = T>
        nonvirtual RESULT_TYPE MeanValue (ArgByValueType<RESULT_TYPE> defaultValue = {}) const;

    public:
        /**
         *  BASED ON Microsoft .net Linq.
         *
         *  \par Example Usage
         *      \code
         *          Iterable<int> c { 1, 2, 3, 4, 5, 9 };
         *          VerifyTestResult (c.Sum () == 24);
         *      \endcode
         *
         *  \note   Equivalent to Reduce ([] (T lhs, T rhs) { return lhs + rhs; })
         *
         *  \note   returns nullopt if empty list
         *
         *  See:
         *      https://msdn.microsoft.com/en-us/library/system.linq.enumerable.sum(v=vs.110).aspx
         */
        template <typename RESULT_TYPE = T>
        nonvirtual optional<RESULT_TYPE> Sum () const;

    public:
        /**
         *  @see @Sum
         */
        template <typename RESULT_TYPE = T>
        nonvirtual RESULT_TYPE SumValue (ArgByValueType<RESULT_TYPE> defaultValue = {}) const;

    public:
        /**
         *  \par Example Usage
         *      \code
         *          Iterable<int> c { 1, 2, 9, 4, 5, 3 };
         *          VerifyTestResult (NearlyEquals (c.Median (), 3.5));
         *      \endcode
         *
         *  \note   returns nullopt if empty list
         *
         *  \note Should be of type IsStrictInOrderComparer (), but not required - for convenience of use (so can be used with any lambda functor)
         */
        template <typename RESULT_TYPE = T, typename INORDER_COMPARE_FUNCTION = less<RESULT_TYPE>>
        nonvirtual optional<RESULT_TYPE> Median (const INORDER_COMPARE_FUNCTION& compare = {}) const;

    public:
        /**
         *  @see @Median
         */
        template <typename RESULT_TYPE = T>
        nonvirtual RESULT_TYPE MedianValue (ArgByValueType<RESULT_TYPE> defaultValue = {}) const;

    public:
        /**
         *  Return this iterable n (count) times. count may be zero, or any other unsigned integer.
         *  Repeat (0) returns an empty list, and Repeat (1) returns *this;
         * 
         *  Similar to https://docs.microsoft.com/en-us/dotnet/api/system.linq.enumerable.repeat?view=netcore-3.1
         * 
         *  \par Example Usage
         *      \code
         *          Iterable<int> c{1};
         *          VerifyTestResult (c.Repeat (5).SequentialEquals ({1, 1, 1, 1, 1}));
         *      \endcode
         */
        nonvirtual Iterable<T> Repeat (size_t count) const;

    public:
        /**
         * \brief not empty () - synonym for .net Any() Linq method.
         *
         *  Second overload (with filter function) same as .Where(filter).Any ();
         *
         *  \note
         *      BASED ON Microsoft .net Linq. (Last)
         *      @see https://docs.microsoft.com/en-us/dotnet/api/system.linq.enumerable.any?view=netframework-4.7.2#System_Linq_Enumerable_Any__1_System_Collections_Generic_IEnumerable___0__System_Func___0_System_Boolean__
         *
         *  \note @see Count
         *  \note @see Where
         */
        nonvirtual bool Any () const;
        nonvirtual bool Any (const function<bool (ArgByValueType<T>)>& includeIfTrue) const;

    public:
        /**
         * \brief with no args, same as size, with function filter arg, returns number of items that pass.
         *
         *  \note
         *      BASED ON Microsoft .net Linq. (Count)
         *      @see https://docs.microsoft.com/en-us/dotnet/api/system.linq.enumerable.count?view=net-6.0
         * 
         *  \note Count/1 same as Where (i).size ();
         *  \note @see Any
         */
        nonvirtual size_t Count () const;
        nonvirtual size_t Count (const function<bool (ArgByValueType<T>)>& includeIfTrue) const;

    public:
        /**
         * \brief STL-ish alias for size() - really in STL only used in string, I think, but still makes sense as an alias.
         */
        nonvirtual size_t length () const;

    protected:
        /**
         *  @see Memory::SharedByValue_State
         *
         *  Don't call this lightly. This is just meant for low level or debugging, and for subclass optimizations
         *  based on the state of the shared common object.
         */
        nonvirtual Memory::SharedByValue_State _GetSharingState () const;

    private:
        static shared_ptr<_IRep> Clone_ (const _IRep& rep);

    private:
#if (__cplusplus < kStrokia_Foundation_Configuration_cplusplus_20) || qCompilerAndStdLib_lambdas_in_unevaluatedContext_Buggy
        struct Rep_Cloner_ {
            auto operator() (const _IRep& t) const -> shared_ptr<_IRep> { return Iterable<T>::Clone_ (t); }
        };
#else
        using Rep_Cloner_ = decltype ([] (const _IRep& t) -> shared_ptr<_IRep> { return Iterable<T>::Clone_ (t); });
#endif

    private:
        template <typename CONTAINER_OF_T>
        static Iterable<T> mk_ (CONTAINER_OF_T&& from);

    protected:
        /**
         *  \brief  Lazy-copying smart pointer mostly used by implementors (can generally be ignored by users).
         *  However, protected because manipulation needed in some subclasses (rarely) - like UpdatableIteratable.
         */
        using _SharedByValueRepType = Memory::SharedByValue<_IRep, Memory::SharedByValue_Traits<_IRep, shared_ptr<_IRep>, Rep_Cloner_>>;

    protected:
        template <typename REP_SUB_TYPE = _IRep>
        class _SafeReadRepAccessor;

    protected:
        template <typename REP_SUB_TYPE = _IRep>
        class _SafeReadWriteRepAccessor;

    protected:
        /**
         *  Rarely access in subclasses, but its occasionally needed, like in UpdatableIterator<T>
         */
        _SharedByValueRepType _fRep;

    protected:
        Debug::AssertExternallySynchronizedMutex _fThisAssertExternallySynchronized;

    public:
        template <typename SHARED_T>
        using PtrImplementationTemplate [[deprecated ("Since Stroika v3.0d1 - use shared_ptr directly")]] = shared_ptr<SHARED_T>;
        template <typename SHARED_T, typename... ARGS_TYPE>
        [[deprecated ("Since Stroika v3.0d1 - use Memory::MakeSharedPtr directly")]] static shared_ptr<SHARED_T> MakeSmartPtr (ARGS_TYPE&&... args)
        {
            return Memory::MakeSharedPtr<SHARED_T> (forward<ARGS_TYPE> (args)...);
        }
        template <typename SHARED_T>
        using enable_shared_from_this_PtrImplementationTemplate [[deprecated ("Since Stroika v3.0d1")]] = std::enable_shared_from_this<SHARED_T>;

    protected:
        using _IterableRepSharedPtr [[deprecated ("Since Stroika v3.0d1 use shared_ptr<_IRep> directly")]] = shared_ptr<_IRep>;
        using _IteratorRepSharedPtr [[deprecated ("Since Stroika v3.0d1 use unique_ptr<typename Iterator<T>::IRep> directly")]] =
            unique_ptr<typename Iterator<T>::IRep>;
    };

    /**
     *  _SafeReadRepAccessor is used by Iterable<> subclasses to assure threadsafety. It takes the
     *  'this' object, and captures a const reference to the internal 'REP'.
     *
     *  For DEBUGGING (catching races) purposes, it also locks the Debug::AssertExternallySynchronizedMutex,
     *  so that IF this object is accessed illegally by other threads while in use (this use), it will
     *  be caught.
     *
     *  \note   _SafeReadRepAccessor also provides type safety, in that you template in the subtype
     *          of the REP object, and we store a single pointer, but cast to the appropriate subtype.
     *
     *          This supports type safe useage because in DEBUG builds we check (AssertMember)
     *          the dynamic type, and if you strucutre your code to assure a given type (say Collection<T>)
     *          only passes in to pass class appropraitely typed objects, and just use that type in
     *          your _SafeReadRepAccessor<> use, you should be safe.
     *
     *  @see _SafeReadWriteRepAccessor
     */
    template <typename T>
    template <typename REP_SUB_TYPE>
    class Iterable<T>::_SafeReadRepAccessor {
    public:
        _SafeReadRepAccessor () = delete;
        _SafeReadRepAccessor (const _SafeReadRepAccessor& src) noexcept;
        _SafeReadRepAccessor (_SafeReadRepAccessor&& src) noexcept;
        _SafeReadRepAccessor (const Iterable<T>* it) noexcept;

    public:
        nonvirtual _SafeReadRepAccessor& operator= (const _SafeReadRepAccessor& rhs) noexcept;

    public:
        nonvirtual const REP_SUB_TYPE& _ConstGetRep () const noexcept;

    public:
        nonvirtual shared_ptr<REP_SUB_TYPE> _ConstGetRepSharedPtr () const noexcept;

    private:
        const REP_SUB_TYPE* fConstRef_;
        const Iterable<T>*  fIterableEnvelope_;

#if qDebug
        Debug::AssertExternallySynchronizedMutex::ReadContext fAssertReadLock_;
#endif
    };

    /**
     *  _SafeReadWriteRepAccessor is used by Iterable<> subclasses to assure threadsafety. It takes the
     *  'this' object, and captures a writable to the internal 'REP'.
     *
     *  For DEBUGGING (catching races) purposes, it also locks the Debug::AssertExternallySynchronizedMutex,
     *  so that IF this object is accessed illegally by other threads while in use (this use), it will
     *  be caught.
     *
     *  @see _SafeReadRepAccessor
     *
     */
    template <typename T>
    template <typename REP_SUB_TYPE>
    class Iterable<T>::_SafeReadWriteRepAccessor {
    public:
        _SafeReadWriteRepAccessor ()                                     = delete;
        _SafeReadWriteRepAccessor (const _SafeReadWriteRepAccessor& src) = default;
        _SafeReadWriteRepAccessor (_SafeReadWriteRepAccessor&& src);
        _SafeReadWriteRepAccessor (Iterable<T>* iterableEnvelope);

    public:
        nonvirtual _SafeReadWriteRepAccessor& operator= (const _SafeReadWriteRepAccessor&) = delete;

    public:
        nonvirtual const REP_SUB_TYPE& _ConstGetRep () const;

    public:
        nonvirtual REP_SUB_TYPE& _GetWriteableRep ();

    private:
        REP_SUB_TYPE* fRepReference_;
#if qDebug
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex::WriteContext fAssertWriteLock_;
        Iterable<T>* fIterableEnvelope_; // mostly saved for assertions, but also for _UpdateRep- when we lose that - we can ifdef qDebug this field (as we do for read accessor)
#endif
    };

    /**
     *  \brief  Implementation detail for iterator implementors.
     *
     *  Abstract class used in subclasses which extend the idea of Iterable.
     *  Most abstract Containers in Stroika subclass of Iterable<T>.
     * 
     *  \note Design Note: shared_from_this/enable_shared_from_this vs explicit
     *        const shared_ptr<_IRep>& thisSharedPtr arguments to most functions.
     * 
     *        Some of these methods - most notably MakeIterator () - create objects which must
     *        reference the underlying Iterable after the iterator returns. That means it needs
     *        to somehow 'increment the reference count'. This isn't done in the API, but all the backend
     *        implementations must do it somehow. They do it - before Stroika v3 - by using enable_shared_from_this.
     * 
     *        Doing that increase the size of each rep instance (by the size of a weak_ptr - DOUBLE CHECK THIS? - which is basically
     *        sizeof (void*) - I think. Not huge, but significant (in particular for string objects).
     * 
     *        The alternative, is to just pass in the shared_ptr<> on the MakeIterator() calls. This doesn't cost much extra (passing ptr on stack
     *        as extra argument). And it is compensatated by avoiding the shared_from_this call (probably a loss over all) - but compoensated
     *        by avoiding the storage of the weak_ptr (to be tested - but hopefully a win).
     * 
     *        Sadly - the default implementation of most Iterable<>::IRep methods vectors to MakeIterator, so those methods likewise need to
     *        pass in the shared_ptr - even if unused (maybe making this not a winner - will test). Except WHY - since they don't actually
     *        hang onto the Iterator result (so dont really need to extend its life). Because of this, the calls to the IRep::MakeIterator () allow
     *        the shared_ptr to be a nullptr, meaning just dont need to bump the reference count.
     * 
     *        So, the rule is - if the caller calls something that returns an iterator, but the iterator is examined and not
     *        RETURNED by the caller which could have saved the shared_ptr and already has it 'locked'/'incremented' so it cannot
     *        change, then it can pass nullptr to such routines.
     */
    template <typename T>
    class Iterable<T>::_IRep {
    protected:
        _IRep () = default;

    public:
        virtual ~_IRep () = default;

    public:
        /**
         */
        virtual shared_ptr<_IRep> Clone () const = 0;

    public:
        /**
         *  _IRep::MakeIterator takes a shared_ptr, whose get-value may be nullptr (only in case where
         *  the returned Iterator definitely is not hung onto) or .get == this
         */
        virtual Iterator<value_type> MakeIterator (const shared_ptr<_IRep>& thisSharedPtr) const = 0;

    public:
        /**
         *  returns the number of elements in iterable. Equivilent to (and defaults to)
         *  i = MakeIterator, followed by counting number of iterations til the end.
         */
        virtual size_t size () const;

    public:
        /**
         *  returns the true if MakeIterator() returns an empty iterator.
         */
        virtual bool empty () const;

    public:
        /**
         *  Apply the given doToElement function to every element of the Iterable (in some arbitrary order).
         */
        virtual void Apply (const function<void (ArgByValueType<T> item)>& doToElement, Execution::SequencePolicy seq) const;

    public:
        /*
         *  \see _IRep::MakeIterator for rules about thisSharedPtr
         *  Defaults to, and is equivilent to, walking the Iterable, and applyting 'that' function, and returning the first (depending on seq) entry that
         *  returns true, or empty iterator if none does.
         * 
         *  \see _IRep::MakeIterator for rules about thisSharedPtr
         */
        virtual Iterator<value_type> Find (const shared_ptr<_IRep>& thisSharedPtr, const function<bool (ArgByValueType<T> item)>& that,
                                           Execution::SequencePolicy seq) const;

    public:
        /**
         *  Find_equal_to is Not LOGICALLY needed, as you can manually iterate (just use Find()). 
         *  But this CAN be much faster (and commonly is) - and is used very heavily by iterables, so
         *  its worth the singling out of this important special case.
         * 
         *  \req Configuration::HasUsableEqualToOptimization<T> (); would like to only define (with requires) but
         *       cannot seem to do in C++20 - requires on virtual function
         * 
         *  Default implemented as
         *      \code
         *          return Find (thisSharedPtr, [] (const T& lhs) { return equal_to<T>{}(lhs, v); }, seq);
         *      \endcode
         * 
         *  \see _IRep::MakeIterator for rules about thisSharedPtr
         */
        virtual Iterator<value_type> Find_equal_to (const shared_ptr<_IRep>& thisSharedPtr, const ArgByValueType<T>& v,
                                                    Execution::SequencePolicy seq) const;
    };

    /**
     *  Compare any two iterables as a sequence of elements that can themselves be compared - like strcmp().
     *  The first pair which is unequally compared - defines the ordering relationship between the two iterables.
     *  And if one ends before the other, if the LHS ends first, treat that as less (like with alphabetizing) and
     *  if the right ends first, treat that as >.
     *
     *  \note If useIterableSize == true (Defaults false), size() method must be quick, and unchanged during the lifetime of the the comparison.
     *
     *  SequentialEqualsComparer is commutative().
     *
     *  Computational Complexity: O(N)
     */
    template <typename T>
    template <
#if qCompilerAndStdLib_template_ForwardDeclareWithConceptsInTypenameCrasher_Buggy
        typename
#else
        Common::IEqualsComparer<T>
#endif
        T_EQUALS_COMPARER>
    struct Iterable<T>::SequentialEqualsComparer : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals> {
        constexpr SequentialEqualsComparer (const T_EQUALS_COMPARER& elementComparer = {}, bool useIterableSize = false);
        nonvirtual bool   operator() (const Iterable& lhs, const Iterable& rhs) const;
        T_EQUALS_COMPARER fElementComparer;
        bool              fUseIterableSize;
    };

    /**
     *  Compare any two iterables as a sequence of elements that can themselves be compared - like strcmp().
     *  The first pair which is unequally compared - defines the ordering relationship between the two iterables.
     *  And if one ends before the other, if the LHS ends first, treat that as less (like with alphabetizing) and
     *  if the right ends first, treat that as >.
     */
    template <typename T>
    template <typename T_THREEWAY_COMPARER>
    struct Iterable<T>::SequentialThreeWayComparer : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eThreeWayCompare> {
        constexpr SequentialThreeWayComparer (const T_THREEWAY_COMPARER& elementComparer = {});
        nonvirtual auto     operator() (const Iterable& lhs, const Iterable& rhs) const;
        T_THREEWAY_COMPARER fElementComparer;
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "Iterable.inl"

#endif /*_Stroika_Foundation_Traversal_Iterable_h_ */
