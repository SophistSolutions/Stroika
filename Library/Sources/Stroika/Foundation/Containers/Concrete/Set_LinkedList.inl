/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Set_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Set_LinkedList_inl_

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

    /**
     */
    template <typename T>
    class Set_LinkedList<T>::IImplRepBase_ : public Set<T>::_IRep {
    private:
        using inherited = typename Set<T>::_IRep;

    protected:
        // @todo - DON'T UNDERTAND WHY these 2 using declarations needed? on visual studio.net?? retest!
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
        Rep_ (const EQUALS_COMPARER& equalsComparer)
            : fEqualsComparer_{equalsComparer}
        {
        }
        Rep_ (const Rep_& from) = delete;
        Rep_ (Rep_* from, [[maybe_unused]] IteratorOwnerID forIterableEnvelope)
            : fEqualsComparer_{from->fEqualsComparer_}
            , fData_{from->fData_}
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
        virtual Iterator<value_type> MakeIterator (IteratorOwnerID suggestedOwner) const override
        {
            Rep_* NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
            return Iterator<value_type> (Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_));
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
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
            // use iterator (which currently implies lots of locks) with this->_Apply ()
            fData_.Apply (doToElement);
        }
        virtual Iterator<value_type> FindFirstThat (const function<bool (ArgByValueType<value_type> item)>& doToElement, IteratorOwnerID suggestedOwner) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            using SHARED_REP_TYPE = Traversal::IteratorBase::PtrImplementationTemplate<IteratorRep_>;
            auto iLink            = fData_.FindFirstThat (doToElement);
            if (iLink == nullptr) {
                return Iterator<value_type>::GetEmptyIterator ();
            }
            Rep_*           NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
            SHARED_REP_TYPE resultRep      = Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
            resultRep->fIterator.SetCurrentLink (iLink);
            // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
            return Iterator<value_type>{move (resultRep)};
        }

        // Set<T>::_IRep overrides
    public:
        virtual ElementEqualityComparerType GetElementEqualsComparer () const override
        {
            return ElementEqualityComparerType{fEqualsComparer_};
        }
        virtual _SetRepSharedPtr CloneEmpty ([[maybe_unused]] IteratorOwnerID forIterableEnvelope) const override
        {
            return Iterable<T>::template MakeSmartPtr<Rep_> (fEqualsComparer_);
        }
        virtual _SetRepSharedPtr CloneAndPatchIterator (Iterator<value_type>* i, IteratorOwnerID obsoleteForIterableEnvelope) const override
        {
            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
            auto                                                      result = Iterable<value_type>::template MakeSmartPtr<Rep_> (const_cast<Rep_*> (this), obsoleteForIterableEnvelope);
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            auto&                                                     mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual bool Equals (const typename Iterable<value_type>::_IRep& rhs) const override
        {
            return this->_Equals_Reference_Implementation (rhs);
        }
        virtual bool Contains (ArgByValueType<value_type> item) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            return fData_.Lookup (item, fEqualsComparer_) != nullptr;
        }
        virtual optional<T> Lookup (ArgByValueType<value_type> item) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            const T*                                                   l = fData_.Lookup (item, fEqualsComparer_);
            return (l == nullptr) ? optional<value_type>{} : optional<value_type>{*l};
        }
        virtual void Add (ArgByValueType<value_type> item) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> lg{fData_};
            for (typename DataStructureImplType_::ForwardIterator it{&fData_}; it.More (nullptr, true);) {
                if (fEqualsComparer_ (it.Current (), item)) {
                    return;
                }
            }
            fData_.Prepend (item);
        }
        virtual void Remove (ArgByValueType<value_type> item) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> lg{fData_};
            using Traversal::kUnknownIteratorOwnerID;
            for (typename DataStructureImplType_::ForwardIterator it{&fData_}; it.More (nullptr, true);) {
                if (fEqualsComparer_ (it.Current (), item)) {
                    fData_.RemoveAt (it);
                    return;
                }
            }
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            auto&                                                     mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            lock_guard<const Debug::AssertExternallySynchronizedLock> lg{fData_};
            auto                                                      next = mir.fIterator.GetCurrentLink ()->fNext;
            fData_.RemoveAt (mir.fIterator);
            if (nextI != nullptr) {
                auto resultRep = Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (i.GetOwner (), &fData_);
                resultRep->fIterator.SetCurrentLink (next);
                *nextI = Iterator<value_type>{move (resultRep)};
            }
        }
#if qDebug
        virtual void AssertNoIteratorsReferenceOwner ([[maybe_unused]] IteratorOwnerID oBeingDeleted) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            //    fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
        }
#endif

    private:
        using DataStructureImplType_ = DataStructures::LinkedList<value_type>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

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
        : Set_LinkedList{equal_to<T>{}}
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
        : Set_LinkedList{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T>
    inline Set_LinkedList<T>::Set_LinkedList (const ElementEqualityComparerType& equalsComparer, const initializer_list<T>& src)
        : Set_LinkedList (equalsComparer)
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<Set_LinkedList<T>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>*>
    inline Set_LinkedList<T>::Set_LinkedList (CONTAINER_OF_ADDABLE&& src)
        : Set_LinkedList{}
    {
        this->AddAll (forward<CONTAINER_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T>>*>
    inline Set_LinkedList<T>::Set_LinkedList (const ElementEqualityComparerType& equalsComparer, CONTAINER_OF_ADDABLE&& src)
        : Set_LinkedList (equalsComparer)
    {
        this->AddAll (forward<CONTAINER_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename COPY_FROM_ITERATOR_OF_T>
    inline Set_LinkedList<T>::Set_LinkedList (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
        : Set_LinkedList{}
    {
        AddAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename COPY_FROM_ITERATOR_OF_T>
    inline Set_LinkedList<T>::Set_LinkedList (const ElementEqualityComparerType& equalsComparer, COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
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
