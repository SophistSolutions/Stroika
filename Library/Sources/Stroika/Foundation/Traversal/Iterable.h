/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Iterable_h_
#define _Stroika_Foundation_Traversal_Iterable_h_ 1

#include "../StroikaPreComp.h"

#include <compare>
#include <functional>
#include <shared_mutex>
#include <vector>

#include "../Common/Compare.h"
#include "../Configuration/Common.h"
#include "../Configuration/Concepts.h"
#include "../Configuration/TypeHints.h"
#include "../Debug/AssertExternallySynchronizedMutex.h"
#include "../Memory/SharedByValue.h"
#include "../Memory/SharedPtr.h"

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
 *      @todo   Add more 'linq' overloads, like groupBy taking different kinds of compare functions, and field selectors.
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
     *  Stroika's Memory::SharedPtr<> appears to be a bit faster than the std::shared_ptr. Iterable
     *  at one time, and on some systems.
     *
     *      This defaults to @see Memory::kSharedPtr_IsFasterThan_shared_ptr
     */
    constexpr bool kIterableUsesStroikaSharedPtr = Memory::kSharedPtr_IsFasterThan_shared_ptr;

    /**
     */
    struct IterableBase {
    public:
        template <typename SHARED_T>
        using PtrImplementationTemplate = conditional_t<kIterableUsesStroikaSharedPtr, Memory::SharedPtr<SHARED_T>, shared_ptr<SHARED_T>>;

    public:
        template <typename SHARED_T, typename... ARGS_TYPE>
        static PtrImplementationTemplate<SHARED_T> MakeSmartPtr (ARGS_TYPE&&... args);

    public:
        template <typename SHARED_T>
        using enable_shared_from_this_PtrImplementationTemplate = conditional_t<kIterableUsesStroikaSharedPtr, Memory::enable_shared_from_this<SHARED_T>, std::enable_shared_from_this<SHARED_T>>;
    };

    /**
     *  \brief  Iterable<T> is a base class for containers which easily produce an Iterator<T>
     *          to traverse them.
     *
     *  Iterable<T> is a base class for containers which easily produce an Iterator<T> to traverse them.
     *
     *  The Stroika iterators can be used either directly, or in the STL begin/end style - and this
     *  class supports both styles of usage.
     *
     *  Iterable<T> also supports read-only applicative operations on the contained data.
     *
     *  Iterable<T> is much like idea of 'abstract readonly container', but which only supports an
     *  exceedingly simplistic pattern of access.
     *
     *  *Important Design Note*:
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
     *  *Important Design Note*:
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
     *  *Important Design Note*:
     *      Iterable's - unlike all Stroika containers - do NOT necessarily implement 'copy on write' - COW. This is because the data can
     *      come from arbitrary, programatic sources (like a sequence of uncomputed random numbers). If you wish to capture something
     *      like an Iterable for later use, but don't want its value to change once you've captured it, consider using Collection<T> which is
     *      almost the same, but will make a copy of the data, and not allow it to change without preserve COW semantics.
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
     *  \note <a href="Coding Conventions.md#Comparisons">Comparisons</a>:
     *          Chose NOT to include an equal_to<Iterable<T>> partial specialization here, but instead duplicatively in
     *          each subclass, so that it could more easily be implemented efficiently (not a biggie), but more
     *          importantly because it doesn't appear to me to make sense so say that a Stack<T> == Set<T>, even if
     *          their values were the same. In other words, the meaning of 'equals' varies between different kinds of
     *          iterables (over the same type).
     *
     *          We DO have methods SetEquals/MultiSetEquals/SequentialEquals (see below), as well as SequentialThreeWayComparer<> etc.
     *
     *  *Important Design Note*:
     *      Probably important - for performance??? - that all these methods are const,
     *      so ??? think through - what this implies- but probably something about not
     *      threading stuff and ???
     *
     *  *Important Design Note*:
     *      Move operations on an interable are mapped to copy operations. We experimented with
     *      true move (before Stroika v2.1d10), but since all operations have to still be somehow valid
     *      after a move, that meants re-creating the rep in the moved-out iterable.
     *
     *      There is little value to move, since Iterable already uses copy-on-write (COW).
     *
     *      And when you take into account the cost of either checking for null in all operations (methods), or
     *      creating a new empty object after each move, this is a poor tradeoff.
     *
     *      See https://stroika.atlassian.net/browse/STK-541 for history and alteratives (allowing real moves)
     * 
     *      In Stroika 2.1b3, switched to simply not defining (&&) move constructor/operator= (&&), instead of defining
     *      it and having map to move.
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
     *  *Design Note*:
     *      Rejected idea:
     *          Add overload of Find() that takes Iterator<T> as arugument instead of T
     *          so you can delete the item pointed to by T.
     *
     *          This was rejected because it can easily be done directly with iterators, and seems
     *          a queeryly specific problem. I cannot see any patterns where one would want to do this.
     *
     *  *Design Note* - Microsoft Linq:
     *      This API implements some of the Microsoft Linq API.
     *          https://msdn.microsoft.com/en-us/library/system.linq.enumerable_methods(v=vs.100).aspx
     *
     *      For example, we implement:
     *          o   Where
     *          o   Take
     *          o   Skip
     *          o   OrderBy
     *          o   First/Last/FirstValue/LastValue (though semantics and later names differ somewhat from .net FirstOrDefault)
     *
     *      We choose explicitly not to implement
     *          o   ToList/ToArray, no need because we have As<>, plus no List/Array classes (exactly).
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#ExternallySynchronized">ExternallySynchronized</a>
     *
     */
    template <typename T>
    class Iterable : public IterableBase, protected Debug::AssertExternallySynchronizedMutex {
    public:
        static_assert (is_copy_constructible_v<Iterator<T>>, "Must be able to create Iterator<T> to use Iterable<T>");

    public:
        /**
         * \brief value_type is an alias for the type iterated over - like vector<T>::value_type
         */
        using value_type = T;

    protected:
        class _IRep;

    protected:
        /**
         *  _IterableRepSharedPtr is logically shared_ptr<_IRep>. However, we may use alternative 'shared ptr' implementations,
         *  so use this type to assure compatability with the approppriate shared ptr implementation.
         */
        using _IterableRepSharedPtr = PtrImplementationTemplate<_IRep>;

    protected:
        /**
         *  _IteratorRepSharedPtr is logically shared_ptr<Iterator<T>::_IRep>. However, we may use alternative 'shared ptr' implementations,
         *  so use this type to assure compatability with the approppriate shared ptr implementation.
         */
        using _IteratorRepSharedPtr = typename Iterator<T>::RepSmartPtr;

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
         * R
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
        explicit Iterable (const _IterableRepSharedPtr& rep) noexcept;
        explicit Iterable (_IterableRepSharedPtr&& rep) noexcept;

    public:
        ~Iterable () = default;

    public:
        /**
         */
        nonvirtual Iterable& operator= (Iterable&& rhs) noexcept = default;
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
        nonvirtual bool Contains (ArgByValueType<T> element, const EQUALS_COMPARER& equalsComparer = EQUALS_COMPARER{}) const;

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
         *
         */
        template <typename LHS_CONTAINER_TYPE, typename RHS_CONTAINER_TYPE, typename EQUALS_COMPARER = equal_to<T>, enable_if_t<Configuration::IsIterable_v<RHS_CONTAINER_TYPE> and Common::IsEqualsComparer<EQUALS_COMPARER> ()>* = nullptr>
        static bool SetEquals (const LHS_CONTAINER_TYPE& lhs, const RHS_CONTAINER_TYPE& rhs, const EQUALS_COMPARER& equalsComparer = EQUALS_COMPARER{});
        template <typename RHS_CONTAINER_TYPE = initializer_list<T>, typename EQUALS_COMPARER = equal_to<T>, enable_if_t<Configuration::IsIterable_v<RHS_CONTAINER_TYPE> and Common::IsEqualsComparer<EQUALS_COMPARER> ()>* = nullptr>
        nonvirtual bool SetEquals (const RHS_CONTAINER_TYPE& rhs, const EQUALS_COMPARER& equalsComparer = EQUALS_COMPARER{}) const;

    public:
        /**
         *  MultiSetEquals () - very inefficiently - but with constant small memory overhead - returns true if
         *  each element in the each iterable is contained in the other. They lengths CAN be different
         *  and the two Iterables<> be SetEquals().
         *
         *  \em Performance:
         *      This algorithm is O(N^^3)
         */
        template <typename LHS_CONTAINER_TYPE, typename RHS_CONTAINER_TYPE, typename EQUALS_COMPARER = equal_to<T>, enable_if_t<Configuration::IsIterable_v<RHS_CONTAINER_TYPE> and Common::IsEqualsComparer<EQUALS_COMPARER> ()>* = nullptr>
        static bool MultiSetEquals (const LHS_CONTAINER_TYPE& lhs, const RHS_CONTAINER_TYPE& rhs, const EQUALS_COMPARER& equalsComparer = EQUALS_COMPARER{});
        template <typename RHS_CONTAINER_TYPE = initializer_list<T>, typename EQUALS_COMPARER = equal_to<T>, enable_if_t<Configuration::IsIterable_v<RHS_CONTAINER_TYPE> and Common::IsEqualsComparer<EQUALS_COMPARER> ()>* = nullptr>
        nonvirtual bool MultiSetEquals (const RHS_CONTAINER_TYPE& rhs, const EQUALS_COMPARER& equalsComparer = EQUALS_COMPARER{}) const;

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
         *
         *  @todo We DO want the check IsEqualsComparer() on these templates, but I've had trouble getting it to compile, and will be changing
         *        that to the new C++20 style soon anyhow... LGP 2020-05-03
         */
        template <typename LHS_CONTAINER_TYPE, typename RHS_CONTAINER_TYPE, typename EQUALS_COMPARER = equal_to<T>, enable_if_t<Configuration::IsIterable_v<RHS_CONTAINER_TYPE> /*and Common::IsEqualsComparer<EQUALS_COMPARER> ()*/>* = nullptr>
        static bool SequentialEquals (const LHS_CONTAINER_TYPE& lhs, const RHS_CONTAINER_TYPE& rhs, const EQUALS_COMPARER& equalsComparer = EQUALS_COMPARER{}, bool useIterableSize = false);
        template <typename RHS_CONTAINER_TYPE = initializer_list<T>, typename EQUALS_COMPARER = equal_to<T>, enable_if_t<Configuration::IsIterable_v<RHS_CONTAINER_TYPE> and Common::IsEqualsComparer<EQUALS_COMPARER> ()>* = nullptr>
        nonvirtual bool SequentialEquals (const RHS_CONTAINER_TYPE& rhs, const EQUALS_COMPARER& equalsComparer = EQUALS_COMPARER{}, bool useIterableSize = false) const;

    public:
        template <typename T_EQUALS_COMPARER = equal_to<T>>
        struct SequentialEqualsComparer;

    public:
        template <typename T_THREEWAY_COMPARER = Common::ThreeWayComparer>
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
         */
        static constexpr Iterator<T> end ();

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
         *  However, Apply () MAY perform the entire iteration atomicly (depending on the
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
         *  \note   Aliases:
         *      o   Apply could have logically been called ForEach, and is nearly identical to
         *          std::for_each (), except for not taking iterators as arguments, and not having
         *          any return value.
         *
         *  \note   \em Thread-Safety   The argument function (lambda) may
         *              directly (or indirectly) access the Iterable<> being iterated over.
         */
        nonvirtual void Apply (const function<void (ArgByValueType<T> item)>& doToElement) const;

    public:
        /**
         *  \brief  Run the argument bool-returning function (or lambda) on each element of the
         *          container, and return an iterator pointing at the first element found true.
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
         *  Note that this used to be called 'ContainsWith' - because it can act the same way (due to
         *  operator bool () method of Iterator<T>).
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
         *  @see First () - often more handy
         *  \note Before Stroika 2.1b14, the overload taking just THAT_FUNCTION was called FindFirstThat, and used function<bool(T)> instead of a templated function.
         * 
         *  \note THAT_FUNTION type used to be hardwired to function<bool (ArgByValueType<T> item)>&, but now use template argument
         *     and (SOON concept) but for now enable_if_t
         * 
         *  \note desipte the name EQUALS_COMPARER, we allow EQUALS_COMPARER to just be IsPotentiallyComparerRelation<> and don't require
         *        IsEqualsComparer, just to simplify use, and because we cannot anticipate any real ambiguity or confusion resulting from this loose restriction.
         */
        template <typename THAT_FUNCTION, enable_if_t<Configuration::IsTPredicate<T, THAT_FUNCTION> ()>* = nullptr>
        nonvirtual Iterator<T> Find (THAT_FUNCTION&& that) const;
        template <typename EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<EQUALS_COMPARER, T> ()>* = nullptr>
        nonvirtual Iterator<T> Find (Configuration::ArgByValueType<T> v, EQUALS_COMPARER&& equalsComparer = {}) const;
        template <typename THAT_FUNCTION, enable_if_t<Configuration::IsTPredicate<T, THAT_FUNCTION> ()>* = nullptr>
        nonvirtual Iterator<T> Find (const Iterator<T>& startAt, THAT_FUNCTION&& that) const;
        template <typename EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<EQUALS_COMPARER, T> ()>* = nullptr>
        nonvirtual Iterator<T> Find (const Iterator<T>& startAt, Configuration::ArgByValueType<T> v, EQUALS_COMPARER&& equalsComparer = {}) const;

    public:
        /**
         *  As<CONTAINER_OF_T> () can be used to easily map an iterable to another container
         *  (for example STL container) which supports begin/end iterator constructor. This is
         *  really just a shorthand for
         *      CONTAINER_OF_T (this->begin (), this->end ());
         *
         *  Note - this also works with (nearly all) of the Stroika containers as well
         *  (e.g. Set<T> x; x.As<Sequence<T>> ());
         *
         *  \em Design Note:
         *      We chose NOT to include an overload taking iterators because there was no connection between
         *      'this' and the used iterators, so you may as well just directly call CONTAINER_OF_T(it1, it2).
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
         *      \endcode
         *
         */
        nonvirtual T NthValue (size_t n, ArgByValueType<T> defaultValue = {}) const;

    public:
        /**
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
         */
        nonvirtual Iterable<T> Where (const function<bool (ArgByValueType<T>)>& includeIfTrue) const;
        template <typename RESULT_CONTAINER>
        nonvirtual RESULT_CONTAINER Where (const function<bool (ArgByValueType<T>)>& includeIfTrue) const;
        template <typename RESULT_CONTAINER>
        nonvirtual RESULT_CONTAINER Where (const function<bool (ArgByValueType<T>)>& includeIfTrue, const RESULT_CONTAINER& emptyResult) const;

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
        nonvirtual Iterable<T> Distinct (const EQUALS_COMPARER& equalsComparer = EQUALS_COMPARER{}) const;
        template <typename RESULT, typename EQUALS_COMPARER = equal_to<RESULT>>
        nonvirtual Iterable<RESULT> Distinct (const function<RESULT (ArgByValueType<T>)>& extractElt, const EQUALS_COMPARER& equalsComparer = EQUALS_COMPARER{}) const;

    public:
        /**
         *  \brief  Compute a projection (or transformation) of the given type T to some argument set of subtypes, and apply that projection
         *          to the entire iterable, creating a new iterable.
         * 
         *  \note - this could have been called map() (@see https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/map)
         *          as it does essentially the same thing. It can be used to completely transform a container of one thing
         *          into a (possibly smaller) container of something else by iterating over all the members, applying a function, and
         *          (optionally) appending the result of that function to the new container.
         * 
         *  \note - this does NOT IMMEDIATELY traverse its argument, but uses @see CreateGenerator - to create a new iterable that dymanically pulls
         *          from 'this' Iterable<>'.
         * 
         *  If the argument function returns optional<THE RETURN TYPE> - then only accomulate those that are returned with has_value () (so also can be used to rollup).
         *
         *  \par Example Usage
         *      \code
         *          Iterable<pair<int,char>> c { {1, 'a'}, {2, 'b'}, {3, 'c'} };
         *          VerifyTestResult (c.Select<int> ([] (pair<int,char> p) { return p.first; }).SequentialEquals (Iterable<int> { 1, 2, 3 }));
         *      \endcode
         *
         *  This can also easily be used to TRANSFORM an iterable.
         *  \par Example Usage
         *      \code
         *          Iterable<int> c { 3, 4, 7 };
         *          VerifyTestResult (c.Select<String> ([] (int i) { return Characters::Format (L"%d", i); }).SequentialEquals (Iterable<String> { L"3", L"4", L"7" }));
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          void ExpectedMethod (const Request* request, const Set<String>& methods, const optional<String>& fromInMessage)
         *          {
         *              String method{request->GetHTTPMethod ()};
         *              Set<String> lcMethods = methods.Select<String> ([](const String& s) { return s.ToLowerCase ();  });
         *              if (not methods.Contains (method.ToLowerCase ())) {
         *                  ...
         *      \endcode
         *
         *  Overload which returns optional<RESULT> and nullopt interpretted as skipping that element
         *
         *  \par Example Usage
         *      \code
         *          // GetAssociatedContentType -> optional<String> - skip items that are 'missing'
         *          possibleFileSuffixes.Select<InternetMediaType> ([&] (String suffix) { return r.GetAssociatedContentType (suffix); }).As<Set<InternetMediaType>> ())
         *      \endcode
         * 
         *  \note   @todo after we lose the multiple-template-arg overloads of Select<> - MAYBE - add an optional template
         *          arg CONTAINER=Iterable<RESULT> to these Select<> templates. Then they can be used to - in one step - transform not
         *          only the type contained, but what we do with As<> template - and create a differnt result type (like Set<T> etc) as needed.
         *          MAY need a third arg to go with this for "Adder".
         * 
         *          NOTE - in some ways - this new Select() 'overload' will be differnt in that the current one creates a generator, and then
         *          new one would (presumably) not - and directly construc tthe new type (though I suppose it too could use the generator apporahc).
         */
        template <typename RESULT>
        nonvirtual Iterable<RESULT> Select (const function<RESULT (const T&)>& extract) const;
        template <typename RESULT>
        nonvirtual Iterable<RESULT> Select (const function<optional<RESULT> (const T&)>& extract) const;

    public:
        /**
         * \brief ape the javascript/python 'join' function - take the parts of 'this' iterable and combine them into a new object (typically a string)
         *
         *  This Join () API - if you use the template, is fairly generic and lets the caller iterate over subelements of this iterable, and
         *  combine them into a new thing (@see Accumulate - similar).
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
         *          VerifyTestResult (c.Join () == L"localhost, INADDR_ANY");
         *          VerifyTestResult (c.Join (L"; ") == L"localhost, INADDR_ANY");
         *      \endcode
         *
         *  See:
         *      @see Accumulate
         */
        template <typename RESULT>
        nonvirtual RESULT Join (const function<RESULT (const T&)>& convertToT, const function<RESULT (const RESULT&, const RESULT&)>& combine) const;
        nonvirtual Characters::String Join (const function<Characters::String (const T&)>& convertToT, const function<Characters::String (const Characters::String&, const Characters::String&)>& combine) const;
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
         *  @see https://www.w3schools.com/jsref/jsref_slice_array.asp  (EXCEPT FOR NOW - we don't support negative indexes or optional args; maybe do that for SEQUENCE subclass?)
         *  @see Take
         *  @see Slice
         */
        nonvirtual Iterable<T> Slice (size_t from, size_t to) const;

    public:
        /**
         *  BASED ON Microsoft .net Linq.
         *
         *  @todo FOR NOW VERY LIMITED API. Linq has params to let you select just the KEY to use in compare. Could add overloads
         *
         *  \par Example Usage
         *      \code
         *          Iterable<int> c { 3, 5, 9, 38, 3, 5 };
         *          VerifyTestResult (c.OrderBy ().SequentialEquals ({ 3, 3, 5, 5, 9, 38 }));
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          Iterable<int> c { 3, 5, 9, 38, 3, 5 };
         *          VerifyTestResult (c.OrderBy ([](int lhs, int rhs) -> bool { return lhs < rhs; }).SequentialEquals ({ 3, 3, 5, 5, 9, 38 }));
         *      \endcode
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
        nonvirtual Iterable<T> OrderBy (const INORDER_COMPARER_TYPE& inorderComparer = INORDER_COMPARER_TYPE{}) const;

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
         *  Walk the entire list of items, and use the argument 'op' to combine items to a resulting single item.
         *
         *  \par Example Usage
         *      \code
         *          Iterable<int> c { 1, 2, 3, 4, 5, 9 };
         *          VerifyTestResult (c.Accumulate ([] (T lhs, T rhs) { return lhs + rhs; }) == 24);
         *      \endcode
         *
         *  \note   returns nullopt if empty list
         *
         *  See:
         *      @see AccumulateValue
         *      @see Join
         *      @see Sum
         *      @see SumValue
         */
        template <typename RESULT_TYPE = T>
        nonvirtual optional<RESULT_TYPE> Accumulate (const function<RESULT_TYPE (ArgByValueType<T>, ArgByValueType<T>)>& op) const;

    public:
        /**
         *  @see @Accumulate, but if value is missing, returns defaultValue arg or {}
         */
        template <typename RESULT_TYPE = T>
        nonvirtual RESULT_TYPE AccumulateValue (const function<RESULT_TYPE (ArgByValueType<T>, ArgByValueType<T>)>& op, ArgByValueType<RESULT_TYPE> defaultValue = {}) const;

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
         *  \note   Equivalent to Accumulate ([] (T lhs, T rhs) { return min (lhs, rhs); })
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
         *  \note   Equivalent to Accumulate ([] (T lhs, T rhs) { return max (lhs, rhs); })
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
         *  \note   Equivalent to Accumulate ([] (T lhs, T rhs) { return lhs + rhs; })
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
         */
        nonvirtual bool Any () const;
        nonvirtual bool Any (const function<bool (ArgByValueType<T>)>& includeIfTrue) const;

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
        static _IterableRepSharedPtr Clone_ (const _IRep& rep);

    private:
#if (__cplusplus < kStrokia_Foundation_Configuration_cplusplus_20) || qCompilerAndStdLib_lambdas_in_unevaluatedContext_Buggy
        struct Rep_Cloner_ {
            auto operator() (const _IRep& t) const -> PtrImplementationTemplate<_IRep>
            {
                return Iterable<T>::Clone_ (t);
            }
        };
#else
        using Rep_Cloner_ = decltype (
            [] (const _IRep& t) -> PtrImplementationTemplate<_IRep> {
                return Iterable<T>::Clone_ (t);
            });
#endif

    private:
        template <typename CONTAINER_OF_T>
        static Iterable<T> mk_ (CONTAINER_OF_T&& from);

    protected:
        /**
         *  \brief  Lazy-copying smart pointer mostly used by implementors (can generally be ignored by users).
         *  However, protected because manipulation needed in some subclasses (rarely) - like UpdatableIteratable.
         */
        using _SharedByValueRepType = Memory::SharedByValue<_IRep, Memory::SharedByValue_Traits<_IRep, _IterableRepSharedPtr, Rep_Cloner_>>;

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
    class Iterable<T>::_SafeReadRepAccessor : private shared_lock<const Debug::AssertExternallySynchronizedMutex> {
    public:
        _SafeReadRepAccessor () = delete;
        _SafeReadRepAccessor (const _SafeReadRepAccessor& src) noexcept;
        _SafeReadRepAccessor (_SafeReadRepAccessor&& src) noexcept;
        _SafeReadRepAccessor (const Iterable<T>* it) noexcept;

    public:
        nonvirtual _SafeReadRepAccessor& operator= (const _SafeReadRepAccessor& rhs) noexcept;

    public:
        nonvirtual const REP_SUB_TYPE& _ConstGetRep () const noexcept;

    private:
        const REP_SUB_TYPE* fConstRef_;
#if qDebug
        const Iterable<T>* fIterableEnvelope_;
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
    class Iterable<T>::_SafeReadWriteRepAccessor : private lock_guard<const Debug::AssertExternallySynchronizedMutex> {
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
        Iterable<T>*  fIterableEnvelope_; // mostly saved for assertions, but also for _UpdateRep- when we lose that - we can ifdef qDebug this field (as we do for read accessor)
        REP_SUB_TYPE* fRepReference_;
    };

#if 0
    /**
     *  AWKWARD attempt at templating / automating the inclusio of the Find_equal_to() methods
     *      -- LGP 2021-11-22
     */
    template <typename T, bool HAS_EQUAL_TO>
    struct Add_FindByEqualTo_PureVirtualDeclaration;
    template <typename T>
    struct Add_FindByEqualTo_PureVirtualDeclaration<T, false> {
    };
    template <typename T>
    struct Add_FindByEqualTo_PureVirtualDeclaration<T, true> {
        virtual Iterator<T> Find_equal_to ([[maybe_unused]] const ArgByValueType<T>& v) const = 0;
    };
    /**
     * @see Add_FindByEqualTo_PureVirtualDeclaration
     */
    template <typename T, bool HAS_EQUAL_TO, typename IMPL>
    struct Add_FindByEqualTo_Override;
    template <typename T, typename IMPL>
    struct Add_FindByEqualTo_Override<T, false, IMPL> {
    };
    template <typename T, typename IMPL>
    struct Add_FindByEqualTo_Override<T, true, IMPL> {
        virtual Iterator<T> Find_equal_to (const ArgByValueType<T>& v) const override
        {
            return IMPL{}(v);
        }
    };
    then below...
              //  : public Add_FindByEqualTo_PureVirtualDeclaration<T, Configuration::HasUsableEqualToOptimization<T> ()>
#endif

    /**
     *  \brief  Implementation detail for iterator implementors.
     *
     *  Abstract class used in subclasses which extend the idea of Iterable.
     *  Most abstract Containers in Stroika subclass of Iterable<T>.
     */
    template <typename T>
    class Iterable<T>::_IRep
#if qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
        : public IterableBase::enable_shared_from_this_PtrImplementationTemplate<typename Iterable<T>::_IRep>
#endif
    {
    protected:
        _IRep () = default;

    public:
        virtual ~_IRep () = default;

    protected:
        using _IterableRepSharedPtr = typename Iterable<T>::_IterableRepSharedPtr;

    public:
        virtual _IterableRepSharedPtr Clone () const = 0;
        /*
         */
        virtual Iterator<value_type> MakeIterator () const                                                    = 0;
        virtual size_t               size () const                                                            = 0;
        virtual bool                 empty () const                                                           = 0;
        virtual void                 Apply (const function<void (ArgByValueType<T> item)>& doToElement) const = 0;
        virtual Iterator<value_type> Find (const function<bool (ArgByValueType<T> item)>& that) const         = 0;
        /**
         *  Find_equal_to is Not LOGICALLY needed, as you can manually iterate (just use Find()). 
         * But this CAN be much faster (and commonly is) - and is used very heavily by iterables, so
         * its worth the singling out of this important special case.
         * 
        // @todo make this pure virtual once I've had a chance to subclass
         *  Not 
         * The COST of having this is that ALL Iterable<T>::_IRep MUST provide this implementation
         * As in:
         * 
                virtual Iterator<value_type> Find_equal_to (const ArgByValueType<value_type>& v) const override
                {
                    return this->_Find_equal_to_default_implementation (v);
                }
         * and obviously imopelmetantiosn that can provide a faster implemetnation will do so.
         */
        virtual Iterator<value_type> Find_equal_to (const ArgByValueType<T>& v) const = 0;

    protected:
        /*
         * Helper functions to simplify implementation of above public APIs. These MAY or MAY NOT be used in
         * actual subclasses.
         */
        nonvirtual bool _IsEmpty () const;
        nonvirtual void _Apply (const function<void (ArgByValueType<value_type> item)>& doToElement) const;
        template <typename THAT_FUNCTION, enable_if_t<Configuration::IsTPredicate<T, THAT_FUNCTION> ()>* = nullptr>
        nonvirtual Iterator<value_type> _Find (THAT_FUNCTION&& that) const;
        /**
         *  Default implementation for Find_equal_to function.
         */
        nonvirtual Iterator<value_type> _Find_equal_to_default_implementation (const ArgByValueType<value_type>& v) const;
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
    template <typename T_EQUALS_COMPARER>
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
