/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_KeyedCollection_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_KeyedCollection_LinkedList_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Memory/BlockAllocated.h"

#include "../Private/IteratorImplHelper.h"
#include "../Private/PatchingDataStructures/LinkedList.h"

namespace Stroika::Foundation::Containers::Concrete {

    using Traversal::IteratorOwnerID;

    /*
    */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    class KeyedCollection_LinkedList<T, KEY_TYPE, TRAITS>::IImplRep_ : public KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep {
    };

    /*
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    class KeyedCollection_LinkedList<T, KEY_TYPE, TRAITS>::Rep_ : public IImplRep_, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
    private:
        using inherited = IImplRep_;
        KeyExtractorType        fKeyExtractor_;
        KeyEqualityComparerType fKeyComparer_;

    public:
        using _IterableRepSharedPtr        = typename Iterable<T>::_IterableRepSharedPtr;
        using _KeyedCollectionRepSharedPtr = typename inherited::_KeyedCollectionRepSharedPtr;
        using _APPLY_ARGTYPE               = typename inherited::_APPLY_ARGTYPE;
        using _APPLYUNTIL_ARGTYPE          = typename inherited::_APPLYUNTIL_ARGTYPE;

    public:
        Rep_ (KeyExtractorType keyExtractor, KeyEqualityComparerType keyComparer)
            : fKeyExtractor_{keyExtractor}
            , fKeyComparer_{keyComparer}
        {
        }
        Rep_ (const Rep_& from) = delete;
        Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope)
            : fKeyExtractor_{from->fKeyExtractor_}
            , fKeyComparer_{from->fKeyComparer_}
            , fData_{&from->fData_, forIterableEnvelope}
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

        // KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep overrides
    public:
        virtual _KeyedCollectionRepSharedPtr CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
        {
            if (fData_.HasActiveIterators ()) {
                // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                auto r = Iterable<T>::template MakeSmartPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                r->fData_.RemoveAll ();
                return r;
            }
            else {
                return Iterable<T>::template MakeSmartPtr<Rep_> (this->fKeyExtractor_, this->fKeyComparer_);
            }
        }
        virtual Iterable<KEY_TYPE> Keys () const override
        {
            AssertNotImplemented ();
            return Iterable<KEY_TYPE>{};
        }
        virtual bool Lookup (ArgByValueType<KeyType> key, optional<value_type>* item) const
        {
            return false;
        }
        virtual void Add (ArgByValueType<T> item) override
        {
            fData_.Prepend (item);
        }
        virtual void Update (const Iterator<T>& i, ArgByValueType<T> newValue) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            const typename Iterator<T>::IRep&                         ir = i.ConstGetRep ();
            AssertMember (&ir, IteratorRep_);
            auto& mir = dynamic_cast<const IteratorRep_&> (ir);
            fData_.SetAt (mir.fIterator, newValue);
        }
        virtual void Remove (const Iterator<T>& i) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            const typename Iterator<T>::IRep&                         ir = i.ConstGetRep ();
            AssertMember (&ir, IteratorRep_);
            auto& mir = dynamic_cast<const IteratorRep_&> (ir);
            fData_.RemoveAt (mir.fIterator);
        }
        virtual void Remove (ArgByValueType<KEY_TYPE> key) override
        {
            if (auto i = this->FindFirstThat ([this, &key] (ArgByValueType<T> item) { return fKeyComparer_ (fKeyExtractor_ (item), key); }, nullptr)) {
                Remove (i);
                return;
            }
            AssertNotImplemented ();
        }
#if qDebug
        virtual void AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
        }
#endif

    private:
        using DataStructureImplType_ = Private::PatchingDataStructures::LinkedList<T>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<T, DataStructureImplType_>;

    private:
        DataStructureImplType_ fData_;
    };

    /*
     ********************************************************************************
     ************************** KeyedCollection_LinkedList<T> ***********************
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline KeyedCollection_LinkedList<T, KEY_TYPE, TRAITS>::KeyedCollection_LinkedList (KeyExtractorType keyExtractor, KeyEqualityComparerType keyComparer)
        : inherited (inherited::template MakeSmartPtr<Rep_> (keyExtractor, keyComparer))
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    KeyedCollection_LinkedList<T, KEY_TYPE, TRAITS>::KeyedCollection_LinkedList (const T* start, const T* end)
        : Collection_LinkedList ()
    {
        Require ((start == end) or (start != nullptr and end != nullptr));
        this->AddAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    KeyedCollection_LinkedList<T, KEY_TYPE, TRAITS>::KeyedCollection_LinkedList (const KeyedCollection<T, KEY_TYPE, TRAITS>& src)
        : Collection_LinkedList ()
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void KeyedCollection_LinkedList<T, KEY_TYPE, TRAITS>::AssertRepValidType_ () const
    {
#if qDebug
        typename inherited::template _SafeReadRepAccessor<Rep_> tmp{this}; // for side-effect of AssertMemeber
#endif
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_KeyedCollection_LinkedList_inl_ */
