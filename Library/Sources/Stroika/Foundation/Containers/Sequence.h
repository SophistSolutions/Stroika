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
 *      (o)         Should inherit from Iterable<T>
 *
 *      @todo       Must support SequenceIterator - and that SequenceIterator must work with qsort().
 *                  In otehrwords, must act as random-access iterator so it can be used in algorithjms that use STL
 *                  random-access iterators.
 *
 *
 */


#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Memory/SharedByValue.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            const   size_t  kBadSequenceIndex   =   kMaxSize_T;

            // I think these are a crock, and need to be looked at more closely. There are used in unsafe,
            // pun fasion - investigate VERY soon - LGP May 23, 1992.
            enum AddMode {
                ePrepend = 1,
                eAppend = -1,
            };


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
            template    <class T>
            class   Sequence : public Iterable<T> {
            public:
                Sequence ();
                Sequence (const Sequence<T>& src);
                Sequence (const T* items, size_t size);

            protected:
                Sequence (SequenceRep<T>* rep);

            public:
                nonvirtual  Sequence<T>& operator= (const Sequence<T>& src);

            public:
                nonvirtual  Boolean Contains (T item) const;

            public:
                nonvirtual  void    RemoveAll ();

            public:
                nonvirtual  void    Compact ();

            public:
                nonvirtual  T       GetAt (size_t index) const;

            public:
                nonvirtual  void    SetAt (T item, size_t index);

            public:
                nonvirtual  T       operator[] (size_t index) const;

            public:
                /*
                 *      Search the sequence and see if the given item is contained in
                 *  it, and return the index of that item. Comparison is done with
                 *  operator==.
                 */
                nonvirtual  size_t  IndexOf (T item) const;
                nonvirtual  size_t  IndexOf (const Sequence<T>& s) const;

            public:
                /*
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
                nonvirtual  void    InsertAt (T item, size_t index);
                nonvirtual  void    InsertAt (const Sequence<T>& items, size_t index);

            public:
                nonvirtual  void    Prepend (T item);
                nonvirtual  void    Prepend (const Sequence<T>& items);

            public:
                nonvirtual  void    Append (T item);
                nonvirtual  void    Append (const Sequence<T>& items);

            public:
                nonvirtual  Sequence<T>&    operator+= (T item);
                nonvirtual  Sequence<T>&    operator+= (const Sequence<T>& items);

            public:
                /*
                 *      Remove the item at the given position of the sequence. Make sure
                 *  that iteration is not disturbed by this removal. In particular, any
                 *  items (other than the one at index) that would have been seen, will
                 *  still be, and no new items will be seen that wouldn't have been.
                 */
                nonvirtual  void    RemoveAt (size_t index);
                nonvirtual  void    RemoveAt (size_t index, size_t amountToRemove);

            public:
                /*
                 * Not an error to remove an item that is not an element of the list, instead has no effect.
                 */
                nonvirtual  void    Remove (T item);
                nonvirtual  void    Remove (const Sequence<T>& items);

            public:
                nonvirtual  Sequence<T>&    operator-= (T item);
                nonvirtual  Sequence<T>&    operator-= (const Sequence<T>& items);

                nonvirtual  operator Iterator<T> () const;
                nonvirtual  SequenceIteratorRep<T>*     MakeSequenceIterator (SequenceDirection d) const;
                nonvirtual  SequenceMutatorRep<T>*      MakeSequenceMutator (SequenceDirection d);
                nonvirtual  operator SequenceIterator<T> () const;
                nonvirtual  operator SequenceMutator<T> ();

            protected:
                nonvirtual  void    AddItems (const T* items, size_t size);

                nonvirtual  const SequenceRep<T>*   GetRep () const;
                nonvirtual  SequenceRep<T>*         GetRep ();

            private:
                Shared<SequenceRep<T> > fRep;

                static  SequenceRep<T>* Clone (const SequenceRep<T>& rep);

                friend  Boolean operator== (const Sequence<T>& lhs, const Sequence<T>& rhs);
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
