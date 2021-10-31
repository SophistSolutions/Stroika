/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Collection_Array_inl_
#define _Stroika_Foundation_Containers_Concrete_Collection_Array_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"

#include "../DataStructures/Array.h"
#include "../Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     ********************************************************************************
     ********************** Collection_Array<T,TRAITS>::Rep_ ************************
     ********************************************************************************
     */
    template <typename T>
    class Collection_Array<T>::Rep_ : public Collection<T>::_IRep, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
    private:
        using inherited = typename Collection<T>::_IRep;

    public:
        Rep_ ()                 = default;
        Rep_ (const Rep_& from) = default;

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

        // Iterable<T>::_IRep overrides
    public:
        virtual _IterableRepSharedPtr Clone () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> readLock{fData_};
            return Iterable<value_type>::template MakeSmartPtr<Rep_> (*this);
        }
        virtual Iterator<value_type> MakeIterator () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> readLock{fData_};
            return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_)};
        }
        virtual size_t GetLength () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> readLock{fData_};
            return fData_.GetLength ();
        }
        virtual bool IsEmpty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> readLock{fData_};
            return fData_.GetLength () == 0;
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> readLock{fData_};
            // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
            // use iterator (which currently implies lots of locks) with this->_Apply ()
            fData_.Apply (doToElement);
        }
        virtual Iterator<value_type> FindFirstThat (const function<bool (ArgByValueType<value_type> item)>& doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> readLock{fData_};
            size_t                                                     i = fData_.FindFirstThat (doToElement);
            if (i == fData_.GetLength ()) {
                return nullptr;
            }
            Traversal::IteratorBase::PtrImplementationTemplate<IteratorRep_> resultRep = Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_);
            resultRep->fIterator.SetIndex (i);
            return Iterator<value_type>{move (resultRep)};
        }

        // Collection<T>::_IRep overrides
    public:
        virtual _CollectionRepSharedPtr CloneEmpty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> readLock{fData_};
            return Iterable<T>::template MakeSmartPtr<Rep_> ();
        }
        virtual _CollectionRepSharedPtr CloneAndPatchIterator (Iterator<value_type>* i) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> readLock{fData_};
            auto                                                       result = Iterable<T>::template MakeSmartPtr<Rep_> (*this);
            auto&                                                      mir    = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual void Add (ArgByValueType<value_type> item) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedLock> writeLock{fData_};
            // Appending is fastest
            fData_.InsertAt (fData_.GetLength (), item);
            fChangeCounts_.PerformedChange ();
        }
        virtual void Update (const Iterator<value_type>& i, ArgByValueType<value_type> newValue) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedLock> writeLock{fData_};
            fData_.SetAt (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator, newValue);
            fChangeCounts_.PerformedChange ();
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedLock> writeLock{fData_};
            if (nextI != nullptr) {
                *nextI    = i;
                auto iRep = Debug::UncheckedDynamicCast<const IteratorRep_*> (&nextI->ConstGetRep ());
                iRep->fIterator.PatchBeforeRemove (&Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator);
            }
            fData_.RemoveAt (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator);
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                Debug::UncheckedDynamicCast<IteratorRep_&> (nextI->GetRep ()).UpdateChangeCount ();
                nextI->Refresh (); // update to reflect changes made to rep
            }
        }

    private:
        using DataStructureImplType_ = DataStructures::Array<value_type>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                          fData_;
        [[NO_UNIQUE_ADDRESS_ATTR]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     ************************* Collection_Array<T,TRAITS> ***************************
     ********************************************************************************
     */
    template <typename T>
    inline Collection_Array<T>::Collection_Array ()
        : inherited{inherited::template MakeSmartPtr<Rep_> ()}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    inline Collection_Array<T>::Collection_Array (const Collection<T>& src)
        : Collection_Array{}
    {
        SetCapacity (src.GetLength ());
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T>
    inline Collection_Array<T>::Collection_Array (const T* start, const T* end)
        : Collection_Array{}
    {
        Require ((start == end) or (start != nullptr and end != nullptr));
        AssertRepValidType_ ();
        if (start != end) {
            SetCapacity (end - start);
            this->AddAll (start, end);
        }
        AssertRepValidType_ ();
    }
    template <typename T>
    inline void Collection_Array<T>::Compact ()
    {
        using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<Rep_>;
        _SafeReadWriteRepAccessor{this}._GetWriteableRep ().fData_.Compact ();
    }
    template <typename T>
    inline size_t Collection_Array<T>::GetCapacity () const
    {
        using _SafeReadRepAccessor = typename inherited::template _SafeReadRepAccessor<Rep_>;
        return _SafeReadRepAccessor{this}._ConstGetRep ().fData_.GetCapacity ();
    }
    template <typename T>
    inline void Collection_Array<T>::SetCapacity (size_t slotsAlloced)
    {
        using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<Rep_>;
        _SafeReadWriteRepAccessor{this}._GetWriteableRep ().fData_.SetCapacity (slotsAlloced);
    }
    template <typename T>
    inline size_t Collection_Array<T>::capacity () const
    {
        return GetCapacity ();
    }
    template <typename T>
    inline void Collection_Array<T>::reserve (size_t slotsAlloced)
    {
        SetCapacity (slotsAlloced);
    }
    template <typename T>
    inline void Collection_Array<T>::AssertRepValidType_ () const
    {
#if qDebug
        typename inherited::template _SafeReadRepAccessor<Rep_> tmp{this}; // for side-effect of AssertMemeber
#endif
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_Collection_Array_inl_ */
