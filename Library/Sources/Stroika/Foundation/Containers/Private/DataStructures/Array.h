/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_DataStructures_Array_h_
#define _Stroika_Foundation_Containers_Private_DataStructures_Array_h_

#include    "../../../StroikaPreComp.h"

#include    "../../../Configuration/Common.h"
#include    "../../../Common/Compare.h"
#include    "../../../Memory/Optional.h"

#include    "../../Common.h"



/**
 *
 *  Description:
 *
 *      Array<T,TRAITS> is a backend implementation. It is not intended to be directly
 *  used by programmers, except in implementing concrete container reps.
 *
 *      Array<T,TRAITS> is a template which provides a dynamic array class. Elements
 *  of type T can be assigned, and accessed much like a normal array, except
 *  that when debug is on, accesses are range-checked.
 *
 *      Array<T,TRAITS> also provides a dynamic sizing capability. It reallocs its
 *  underlying storgage is such a ways as to keep a buffer of n(currently 5)%
 *  extra, so that reallocs on resizes only occur logn times on n appends.
 *  To save even this space, you can call Compact().
 *
 *      Unlike other dynamic array implementations, when an item is removed,
 *  it is destructed then. So the effects of buffering have no effects on the
 *  semantics of the Array.
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
 *  final test would be fCurrent < _fStart. This would be illegal in ANSI C.
 *
 *      The next possible trick is for backwards iteration always point one
 *  past the one you mean, and have it.Current () subtract one before
 *  dereferncing. This works pretty well, but makes source code sharing between
 *  the forwards and backwards cases difficult.
 *
 *      The next possible trick, and the one we use for now, is to have
 *  fCurrent point to the current item, and in the Next() code, when
 *  going backwards, reset fCurrent to _fEnd - bizzare as this may seem
 *  then the test code can be shared among the forwards and backwards
 *  implemenations, all the patching code can be shared, with only this
 *  one minor check. Other potential choices are presented in the TODO
 *  below.
 *
 *
 *
 *  TODO:
 *
 *      @todo   Replace Contains() with Lookup () - as we did for LinkedList<T>
 *
 *      @todo   For each class (Array/Array_Patchikng, and all iteraotrs, add explicit
 *              declares about COPY CTORs etc. NOT safe (generally) to use default (esp for patching iterators)
 *
 *      @todo   Alot of implemenation uses 'last' paradim. Switch to modern C++ / STL begin/end style.
 *
 *      @todo   ADD DATA MEMBERS TO ARRAYITERATORBASE WHCI ALLOW MANIP OF NEEDED STUFF IN ARRAY
 *              SO NOT COPIED!!! - like fLength, fPtr etc. THEN - we can do stuff safely
 *              WRT type safety / protected!!!
 *
 *              RETHINK. Above may not be a problem. Real issue is array stores base and length
 *              and iterators use start/end. Maybe THATs what we really need to change in the
 *              array class (probably).
 *
 *      @todo   FIX realloc() stuff. We probably need to get rid of realloc altogether. Look at what
 *              std::vector<> does for hints about most efficient way..
 *
 *      @todo   Add RVALUE-REF (MOVE) stuff.
 *
 *      @todo   (close to rvalue thing above) - fix construction/destruction stuff. More mdoern C++.
 *              That crap was written around 1990!!!
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {
                namespace   DataStructures {


                    /**
                     * VERY PRELIMINARY DRAFT OF HOW TO HANDLE THIS - UNSURE ABOUT ISSUE OF FORWARDABILITY AND COPYABILIUTY OF COMPARERES!!!!
                     */
                    template    <typename T, typename EQUALS_COMPARER = Common::ComparerWithEquals <T>>
                    struct   Array_DefaultTraits {
                        typedef EQUALS_COMPARER EqualsCompareFunctionType;
                    };


                    /**
                     *      This class is the main core of the implementation. It provides
                     *  an array abstraction, where the size can be set dynamically, and
                     *  extra sluff is maintained off the end to reduce copying from reallocs.
                     *  Only items 0..GetLength ()-1 are kept constructed. The rest (GetLength()+1
                     *  ..fSlotsAlloced) are uninitialized memory. This is important because
                     *  it means you can count on DTORs of your T being called when you
                     *  remove them from contains, not when the caches happen to empty.
                     */
                    template      <typename  T, typename TRAITS = Array_DefaultTraits<T>>
                    class   Array {
                    public:
                        using   value_type  =   T;

                    public:
                        Array ();
                        Array (const Array<T, TRAITS>& from);

                    public:
                        ~Array ();

                    public:
                        nonvirtual  Array<T, TRAITS>& operator= (const Array<T, TRAITS>& rhs);

                    public:
                        nonvirtual  T       GetAt (size_t i) const;
                        nonvirtual  void    SetAt (size_t i, T item);
                        nonvirtual  T&      operator[] (size_t i);
                        nonvirtual  T       operator[] (size_t i) const;

                        nonvirtual  size_t  GetLength () const;
                        nonvirtual  void    SetLength (size_t newLength, T fillValue);

                        nonvirtual  void    InsertAt (size_t index, T item);
                        nonvirtual  void    RemoveAt (size_t index);
                        nonvirtual  void    RemoveAll ();

                        nonvirtual  bool    Contains (T item) const;

                    public:
                        /*
                         * Memory savings/optimization methods.  Use this to tune useage
                         * of arrays so that they dont waste time in Realloc's.
                         */
                        nonvirtual  size_t  GetCapacity () const;
                        nonvirtual  void    SetCapacity (size_t slotsAlloced);

                        nonvirtual  void    Compact ();

                    protected:
                        class   _ArrayIteratorBase;

                    public:
                        class   ForwardIterator;
                        class   BackwardIterator;

                    public:
                        nonvirtual  void    RemoveAt (const ForwardIterator& i, T newValue);
                        nonvirtual  void    RemoveAt (const BackwardIterator& i, T newValue);

                    public:
                        nonvirtual  void    SetAt (const ForwardIterator& i, T newValue);
                        nonvirtual  void    SetAt (const BackwardIterator& i, T newValue);

                    public:
                        //  NB: Can be called if done
                        nonvirtual  void    AddBefore (const ForwardIterator& i, T item);
                        nonvirtual  void    AddBefore (const BackwardIterator& i, T item);

                    public:
                        nonvirtual  void    AddAfter (const ForwardIterator& i, T item);
                        nonvirtual  void    AddAfter (const BackwardIterator& i, T item);

                    public:
                        nonvirtual  void    Invariant () const;

#if     qDebug
                    protected:
                        virtual void    Invariant_ () const;
#endif

                    protected:
                        size_t          _fLength;            // #items advertised/constructed
                        size_t          _fSlotsAllocated;    // #items allocated (though not necessarily initialized)
                        T*              _fItems;
                    };


                    /**
                     *      _ArrayIteratorBase<T> is an un-advertised implementation
                     *  detail designed to help in source-code sharing among various
                     *  iterator implementations.
                     */
                    template      <typename  T, typename TRAITS>
                    class   Array<T, TRAITS>::_ArrayIteratorBase {
                    private:
                        _ArrayIteratorBase ();       // not defined - do not call.

                    public:
                        _ArrayIteratorBase (const Array<T, TRAITS>* data);

                        nonvirtual  T       Current () const;           //  Error to call if Done (), otherwise OK
                        nonvirtual  size_t  CurrentIndex () const;      //  NB: This can be called if we are done - if so, it returns GetLength() + 1.
                        nonvirtual  bool    More (T* current, bool advance);
                        nonvirtual  bool    Done () const;

                    public:
                        nonvirtual  bool    Equals (const typename Array<T, TRAITS>::_ArrayIteratorBase& rhs) const
                        {
                            return _fCurrent == rhs._fCurrent and _fSuppressMore == rhs._fSuppressMore;
                        }

                    public:
                        nonvirtual  void    Invariant () const;

#if     qDebug
                    protected:
                        const Array<T, TRAITS>*     _fData;
#endif
                    protected:
                        const T*            _fStart;         // points to FIRST elt
                        const T*            _fEnd;           // points 1 PAST last elt
                        const T*            _fCurrent;       // points to CURRENT elt (SUBCLASSES MUST INITIALIZE THIS!)
                        bool                _fSuppressMore;  // Indicates if More should do anything, or if were already Mored...

#if     qDebug
                    protected:
                        virtual void    Invariant_ () const;
#endif
                    };


                    /**
                     *      Use this iterator to iterate forwards over the array. Be careful
                     *  not to add or remove things from the array while using this iterator,
                     *  since it is not safe. Use ForwardIterator_Patch for those cases.
                     */
                    template      <typename  T, typename TRAITS>
                    class   Array<T, TRAITS>::ForwardIterator : public Array<T, TRAITS>::_ArrayIteratorBase {
                    private:
                        using   inherited   =   typename Array<T, TRAITS>::_ArrayIteratorBase;

                    public:
                        ForwardIterator (const Array<T, TRAITS>* data);

                    public:
                        nonvirtual  bool    More (T* current, bool advance);
                        nonvirtual  void    More (Memory::Optional<T>* result, bool advance);
                        nonvirtual  bool    More (nullptr_t, bool advance)
                        {
                            return More (static_cast<T*> (nullptr), advance);
                        }
                    };


                    /**
                     *      Use this iterator to iterate backwards over the array. Be careful
                     *  not to add or remove things from the array while using this iterator,
                     *  since it is not safe. Use BackwardIterator_Patch for those cases.
                     */
                    template      <typename  T, typename TRAITS>
                    class   Array<T, TRAITS>::BackwardIterator : public Array<T, TRAITS>::_ArrayIteratorBase {
                    private:
                        using   inherited   =   typename    Array<T, TRAITS>::_ArrayIteratorBase;

                    public:
                        BackwardIterator (const Array<T, TRAITS>* data);

                    public:
                        nonvirtual  bool    More (T* current, bool advance);
                        nonvirtual  void    More (Memory::Optional<T>* result, bool advance);
                        nonvirtual  bool    More (nullptr_t, bool advance)
                        {
                            return More (static_cast<T*> (nullptr), advance);
                        }
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
