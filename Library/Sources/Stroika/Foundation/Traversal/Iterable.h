/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Iterable_h_
#define _Stroika_Foundation_Traversal_Iterable_h_ 1

#include "../StroikaPreComp.h"

#include <functional>
#include <shared_mutex>
#include <vector>

#include "../Common/Compare.h"
#include "../Configuration/Common.h"
#include "../Configuration/TypeHints.h"
#include "../Debug/AssertExternallySynchronizedLock.h"
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
 *              Where... But tricky to unformly add to differnt container types. Maybe only ones you can say add, or the adder is
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
 *      @todo   Add more 'linq' overloads, like groupBy taking differnt kinds of compare functions, and field selectors.
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

namespace Stroika::Foundation::Traversal {

    using Configuration::ArgByValueType;

    /*
     *  COULD possibly fix this by - when we have active iterators - just set them to 'at end'. But I guess that doesn't fix the tracking?
     *  Maybe need actualy move them from one container rep to the other (we do in other cases have code for this).
     *
     *      https://stroika.atlassian.net/browse/STK-570
     */
#ifndef qStroika_Foundation_Traveral_OverwriteContainerWhileIteratorRunning_Buggy
#define qStroika_Foundation_Traveral_OverwriteContainerWhileIteratorRunning_Buggy 1
#endif

    /**
     *  Stroika's Memory::SharedPtr<> appears to be a bit faster than the std::shated_ptr. Iterable
     *  at one time, and on some systems.
     *
     *      This defaults to @see Memory::kSharedPtr_IsFasterThan_shared_ptr
     */
    constexpr bool kIterableUsesStroikaSharedPtr = Memory::kSharedPtr_IsFasterThan_shared_ptr;

    /**
     *  EXPERIMENTAL AS OF v2.0a22x
     *
     *  @todo - TEST. I don't think this is important one way or the other, but I think it may aid performance,
     *          especially if NOT using kIterableUsesStroikaSharedPtr, because of the single
     *          memory allocation (like make_shared<>?).
     */
#ifndef qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
#define qStroika_Foundation_Traveral_IterableUsesSharedFromThis_ 1
#endif

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

    public:
        // SharedPtrImplementationTemplate deprecated - Stroika v2.1b6
        template <typename SHARED_T>
        using SharedPtrImplementationTemplate = PtrImplementationTemplate<SHARED_T>;

    public:
        template <typename SHARED_T, typename... ARGS_TYPE>
        [[deprecated ("use MakeSmartPtr since version 2.1d6???")]] static PtrImplementationTemplate<SHARED_T> MakeSharedPtr (ARGS_TYPE&&... args)
        {
            return MakeSmartPtr<SHARED_T> (forward<ARGS_TYPE> (args)...);
        }
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
     *  *Design Note*:
     *      Why does Iterable<T> contain a GetLength () method?
     *
     *          o   It’s always well defined what GetLength() means (what you would get if you called
     *              MakeIterable() and iterated a bunch of times til the end).
     *
     *          o   Its almost always (and trivial) to perform that computation more efficiently than the
     *              iterate over each element apporach.
     *
     *          The gist of these two consideration means that if you need to find the length of
     *          an Iterable<T>, if it was defined as a method, you can access the trivial implemeantion,
     *          and if it was not defined, you would be forced into the costly implementation.
     *
     *      Adding GetLength () adds no conceptual cost – because its already so well and clearly defined
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
     *      Chose NOT to include an Equals (Iterable<T> rhs) const method here, but instead duplicatively in
     *      each subclass, so that it could more easily be implemented efficiently (not a biggie), but more
     *      importantly because it doesn't appear to me to make sense so say that a Stack<T> == Set<T>, even if
     *      their values were the same.
     *
     *      Also, Equals() meaning is just wrong if you implement it the only way you can for an Iterable,
     *      (iterating over elements) if the container has a differnt logical notion of equals (like Set).
     *
     *      We DO have methods SetEquals/MultiSetEquals/SequnceEquals (see below).
     *
     *  *Important Design Note*:
     *      Probably important - for performance??? - that all these methods are const,
     *      so ??? think through - what this implies- but probably something about not
     *      threading stuff and ???
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
     *          Add overload of FindFirstThat() that takes Iterator<T> as arugument instead of T
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
     *  \note   \em Thread-Safety   <a href="thread_safety.html#ExternallySynchronized">ExternallySynchronized</a>
     *
     */
    template <typename T>
    class Iterable : public IterableBase, protected Debug::AssertExternallySynchronizedLock {
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
         *  \brief  Iterable are safely copyable (by value).
         */
        Iterable (const Iterable& from) noexcept;

