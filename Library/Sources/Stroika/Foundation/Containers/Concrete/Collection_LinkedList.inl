/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Collection_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Collection_LinkedList_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"

#include "../DataStructures/LinkedList.h"
#include "../Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
    */
    template <typename T>
    class Collection_LinkedList<T>::IImplRep_ : public Collection<T>::_IRep {
    };

    /*
     */
    template <typename T>
    class Collection_LinkedList<T>::Rep_ : public IImplRep_, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
    private:
        using inherited = IImplRep_;

    public:
        Rep_ ()                 = default;
        Rep_ (const Rep_& from) = default;

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

        // Iterable<T>::_IRep overrides
    public:
        virtual _IterableRepSharedPtr Clone () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return Iterable<value_type>::template MakeSmartPtr<Rep_> (*this);
        }
        virtual Iterator<value_type> MakeIterator () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_)};
        }
        virtual size_t GetLength () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return fData_.GetLength ();
        }
        virtual bool IsEmpty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return fData_.IsEmpty ();
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            fData_.Apply (doToElement);
        }
        virtual Iterator<value_type> FindFirstThat (const function<bool (ArgByValueType<value_type> item)>& doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> critSec{fData_};
            if (auto iLink = fData_.FindFirstThat (doToElement)) {
                return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_, iLink)};
            }
            return nullptr;
        }

        // Collection<T>::_IRep overrides
    public:
        virtual _CollectionRepSharedPtr CloneEmpty () const override
        {
            return Iterable<value_type>::template MakeSmartPtr<Rep_> ();
        }
        virtual _CollectionRepSharedPtr CloneAndPatchIterator (Iterator<value_type>* i) const override
        {
            RequireNotNull (i);
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            auto                                                        result = Iterable<T>::template MakeSmartPtr<Rep_> (*this);
            auto&                                                       mir    = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual void Add (ArgByValueType<value_type> item) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            fData_.Prepend (item); // order meaningless for collection, and prepend cheaper on linked list
            fChangeCounts_.PerformedChange ();
        }
        virtual void Update (const Iterator<value_type>& i, ArgByValueType<value_type> newValue, Iterator<value_type>* nextI) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex>            writeLock{fData_};
            optional<typename DataStructureImplType_::UnderlyingIteratorRep> savedUnderlyingIndex;
            if (nextI != nullptr) {
                savedUnderlyingIndex = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator.GetUnderlyingIteratorRep ();
            }
            fData_.SetAt (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator, newValue);
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_, *savedUnderlyingIndex)};
            }
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            if (nextI != nullptr) {
                *nextI = i;
                ++(*nextI);
            }
            fData_.RemoveAt (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator);
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                Debug::UncheckedDynamicCast<IteratorRep_&> (nextI->GetRep ()).UpdateChangeCount ();
                nextI->Refresh (); // update to reflect changes made to rep
            }
        }

    private:
        using DataStructureImplType_ = DataStructures::LinkedList<value_type>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                          fData_;
        [[NO_UNIQUE_ADDRESS_ATTR]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     ************************** Collection_LinkedList<T> ****************************
     ********************************************************************************
     */
    template <typename T>
    inline Collection_LinkedList<T>::Collection_LinkedList ()
        : inherited{inherited::template MakeSmartPtr<Rep_> ()}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    Collection_LinkedList<T>::Collection_LinkedList (const T* start, const T* end)
        : Collection_LinkedList{}
    {
        Require ((start == end) or (start != nullptr and end != nullptr));
        this->AddAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename T>
    Collection_LinkedList<T>::Collection_LinkedList (const Collection<T>& src)
        : Collection_LinkedList{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T>
    inline void Collection_LinkedList<T>::AssertRepValidType_ () const
    {
#if qDebug
        typename inherited::template _SafeReadRepAccessor<Rep_> tmp{this}; // for side-effect of AssertMemeber
#endif
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_Collection_LinkedList_inl_ */
