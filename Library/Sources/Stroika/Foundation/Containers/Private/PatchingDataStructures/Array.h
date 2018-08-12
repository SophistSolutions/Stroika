/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_PatchingDataStructures_Array_h_
#define _Stroika_Foundation_Containers_Private_PatchingDataStructures_Array_h_

#include "../../../StroikaPreComp.h"

#include "../../DataStructures/Array.h"

#include "PatchableContainerHelper.h"

/**
 *
 *  TODO
 *
 *  (GOOD TODO NEXT)
 *      @todo   Redo Array<T,TRAITS>::ForwardIterator - so ut uses Array<T,TRAITS>::ForwardIterator- where
 *              Array<T,TRAITS>::opatchiteraorbase - is a mixin class! Instead of
 *              DataStructures::Array<T,TRAITS>::_ArrayIteratorBase subtype... MAYBE - THINK THROUGH.
 *              PERHAPS EXPERIMETNT...
 *
 *              KEY IS LESS COPY-PASE OF IMPL FROM BASE
 *
 *      @todo   Update this patching code to use the same new UpdateAt/RemoveAt paradigm used in base class Array
 *              And update code that uses these iterators - patching arrays - to replace that for RemoveCurrnet()
 */

namespace Stroika::Foundation::Containers::Private::PatchingDataStructures {

    /*
     *  Patching Support:
     *
     *      This class wraps a basic container (in this case DataStructures::Array)
     *  and adds in patching support (tracking a list of iterators - and managing thier
     *  patching when appropriately wrapped changes are made to the data structure container.
     *
     *      This code leverages PatchableContainerHelper<> to do alot of the book-keeping.
     *
     *      Note: Disallow X(const X&), and operator=() (copy constructor/assignement operator), and
     *  instead require use of X(X*,IteratorOwnerID) for copying - so we always get both values -
     *  the source to copy from and the newOwnerID to copy INTO.
     */
    template <typename T>
    class Array : public PatchableContainerHelper<DataStructures::Array<T>> {
    private:
        using inherited = PatchableContainerHelper<DataStructures::Array<T>>;

    public:
        Array ();
        Array (Array<T>* rhs, IteratorOwnerID newOwnerID);
        Array (const Array<T>& from) = delete;

    public:
        ~Array ();

    public:
        nonvirtual Array<T>& operator= (const Array<T>& rhs) = delete;

        /*
         * Methods to do the patching yourself. Iterate over all the iterators and
         * perform patching.
         */
    public:
        nonvirtual void PatchViewsAdd (size_t index) const;    //  call after add
        nonvirtual void PatchViewsRemove (size_t index) const; //  call before remove
        nonvirtual void PatchViewsRemoveAll () const;          //  call after removeall
        nonvirtual void PatchViewsRealloc () const;            //  call after realloc could have happened

    protected:
        class _ArrayIteratorBase;

    public:
        class ForwardIterator;
        class BackwardIterator;

    public:
        using UnpatchedForwardIterator  = typename inherited::ForwardIterator;
        using UnpatchedBackwardIterator = typename inherited::BackwardIterator;

        /*
         * Methods we shadow so that patching is done. If you want to circumvent the
         * patching, thats fine - use scope resolution operator to call this's base
         * class version.
         */
    public:
        nonvirtual void SetLength (size_t newLength, T fillValue);
        nonvirtual void InsertAt (size_t index, T item);
        nonvirtual void RemoveAt (size_t index);
        nonvirtual void RemoveAll ();
        nonvirtual void RemoveAt (const ForwardIterator& i);
        nonvirtual void RemoveAt (const BackwardIterator& i);
        nonvirtual void SetAt (size_t i, T item);
        nonvirtual void SetAt (const ForwardIterator& i, T newValue);
        nonvirtual void SetAt (const BackwardIterator& i, T newValue);
        nonvirtual void AddBefore (const ForwardIterator& i, T item);
        nonvirtual void AddBefore (const BackwardIterator& i, T item);
        nonvirtual void AddAfter (const ForwardIterator& i, T item);
        nonvirtual void AddAfter (const BackwardIterator& i, T item);
        nonvirtual void SetCapacity (size_t slotsAlloced);
        nonvirtual void Compact ();

