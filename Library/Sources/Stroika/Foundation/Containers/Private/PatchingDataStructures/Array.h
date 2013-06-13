/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_PatchingDataStructures_Array_h_
#define _Stroika_Foundation_Containers_Private_PatchingDataStructures_Array_h_

#include    "../../../StroikaPreComp.h"

#include    "../DataStructures/Array.h"



/**
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {
                namespace   PatchingDataStructures {


                    /*
                     *      Array_Patch<T> is an array implemantion that keeps a list of patchable
                     *  iterators, and handles the patching automatically for you. Use this if
                     *  you ever plan to use patchable iterators.
                     */
                    template    <typename T>
                    class   Array_Patch : public DataStructures::Array<T> {
                    private:
                        typedef typename DataStructures::Array<T> inherited;
                    public:
                        Array_Patch ();
                        Array_Patch (const Array_Patch<T>& from);
                        ~Array_Patch ();

                        nonvirtual  Array_Patch<T>& operator= (const Array_Patch<T>& rhs);

                        /*
                         * Methods we shadow so that patching is done. If you want to circumvent the
                         * patching, thats fine - use scope resolution operator to call this's base
                         * class version.
                         */
                    public:
                        nonvirtual  void    SetLength (size_t newLength, T fillValue);
                        nonvirtual  void    InsertAt (T item, size_t index);
                        nonvirtual  void    RemoveAt (size_t index);
                        nonvirtual  void    RemoveAll ();
                        nonvirtual  void    SetCapacity (size_t slotsAlloced);
                        nonvirtual  void    Compact ();

                        /*
                         * Methods to do the patching yourself. Iterate over all the iterators and
                         * perform patching.
                         */
                    public:
                        nonvirtual  bool    HasActiveIterators () const;            //  are there any iterators to be patched?
                        nonvirtual  void    PatchViewsAdd (size_t index) const;     //  call after add
                        nonvirtual  void    PatchViewsRemove (size_t index) const;  //  call before remove
                        nonvirtual  void    PatchViewsRemoveAll () const;           //  call after removeall
                        nonvirtual  void    PatchViewsRealloc () const;             //  call after realloc could have happened


                        // THIS SB PROTECTED
                    public:
                        class   ArrayIterator_PatchBase;

                        /*
                         *  Check Invariants for this class, and all the iterators we own.
                         */
                    public:
                        nonvirtual  void    Invariant () const;

                    private:
                        ArrayIterator_PatchBase* fIterators;

#if 0
                        friend  class   ArrayIterator_PatchBase<T>;
#endif

#if     qDebug
                        virtual void    Invariant_ () const override;
                        nonvirtual  void    InvariantOnIterators_ () const;
#endif
                    };


                    /*
                     *      ArrayIterator_PatchBase<T> is a private utility class designed
                     *  to promote source code sharing among the patched iterator implementations.
                     */
                    template    <typename T>
                    class   Array_Patch<T>::ArrayIterator_PatchBase : public DataStructures::Array<T>::_ArrayIteratorBase {
                    private:
                        typedef typename DataStructures::Array<T>::_ArrayIteratorBase    inherited;

                    public:
                        ArrayIterator_PatchBase (const Array_Patch<T>& data);
                        ArrayIterator_PatchBase (const ArrayIterator_PatchBase& from);
                        ~ArrayIterator_PatchBase ();

                        nonvirtual  ArrayIterator_PatchBase& operator= (const ArrayIterator_PatchBase& rhs);

                        nonvirtual  size_t  CurrentIndex () const;  // shadow to avoid scope ambiguity

                        nonvirtual  void    Invariant () const;     // shadow to avoid scope ambiguity

                    public:
                        nonvirtual  void    PatchAdd (size_t index);        //  call after add
                        nonvirtual  void    PatchRemove (size_t index);     //  call before remove
                        nonvirtual  void    PatchRemoveAll ();              //  call after removeall
                        nonvirtual  void    PatchRealloc ();                //  call after realloc could have happened

                    protected:
                        const Array_Patch<T>*       fData;
                        ArrayIterator_PatchBase* fNext;

#if     qDebug
                        virtual void    Invariant_ () const override;
#endif

                        virtual     void    PatchRemoveCurrent ()   =   0;  // called from patchremove if patching current item...

                        friend  class   Array_Patch<T>;
                    };


                    /*
                     *      ForwardArrayIterator_Patch<T> is forwards iterator that can be used
                     *  while modifing its owned array. It can only be used with Array_Patch<T>
                     *  since the classes know about each other, and keep track of each other.
                     *  This is intended to be a convienience in implementing concrete container
                     *  mixins.
                     */
                    template    <typename T>
                    class  ForwardArrayIterator_Patch : public Array_Patch<T>::ArrayIterator_PatchBase {
                    private:
                        typedef typename Array_Patch<T>::ArrayIterator_PatchBase    inherited;

                    public:
                        ForwardArrayIterator_Patch (const Array_Patch<T>& data);

                    public:
                        nonvirtual  bool    More (T* current, bool advance);

                    protected:
                        virtual void    PatchRemoveCurrent () override;
                    };


                    /*
                     *      ForwardArrayMutator_Patch<T> is the same as ForwardArrayIterator_Patch<T> but
                     *  adds the ability to update the contents of the array as you go along.
                     */
                    template    <typename T>
                    class  ForwardArrayMutator_Patch : public ForwardArrayIterator_Patch<T> {
                    private:
                        typedef ForwardArrayIterator_Patch<T>    inherited;

                    public:
                        ForwardArrayMutator_Patch (Array_Patch<T>& data);

                    public:
                        nonvirtual  void    RemoveCurrent ();
                        nonvirtual  void    UpdateCurrent (T newValue);
                        nonvirtual  void    AddBefore (T item);             //  NB: Can be called if done
                        nonvirtual  void    AddAfter (T item);
                    };


                    /*
                     *      BackwardArrayIterator_Patch<T> is backwards iterator that can be used
                     *  while modifing its owned array. It can only be used with Array_Patch<T>
                     *  since the classes know about each other, and keep track of each other.
                     *  This is intended to be a convienience in implementing concrete container
                     *  mixins.
                     */
                    template    <typename T>
                    class  BackwardArrayIterator_Patch : public Array_Patch<T>::ArrayIterator_PatchBase {
                    private:
                        typedef typename Array_Patch<T>::ArrayIterator_PatchBase    inherited;

                    public:
                        BackwardArrayIterator_Patch (const Array_Patch<T>& data);

                    public:
                        nonvirtual  bool    More (T* current, bool advance);

                    protected:
                        virtual void    PatchRemoveCurrent () override;
                    };


                    /*
                     *      BackwardArrayMutator_Patch<T> is the same as BackwardArrayIterator_Patch<T> but
                     *  adds the ability to update the contents of the array as you go along.
                     */
                    template    <typename T>
                    class  BackwardArrayMutator_Patch : public BackwardArrayIterator_Patch<T> {
                    private:
                        typedef BackwardArrayIterator_Patch<T>    inherited;

                    public:
                        BackwardArrayMutator_Patch (Array_Patch<T>& data);

                    public:
                        nonvirtual  void    RemoveCurrent ();
                        nonvirtual  void    UpdateCurrent (T newValue);
                        nonvirtual  void    AddBefore (T item);
                        nonvirtual  void    AddAfter (T item);              //  NB: Can be called if done
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

#endif  /*_Stroika_Foundation_Containers_Private_PatchingDataStructures_Array_h_ */
