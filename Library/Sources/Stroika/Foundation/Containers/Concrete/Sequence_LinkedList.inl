/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Sequence_LinkedList_inl_

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

    template <typename T>
    class Sequence_LinkedList<T>::Rep_ : public Sequence<T>::_IRep, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
    private:
        using inherited = typename Sequence<T>::_IRep;

    public:
        using _IterableRepSharedPtr = typename Iterable<T>::_IterableRepSharedPtr;
        using _SequenceRepSharedPtr = typename inherited::_IRepSharedPtr;
        using _APPLY_ARGTYPE        = typename inherited::_APPLY_ARGTYPE;
        using _APPLYUNTIL_ARGTYPE   = typename inherited::_APPLYUNTIL_ARGTYPE;

    protected:
        static constexpr size_t _kSentinalLastItemIndex = inherited::_kSentinalLastItemIndex;

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

        // Sequence<T>::_IRep overrides
    public:
        virtual _SequenceRepSharedPtr CloneEmpty ([[maybe_unused]] IteratorOwnerID forIterableEnvelope) const override
        {
            return Iterable<T>::template MakeSmartPtr<Rep_> ();
        }
        virtual _SequenceRepSharedPtr CloneAndPatchIterator (Iterator<T>* i, IteratorOwnerID obsoleteForIterableEnvelope) const override
        {
            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
            auto                                                      result = Iterable<T>::template MakeSmartPtr<Rep_> (const_cast<Rep_*> (this), obsoleteForIterableEnvelope);
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            auto&                                                     mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            return result;
        }
        virtual T GetAt (size_t i) const override
        {
            Require (not IsEmpty ());
            Require (i == _kSentinalLastItemIndex or i < GetLength ());
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            if (i == _kSentinalLastItemIndex) {
                i = GetLength () - 1;
            }
            return fData_.GetAt (i);
        }
        virtual void SetAt (size_t i, ArgByValueType<T> item) override
        {
            Require (i < GetLength ());
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            fData_.SetAt (item, i);
        }
        virtual size_t IndexOf (const Iterator<T>& i) const override
        {
            auto&                                                      mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            return mir.fIterator.CurrentIndex ();
        }
        virtual void Remove (const Iterator<T>& i, Iterator<T>* nextI) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            if (nextI != nullptr) {
                *nextI = i;
                ++(*nextI);
            }
            fData_.RemoveAt (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator);
            if (nextI != nullptr) {
                nextI->Refresh (); // update to reflect changes made to rep
            }
        }
        virtual void Update (const Iterator<T>& i, ArgByValueType<T> newValue) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            fData_.SetAt (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator, newValue);
        }
        virtual void Insert (size_t at, const T* from, const T* to) override
        {
            Require (at == _kSentinalLastItemIndex or at <= GetLength ());
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            if (at == _kSentinalLastItemIndex) {
                at = fData_.GetLength ();
            }
            // quickie poor impl
            // See Stroika v1 - much better - handling cases of remove near start or end of linked list
            if (at == 0) {
                for (size_t i = (to - from); i > 0; --i) {
                    fData_.Prepend (from[i - 1]);
                }
            }
            else if (at == fData_.GetLength ()) {
                for (const T* p = from; p != to; ++p) {
                    fData_.Append (*p);
                }
            }
            else {
                size_t index = at;
                for (typename DataStructureImplType_::ForwardIterator it{&fData_}; it.More (nullptr, true);) {
                    if (--index == 0) {
                        for (const T* p = from; p != to; ++p) {
                            fData_.AddBefore (it, *p);
                        }
                        break;
                    }
                }
                //Assert (not it.Done ());      // cuz that would mean we never added
            }
        }
        virtual void Remove (size_t from, size_t to) override
        {
            // quickie poor impl
            // See Stroika v1 - much better - handling cases of remove near start or end of linked list
            size_t                                                    index          = from;
            size_t                                                    amountToRemove = (to - from);
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            for (typename DataStructureImplType_::ForwardIterator it{&fData_}; it.More (nullptr, true);) {
                if (index-- == 0) {
                    while (amountToRemove-- != 0) {
                        it = fData_.RemoveAt (it);
                    }
                    break;
                }
            }
        }
#if qDebug
        virtual void AssertNoIteratorsReferenceOwner ([[maybe_unused]] IteratorOwnerID oBeingDeleted) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            //      fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
        }
#endif

    private:
        using DataStructureImplType_ = DataStructures::LinkedList<T>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<T, DataStructureImplType_>;

    private:
        DataStructureImplType_ fData_;
    };

    /*
     ********************************************************************************
     **************************** Sequence_LinkedList<T> ****************************
     ********************************************************************************
     */
    template <typename T>
    inline Sequence_LinkedList<T>::Sequence_LinkedList ()
        : inherited{inherited::template MakeSmartPtr<Rep_> ()}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<Sequence_LinkedList<T>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>*>
    inline Sequence_LinkedList<T>::Sequence_LinkedList (CONTAINER_OF_ADDABLE&& src)
        : Sequence_LinkedList{}
    {
        this->AppendAll (forward<CONTAINER_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename COPY_FROM_ITERATOR_OF_T>
    inline Sequence_LinkedList<T>::Sequence_LinkedList (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
        : Sequence_LinkedList{}
    {
        this->AppendAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename T>
    inline void Sequence_LinkedList<T>::AssertRepValidType_ () const
    {
#if qDebug
        typename inherited::template _SafeReadRepAccessor<Rep_> tmp{this}; // for side-effect of AssertMemeber
#endif
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_Sequence_LinkedList_inl_ */
