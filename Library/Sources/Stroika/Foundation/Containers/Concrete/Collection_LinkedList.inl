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

    using Traversal::IteratorOwnerID;

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
        using _IterableRepSharedPtr   = typename Iterable<T>::_IterableRepSharedPtr;
        using _CollectionRepSharedPtr = typename inherited::_IRepSharedPtr;
        using _APPLY_ARGTYPE          = typename inherited::_APPLY_ARGTYPE;
        using _APPLYUNTIL_ARGTYPE     = typename inherited::_APPLYUNTIL_ARGTYPE;

    public:
        Rep_ ()                 = default;
        Rep_ (const Rep_& from) = delete;
        Rep_ (Rep_* from, [[maybe_unused]] IteratorOwnerID forIterableEnvelope)
            : fData_{from->fData_}
        {
            RequireNotNull (from);
        }

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

        // Iterable<T>::_IRep overrides
    public:
        virtual _IterableRepSharedPtr Clone (IteratorOwnerID forIterableEnvelope) const override
        {
            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
            return Iterable<T>::template MakeSmartPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
        }
        virtual Iterator<T> MakeIterator (IteratorOwnerID suggestedOwner) const override
        {
            Rep_* NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
            return Iterator<T> (Iterator<T>::template MakeSmartPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_));
        }
        virtual size_t GetLength () const override
        {
            return fData_.GetLength ();
        }
        virtual bool IsEmpty () const override
        {
            return fData_.IsEmpty ();
        }
        virtual void Apply (_APPLY_ARGTYPE doToElement) const override
        {
            // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
            // use iterator (which currently implies lots of locks) with this->_Apply ()
            fData_.Apply (doToElement);
        }
        virtual Iterator<T> FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            using RESULT_TYPE     = Iterator<T>;
            using SHARED_REP_TYPE = Traversal::IteratorBase::PtrImplementationTemplate<IteratorRep_>;
            auto iLink            = fData_.FindFirstThat (doToElement);
            if (iLink == nullptr) {
                return RESULT_TYPE::GetEmptyIterator ();
            }
            Rep_*           NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
            SHARED_REP_TYPE resultRep      = Iterator<T>::template MakeSmartPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
            resultRep->fIterator.SetCurrentLink (iLink);
            // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
            return RESULT_TYPE (move (resultRep));
        }

        // Collection<T>::_IRep overrides
    public:
        virtual _CollectionRepSharedPtr CloneEmpty ([[maybe_unused]] IteratorOwnerID forIterableEnvelope) const override
        {
            return Iterable<T>::template MakeSmartPtr<Rep_> ();
        }
        virtual void Add (ArgByValueType<T> item) override
        {
            fData_.Prepend (item);
        }
        virtual void Update (const Iterator<T>& i, ArgByValueType<T> newValue) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            auto&                                                     mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            fData_.SetAt (mir.fIterator, newValue);
        }
        virtual void Remove (const Iterator<T>& i) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            fData_.RemoveAt (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator);
        }
#if qDebug
        virtual void AssertNoIteratorsReferenceOwner ([[maybe_unused]] IteratorOwnerID oBeingDeleted) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            //    fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
        }
#endif

    private:
        using DataStructureImplType_ = DataStructures::LinkedList<T>;
        using IteratorRep_           = typename Private::IteratorImplHelper2_<T, DataStructureImplType_>;

    private:
        DataStructureImplType_ fData_;
    };

    /*
     ********************************************************************************
     ************************** Collection_LinkedList<T> ****************************
     ********************************************************************************
     */
    template <typename T>
    inline Collection_LinkedList<T>::Collection_LinkedList ()
        : inherited (inherited::template MakeSmartPtr<Rep_> ())
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    Collection_LinkedList<T>::Collection_LinkedList (const T* start, const T* end)
        : Collection_LinkedList ()
    {
        Require ((start == end) or (start != nullptr and end != nullptr));
        this->AddAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename T>
    Collection_LinkedList<T>::Collection_LinkedList (const Collection<T>& src)
        : Collection_LinkedList ()
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