    public:
        /**
         *  Make a copy of the given argument, and treat it as an iterable.
         */
        template <typename CONTAINER_OF_T, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_T>>* = nullptr>
        explicit Iterable (CONTAINER_OF_T&& from);

    public:
        Iterable (const initializer_list<T>& from);

    public:
        /**
         *  \brief  move CTOR - clears source
         */
        explicit Iterable (Iterable&& from) noexcept;

    protected:
        /**
         *  \brief  Iterable's are typically constructed as concrete subtype objects, whose CTOR passed in a shared copyable rep.
         */
        explicit Iterable (const _IterableRepSharedPtr& rep) noexcept;

    protected:
        /**
         *  \brief  Iterable's are typically constructed as concrete subtype objects, whose CTOR passed in a shared copyable rep.
         */
        explicit Iterable (_IterableRepSharedPtr&& rep) noexcept;

    public:
        ~Iterable () = default;

    public:
        /**
         *  \brief  Iterable are safely copyable (by value).
         *
         *  But the 'move' assignment operator requires the rhs to not have any other threads accessing it
         *  at the time of move.
         */
        nonvirtual Iterable<T>& operator= (const Iterable& rhs);
        nonvirtual Iterable<T>& operator= (Iterable&& rhs) noexcept;

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
         * GetLength () returns the number of elements in this 'Iterable' object. Its defined to be
         * the same number of elements you would visit if you created an iterator (MakeIterator())
         * and visited all items. In practice, as the actual number might vary as the underlying
         * iterable could change while being iterated over.
         *
         *  Also note that GetLength () can return a ridiculous number - like numeric_limits<size_t>::max () -
         *  for logically infinite sequences... like a sequence of random numbers.
         *
         *  \em Performance:
         *      The performance of GetLength() may vary wildly. It could be anywhere from O(1) to O(N)
         *      depending on the underlying type of Iterable<T>.
         */
        nonvirtual size_t GetLength () const;

    public:
        /**
         * \brief Returns true iff GetLength() == 0
         *
         *  \em Performance:
         *      The performance of IsEmpty() may vary wildly (@see GetLength) but will nearly always be O(1).
         */
        nonvirtual bool IsEmpty () const;

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
         *  each element in the each iterable is contained in the other. They lengths CAN be differnt
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
        template <typename RHS_CONTAINER_TYPE, typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual bool SetEquals (const RHS_CONTAINER_TYPE& rhs, const EQUALS_COMPARER& equalsComparer = EQUALS_COMPARER{}) const;

    public:
        /**
         *  MultiSetEquals () - very inefficiently - but with constant small memory overhead - returns true if
         *  each element in the each iterable is contained in the other. They lengths CAN be differnt
         *  and the two Iterables<> be SetEquals().
         *
         *  \em Performance:
         *      This algorithm is O(N^^3)
         */
        template <typename RHS_CONTAINER_TYPE, typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual bool MultiSetEquals (const RHS_CONTAINER_TYPE& rhs, const EQUALS_COMPARER& equalsComparer = EQUALS_COMPARER{}) const;

    public:
        /**
         *  SequnceEquals () - very measures if iteration over the two containers produces identical sequences
         *  of elements (identical by compare with EQUALS_COMPARER).
         *
         *  \em Performance:
         *      This algorithm is O(N)
         */
        template <typename RHS_CONTAINER_TYPE, typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual bool SequnceEquals (const RHS_CONTAINER_TYPE& rhs, const EQUALS_COMPARER& equalsComparer = EQUALS_COMPARER{}) const;

