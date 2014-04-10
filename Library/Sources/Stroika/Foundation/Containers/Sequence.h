/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Sequence_h_
#define _Stroika_Foundation_Containers_Sequence_h_  1

#include    "../StroikaPreComp.h"

#include    <limits>
#include    <vector>

#include    "../Configuration/Common.h"
#include    "../Configuration/Concepts.h"
#include    "../Common/Compare.h"
#include    "../Memory/SharedByValue.h"
#include    "../Traversal/Iterable.h"
#include    "../Traversal/Iterator.h"



/*
 *
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *
 *  TODO:
 *
 *      @todo       Sequence<T> x;
 *                  x[3] = T();     // This compiles but does the wrong thing (because the return value is rvalue not lvalue)
 *                  Find a safe reliable way to turn this into a warning (not needed to work).
 *
 *                  This
 *                      nonvirtual  T&       operator[] (size_t i) = delete;
 *                  didn't work.
 *
 *      @todo       Stroika v1 had REVERSE_ITERATORS - and so does STL. At least for sequences, we need reverse iterators!
 *                  NOTE - this is NOT a specail TYPE of itearator (unlike STL). Its just iterator returned from rbegin(), rend().
 *
 *      @todo       Sequence<> must support RandomAccessIterator<>
 *
 *      @todo       EachWith() and probably other things should use new EmptyClone() strategy - so cheaper and
 *                  returns something of same underlying data structure  type.
 *
 *      @todo       Add insert(Iterator<T>,T) overload (so works with Mapping<..>::As<...> ()
 *
 *      @todo       Must support Iterator<T>::operator-(Itertoar<T>) or some-such so that SequenceIterator must work with qsort().
 *                  In otehrwords, must act as random-access iterator so it can be used in algorithjms that use STL
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
 *      @todo       Add backend implementation of Sequence<T> using stl types - like
 *                  Sequence_stdvector, and Sequence_stdlist<>
 *
 *      @todo       Make sure that Sequence<T> (vector<T>) CTOR reserves the appropriate size before appending,
 *                  by using type_traits logic to figure out of legal to compare - and see length. Same for
 *                  Sequence<T> (ITER iFrom, ITER iTo) - do re-allocate size if appropriate - can do diff
 *                  iTo-iFrom.
 *
 *  Long-Term TODO:
 *
 *      @todo       Consider adding a SubSequence(i,j) method - like std::basic_string<>::substr(). Make it efficient
 *                  so lazy-copy except on update. Medium/low priority
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            using   Traversal::Iterable;
            using   Traversal::Iterator;


            constexpr   size_t  kBadSequenceIndex   =   numeric_limits<size_t>::max ();


            /**
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
             *  ->  Add SetLength() method. Make sure it is optimally efficeint, but try
             *      to avoid introducing a virtual function. Probably overload, and 1 arg
             *      version will use T default CTOR. If done nonvirtually with templates
             *      then we only require no arg CTOR when this function called - GOOD.
             *      Cannot really do with GenClass (would need to compile in seperate .o,
             *      even that wont work - need to not compile except when called).
             *
             *  ->  Condider doing a T  operator[] (size_t index) const that returns a
             *      T& by having it return a different object that does magic - not
             *      quite sure how ???
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
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
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
             */
            template    <typename T>
            class   Sequence : public Iterable<T> {
            private:
                using   inherited   =   Iterable<T>;

            protected:
                class   _IRep;
                using   _SharedPtrIRep  =   typename inherited::template SharedPtrImplementationTemplate<_IRep>;

            public:
                /**
                 *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
                 */
                using   ArchetypeContainerType  =   Sequence<T>;

            public:
                /**
                 *  For the CTOR overload with CONTAINER_OF_T, its anything that supports c.begin(), c.end () to find
                 *  all the elements.
                 */
                Sequence ();
                Sequence (const Sequence<T>& s);
                Sequence (const initializer_list<T>& s);
                Sequence (const vector<T>& s);
                template <typename CONTAINER_OF_T>
                explicit Sequence (const CONTAINER_OF_T& s);
                template <typename COPY_FROM_ITERATOR_OF_T>
                explicit Sequence (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

            protected:
                explicit Sequence (const _SharedPtrIRep& rep);

#if     qDebug
            public:
                ~Sequence ();
#endif

            public:
                /**
                 */
                nonvirtual  Sequence<T>& operator= (const Sequence<T>& rhs);

            public:
                /**
                 *  \note   Why no Functional optional parameter to Contains()?
                 *      You can call FindFirstThat () directly - and pass in a lambda (bound with whatever data) to get this
                 *      effect.
                 *
                 *      Contains<EQUALS_COMPARERE> is defined as
                 *          return c.FindFirstThat ([item] (T i) -> bool {
                 *              return EQUALS_COMPARERE::Equals (i, item);
                 *          });
                 */
                template    <typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
                nonvirtual  bool    Contains (T item) const;

            public:
                /**
                 *  \em EXPERIMENTAL API (2014-01-08)
                 *
                 *  Apply the function funciton to each element, and return all the ones for which it was true.
                 */
                nonvirtual  Sequence<T>    EachWith (const function<bool(const T& item)>& doToElement) const;

            public:
                /**
                 * Only supported of T::Compare() or T::compare() defined
                 *      (CONSIDER NEW code to detect methods in templates)
                 *      (MAYBE always use compare() - not Compare)
                 */
                template    <typename ELEMENT_COMPARER = Common::ComparerWithWellOrder<T>>
                nonvirtual  int     Compare (const Iterable<T>& rhs) const;

            public:
                /**
                 *  A Sequence<T> doesnt generally require a comparison for individual elements
                 *  be be defined, but obviously to compare if the containers are equal, you must
                 *  compare the individual elements (at least sometimes).
                 *
                 *  If == is predefined, you can just call Equals() - but if its not, or if you wish
                 *  to compare with an alternative comparer, just pass it as a template parameter.
                 */
                template    <typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
                nonvirtual  bool    Equals (const Sequence<T>& rhs) const;

            public:
                /**
                 *  Basic operator overloads with the obivous meaning, and simply indirect to
                 *  @Compare (const Sequence<T>& rhs) and uses  Equals() for ==/!=
                 *
                 *  Note that these are trivial wrappers on Compare () and Equals () - which
                 *  take optional COMPARER arguments (for the elements). These don't take
                 *  any such arguments because it would ruin the syntactic sugar, and
                 *  if we are losing that anyhow, just use Equals<> or Compare<>.
                 */
                nonvirtual  bool operator< (const Sequence<T>& rhs) const;
                nonvirtual  bool operator<= (const Sequence<T>& rhs) const;
                nonvirtual  bool operator> (const Sequence<T>& rhs) const;
                nonvirtual  bool operator>= (const Sequence<T>& rhs) const;
                nonvirtual  bool operator== (const Sequence<T>& rhs) const;
                nonvirtual  bool operator!= (const Sequence<T>& rhs) const;

            public:
                /**
                 *  \ens size () == 0
                 */
                nonvirtual  void    RemoveAll ();

            public:
                /**
                 *  \req i < size ()
                 */
                nonvirtual  T       GetAt (size_t i) const;

            public:
                /**
                 *  \req i < size ()
                 */
                nonvirtual  void    SetAt (size_t i, T item);

            public:
                /**
                 *  \req i < size ()
                 */
                nonvirtual  T       operator[] (size_t i) const;

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
                 */
                template    <typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
                nonvirtual  size_t  IndexOf (T i) const;
                template    <typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
                nonvirtual  size_t  IndexOf (const Sequence<T>& s) const;
                template    <typename IGNORED = void>
                nonvirtual  size_t  IndexOf (const Iterator<T>& i) const;

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
                 */
                nonvirtual  void    Insert (size_t i, T item);

            public:
                /**
                 *  \brief Insert all the given items into this sequence, starting at offset 'i'.
                 */
                nonvirtual  void    InsertAll (size_t i, const Iterable<T>& items);
                template    <typename COPY_FROM_ITERATOR_OF_T>
                nonvirtual  void    InsertAll (size_t i, COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);
                template    <typename CONTAINER_OF_T>
                nonvirtual  void    InsertAll (size_t i, const CONTAINER_OF_T& s);

            public:
                /**
                 */
                nonvirtual  void    Prepend (T item);

            public:
                /**
                 */
                template    <typename CONTAINER_OF_T>
                nonvirtual  void    PrependAll (const CONTAINER_OF_T& s);
                template    <typename COPY_FROM_ITERATOR_OF_T>
                nonvirtual  void    PrependAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

            public:
                /**
                 *  This is roughly Insert (GetLength(), item), except that there is a race after you call GetLength, and before
                 *  Insert, which calling Append () avoids.
                 */
                nonvirtual  void    Append (T item);

            public:
                /**
                 *  This is roughly AppendAll (GetLength(), s), except that there is a race after you call GetLength,
                 *  and before Insert, which calling Append () avoids. Also note - if used in a multithreaded enivonment,
                 *  the appended items wont necesarily all get appended at once, since other threads could make
                 *  changes in between.
                 */
                template    <typename CONTAINER_OF_T>
                nonvirtual  void    AppendAll (const CONTAINER_OF_T& s);
                template    <typename COPY_FROM_ITERATOR_OF_T>
                nonvirtual  void    AppendAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

            public:
                /**
                 * This function requires that the iterator 'i' came from this container.
                 *
                 * The value pointed to by 'i' is updated - replaced with the value 'newValue'.
                 */
                nonvirtual  void    Update (const Iterator<T>& i, T newValue);

            public:
                /**
                 * This function requires that the iterator 'i' came from this container.
                 *
                 * The value pointed to by 'i' is removed.
                 *
                 * Not an error to remove an item that is not an element of the list, instead has no effect.
                 *
                 *      Remove the item at the given position of the sequence. Make sure
                 *  that iteration is not disturbed by this removal. In particular, any
                 *  items (other than the one at index) that would have been seen, will
                 *  still be, and no new items will be seen that wouldn't have been.
                 */
                nonvirtual  void    Remove (size_t i);
                nonvirtual  void    Remove (size_t start, size_t end);
                nonvirtual  void    Remove (const Iterator<T>& i);

            public:
                /*
                 *  Convert Sequence<T> losslessly into a standard supported C++ type.
                 *  Supported types include:
                 *      o   vector<T>
                 *      o   list<T>
                 *      (maybe any container that takes CTOR (IT BEGIN, IT END) - but dont count on that yet...
                 */
                template    <typename   CONTAINER_OF_T>
                nonvirtual  CONTAINER_OF_T   As () const;
                template    <typename   CONTAINER_OF_T>
                nonvirtual  void    As (CONTAINER_OF_T* into) const;

            public:
                /**
                 *  \req not empty ();
                 */
                nonvirtual  T       GetFirst () const;

            public:
                /**
                 *  \req not empty ();
                 */
                nonvirtual  T       GetLast () const;

            public:
                /**
                 */
                nonvirtual  void    push_back (T item);

            public:
                /**
                 */
                nonvirtual  T       back () const;

            public:
                /**
                 */
                nonvirtual  T       front () const;

            public:
                /**
                 * \brief STL-ish alias for RemoveAll ().
                 */
                nonvirtual  void    clear ();

            public:
                nonvirtual  Sequence<T>&    operator+= (T item);
                nonvirtual  Sequence<T>&    operator+= (const Sequence<T>& items);

            public:
                nonvirtual  Sequence<T>&    operator-= (T item);
                nonvirtual  Sequence<T>&    operator-= (const Sequence<T>& items);

            protected:
#if     qCompilerAndStdLib_SafeReadRepAccessor_mystery_Buggy
                template    <typename REP_SUB_TYPE>
                struct  _SafeReadRepAccessor  {
                    typename Iterable<T>::_ReadOnlyIterableIRepReference    fAccessor;
                    _SafeReadRepAccessor (const Iterable<T>* s)
                        : fAccessor (s->_GetReadOnlyIterableIRepReference ())
                    {
                    }
                    nonvirtual  const REP_SUB_TYPE&    _ConstGetRep () const
                    {
                        EnsureMember (fAccessor.get (), REP_SUB_TYPE);
                        return static_cast<const REP_SUB_TYPE&> (*fAccessor.get ());   // static cast for performance sake - dynamic cast in Ensure
                    }
                };
#else
                /**
                 */
                template    <typename T2>
                using   _SafeReadRepAccessor = typename Iterable<T>::template _SafeReadRepAccessor<T2>;

#endif

            protected:
                nonvirtual  const _IRep&    _ConstGetRep () const;

            protected:
                nonvirtual  const _IRep&    _GetRep () const;
                nonvirtual  _IRep&          _GetRep ();
            };


            using   Traversal::IteratorOwnerID;


            /**
             *  \brief  Implementation detail for Sequence<T> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the Sequence<T> container API.
             */
            template    <typename T>
            class   Sequence<T>::_IRep : public Iterable<T>::_IRep {
            protected:
                _IRep () = default;

            public:
                virtual ~_IRep () = default;

            public:
                // 'i' argument to GetAt MAYBE kBadSequenceIndex - indictating last element
                virtual T       GetAt (size_t i) const                                                  =   0;
                virtual void    SetAt (size_t i, const T& item)                                         =   0;
                virtual size_t  IndexOf (const Iterator<T>& i) const                                    =   0;
                virtual void    Remove (const Iterator<T>& i)                                           =   0;
                virtual void    Update (const Iterator<T>& i, T newValue)                               =   0;
                // 'at' argument to Insert MAYBE kBadSequenceIndex - indictating append
                virtual void    Insert (size_t at, const T* from, const T* to)                          =   0;
                virtual void    Remove (size_t from, size_t to)                                         =   0;
#if     qDebug
                virtual void    AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const   =   0;
#endif
            };


        }
    }
}



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "Sequence.inl"

#endif  /*_Stroika_Foundation_Containers_Sequence_h_ */
