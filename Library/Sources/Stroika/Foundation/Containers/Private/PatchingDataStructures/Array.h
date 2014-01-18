/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_PatchingDataStructures_Array_h_
#define _Stroika_Foundation_Containers_Private_PatchingDataStructures_Array_h_

#include    "../../../StroikaPreComp.h"

#include    "../DataStructures/Array.h"



/**
 *
 *  TODO
 *
 *  (GOOD TODO NEXT)
 *      @todo   Redo Array_Patch<T,TRAITS>::ForwardIterator - so ut uses Array<T,TRAITS>::ForwardIterator- where
 *              Array_Patch<T,TRAITS>::opatchiteraorbase - is a mixin class! Instead of
 *              DataStructures::Array<T,TRAITS>::_ArrayIteratorBase subtype... MAYBE - THINK THROUGH.
 *              PERHAPS EXPERIMETNT...
 *
 *              KEY IS LESS COPY-PASE OF IMPL FROM BASE
 *
 *
 *
 *
 *
 *
 *      @todo   Update this patching code to use the same new UpdateAt/RemoveAt paradigm used in base class Array
 *              And update code that uses these iterators - patching arrays - to replace that for RemoveCurrnet()
 *
 *      @todo   Possibly replace Array_Patch with just Array<T,TRAITS> (in this new namespcae)
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {
                namespace   PatchingDataStructures {


                    /**
                     *      Array_Patch<T,TRAITS> is an array implemantion that keeps a list of patchable
                     *  iterators, and handles the patching automatically for you. Use this if
                     *  you ever plan to use patchable iterators.
                     */
                    template      <typename  T, typename TRAITS = DataStructures::Array_DefaultTraits<T>>
                    class   Array_Patch : public DataStructures::Array<T, TRAITS> {
                    private:
                        using   inherited   =   typename DataStructures::Array<T, TRAITS>;

                    public:
                        Array_Patch ();
                        Array_Patch (const Array_Patch<T, TRAITS>& from);
                        ~Array_Patch ();

                        nonvirtual  Array_Patch<T, TRAITS>& operator= (const Array_Patch<T, TRAITS>& rhs);

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

                    protected:
                        class   _ArrayIteratorBase;

                    public:
                        class  ForwardIterator;
                        class  BackwardIterator;

                        /*
                         * Methods we shadow so that patching is done. If you want to circumvent the
                         * patching, thats fine - use scope resolution operator to call this's base
                         * class version.
                         */
                    public:
                        nonvirtual  void    SetLength (size_t newLength, T fillValue);
                        nonvirtual  void    InsertAt (size_t index, T item);
                        nonvirtual  void    RemoveAt (size_t index);
                        nonvirtual  void    RemoveAll ();
                        nonvirtual  void    RemoveAt (const ForwardIterator& i);
                        nonvirtual  void    RemoveAt (const BackwardIterator& i);
                        nonvirtual  void    SetAt (size_t i, T item);
                        nonvirtual  void    SetAt (const ForwardIterator& i, T newValue);
                        nonvirtual  void    SetAt (const BackwardIterator& i, T newValue);
                        nonvirtual  void    AddBefore (const ForwardIterator& i, T item);
                        nonvirtual  void    AddBefore (const BackwardIterator& i, T item);
                        nonvirtual  void    AddAfter (const ForwardIterator& i, T item);
                        nonvirtual  void    AddAfter (const BackwardIterator& i, T item);
                        nonvirtual  void    SetCapacity (size_t slotsAlloced);
                        nonvirtual  void    Compact ();

                        /*
                         *  Check Invariants for this class, and all the iterators we own.
                         */
                    public:
                        nonvirtual  void    Invariant () const;
#if     qDebug
                        using   OwnerID     =   const void* ;
                        nonvirtual void    AssertNoIteratorsReferenceOwner (OwnerID oBeingDeleted);
#endif

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
                    template      <typename  T, typename TRAITS>
                    class   Array_Patch<T, TRAITS>::_ArrayIteratorBase : public DataStructures::Array<T, TRAITS>::_ArrayIteratorBase {
                    private:
                        using   inherited   =   typename DataStructures::Array<T, TRAITS>::_ArrayIteratorBase;

                    public:
                        _ArrayIteratorBase (const Array_Patch<T, TRAITS>* data);
                        _ArrayIteratorBase (const _ArrayIteratorBase& from);
                        ~_ArrayIteratorBase ();

                    public:
                        nonvirtual  _ArrayIteratorBase& operator= (const _ArrayIteratorBase& rhs);

                    public:
                        nonvirtual  size_t  CurrentIndex () const;  // shadow to avoid scope ambiguity

                        nonvirtual  void    Invariant () const;     // shadow to avoid scope ambiguity

                    public:
                        using   OwnerID     =   const void* ;
                        OwnerID fOwnerID;
                        OwnerID GetOwner () const { return fOwnerID; }

                    public:
                        nonvirtual  void    PatchAdd (size_t index);        //  call after add
                        nonvirtual  void    PatchRemove (size_t index);     //  call before remove
                        nonvirtual  void    PatchRemoveAll ();              //  call after removeall
                        nonvirtual  void    PatchRealloc ();                //  call after realloc could have happened

                    protected:
                        const Array_Patch<T, TRAITS>*   fData;
                        _ArrayIteratorBase*             fNext;

#if     qDebug
                        virtual void    Invariant_ () const override;
#endif

                        virtual     void    PatchRemoveCurrent ()   =   0;  // called from patchremove if patching current item...

                        friend  class   Array_Patch<T, TRAITS>;
                    };


                    /*
                     *      Array_Patch<T,TRAITS>::ForwardIterator is forwards iterator that can be used
                     *  while modifing its owned array. It can only be used with Array_Patch<T,TRAITS>
                     *  since the classes know about each other, and keep track of each other.
                     *  This is intended to be a convienience in implementing concrete container
                     *  mixins.
                     */
                    template      <typename  T, typename TRAITS>
                    class   Array_Patch<T, TRAITS>::ForwardIterator : public Array_Patch<T, TRAITS>::_ArrayIteratorBase {
                    private:
                        using   inherited   =   typename Array_Patch<T, TRAITS>::_ArrayIteratorBase;

                    public:
                        ForwardIterator (const Array_Patch<T, TRAITS>* data);

                    public:
                        nonvirtual  bool    More (T* current, bool advance);
                        nonvirtual  void    More (Memory::Optional<T>* result, bool advance);
                        nonvirtual  bool    More (nullptr_t, bool advance)
                        {
                            return More (static_cast<T*> (nullptr), advance);
                        }

                    protected:
                        virtual void    PatchRemoveCurrent () override;
                    };


                    /*
                     *      Array_Patch<T,TRAITS>::BackwardIterator is backwards iterator that can be used
                     *  while modifing its owned array. It can only be used with Array_Patch<T,TRAITS>
                     *  since the classes know about each other, and keep track of each other.
                     *  This is intended to be a convienience in implementing concrete container
                     *  mixins.
                     */
                    template      <typename  T, typename TRAITS>
                    class  Array_Patch<T, TRAITS>::BackwardIterator : public Array_Patch<T, TRAITS>::_ArrayIteratorBase {
                    private:
                        using   inherited   =   typename Array_Patch<T, TRAITS>::_ArrayIteratorBase;

                    public:
                        BackwardIterator (const Array_Patch<T, TRAITS>& data);

                    public:
                        nonvirtual  bool    More (T* current, bool advance);
                        nonvirtual  void    More (Memory::Optional<T>* result, bool advance);
                        nonvirtual  bool    More (nullptr_t, bool advance)
                        {
                            return More (static_cast<T*> (nullptr), advance);
                        }

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