    public:
        /**
         * \brief Support for ranged for, and STL syntax in general
         *
         *  begin ()/end() are similar to MakeIterator(), except that they allow for iterators to be
         *  used in an STL-style, which is critical for using C++ ranged iteration.
         *
         *  \par Example Usage
         *      \code
         *      for (Iterator<T> i = c.begin (); i != c.end (); ++i) {
         *          if (*i = T{}) {
         *              break;
         *          }
         *      }
         *      \endcode
         *
         * OR
         *      \code
         *      for (T& i : c) {
         *          if (*i = T{}) {
         *              break;
         *          }
         *      }
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
        nonvirtual void Apply (const function<void(ArgByValueType<T> item)>& doToElement) const;

    public:
        /**
         *  \brief  Run the argument bool-returning function (or lambda) on each element of the
         *          container, and return an iterator pointing at the first element found true.
         *
         *  Take the given function argument, and call it for each element of the container. This is
         *  equivalent to:
         *
         *      for (Iterator<T> i = begin (); i != end (); ++i) {
         *          if ((doToElement) (*i)) {
         *              return it;
         *          }
         *      }
         *      return end();
         *
         *  This function returns an iteartor pointing to the element that triggered the abrupt loop
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
         *  \par Example Usage
         *      \code
         *          bool IsAllWhitespace (String s) const
         *          {
         *              return not s.FindFirstThat ([] (Character c) -> bool { return not c.IsWhitespace (); });
         *          }
         *      \endcode
         *
         *  @see First () - often more handy
         */
        nonvirtual Iterator<T> FindFirstThat (const function<bool(ArgByValueType<T> item)>& doToElement) const;
        nonvirtual Iterator<T> FindFirstThat (const Iterator<T>& startAt, const function<bool(ArgByValueType<T> item)>& doToElement) const;

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
         *  EXPERIMENTAL
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
         *          VerifyTestResult (c.Where ([] (int i) { return i % 2 == 0; }).SequnceEquals (Iterable<int> { 2, 4, 6 }));
         *      \endcode
         *
         *  \note   Could have been called EachWith, EachWhere, or EachThat ().
         */
        nonvirtual Iterable<T> Where (const function<bool(ArgByValueType<T>)>& includeIfTrue) const;
        template <typename RESULT_CONTAINER>
        nonvirtual RESULT_CONTAINER Where (const function<bool(ArgByValueType<T>)>& includeIfTrue) const;
        template <typename RESULT_CONTAINER>
        nonvirtual RESULT_CONTAINER Where (const function<bool(ArgByValueType<T>)>& includeIfTrue, const RESULT_CONTAINER& emptyResult) const;

    public:
        /**
         *  EXPERIMENTAL
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

    private:
        template <typename RESULT>
        static Iterable<RESULT> Distinct_mkGenerator_ (const vector<RESULT>& container);

    public:
        /**
         *  \brief  Compute a projection of the given type T to some argument set of subtypes, and apply that projection
         *          to the entire iterable, creating a new iterable.
         *
         *  EXPERIMENTAL
         *
         *  \par Example Usage
         *      \code
         *          Iterable<pair<int,char>> c { {1, 'a'}, {2, 'b'}, {3, 'c'} };
         *          VerifyTestResult (c.Select<int> ([] (pair<int,char> p) { return p.first; }).SequnceEquals (Iterable<int> { 1, 2, 3 }));
         *      \endcode
         *
         *  This can also easily be used to TRANSFORM an iterable.
         *  \par Example Usage
         *      \code
         *          Iterable<int> c { 3, 4, 7 };
         *          VerifyTestResult (c.Select<String> ([] (int i) { return Characters::Format (L"%d", i); }).SequnceEquals (Iterable<String> { L"3", L"4", L"7" }));
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
         *      @todo provide overload that is more terse, where instead of specifing funciton, you specify ptr-to-member or some such?
         */
        template <typename T1, typename RESULT = T1>
        nonvirtual Iterable<RESULT> Select (const function<T1 (const T&)>& extract1) const;
        template <typename T1, typename T2, typename RESULT = pair<T1, T2>>
        nonvirtual Iterable<RESULT> Select (const function<T1 (const T&)>& extract1, const function<T2 (const T&)>& extract2) const;
        template <typename T1, typename T2, typename T3, typename RESULT = tuple<T1, T2, T3>>
        nonvirtual Iterable<RESULT> Select (const function<T1 (const T&)>& extract1, const function<T2 (const T&)>& extract2, const function<T3 (const T&)>& extract3) const;

    public:
        /**
         *  EXPERIMENTAL
         *  BASED ON Microsoft .net Linq.
         *
         *  This returns an Iterable<T> with a subset of data after skipping the argument number of items.
         *  If the number of items skipped is greater or equal to the length of the original Iterable, then
         *  an empty Iterable is returned.
         *
         *  \par Example Usage
         *      \code
         *          Iterable<int> c { 1, 2, 3, 4, 5, 6 };
         *          VerifyTestResult (c.Skip (3).SequnceEquals (Iterable<int> { 4, 5, 6 }));
         *      \endcode
         *
         *  @see https://msdn.microsoft.com/en-us/library/bb358985%28v=vs.100%29.aspx?f=255&MSPPError=-2147217396
         *      @Take
         */
        nonvirtual Iterable<T> Skip (size_t nItems) const;