    public:
        nonvirtual void Invariant () const;

#if qDebug
    protected:
        nonvirtual void _Invariant () const;
#endif
    };

    /*
     *      _ArrayIteratorBase<T> is a private utility class designed
     *  to promote source code sharing among the patched iterator implementations.
     *
     *  \note   Subtle - but PatchableIteratorMixIn must come last in bases so it gets constructed (added to list of patchable stuff) after
     *          and removed before destruction of other bases
     */
    template <typename T>
    class Array<T>::_ArrayIteratorBase
        : public DataStructures::Array<T>::_ArrayIteratorBase,
          public PatchableContainerHelper<DataStructures::Array<T>>::PatchableIteratorMixIn {
    private:
        using inherited_DataStructure = typename DataStructures::Array<T>::_ArrayIteratorBase;
        using inherited_PatchHelper   = typename PatchableContainerHelper<DataStructures::Array<T>>::PatchableIteratorMixIn;

    public:
        _ArrayIteratorBase (IteratorOwnerID ownerID, const Array<T>* data);
        _ArrayIteratorBase (const _ArrayIteratorBase& from);

    public:
        ~_ArrayIteratorBase ();

    public:
        nonvirtual _ArrayIteratorBase& operator= (const _ArrayIteratorBase& rhs) = delete;

    public:
        nonvirtual void PatchAdd (size_t index);    //  call after add
        nonvirtual void PatchRemove (size_t index); //  call before remove
        nonvirtual void PatchRemoveAll ();          //  call after removeall
        nonvirtual void PatchRealloc ();            //  call after realloc could have happened

#if qDebug
    protected:
        virtual void _Invariant () const override;
#endif

    protected:
        virtual void PatchRemoveCurrent () = 0; // called from patchremove if patching current item...

    private:
        friend class Array<T>;
    };

    /*
     *      Array<T,TRAITS>::ForwardIterator is forwards iterator that can be used
     *  while modifing its owned array. It can only be used with Array<T,TRAITS>
     *  since the classes know about each other, and keep track of each other.
     *  This is intended to be a convienience in implementing concrete container
     *  mixins.
     */
    template <typename T>
    class Array<T>::ForwardIterator : public Array<T>::_ArrayIteratorBase {
    private:
        using inherited = typename Array<T>::_ArrayIteratorBase;

    public:
        ForwardIterator (IteratorOwnerID ownerID, const Array<T>* data);

    public:
        nonvirtual bool More (T* current, bool advance);
        nonvirtual void More (optional<T>* result, bool advance);
        nonvirtual bool More (nullptr_t, bool advance);

    protected:
        virtual void PatchRemoveCurrent () override;
    };

    /*
     *      Array<T,TRAITS>::BackwardIterator is backwards iterator that can be used
     *  while modifing its owned array. It can only be used with Array<T,TRAITS>
     *  since the classes know about each other, and keep track of each other.
     *  This is intended to be a convienience in implementing concrete container
     *  mixins.
     */
    template <typename T>
    class Array<T>::BackwardIterator : public Array<T>::_ArrayIteratorBase {
    private:
        using inherited = typename Array<T>::_ArrayIteratorBase;

    public:
        BackwardIterator (IteratorOwnerID ownerID, const Array<T>* data);

    public:
        nonvirtual bool More (T* current, bool advance);
        nonvirtual void More (optional<T>* result, bool advance);
        nonvirtual bool More (nullptr_t, bool advance);

    protected:
        virtual void PatchRemoveCurrent () override;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Array.inl"

#endif /*_Stroika_Foundation_Containers_Private_PatchingDataStructures_Array_h_ */
