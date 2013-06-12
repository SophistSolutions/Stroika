/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_DataStructures_Array_h_
#define _Stroika_Foundation_Containers_Private_DataStructures_Array_h_

#include    "../../../StroikaPreComp.h"

#include    "../../../Configuration/Common.h"

#include    "../../Common.h"



/**
 *
 *  Description:
 *
 *      Array<T> is a backend implementation. It is not intended to be directly
 *  used by programmers, except in implementing concrete container reps.
 *
 *      Array<T> is a template which provides a dynamic array class. Elements
 *  of type T can be assigned, and accessed much like a normal array, except
 *  that when debug is on, accesses are range-checked.
 *
 *      Array<T> also provides a dynamic sizing capability. It reallocs its
 *  underlying storgage is such a ways as to keep a buffer of n(currently 5)%
 *  extra, so that reallocs on resizes only occur logn times on n appends.
 *  To save even this space, you can call Compact().
 *
 *      Unlike other dynamic array implementations, when an item is removed,
 *  it is destructed then. So the effects of buffering have no effects on the
 *  semantics of the Array.
 *
 *
 *
 *  Notes:
 *
 *  Warning:
 *      This implemenation prohibits use of internal pointers within T
 *  since we sometimes copy the contents of the array without excplicitly using
 *  the X(X&) and T::operator= functions.
 *
 *
 *  C++/StandardC arrays and segmented architectures:
 *
 *      Our iterators use address arithmatic since that is faster than
 *  array indexing, but that requires care in the presence of patching,
 *  and in iterating backwards.
 *
 *      The natural thing to do in iteration would be to have fCurrent
 *  point to the current item, but that would pose difficulties in the
 *  final test at the end of the iteration when iterating backwards. The
 *  final test would be fCurrent < fStart. This would be illegal in ANSI C.
 *
 *      The next possible trick is for backwards iteration always point one
 *  past the one you mean, and have it.Current () subtract one before
 *  dereferncing. This works pretty well, but makes source code sharing between
 *  the forwards and backwards cases difficult.
 *
 *      The next possible trick, and the one we use for now, is to have
 *  fCurrent point to the current item, and in the Next() code, when
 *  going backwards, reset fCurrent to fEnd - bizzare as this may seem
 *  then the test code can be shared among the forwards and backwards
 *  implemenations, all the patching code can be shared, with only this
 *  one minor check. Other potential choices are presented in the TODO
 *  below.
 *
 *
 *
 *  TODO:
 *
 *      @todo   Move Forward/Backward/Etc interators to be NESTED CLASSES!
 *
 *
 *      @todo   FIX realloc() stuff. We probably need to get rid of realloc altogether. Look at what
 *              std::vector<> does for hints about most efficient way..
 *
 *
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {
                namespace   DataStructures {


                    /*
                     *      This class is the main core of the implementation. It provides
                     *  an array abstraction, where the size can be set dynamically, and
                     *  extra sluff is maintained off the end to reduce copying from reallocs.
                     *  Only items 0..GetLength ()-1 are kept constructed. The rest (GetLength()+1
                     *  ..fSlotsAlloced) are uninitialized memory. This is important because
                     *  it means you can count on DTORs of your T being called when you
                     *  remove them from contains, not when the caches happen to empty.
                     */
                    template      <typename  T>
                    class   Array {
                    public:
                        Array ();
                        Array (const Array<T>& from);
                        ~Array ();

                    public:
                        nonvirtual  Array<T>& operator= (const Array<T>& rhs);

                    public:
                        nonvirtual  T       GetAt (size_t i) const;
                        nonvirtual  void    SetAt (T item, size_t i);
                        nonvirtual  T&      operator[] (size_t i);
                        nonvirtual  T       operator[] (size_t i) const;

                        nonvirtual  size_t  GetLength () const;
                        nonvirtual  void    SetLength (size_t newLength, T fillValue);

                        nonvirtual  void    InsertAt (T item, size_t index);
                        nonvirtual  void    RemoveAt (size_t index);
                        nonvirtual  void    RemoveAll ();

                        nonvirtual  bool    Contains (T item) const;

                        /*
                         * Memory savings/optimization methods.  Use this to tune useage
                         * of arrays so that they dont waste time in Realloc's.
                         */
                        nonvirtual  size_t  GetCapacity () const;
                        nonvirtual  void    SetCapacity (size_t slotsAlloced);

                        nonvirtual  void    Compact ();

                        nonvirtual  void    Invariant () const;

                        // probably SB protected
                    public:
                        class   ArrayIteratorBase;

                    protected:
                        size_t          _fLength;            // #items advertised/constructed
                        size_t          _fSlotsAllocated;    // #items allocated (though not necessarily initialized)
                        T*              _fItems;

#if     qDebug
                    protected:
                        virtual void    Invariant_ () const;
#endif
#if 0
                    private:
                        friend  class   ArrayIteratorBase<T>;
#endif
                    };


                    /*
                     *      ArrayIteratorBase<T> is an un-advertised implementation
                     *  detail designed to help in source-code sharing among various
                     *  iterator implementations.
                     */
                    template    <typename T>
                    class   Array<T>::ArrayIteratorBase {
                    private:
                        ArrayIteratorBase ();       // not defined - do not call.

                    public:
                        ArrayIteratorBase (const Array<T>& data);

                        nonvirtual  T       Current () const;           //  Error to call if Done (), otherwise OK
                        nonvirtual  size_t  CurrentIndex () const;      //  NB: This can be called if we are done - if so, it returns GetLength() + 1.
                        nonvirtual  bool    More (T* current, bool advance);
                        nonvirtual  bool    Done () const;

                        nonvirtual  void    Invariant () const;

                    protected:
#if     qDebug
                        const Array<T>*     fData;
#endif
                        const T*            fStart;         // points to FIRST elt
                        const T*            fEnd;           // points 1 PAST last elt
                        const T*            fCurrent;       // points to CURRENT elt (SUBCLASSES MUST INITIALIZE THIS!)
                        bool                fSuppressMore;  // Indicates if More should do anything, or if were already Mored...

#if     qDebug
                        virtual void    Invariant_ () const;
#endif
                    };


                    /*
                     *      Use this iterator to iterate forwards over the array. Be careful
                     *  not to add or remove things from the array while using this iterator,
                     *  since it is not safe. Use ForwardArrayIterator_Patch for those cases.
                     */
                    template    <typename T>
                    class   ForwardArrayIterator : public Array<T>::ArrayIteratorBase {
                    private:
                        typedef typename Array<T>::ArrayIteratorBase    inherited;

                    public:
                        ForwardArrayIterator (const Array<T>& data);

                        nonvirtual  bool    More (T* current, bool advance);
                        nonvirtual  bool    Done () const;          // shadow to avoid scope ambiguity

                        nonvirtual  void    Invariant () const;     // shadow to avoid scope ambiguity
                    };


                    /*
                     *      ForwardArrayMutator<T> is the same as ForwardArrayIterator<T> but
                     *  adds the ability to update the contents of the array as you go along.
                     */
                    template    <typename T> class  ForwardArrayMutator : public ForwardArrayIterator<T> {
                    public:
                        ForwardArrayMutator (Array<T>& data);

                        nonvirtual  bool    More (T* current, bool advance);                // shadow to avoid scope ambiguity
                        nonvirtual  bool    Done () const;          // shadow to avoid scope ambiguity

                        nonvirtual  void    Invariant () const;     // shadow to avoid scope ambiguity

                        nonvirtual  void    UpdateCurrent (T newValue);

                    private:
                        typedef ForwardArrayIterator<T>    inherited;
                    };


                    /*
                     *      Use this iterator to iterate backwards over the array. Be careful
                     *  not to add or remove things from the array while using this iterator,
                     *  since it is not safe. Use BackwardArrayIterator_Patch for those cases.
                     */
                    template    <typename T>
                    class   BackwardArrayIterator : public Array<T>::ArrayIteratorBase {
                    private:
                        typedef typename    Array<T>::ArrayIteratorBase    inherited;

                    public:
                        BackwardArrayIterator (const Array<T>& data);

                        nonvirtual  bool    More (T* current, bool advance);
                        nonvirtual  bool    Done () const;          // shadow to avoid scope ambiguity

                        nonvirtual  void    Invariant () const;     // shadow to avoid scope ambiguity

                    };


                    /*
                     *      BackwardArrayMutator<T> is the same as BackwardArrayIterator<T> but
                     *  adds the ability to update the contents of the array as you go along.
                     */
                    template    <typename T>
                    class  BackwardArrayMutator : public BackwardArrayIterator<T> {
                    public:
                        BackwardArrayMutator (Array<T>& data);

                        nonvirtual  bool    More (T* current, bool advance);      // shadow to avoid scope ambiguity
                        nonvirtual  bool    Done () const;          // shadow to avoid scope ambiguity

                        nonvirtual  void    Invariant () const;     // shadow to avoid scope ambiguity

                        nonvirtual  void    UpdateCurrent (T newValue);

                    private:
                        typedef BackwardArrayIterator<T>    inherited;
                    };


                }
            }
        }
    }
}


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Array.inl"

#endif  /*_Stroika_Foundation_Containers_Private_DataStructures_Array_h_ */