    public:
        /**
         *  EXPERIMENTAL
         *  BASED ON Microsoft .net Linq.
         *
         *  This returns an Iterable<T> with up to nItems taken from the front of this starting iterable. If this Iterable
         *  is shorter, Take () returns just the original Iterable
         *
         *  \par Example Usage
         *      \code
         *          Iterable<int> c { 1, 2, 3, 4, 5, 6 };
         *          VerifyTestResult (c.Take (3).SequnceEquals (Iterable<int> { 1, 2, 3 }));
         *      \endcode
         *
         *  @see    https://msdn.microsoft.com/en-us/library/bb503062(v=vs.110).aspx
         *      @Skip
         */
        nonvirtual Iterable<T> Take (size_t nItems) const;

    public:
        /**
         *  EXPERIMENTAL
         *  BASED ON Microsoft .net Linq.
         *
         *  @todo FOR NOW VERY LIMITED API. Linq has params to let you select just the KEY to use in compare. Could add overloads
         *
         *  \par Example Usage
         *      \code
         *          Iterable<int> c { 3, 5, 9, 38, 3, 5 };
         *          VerifyTestResult (c.OrderBy ().SequnceEquals (Iterable<int> { 3, 3, 5, 5, 9, 38 }));
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          Iterable<int> c { 3, 5, 9, 38, 3, 5 };
         *          VerifyTestResult (c.OrderBy ([](const T& lhs, const T& rhs) -> bool { return lhs < rhs; }).SequnceEquals (Iterable<int> { 3, 3, 5, 5, 9, 38 }));
         *      \endcode
         *
         * \note alias for Sort ()
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
        nonvirtual optional<T> First (const function<bool(ArgByValueType<T>)>& that) const;

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
        nonvirtual optional<T> Last (const function<bool(ArgByValueType<T>)>& that) const;

    public:
        /**
         *  EXPERIMENTAL
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
         *  EXPERIMENTAL
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
         *  EXPERIMENTAL
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
         *  \note   Equivilent to Accumulate ([] (T lhs, T rhs) { return min (lhs, rhs); })
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
         *  EXPERIMENTAL
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
         *  \note   Equivilent to Accumulate ([] (T lhs, T rhs) { return max (lhs, rhs); })
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
         *  EXPERIMENTAL
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
         *  EXPERIMENTAL
         *  BASED ON Microsoft .net Linq.
         *
         *  \par Example Usage
         *      \code
         *      Iterable<int> c { 1, 2, 3, 4, 5, 9 };
         *      VerifyTestResult (c.Sum () == 24);
         *      \endcode
         *
         *  \note   Equivilent to Accumulate ([] (T lhs, T rhs) { return lhs + rhs; })
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
         *  EXPERIMENTAL
         *
         *  \par Example Usage
         *      \code
         *      Iterable<int> c { 1, 2, 9, 4, 5, 3 };
         *      VerifyTestResult (NearlyEquals (c.Median (), 3.5));
         *      \endcode
         *
         *  \note   returns nullopt if empty list
         */
        template <typename RESULT_TYPE = T>
        nonvirtual optional<RESULT_TYPE> Median (const function<bool(T, T)>& compare = [](const T& lhs, const T& rhs) -> bool { return lhs < rhs; }) const;

    public:
        /**
         *  @see @Median
         */
        template <typename RESULT_TYPE = T>
        nonvirtual RESULT_TYPE MedianValue (ArgByValueType<RESULT_TYPE> defaultValue = {}) const;

    public:
        /**
         * \brief not empty () - synonym for .net Any() Linq method.
         *
         *  Second overload (with filter function) same as .Where(filter).Any ();
         */
        nonvirtual bool Any () const;
        nonvirtual bool Any (const function<bool(ArgByValueType<T>)>& includeIfTrue) const;

    public:
        /**
         * \brief STL-ish alias for IsEmpty()
         */
        nonvirtual bool empty () const;

    public:
        /**
         * \brief STL-ish alias for GetLength()
         */
        nonvirtual size_t length () const;

