/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Sequence_h_
#define _Stroika_Foundation_Containers_Sequence_h_ 1

#include "../StroikaPreComp.h"

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
 *      @todo       Started using concepts on CTORs, but make sure THIS supports the appropriate new Container
 *                  concepts and that it USES that for the appropriate overloaded constructors.
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
 *      @todo       Must support Iterator<T>::operator-(Itertoar<T>) or some-such so that SequenceIterator must work with qsort().
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
 *
 *  Long-Term TODO:
 *
 *      @todo       Consider adding a SubSequence(i,j) method - like std::basic_string<>::substr(). Make it efficient
 *                  so lazy-copy except on update. Medium/low priority
 */

namespace Stroika::Foundation {
    namespace Containers {

        using Configuration::ArgByValueType;
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
         *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
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
         *  \note   See coding conventions document about operator usage: Compare () and operator<, operator>, etc
         *
         *  \note Note About Iterators
         *      o   Stroika container iterators must have shorter lifetime than the container they are iterating over.
         *
         *      o   Stroika container iterators are all automatically patched, so that if you change the underlying container
         *          the iterators are automatically updated internally to behave sensibly.
         *
         */
        template <typename T>
        class Sequence : public Iterable<T> {
        private:
            using inherited = Iterable<T>;

        protected:
            class _IRep;

        protected:
            using _SequenceRepSharedPtr = typename inherited::template SharedPtrImplementationTemplate<_IRep>;

        public:
            /**
             *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
             */
            using ArchetypeContainerType = Sequence<T>;

        public:
            /**
             *  \brief check if the argument type can be passed as argument to the arity/1 overload of Add (prepend/append)
             *
             *  \todo https://stroika.atlassian.net/browse/STK-651 - Experimental feature which might be used as a concept check on various templates
             */
            template <typename POTENTIALLY_ADDABLE_T>
            static constexpr bool IsAddable = is_convertible_v<POTENTIALLY_ADDABLE_T, T>;

        public:
            /**
             *  For the CTOR overload with CONTAINER_OF_ADDABLE, its anything that supports c.begin(), c.end () to find
             *  all the elements.
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
            Sequence (const Sequence& src) noexcept = default;
            Sequence (Sequence&& src) noexcept      = default;
            Sequence (const initializer_list<T>& src);
            template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT<CONTAINER_OF_ADDABLE, T>::value and not is_convertible_v<const CONTAINER_OF_ADDABLE*, const Sequence<T>*>>* = nullptr>
            Sequence (const CONTAINER_OF_ADDABLE& src);
            template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
            Sequence (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);

        protected:
            explicit Sequence (const _SequenceRepSharedPtr& rep) noexcept;
            explicit Sequence (_SequenceRepSharedPtr&& rep) noexcept;

#if qDebug
        public:
            ~Sequence ();
#endif

        public:
            /**
             */
            nonvirtual Sequence<T>& operator= (const Sequence<T>& rhs) = default;
            nonvirtual Sequence<T>& operator= (Sequence<T>&& rhs) = default;

        public:
            /**
             *  Apply the function funciton to each element, and return all the ones for which it was true.
             *
             *  @see Iterable<T>::Where
             */
            nonvirtual Sequence<T> Where (const function<bool(ArgByValueType<T>)>& doToElement) const;

        public:
            /**
             * Only supported of T::Compare() or T::compare() defined
             *      (CONSIDER NEW code to detect methods in templates)
             *      (MAYBE always use compare() - not Compare)
             */
            template <typename ELEMENT_COMPARER = Common::ThreeWayCompare<T>>
            nonvirtual int Compare (const Iterable<T>& rhs, const ELEMENT_COMPARER& comparer = {}) const;

        public:
            /**
             *  A Sequence<T> doesn't generally require a comparison for individual elements
             *  be be defined, but obviously to compare if the containers are equal, you must
             *  compare the individual elements (at least sometimes).
             *
             *  If operator==(T,T) is predefined, you can just call Equals() - but if its not, or if you wish
             *  to compare with an alternative comparer.
             */
            template <typename EQUALS_COMPARER = std::equal_to<T>>
            nonvirtual bool Equals (const Sequence& rhs, const EQUALS_COMPARER& equalsComparer = {}) const;

