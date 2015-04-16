/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Iterable_h_
#define _Stroika_Foundation_Traversal_Iterable_h_  1

#include    "../StroikaPreComp.h"

#include    <functional>

#include    "../Common/Compare.h"
#include    "../Configuration/Common.h"
#include    "../Memory/SharedByValue.h"
#include    "../Memory/SharedPtr.h"

#include    "Iterator.h"



/**
 *  \file
 *
 *  TODO:
 *
 *      @todo   Consider if we should use range_error, some other error, or assertion errors for invalid use
 *              cases of Mean/Median, etc.
 *
 *              Probably do variants (default one asserts, and rest are OrDefault or OrException).
 *
 *      @todo   SUBCLASSES of Iterable<> need to overload/replace several of these functions taking
 *              into account (by default) their comparers... Eg. Set<> should overload Distinct to do nothing by
 *              default.
 *
 *      @todo   max should take lambda that folds in the select,
 *              and Distinct and take lambda
 *
 *      @todo   Document (which -not all) Linq-like functions only pull as needed from the
 *              original source, and which force a pull (like where doesnt but max does).
 *
 *      @todo   Add more Linq-like functions, at least including:
 *              First/Last (have them throw),
 *              FirstValue/LastValue() - ahve them do default. DOCUMENT how this differences from
 *              LINQ versions.
 *              Do GroupBy.
 *
 *      @todo   Add more 'linq' overloads, taking differnt kinds of compare functions, and field selectors.
 *
 *      @todo   Consider having Linq-like functions do DELAYED EVALUATION, so the computation only
 *              happens when you iterate. Maybe to some degree this already happens, but could do
 *              more (as MSFT does).
 *
 *      @todo   Ordering of parameters to SetEquals() etc templates? Type deduction versus
 *              default parameter?
 *
 *      @todo   REDO DOCS FOR ITERABLE - SO CLEAR ITS ALSO THE BASIS OF "GENERATORS". IT COULD  BE RENAMED
 *              GENERATOR (though dont)
 *
 *      @todo   since Iterator<T> now uses iterator<> traits stuff, so should Iterable<T>?
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /**
             *  Stroika's Memory::SharedPtr<> appears to be a bit faster than the std::shated_ptr. Iterable
             *  can be configured (at compile time) to use one or the other, but not both.
             *
             *  Note - main reasion Stroika SharedPtr<> faster is that we aren't using make_shared. It's possible
             *  that might be a better strategy? -- LGP 2014-04-19
             */
#ifndef qStroika_Foundation_Traveral_IterableUsesStroikaSharedPtr
#define qStroika_Foundation_Traveral_IterableUsesStroikaSharedPtr   1
#endif


            /**
             *  EXPERIMENTAL AS OF v2.0a22x
             *
             *  @todo - TEST. I dont think this is important one way or the other, but I think it may aid performance,
             *          especailly if NOT using qStroika_Foundation_Traveral_IterableUsesStroikaSharedPtr, because of the single
             *          memory allocation (like make_shared<>?).
             */
#ifndef qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
#define qStroika_Foundation_Traveral_IterableUsesSharedFromThis_    1
#endif


            /**
             *  EXPERIMENTAL AS OF v2.0a22x
             *
             *  This has a moderate performance cost (overhead starting iteration). However, it assures bug-free
             *  behavior if iterating when the container (still exists so owner OK) - but its rep gets replaced
             *  due to assignment. Key to envelope thread safety.
             *
             *  Probably just always leave on.
             */
#ifndef qStroika_Foundation_Traveral_IteratorRepHoldsIterableOwnerSharedPtr_
#define qStroika_Foundation_Traveral_IteratorRepHoldsIterableOwnerSharedPtr_        1
#endif


            /**
             *  EXPERIMENTAL AS OF v2.0a22x
             *  WILL ALMOST CERTAINLY leave this true, but make an option so I can test performance impact
             */
