/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_PatchingDataStructures_Array_h_
#define _Stroika_Foundation_Containers_Private_PatchingDataStructures_Array_h_

#include    "../../../StroikaPreComp.h"

#include    "../DataStructures/Array.h"



/**
 *
 *  TODO
 *
 *      @todo   Update this patching code to use the same new UpdateAt/RemoveAt paradigm used in base class Array
 *              And update code that uses these iterators - patching arrays - to replace that for RemoveCurrnet()
 *
 *      @todo   Possibly replace Array_Patch with just Array<T> (in this new namespcae)
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {
                namespace   PatchingDataStructures {


                    /**
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
                         * Methods to do the patching yourself. Iterate over all the iterators and
                         * perform patching.
                         */
                    public:
                        nonvirtual  bool    HasActiveIterators () const;            //  are there any iterators to be patched?
                        nonvirtual  void    PatchViewsAdd (size_t index) const;     //  call after add
                        nonvirtual  void    PatchViewsRemove (size_t index) const;  //  call before remove
                        nonvirtual  void    PatchViewsRemoveAll () const;           //  call after removeall
                        nonvirtual  void    PatchViewsRealloc () const;             //  call after realloc could have happened

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

                    protected:
                        class   _ArrayIteratorBase;

                    public:
                        class  ForwardIterator;
                        class  BackwardIterator;

                        // TO BE LOST/REPLACED
                    public:
                        class  ForwardArrayMutator_Patch;
                        class  BackwardArrayMutator_Patch;

                        /*
                         *  Check Invariants for this class, and all the iterators we own.
                         */
                    public:
                        nonvirtual  void    Invariant () const;

                    private:
                        _ArrayIteratorBase*     fIterators_;

#if     qDebug
                        virtual void    Invariant_ () const override;
                        nonvirtual  void    InvariantOnIterators_ () const;
#endif
                    };


                    /*
                     *      _ArrayIteratorBase<T> is a private utility class designed
                     *  to promote source code sharing among the patched iterator implementations.
                     */
                    template    <typename T>
                    class   Array_Patch<T>::_ArrayIteratorBase : public DataStructures::Array<T>::_ArrayIteratorBase {
                    private:
                        typedef typename DataStructures::Array<T>::_ArrayIteratorBase    inherited;

                    public:
                        _ArrayIteratorBase (const Array_Patch<T>& data);
                        _ArrayIteratorBase (const _ArrayIteratorBase& from);
                        ~_ArrayIteratorBase ();

                    public:
                        nonvirtual  _ArrayIteratorBase& operator= (const _ArrayIteratorBase& rhs);

                    public:
                        nonvirtual  size_t  CurrentIndex () const;  // shadow to avoid scope ambiguity

                        nonvirtual  void    Invariant () const;     // shadow to avoid scope ambiguity

                    public:
                        nonvirtual  void    PatchAdd (size_t index);        //  call after add
                        nonvirtual  void    PatchRemove (size_t index);     //  call before remove
                        nonvirtual  void    PatchRemoveAll ();              //  call after removeall
                        nonvirtual  void    PatchRealloc ();                //  call after realloc could have happened

                    protected:
                        const Array_Patch<T>*       fData;
                        _ArrayIteratorBase*         fNext;

#if     qDebug
                        virtual void    Invariant_ () const override;
#endif

                        virtual     void    PatchRemoveCurrent ()   =   0;  // called from patchremove if patching current item...

                        friend  class   Array_Patch<T>;
                    };


                    /*
                     *      Array_Patch<T>::ForwardIterator is forwards iterator that can be used
                     *  while modifing its owned array. It can only be used with Array_Patch<T>
                     *  since the classes know about each other, and keep track of each other.
                     *  This is intended to be a convienience in implementing concrete container
                     *  mixins.
                     */
                    template    <typename T>
                    class   Array_Patch<T>::ForwardIterator : public Array_Patch<T>::_ArrayIteratorBase {
                    private:
                        typedef typename Array_Patch<T>::_ArrayIteratorBase    inherited;

                    public:
                        ForwardIterator (const Array_Patch<T>& data);

                    public:
                        nonvirtual  bool    More (T* current, bool advance);

                    protected:
                        virtual void    PatchRemoveCurrent () override;
                    };


                    /*
                     *      Array_Patch<T>::BackwardIterator is backwards iterator that can be used
                     *  while modifing its owned array. It can only be used with Array_Patch<T>
                     *  since the classes know about each other, and keep track of each other.
                     *  This is intended to be a convienience in implementing concrete container
                     *  mixins.
                     */
                    template    <typename T>
                    class  Array_Patch<T>::BackwardIterator : public Array_Patch<T>::_ArrayIteratorBase {
                    private:
                        typedef typename Array_Patch<T>::_ArrayIteratorBase    inherited;

                    public:
                        BackwardIterator (const Array_Patch<T>& data);

                    public:
                        nonvirtual  bool    More (T* current, bool advance);

                    protected:
                        virtual void    PatchRemoveCurrent () override;
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