        public:
            /**
             *  \ens size () == 0
             */
            nonvirtual void RemoveAll ();

        public:
            /**
             *  \req i < size ()
             */
            nonvirtual T GetAt (size_t i) const;

        public:
            /**
             *  \req i < size ()
             */
            nonvirtual void SetAt (size_t i, ArgByValueType<T> item);

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
            nonvirtual T operator[] (size_t i) const;
#if Stroika_Foundation_Containers_Sequence_SupportProxyModifiableOperatorBracket
            nonvirtual TemporaryElementReference_<T> operator[] (size_t i);
#endif

#if Stroika_Foundation_Containers_Sequence_SupportProxyModifiableOperatorOpenCloseParens
        public:
            /**
             *  \req i < size ()
             *
             *  \note - variant returning TemporaryElementReference_ is EXPERIMENTAL as of 2017-02-21 - if Stroika_Foundation_Containers_Sequence_SupportProxyModifiableOperatorBracket
             */
            nonvirtual TemporaryElementReference_<T> operator() (size_t i);
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
            template <typename EQUALS_COMPARER = std::equal_to<T>>
            nonvirtual optional<size_t> IndexOf (ArgByValueType<T> i, const EQUALS_COMPARER& equalsComparer = {}) const;
            template <typename EQUALS_COMPARER = std::equal_to<T>>
            nonvirtual optional<size_t> IndexOf (const Sequence<T>& s, const EQUALS_COMPARER& equalsComparer = {}) const;
            template <typename IGNORED = void>
            nonvirtual size_t IndexOf (const Iterator<T>& i) const;

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
            nonvirtual void Insert (size_t i, ArgByValueType<T> item);
            nonvirtual void Insert (const Iterator<T>& i, ArgByValueType<T> item);

        public:
            /**
             *  \brief Insert all the given items into this sequence, starting at offset 'i'.
             */
            template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
            nonvirtual void InsertAll (size_t i, COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);
            template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::has_beginend<CONTAINER_OF_ADDABLE>::value>* = nullptr>
            nonvirtual void InsertAll (size_t i, const CONTAINER_OF_ADDABLE& s);

        public:
            /**
             */
            nonvirtual void Prepend (ArgByValueType<T> item);

        public:
            /**
             */
            template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::has_beginend<CONTAINER_OF_ADDABLE>::value>* = nullptr>
            nonvirtual void PrependAll (const CONTAINER_OF_ADDABLE& s);
            template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
            nonvirtual void PrependAll (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);

        public:
            /**
             *  This is roughly Insert (GetLength(), item), except that there is a race after you call GetLength, and before
             *  Insert, which calling Append () avoids.
             */
            nonvirtual void Append (ArgByValueType<T> item);

        public:
            /**
             *  This is roughly AppendAll (GetLength(), s), except that there is a race after you call GetLength,
             *  and before Insert, which calling Append () avoids. Also note - if used in a multithreaded enivonment,
             *  the appended items wont necesarily all get appended at once, since other threads could make
             *  changes in between.
             */
            template <typename CONTAINER_OF_ADDABLE, typename ENABLE_IF = enable_if_t<Configuration::has_beginend<CONTAINER_OF_ADDABLE>::value>>
            nonvirtual void AppendAll (const CONTAINER_OF_ADDABLE& s);
            template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
            nonvirtual void AppendAll (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);

        public:
            /**
             * This function requires that the iterator 'i' came from this container.
             *
             * The value pointed to by 'i' is updated - replaced with the value 'newValue'.
             */
            nonvirtual void Update (const Iterator<T>& i, ArgByValueType<T> newValue);

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
            nonvirtual void Remove (size_t i);
            nonvirtual void Remove (size_t start, size_t end);
            nonvirtual void Remove (const Iterator<T>& i);