#ifndef qStroika_Foundation_Traveral_Iterator_SafeRepAccessorIsSafe_
#define qStroika_Foundation_Traveral_Iterator_SafeRepAccessorIsSafe_    1
#endif


            /**
             */
            struct  IterableBase {
            protected:
                /**
                 *      Temporary name/define - for what SharedPtr/shared_ptr impl we are using.
                 *      Experimental, so dont use directly (yet) - til stablized.
                 *          -- LGP 2014-02-23
                 */
#if     qStroika_Foundation_Traveral_IterableUsesStroikaSharedPtr
                template    <typename SHARED_T>
                using   SharedPtrImplementationTemplate =   Memory::SharedPtr<SHARED_T>;
#else
                template    <typename SHARED_T>
                using   SharedPtrImplementationTemplate =   shared_ptr<SHARED_T>;
#endif

#if     qStroika_Foundation_Traveral_IterableUsesStroikaSharedPtr
                template    <typename SHARED_T>
                using   enable_shared_from_this_SharedPtrImplementationTemplate =   Memory::enable_shared_from_this<SHARED_T>;
#else
                template    <typename SHARED_T>
                using   enable_shared_from_this_SharedPtrImplementationTemplate =   std::enable_shared_from_this<SHARED_T>;
#endif
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
             *          nonvirtual  void    _SetRep (SharedIRepPtr rep);
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
             *      Chose NOT to include an Equals(Iterable<T> rhs) const method here, but instead duplicatively in
             *      each subclass, so that it could more easily be implemented efficiently (not a biggie), but more
             *      importantly because it doesnt appear to me to make sense so say that a Stack<T> == Set<T>, even if
             *      their values were the same.
             *
             *      ((REVISION - 2013-12-21 - SEE NEW SetEquals/MultiSetEquals/SequnceEquals methods below)
             *
             *  *Important Design Note*:
             *      Probably important - for performance??? - that all these methods are const,
             *      so ??? think through - what this implies- but probably something about not
             *      threading stuff and ???
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
             *
             *      We choose explicitly not to implement
             *          o   First() - because based on seeing it used quite a bit in .net code, it appears
             *              to encourage buggy usage? - Crash? - maybe add Assert, and throw overloads?/variants.
             *          o   Last() - same as First ()
             *          o   ToList/ToArray, no need because we have As<>, plus no List/Array classes (exactly).
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             */
            template    <typename T>
            class   Iterable : public IterableBase {
            public:
                /**
                 *      \brief  ElementType is just a handly copy of the *T* template type which this
                 *              Iterable<T> parameterizes access to.
                 */
                using   ElementType =    T;

            protected:
                class  _IRep;

            protected:
                /**
                 *  For now, just use the std::shared_ptr<> - but we may want to go back to older Stroika
                 *  SharedPtr<> code - which I believe is more performant (but lacks the weak_ptr feature).
                 *
                 *  To support that possability, be sure to refernece _SharedPtrIRep instead of using shared_ptr<>
                 *  directly.
                 *
                 *  @todo - USE _IterableSharedPtr INSTEAD
                 *  @todo PROBABLY DEPRECATED/DEPRECATE
                 */
                using   _SharedPtrIRep      =   SharedPtrImplementationTemplate<_IRep>;

            protected:
                /**
                 *  _IterableSharedPtr is logically shared_ptr<_IRep>. However, we may use alternative 'shared ptr' implementations,
                 *  so use this type to assure compatability with the approppriate shared ptr implementation.
                 */
                using   _IterableSharedPtr  =   SharedPtrImplementationTemplate<_IRep>;

            public:
                /**
                 *  UNSURE if we need this to be public or not, but leave this around for a while ...
                 *      -- LGP 2014-04-05
                 */
                using   IterableSharedPtr   =   _IterableSharedPtr;

            protected:
                /**
                 *  _IteratorSharedPtr is logically shared_ptr<Iterator<T>::_IRep>. However, we may use alternative 'shared ptr' implementations,
                 *  so use this type to assure compatability with the approppriate shared ptr implementation.
                 */
                using   _IteratorSharedPtr  =   typename Iterator<T>::SharedIRepPtr;

            public:
                /**
                 *  \brief  Iterable are safely copyable (by value).
                 */
                Iterable (const Iterable<T>& from) noexcept;

            public:
                /**
                 *  Make a copy of the given argument, and treat it as an iterable.
                 */
                Iterable (const initializer_list<T>& from);

            public:
                /**
                 *  @todo replace Iterable (const initializer_list<T>& from) CTOR with soemthing like

                        template    <template CONTAINER <typename T>, typename ENABLE_IF = enable_if<hasBeginEndConcept>
                        Iterable (const CONTAINER<T>& from);
                 */

            public:
                /**
                 *  \brief  move CTOR - clears source
                 */
                explicit Iterable (Iterable<T>&& from) noexcept;

            protected:
                /**
                 *  \brief  Iterable's are typically constructed as concrete subtype objects, whose CTOR passed in a shared copyable rep.
                 */
                explicit Iterable (const _SharedPtrIRep& rep) noexcept;

            protected:
                /**
                 *  \brief  Iterable's are typically constructed as concrete subtype objects, whose CTOR passed in a shared copyable rep.
                 */
                explicit Iterable (_SharedPtrIRep&& rep) noexcept;

            public:
                ~Iterable () = default;

            public:
                /**
                 *  \brief  Iterable are safely copyable (by value).
                 *
                 *  But the 'move' assignment operator requires the rhs to not have any other threads accessing it
                 *  at the time of move.
                 */
                nonvirtual  Iterable<T>&    operator= (const Iterable<T>& rhs);
                nonvirtual  Iterable<T>&    operator= (Iterable<T> && rhs);

            public:
                /**
                 * \brief Create an iterator object which can be used to traverse the 'Iterable'.
                 *
                 * Create an iterator object which can be used to traverse the 'Iterable' - this object -
                 * and visit each element.
                 */
                nonvirtual  Iterator<T>      MakeIterator () const;

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
                nonvirtual  size_t          GetLength () const;

            public:
                /**
                 * \brief Returns true iff GetLength() == 0
                 *
                 *  \em Performance:
                 *      The performance of IsEmpty() may vary wildly (@see GetLength) but will nearly always be O(1).
                 */
                nonvirtual  bool    IsEmpty () const;

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
                template    <typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
                nonvirtual  bool    Contains (const T& element) const;

            public:
                /**
                 *  SetEquals () - very inefficiently - but with constant small memory overhead - returns true if
                 *  each element in the each iterable is contained in the other. They lengths CAN be differnt
                 *  and the two Iterables<> be SetEquals().
                 *
                 *  \em Performance:
                 *      This algorithm is O(N) * O(M) where N and M are the length of the two respective iterables.
                 *
                 */
                template    <typename RHS_CONTAINER_TYPE, typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
                nonvirtual  bool    SetEquals (const RHS_CONTAINER_TYPE& rhs) const;

            public:
                /**
                 *  MultiSetEquals () - very inefficiently - but with constant small memory overhead - returns true if
                 *  each element in the each iterable is contained in the other. They lengths CAN be differnt
                 *  and the two Iterables<> be SetEquals().
                 *
                 *  \em Performance:
                 *      This algorithm is N^^3
                 *
                 */
                template    <typename RHS_CONTAINER_TYPE, typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
                nonvirtual  bool    MultiSetEquals (const RHS_CONTAINER_TYPE& rhs) const;

            public:
                /**
                 *  SequnceEquals () - very measures if iteration over the two containers produces identical sequences
                 *  of elements (identical by compare with EQUALS_COMPARER).
                 *
                 *  \em Performance:
                 *      This algorithm is O(N)
                 */
                template    <typename RHS_CONTAINER_TYPE, typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
                nonvirtual  bool    SequnceEquals (const RHS_CONTAINER_TYPE& rhs) const;

            public:
                /**
                 * \brief Support for ranged for, and STL syntax in general
                 *
                 *  begin ()/end() are similar to MakeIterator(), except that they allow for iterators to be
                 *  used in an STL-style, which is critical for using C++ ranged iteration.
                 *
                 *  For example:
                 *
                 *      for (Iterator<T> i = c.begin (); i != c.end (); ++i) {
                 *          if (*i = T ()) {
                 *              break;
                 *          }
                 *      }
                 *
                 * OR
                 *      for (T& i : c) {
                 *          if (*i = T ()) {
                 *              break;
                 *          }
                 *      }
                 *
                 */
                nonvirtual  Iterator<T> begin () const;

            public:
                /**
                 * \brief Support for ranged for, and STL syntax in general
                 */
                static  Iterator<T> end ();

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
                 *  EXAMPLE USAGE:
                 *      unsigned int cnt { 0 };
                 *      s.Apply ([&cnt] (int i) {
                 *         cnt += i;
                 *      });
                 *      DbgTrace ("cnt=%d", cnt);
                 *
                 *  \note   Aliases:
                 *      o   Apply could have logically been called ForEach, and is nearly identical to
                 *          std::for_each (), except for not taking iterators as arguments, and not having
                 *          any return value.
                 *
                 *  \note   \em Thread-Safety   It is critical the argument function (lambda) must not
                 *              directly or indirectly access the Iterable<> being iterated over.
                 *
                 *              Doing so MAY result in a deadlock.
                 *
                 *              If there is a need to do this, just use explicit iteration instead.
                 */
                nonvirtual  void    Apply (const function<void(const T& item)>& doToElement) const;

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
                 *  \note   \em Thread-Safety   It is critical the argument function (lambda) must not
                 *              directly or indirectly access the Iterable<> being iterated over.
                 *
                 *              Doing so MAY result in a deadlock.
                 *
                 *              If there is a need to do this, just use explicit iteration instead.
                 *
                 *  EXAMPLE USAGE:
                 *      bool IsAllWhitespace (String s) const
                 *      {
                 *          return not s.FindFirstThat ([] (Character c) -> bool { return not c.IsWhitespace (); });
                 *      }
                 */
                nonvirtual  Iterator<T>    FindFirstThat (const function<bool (const T& item)>& doToElement) const;
                nonvirtual  Iterator<T>    FindFirstThat (const Iterator<T>& startAt, const function<bool (const T& item)>& doToElement) const;

            public:
                /**
                 *  As<CONTAINER_OF_T> () can be used to easily map an iterable to another container
                 *  (for exmaple STL container) which supports begin/end iterator constructor. This is
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
                template    <typename CONTAINER_OF_T>
                nonvirtual  CONTAINER_OF_T    As () const;

            public:
                /**
                 *  EXPERIMENTAL
                 *  BASED ON Microsoft .net Linq.
                 *
                 *  This returns an Iterable<T> with a subset of data - including only the items that pass the argument filter funtion.
                 *
                 *  EXAMPLE:
                 *      Iterable<int> c { 1, 2, 3, 4, 5, 6 };
                 *      VerifyTestResult (c.Where ([] (int i) { return i % 2 == 0; }).SequnceEquals (Iterable<int> { 2, 4, 6 }));
                 */
                nonvirtual  Iterable<T> Where (const function<bool(T)>& includeIfTrue) const;

            public:
                /**
                 *  EXPERIMENTAL
                 *  BASED ON Microsoft .net Linq.
                 *
                 *  This returns an Iterable<T> that contains just the subset of the items which are distinct (equality comparer)
                 *
                 *  EXAMPLE:
                 *      Iterable<int> c { 1, 2, 2, 5, 9, 4, 5, 6 };
                 *      VerifyTestResult (c.Distinct ().SetEquals (Iterable<int> { 1, 2, 4, 5, 6, 9 }));
                 *
                 *  @todo need overloads taking lambda that projects
                 *  @todo for now use builtin stl set to accumulate, but need flexability on where compare and maybe also redo with hash?
                 */
                nonvirtual  Iterable<T> Distinct () const;
                template    <typename RESULT>
                nonvirtual  Iterable<RESULT> Distinct (const function<RESULT(T)>& extractElt) const;

            public:
                /**
                 *  \brief  Compute a projection of the given type T to some argument set of subtypes, and apply that projection
                 *          to the entire iterable, creating a new iterable.
                 *
                 *  EXPERIMENTAL
                 *
                 *  EXAMPLE:
                 *      Iterable<pair<int,char>> c { {1, 'a'}, {2, 'b'}, {3, 'c'} };
                 *      VerifyTestResult (c.Select<int> ([] (pair<int,char> p) { return p.first; }).SequnceEquals (Iterable<int> { 1, 2, 3 }));
                 *
                 *      @todo provide overload that is more terse, where instead of specifing funciton, you specify ptr-to-member or some such?
                 */
                template    <typename   T1, typename RESULT = T1>
                nonvirtual  Iterable<RESULT>    Select (const function<T1(const T&)>& extract1) const;
                template    <typename   T1, typename   T2, typename RESULT = pair<T1, T2>>
                nonvirtual  Iterable<RESULT>    Select (const function<T1(const T&)>& extract1, const function<T2(const T&)>& extract2) const;
                template    <typename   T1, typename   T2, typename   T3, typename RESULT = tuple<T1, T2, T3>>
                nonvirtual  Iterable<RESULT>    Select (const function<T1(const T&)>& extract1, const function<T2(const T&)>& extract2, const function<T3(const T&)>& extract3) const;

            public:
                /**
                 *  EXPERIMENTAL
                 *  BASED ON Microsoft .net Linq.
                 *
                 *  This returns an Iterable<T> with a subset of data after skipping the argument number of items.
                 *  If the number of items skipped is greater or equal to the length of the original Iterable, then
                 *  an empty Iterable is returned.
                 *
                 *  EXAMPLE:
                 *      Iterable<int> c { 1, 2, 3, 4, 5, 6 };
                 *      VerifyTestResult (c.Skip (3).SequnceEquals (Iterable<int> { 4, 5, 6 }));
                 *
                 *  See:
                 *      https://msdn.microsoft.com/en-us/library/bb358985%28v=vs.100%29.aspx?f=255&MSPPError=-2147217396
                 *      @Take
                 */
                nonvirtual  Iterable<T> Skip (size_t nItems) const;

            public:
                /**
                 *  EXPERIMENTAL
                 *  BASED ON Microsoft .net Linq.
                 *
                 *  This returns an Iterable<T> with up to nItems taken from the front of this starting iterable. If this Iterable
                 *  is shorter, Take () returns just the original Iterable
                 *
                 *  EXAMPLE:
                 *      Iterable<int> c { 1, 2, 3, 4, 5, 6 };
                 *      VerifyTestResult (c.Take (3).SequnceEquals (Iterable<int> { 1, 2, 3 }));
                 *
                 *  See:
                 *      https://msdn.microsoft.com/en-us/library/bb503062%28v=vs.100%29.aspx?f=255&MSPPError=-2147217396
                 *      @Skip
                 */
                nonvirtual  Iterable<T> Take (size_t nItems) const;

#if     qCompilerAndStdLib_DefaultParamerOfStaticFunctionWithValueLambdaOfWithEmptyClosure_Buggy
            private:
                inline  static  bool    DefaultCompareLessArg_ (const T& lhs, const T& rhs)               {       return lhs < rhs;       }
#endif

            public:
                /**
                 *  EXPERIMENTAL
                 *  BASED ON Microsoft .net Linq.
                 *
                 *  @todo FOR NOW VERY LIMITED API. Linq has params to let you select just the KEY to use in compare. Could add overloads
                 *
                 *  EXAMPLE:
                 *      Iterable<int> c { 3, 5, 9, 38, 3, 5 };
                 *      VerifyTestResult (c.OrderBy ().SequnceEquals (Iterable<int> { 3, 3, 5, 5, 9, 38 }));
                 *
                 *  See:
                 */
                nonvirtual  Iterable<T> OrderBy (const function<bool(T, T)>& compare =
#if     qCompilerAndStdLib_DefaultParamerOfStaticFunctionWithValueLambdaOfWithEmptyClosure_Buggy
                                                     DefaultCompareLessArg_
#else
                                                     [] (const T& lhs, const T& rhs) -> bool { return lhs < rhs; }
#endif
                                                ) const;

            public:
                /**
                 *  EXPERIMENTAL
                 *  BASED ON Microsoft .net Linq.
                 *
                 *  EXAMPLE:
                 *      Iterable<int> c { 1, 2, 3, 4, 5, 6 };
                 *      VerifyTestResult (c.Min () == 1);
                 *
                 *  Throws (docuemnt what) exception if list is empty.
                 *
                 *  See:
                 *      https://msdn.microsoft.com/en-us/library/bb503062%28v=vs.100%29.aspx?f=255&MSPPError=-2147217396
                 *      @Max
                 */
                nonvirtual  T Min () const;

            public:
                /**
                 *  EXPERIMENTAL
                 *  BASED ON Microsoft .net Linq.
                 *
                 *  EXAMPLE:
                 *      Iterable<int> c { 1, 2, 3, 4, 5, 6 };
                 *      VerifyTestResult (c.Max () == 6);
                 *
                 *  Throws (docuemnt what) exception if list is empty.
                 *
                 *  See:
                 *      https://msdn.microsoft.com/en-us/library/bb503062%28v=vs.100%29.aspx?f=255&MSPPError=-2147217396
                 *      @Min
                 */
                nonvirtual  T Max () const;

            public:
                /**
                 *  EXPERIMENTAL
                 *  BASED ON Microsoft .net Linq.
                 *
                 *  EXAMPLE:
                 *      Iterable<int> c { 1, 2, 3, 4, 5, 9 };
                 *      VerifyTestResult (c.Mean () == 4);
                 *
                 *  Throws (docuemnt what) exception if list is empty.
                 *
                 *  AKA "Average"
                 *
                 *  See:
                 *      https://msdn.microsoft.com/en-us/library/bb548647(v=vs.100).aspx
                 */
                template    <typename   RESULT_TYPE = T>
                nonvirtual  RESULT_TYPE Mean () const;

            public:
                /**
                 *  EXPERIMENTAL
                 *
                 *  EXAMPLE:
                 *      Iterable<int> c { 1, 2, 9, 4, 5, 3 };
                 *      VerifyTestResult (NearlyEquals (c.Median (), 3.5));
                 *
                 *  Throws (docuemnt what) exception if list is empty.
                 */
                template    <typename   RESULT_TYPE = T>
                nonvirtual  RESULT_TYPE Median (const function<bool(T, T)>& compare =
#if     qCompilerAndStdLib_DefaultParamerOfStaticFunctionWithValueLambdaOfWithEmptyClosure_Buggy
                                                    DefaultCompareLessArg_
#else
                                                    [] (const T& lhs, const T& rhs) -> bool { return lhs < rhs; }
#endif
#if     qCompilerAndStdLib_TemplateDiffersOnReturnTypeOnly_Buggy
                                                    , RESULT_TYPE ignored = {}
#endif
                                               ) const;

            public:
                /**
                 * \brief not empty () - synonym for .net Any() Linq method.
                 *
                 *  Second overload (with filter function) same as .Where(filter).Any ();
                 */
                nonvirtual  bool    Any () const;
                nonvirtual  bool    Any (const function<bool(T)>& includeIfTrue) const;

            public:
                /**
                 * \brief STL-ish alias for IsEmpty()
                 */
                nonvirtual  bool    empty () const;

            public:
                /**
                 * \brief STL-ish alias for GetLength()
                 */
                nonvirtual  size_t  length () const;

            public:
                /**
                 * \brief STL-ish alias for GetLength()
                 */
                nonvirtual  size_t  size () const;

            public:
                /**
                 * \brief STL-ish alias for ElementType
                 */
                using   value_type   =       ElementType;

            protected:
                /**
                 *  @see Memory::SharedByValue_State
                 *
                 *  Don't call this lightly. This is just meant for low level or debugging, and for subclass optimizations
                 *  based on the state of the shared common object.
                 */
                nonvirtual  Memory::SharedByValue_State    _GetSharingState () const;

            protected:
                /**
                 *  Return a const reference to owned rep pointer. This ensures result non-null before dereferencing.
                 *
                 *  Note - this is NOT thread safe, so use care (like only use in CTOR/DTOR cuz then we know
                 *  we dont need to be threadsafe).
                 */
                nonvirtual  const typename Iterable<T>::_IRep&   _ConstGetRep () const;

            private:
                struct  Rep_Cloner_ {
                    inline  static  SharedPtrImplementationTemplate<_IRep>  Copy (const _IRep& t, IteratorOwnerID forIterableEnvelope)
                    {
                        return Iterable<T>::Clone_ (t, forIterableEnvelope);
                    }
                };

            private:
                static  _SharedPtrIRep  Clone_ (const _IRep& rep, IteratorOwnerID forIterableEnvelope);

            private:
                static  Iterable<T> mk_ (const initializer_list<T>& from);

            protected:
                /**
                 *  \brief  Lazy-copying smart pointer mostly used by implementors (can generally be ignored by users).
                 *  However, protected because manipulation needed in some subclasses (rarely) - like UpdatableIteratable.
                 */
                using   _SharedByValueRepType =       Memory::SharedByValue<Memory::SharedByValue_Traits<_IRep, _SharedPtrIRep, Rep_Cloner_>>;

            protected:
                /**
                 *  EXPERIMENTAL -- LGP 2014-02-21
                 */
                using   _ReadOnlyIterableIRepReference = typename _SharedByValueRepType::ReadOnlyReference;

            protected:
#if     qCompilerAndStdLib_SafeReadRepAccessor_mystery_Buggy
            public:
#endif
                /**
                 *  EXPERIMENTAL -- LGP 2014-02-21
                 */
                nonvirtual  _ReadOnlyIterableIRepReference   _GetReadOnlyIterableIRepReference () const;

            protected:
                /**
                 *  EXPERIMENTAL -- LGP 2014-02-21
                 */
                template <typename REP_SUB_TYPE = _IRep>
                class   _SafeReadRepAccessor;

            protected:
                /**
                 *  Rarely access in subclasses, but its occasionally needed, like in UpdatableIterator<T>
                 */
                _SharedByValueRepType    _fRep;
            };


            /**
             *  EXPERIMENTAL -- LGP 2014-02-21 - 2.0a22
             *
             *  _SafeReadRepAccessor is used by Iterable<> subclasses to assure threadsafety. It takes the
             *  'this' object, and makes a copy incrementing the reference count, and the caller accesses the
             *  rep through the copied/bumped reference.
             *
             *  This assures that if another thread assigns to *this, that has no corruption effect on this
             *  operation/method running on the prior '*this' object.
             */
            template    <typename T>
            template    <typename REP_SUB_TYPE>
            class   Iterable<T>::_SafeReadRepAccessor  {
            private:
#if     qStroika_Foundation_Traveral_Iterator_SafeRepAccessorIsSafe_
                _ReadOnlyIterableIRepReference    fAccessor_;
#else
                const REP_SUB_TYPE& fConstRef_;
#endif

            public:
                _SafeReadRepAccessor () = delete;
                _SafeReadRepAccessor (const _SafeReadRepAccessor&) = default;
                _SafeReadRepAccessor (const Iterable<T>* it);

            public:
                nonvirtual  _SafeReadRepAccessor& operator= (const _SafeReadRepAccessor&) = default;

            public:
                nonvirtual  const REP_SUB_TYPE&    _ConstGetRep () const;
            };


            /**
             *  \brief  Implementation detail for iterator implementors.
             *
             *  Abstract class used in subclasses which extend the idea of Iterable.
             *  Most abstract Containers in Stroika subclass of Iterable<T>.
             */
            template    <typename T>
            class   Iterable<T>::_IRep
#if     qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
#if     qCompilerAndStdLib_Template_Baseclass_WierdIterableBaseBug
                : public Iterable<T>::enable_shared_from_this_SharedPtrImplementationTemplate<Iterable<T>::_IRep>
#else
                : public IterableBase::enable_shared_from_this_SharedPtrImplementationTemplate<Iterable<T>::_IRep>
#endif
#endif
            {
            protected:
                _IRep () = default;

            public:
                virtual ~_IRep () = default;

            protected:
                using   _SharedPtrIRep = typename Iterable<T>::_SharedPtrIRep;

            public:
                using   _APPLY_ARGTYPE      =   const function<void(const T& item)>& ;
                using   _APPLYUNTIL_ARGTYPE =   const function<bool(const T& item)>& ;

            public:
                virtual _SharedPtrIRep      Clone (IteratorOwnerID forIterableEnvelope) const                           =   0;
                /*
                 *  NB - the suggestedOwnerID argument to MakeIterator() may be used, or ignored by particular subtypes
                 *  of iterator/iterable. There is no gaurantee about the resulting GetOwner() result from the
                 *  iterator returned.
                 *
                 *  \em Design Note
                 *      It might have been better design to make the argument to Iterable<T>::Rep::MakeIterator ()
                 *      be owner instead of suggestedOwner, and then require that it get tracked. But that would
                 *      have imposed a memory (and copying) overhead on each iterator, and the current
                 *      use cases for iterators dont warrant that.
                 *
                 *      I think its good enough that particular subtypes - where tracking an owner makes sense and
                 *      is useful, we be done. And when not useful, it can be optimized away.
                 */
                virtual Iterator<T>         MakeIterator (IteratorOwnerID suggestedOwner) const                         =   0;
                virtual size_t              GetLength () const                                                          =   0;
                virtual bool                IsEmpty () const                                                            =   0;
                virtual void                Apply (_APPLY_ARGTYPE doToElement) const                                    =   0;
                virtual Iterator<T>         FindFirstThat (_APPLYUNTIL_ARGTYPE, IteratorOwnerID suggestedOwner) const   =   0;

            protected:
                /*
                 * Helper functions to simplify implementation of above public APIs. These MAY or MAY NOT be used in
                 * actual subclasses.
                 */
                nonvirtual  bool        _IsEmpty () const;
                nonvirtual  void        _Apply (_APPLY_ARGTYPE doToElement) const;
                nonvirtual  Iterator<T> _FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const;
            };


        }
    }
}



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "Iterable.inl"

#endif  /*_Stroika_Foundation_Traversal_Iterable_h_ */
