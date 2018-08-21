/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Set_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Set_LinkedList_inl_

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

    /**
     */
    template <typename T>
    class Set_LinkedList<T>::IImplRepBase_ : public Set<T>::_IRep {
    private:
        using inherited = typename Set<T>::_IRep;

    protected:
        // @todo - DONT UNDERTAND WHY these 2 using declarations needed? on visual studio.net?? retest!
        using _APPLY_ARGTYPE      = typename inherited::_APPLY_ARGTYPE;
        using _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;
    };

    /**
     */
    template <typename T>
    template <typename EQUALS_COMPARER>
    class Set_LinkedList<T>::Rep_ : public IImplRepBase_, public Memory::UseBlockAllocationIfAppropriate<Rep_<EQUALS_COMPARER>> {
    private:
        using inherited = IImplRepBase_;

    public:
        using _IterableRepSharedPtr = typename Iterable<T>::_IterableRepSharedPtr;
        using _SetRepSharedPtr      = typename inherited::_SetRepSharedPtr;
        using _APPLY_ARGTYPE        = typename inherited::_APPLY_ARGTYPE;
        using _APPLYUNTIL_ARGTYPE   = typename inherited::_APPLYUNTIL_ARGTYPE;
        using EqualityComparerType  = typename Set<T>::EqualityComparerType;

    public:
        Rep_ (const EQUALS_COMPARER& equalsComparer)
            : fEqualsComparer_ (equalsComparer)
        {
        }
        Rep_ (const Rep_& from) = delete;
        Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope)
            : inherited ()
            , fEqualsComparer_ (from->fEqualsComparer_)
            , fData_ (&from->fData_, forIterableEnvelope)
        {
            RequireNotNull (from);
        }

    private:
        [[NO_UNIQUE_ADDRESS_ATTR]] EQUALS_COMPARER fEqualsComparer_;

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
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            return fData_.GetLength ();
        }
        virtual bool IsEmpty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            return fData_.IsEmpty ();
        }
        virtual void Apply (_APPLY_ARGTYPE doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
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

        // Set<T>::_IRep overrides
    public:
        virtual EqualityComparerType GetEqualsComparer () const override
        {
            return EqualityComparerType{fEqualsComparer_};
        }
        virtual _SetRepSharedPtr CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
        {
            if (fData_.HasActiveIterators ()) {
                // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                auto r = Iterable<T>::template MakeSmartPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                r->fData_.RemoveAll ();
                return r;
            }
            else {
                return Iterable<T>::template MakeSmartPtr<Rep_> (fEqualsComparer_);
            }
        }
        virtual bool Equals (const typename Set<T>::_IRep& rhs) const override
        {
            return this->_Equals_Reference_Implementation (rhs);
        }
        virtual bool Contains (ArgByValueType<T> item) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            return fData_.Lookup (item, fEqualsComparer_) != nullptr;
        }
        virtual optional<T> Lookup (ArgByValueType<T> item) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            const T*                                                   l = fData_.Lookup (item, fEqualsComparer_);
            return (l == nullptr) ? optional<T> () : optional<T> (*l);
        }
        virtual void Add (ArgByValueType<T> item) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> lg{fData_};
            // safe to use UnpatchedForwardIterator cuz locked and no updates
            for (typename DataStructureImplType_::UnpatchedForwardIterator it (&fData_); it.More (nullptr, true);) {
                if (fEqualsComparer_ (it.Current (), item)) {
                    return;
                }
            }
            fData_.Prepend (item);
        }
        virtual void Remove (ArgByValueType<T> item) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> lg{fData_};
            using Traversal::kUnknownIteratorOwnerID;
            for (typename DataStructureImplType_::ForwardIterator it (kUnknownIteratorOwnerID, &fData_); it.More (nullptr, true);) {
                if (fEqualsComparer_ (it.Current (), item)) {
                    fData_.RemoveAt (it);
                    return;
                }
            }
        }
        virtual void Remove (const Iterator<T>& i) override
        {
            const typename Iterator<T>::IRep& ir = i.ConstGetRep ();
            AssertMember (&ir, IteratorRep_);
            auto&                                                     mir = dynamic_cast<const IteratorRep_&> (ir);
            lock_guard<const Debug::AssertExternallySynchronizedLock> lg{fData_};
            fData_.RemoveAt (mir.fIterator);
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
     ******************************** Set_LinkedList<T> *****************************
     ********************************************************************************
     */
    template <typename T>
    inline Set_LinkedList<T>::Set_LinkedList ()
        : Set_LinkedList (equal_to<T>{})
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    inline Set_LinkedList<T>::Set_LinkedList (const EQUALS_COMPARER& equalsComparer)
        : inherited (inherited::template MakeSmartPtr<Rep_<EQUALS_COMPARER>> (equalsComparer))
    {
        static_assert (Common::IsEqualsComparer<EQUALS_COMPARER> (), "Equals comparer required with Set_LinkedList");
        AssertRepValidType_ ();
    }
    template <typename T>
    inline Set_LinkedList<T>::Set_LinkedList (const initializer_list<T>& src)
        : Set_LinkedList ()
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T>
    inline Set_LinkedList<T>::Set_LinkedList (const EqualityComparerType& equalsComparer, const initializer_list<T>& src)
        : Set_LinkedList (equalsComparer)
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename CONTAINER_OF_T, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_T> and not is_convertible_v<const CONTAINER_OF_T*, const Set_LinkedList<T>*>>*>
    inline Set_LinkedList<T>::Set_LinkedList (const CONTAINER_OF_T& src)
        : Set_LinkedList ()
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename CONTAINER_OF_T, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_T> and not is_convertible_v<const CONTAINER_OF_T*, const Set_LinkedList<T>*>>*>
    inline Set_LinkedList<T>::Set_LinkedList (const EqualityComparerType& equalsComparer, const CONTAINER_OF_T& src)
        : Set_LinkedList (equalsComparer)
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename COPY_FROM_ITERATOR_OF_T>
    inline Set_LinkedList<T>::Set_LinkedList (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
        : Set_LinkedList ()
    {
        AddAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename COPY_FROM_ITERATOR_OF_T>
    inline Set_LinkedList<T>::Set_LinkedList (const EqualityComparerType& equalsComparer, COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
        : Set_LinkedList (equalsComparer)
    {
        AddAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename T>
    inline void Set_LinkedList<T>::AssertRepValidType_ () const
    {
#if qDebug
        typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMember
#endif
    }

}
#endif /* _Stroika_Foundation_Containers_Concrete_Set_LinkedList_inl_ */
