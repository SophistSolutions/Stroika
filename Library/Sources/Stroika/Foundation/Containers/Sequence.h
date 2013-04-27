/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Sequence_h_
#define _Stroika_Foundation_Containers_Sequence_h_  1

/*
 *
 *
 *  STATUS:
 *      CODE NO WHERE NEAR COMPILING - just rough draft of API based on 1992 Stroika
 *
 *
 *
 *  TODO:
 *
 *      (o)         Implement first draft of code based on
 *                  http://github.com/SophistSolutions/Stroika/blob/master/Archive/Stroika_FINAL_for_STERL_1992/Library/Foundation/Headers/Sequence.hh
 *
 *      (o)         Need CTOR that works with iterators. Ideally, dynamically check with C++11 traits if operator- supported
 *                  and use that to allocate if we can, and otherwise just iterate and append.
 *
 *      @todo       CTOR must work with STL containers - like vector, and list so
 *                      Sequence<T> (vector<T> ()) must work, as must
 *                      Sequence<T> (list<T> ()) - and in the former case we should be
 *                      able to figure out size and pre-allocate using new template typetraits stuff.
 *
 *      @todo       Must support Iterator<T>::operator-(Itertoar<T>) or some-such so that SequenceIterator must work with qsort().
 *                  In otehrwords, must act as random-access iterator so it can be used in algorithjms that use STL
 *                  random-access iterators. (FOLLOW RULES OF RANDOM ACCESS ITERAOTRS)
 *
 *      @todo       Maybe add (back) SequenceIterator - with support for operator- (difference), and UpdateCurrent, and GetIndex()
 *                  Maybe also AdvanceBy(), BackBy() methods. Though All these COULD be methods of the underlying Sequence object.
 *
 *      @todo       Implement stuff like Contains () using ApplyUnti.... and lambdas, so locking works cheaply, and
 *                  so no virtual references to operator== - so can always create Sequence<T> even if no operator== defined
 *                  for T.
 *
 *      @todo       Assure well documetned that Stroika 1.0 mutators are replaced with modications directly on the container,
 *                  taking the iterator as argument!
 *
 *      @todo       Document and Consider that though iterator compares with CONTAINER.end () work fine with Stroika iterators,
 *                  other comparisons fail. For example, i < it.end (); and more importantly, constructs like i-s.begin() fail.
 *
 *                  Consider  if we can make this work, or document why and that we cannot. Maybe we need a concept of
 *                  SequenceIterator (like we had in Stroika 1) - which adds operator-?
 *
 *      @todo       Stroika had REVERSE_ITERATORS - and so does STL. At least for sequences, we need reverse iterators!
 *
 *      @todo       DOCUMENT - and use some library for OCNPETS (ElementsTraits.h).
 *
 *      @todo       Add Sequence_SparseArray<T> - using btree implementation
 *                  it requires there is an empty T CTOR. But then uses btree to store values when they differ from T()
 *                  (implement using redback tree or using stl map<>)
 *
 *      @todo       Add backend implementaiton of Sequence<T> using stl types - like
 *                  Sequence_stdvector, and Sequence_stdlist<>
 *
 */


#include    "../StroikaPreComp.h"

#include    <limits>

#include    "../Configuration/Common.h"
#include    "../Memory/SharedByValue.h"

#include    "Iterable.h"
#include    "Iterator.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


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
             *
             *
             */
            template    <typename T>
            class   Sequence : public Iterable<T> {
            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                /*
                 */
                Sequence ();
                Sequence (const Sequence<T>& s);
                explicit Sequence (const T* start, const T* end);

            protected:
                explicit Sequence (const _SharedPtrIRep& rep);

            public:
                /**
                 */
                nonvirtual  Sequence<T>& operator= (const Sequence<T>& src);

            public:
                /**
                 * Only supported if T::operator==()
                 */
                nonvirtual  bool Contains (T item) const;

            public:
                /**
                 * Only supported of T::Compare() or T::compare() defined
                 *      (CONSIDER NEW code to detect methods in templates)
                 *      (MAYBE always use compare() - not Compare)
                 */
                nonvirtual  int     Compare (const Iterable<T>& rhs) const;

            public:
                /**
                 * Only supported if T::operator==() defined.
                 */
                nonvirtual  bool    Equals (const Iterable<T>& rhs) const;

            public:
                /**
                 */
                nonvirtual  void    RemoveAll ();

            public:
                /**
                 */
                nonvirtual  T       GetAt (size_t i) const;

            public:
                /**
                 */
                nonvirtual  void    SetAt (size_t i, T item);

            public:
                /**
                 */
                nonvirtual  T       operator[] (size_t i) const;

            public:
                /**
                 *      Search the sequence and see if the given item is contained in
                 *  it, and return the index of that item. Comparison is done with
                 *  operator== (if its defined). - require concept T::operator==)
                 *  for first two overloads - third taking iterator always works)
                 */
                nonvirtual  size_t  IndexOf (T item) const;
                nonvirtual  size_t  IndexOf (const Sequence<T>& s) const;
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
                nonvirtual  void    Insert (size_t i, const Iterable<T>& items);

            public:
                /**
                 */
                nonvirtual  void    Prepend (T item);
                nonvirtual  void    Prepend (const Iterable<T>& items);

            public:
                /**
                 */
                nonvirtual  void    Append (T item);
                nonvirtual  void    Append (const Iterable<T>& items);


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
                 */
                /**
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
                 *  Convert Sequence<T> losslessly into a standard supproted C++ type.
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
                nonvirtual  const _IRep&    _GetRep () const;
                nonvirtual  _IRep&          _GetRep ();
            };


            /**
             *  \brief  Implementation detail for Sequence<T> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the Sequence<T> container API.
             */
            template    <typename T>
            class   Sequence<T>::_IRep : public Iterable<T>::_IRep {
            protected:
                _IRep ();

            public:
                virtual ~_IRep ();

            public:
                virtual T       GetAt (size_t i) const                              =   0;
                virtual void    SetAt (size_t i, const T& item)                     =   0;
                virtual size_t  IndexOf (const Iterator<T>& i) const                =   0;
                virtual void    Remove (const Iterator<T>& i)                       =   0;
                virtual void    Update (const Iterator<T>& i, T newValue)           =   0;
                virtual void    Insert (size_t at, const T* from, const T* to)      =   0;
                virtual void    Remove (size_t from, size_t to)                     =   0;
            };


        }
    }
}

#endif  /*_Stroika_Foundation_Containers_Sequence_h_ */



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include    "Sequence.inl"