    public:
        /**
         * \brief STL-ish alias for GetLength()
         */
        nonvirtual size_t size () const;

    protected:
        /**
         *  @see Memory::SharedByValue_State
         *
         *  Don't call this lightly. This is just meant for low level or debugging, and for subclass optimizations
         *  based on the state of the shared common object.
         */
        nonvirtual Memory::SharedByValue_State _GetSharingState () const;

    private:
        struct Rep_Cloner_ {
            inline PtrImplementationTemplate<_IRep> operator() (const _IRep& t, IteratorOwnerID forIterableEnvelope) const
            {
                return Iterable<T>::Clone_ (t, forIterableEnvelope);
            }
        };

    private:
        static _IterableRepSharedPtr Clone_ (const _IRep& rep, IteratorOwnerID forIterableEnvelope);

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
     *  For DEBUGGING (catching races) purposes, it also locks the Debug::AssertExternallySynchronizedLock,
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
     *
     *  <<<DOCS_OBSOLETE_AS_OF_2015-12-24>>>
     *      EXPERIMENTAL -- LGP 2014-02-21 - 2.0a22
     *
     *      _SafeReadRepAccessor is used by Iterable<> subclasses to assure threadsafety. It takes the
     *      'this' object, and makes a copy incrementing the reference count, and the caller accesses the
     *      rep through the copied/bumped reference.
     *
     *      This assures that if another thread assigns to *this, that has no corruption effect on this
     *      operation/method running on the prior '*this' object.
     *  <<</DOCS_OBSOLETE_AS_OF_2015-12-24>>>
     */
    template <typename T>
    template <typename REP_SUB_TYPE>
    class Iterable<T>::_SafeReadRepAccessor : private shared_lock<const Debug::AssertExternallySynchronizedLock> {
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
     *  For DEBUGGING (catching races) purposes, it also locks the Debug::AssertExternallySynchronizedLock,
     *  so that IF this object is accessed illegally by other threads while in use (this use), it will
     *  be caught.
     *
     *  @see _SafeReadRepAccessor
     *
     */
    template <typename T>
    template <typename REP_SUB_TYPE>
    class Iterable<T>::_SafeReadWriteRepAccessor : private lock_guard<const Debug::AssertExternallySynchronizedLock> {
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

    public:
        nonvirtual void _UpdateRep (const typename _SharedByValueRepType::shared_ptr_type& sp);

    private:
        Iterable<T>*  fIterableEnvelope_;
        REP_SUB_TYPE* fRepReference_;
    };

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
        using _APPLY_ARGTYPE      = const function<void(ArgByValueType<T> item)>&;
        using _APPLYUNTIL_ARGTYPE = const function<bool(ArgByValueType<T> item)>&;

    public:
        virtual _IterableRepSharedPtr Clone (IteratorOwnerID forIterableEnvelope) const = 0;
        /*
         *  NB - the suggestedOwnerID argument to MakeIterator() may be used, or ignored by particular subtypes
         *  of iterator/iterable. There is no gaurantee about the resulting GetOwner() result from the
         *  iterator returned.
         *
         *  \em Design Note
         *      It might have been better design to make the argument to Iterable<T>::Rep::MakeIterator ()
         *      be owner instead of suggestedOwner, and then require that it get tracked. But that would
         *      have imposed a memory (and copying) overhead on each iterator, and the current
         *      use cases for iterators don't warrant that.
         *
         *      I think its good enough that particular subtypes - where tracking an owner makes sense and
         *      is useful, we be done. And when not useful, it can be optimized away.
         */
        virtual Iterator<T> MakeIterator (IteratorOwnerID suggestedOwner) const                       = 0;
        virtual size_t      GetLength () const                                                        = 0;
        virtual bool        IsEmpty () const                                                          = 0;
        virtual void        Apply (_APPLY_ARGTYPE doToElement) const                                  = 0;
        virtual Iterator<T> FindFirstThat (_APPLYUNTIL_ARGTYPE, IteratorOwnerID suggestedOwner) const = 0;

    protected:
        /*
         * Helper functions to simplify implementation of above public APIs. These MAY or MAY NOT be used in
         * actual subclasses.
         */
        nonvirtual bool _IsEmpty () const;
        nonvirtual void _Apply (_APPLY_ARGTYPE doToElement) const;
        nonvirtual Iterator<T> _FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const;
    };
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "Iterable.inl"

#endif /*_Stroika_Foundation_Traversal_Iterable_h_ */