        public:
            /*
             *  Convert Sequence<T> losslessly into a standard supported C++ type.
             *  Supported types include:
             *      o   vector<T>
             *      o   list<T>
             *      (maybe any container that takes CTOR (IT BEGIN, IT END) - but dont count on that yet...
             */
            template <typename CONTAINER_OF_ADDABLE>
            nonvirtual CONTAINER_OF_ADDABLE As () const;
            template <typename CONTAINER_OF_ADDABLE>
            nonvirtual void As (CONTAINER_OF_ADDABLE* into) const;

        public:
            /**
             *  @see Iterable<T>::First ()
             */
            nonvirtual optional<T> First () const;
            nonvirtual optional<T> First (const function<bool(ArgByValueType<T>)>& that) const;

        public:
            /**
             *  @see Iterable<T>::FirstValue ()
             */
            nonvirtual T FirstValue (ArgByValueType<T> defaultValue = {}) const;

        public:
            /**
             *  @see Iterable<T>::Last ()
             */
            nonvirtual optional<T> Last () const;
            nonvirtual optional<T> Last (const function<bool(ArgByValueType<T>)>& that) const;

        public:
            /**
             *  @see Iterable<T>::LastValue ()
             */
            nonvirtual T LastValue (ArgByValueType<T> defaultValue = {}) const;

        public:
            /**
                 */
            nonvirtual void push_back (ArgByValueType<T> item);

        public:
            /**
             *  Read the last element (GetLast()). Requires not empty.
             */
            nonvirtual T back () const;

        public:
            /**
             */
            nonvirtual T front () const;

        public:
            /**
             * \brief STL-ish alias for RemoveAll ().
             */
            nonvirtual void clear ();

        public:
            nonvirtual Sequence<T>& operator+= (ArgByValueType<T> item);
            nonvirtual Sequence<T>& operator+= (const Sequence<T>& items);

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

        using Traversal::IteratorOwnerID;

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
            static constexpr size_t _kBadSequenceIndex = numeric_limits<size_t>::max ();

        protected:
            using _SequenceRepSharedPtr = typename Sequence<T>::_SequenceRepSharedPtr;

        public:
            virtual _SequenceRepSharedPtr CloneEmpty (IteratorOwnerID forIterableEnvelope) const = 0;
            // 'i' argument to GetAt MAYBE kBadSequenceIndex - indictating last element
            virtual T      GetAt (size_t i) const                                    = 0;
            virtual void   SetAt (size_t i, ArgByValueType<T> item)                  = 0;
            virtual size_t IndexOf (const Iterator<T>& i) const                      = 0;
            virtual void   Remove (const Iterator<T>& i)                             = 0;
            virtual void   Update (const Iterator<T>& i, ArgByValueType<T> newValue) = 0;
            // 'at' argument to Insert MAYBE kBadSequenceIndex - indictating append
            virtual void Insert (size_t at, const T* from, const T* to) = 0;
            virtual void Remove (size_t from, size_t to)                = 0;
#if qDebug
            virtual void AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const = 0;
#endif
        private:
            friend Sequence<T>;
        };

        /**
         *  operator indirects to Sequence<>::Compare()
         */
        template <typename T>
        bool operator< (const Sequence<T>& lhs, const Sequence<T>& rhs);

        /**
             *  operator indirects to Sequence<>::Compare()
             */
        template <typename T>
        bool operator<= (const Sequence<T>& lhs, const Sequence<T>& rhs);

        /**
         *  operator indirects to Sequence<>::Equals()
         */
        template <typename T>
        bool operator== (const Sequence<T>& lhs, const Sequence<T>& rhs);

        /**
         *  operator indirects to Sequence<>::Equals()
         */
        template <typename T>
        bool operator!= (const Sequence<T>& lhs, const Sequence<T>& rhs);

        /**
         *  operator indirects to Sequence<>::Compare()
         */
        template <typename T>
        bool operator>= (const Sequence<T>& lhs, const Sequence<T>& rhs);

        /**
         *  operator indirects to Sequence<>::Compare()
         */
        template <typename T>
        bool operator> (const Sequence<T>& lhs, const Sequence<T>& rhs);
    }
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "Sequence.inl"

#endif /*_Stroika_Foundation_Containers_Sequence_h_ */
