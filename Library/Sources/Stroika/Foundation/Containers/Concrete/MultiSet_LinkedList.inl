/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_MultiSet_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_MultiSet_LinkedList_inl_ 1

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
     ********************************************************************************
     ******************** MultiSet_LinkedList<T, TRAITS>::IImplRepBase_ *************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    class MultiSet_LinkedList<T, TRAITS>::IImplRepBase_ : public MultiSet<T, TRAITS>::_IRep {
    };

    /*
     ********************************************************************************
     ********************** MultiSet_LinkedList<T, TRAITS>::Rep_ ********************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    template <typename EQUALS_COMPARER>
    class MultiSet_LinkedList<T, TRAITS>::Rep_ : public IImplRepBase_, public Memory::UseBlockAllocationIfAppropriate<Rep_<EQUALS_COMPARER>> {
    private:
        using inherited = IImplRepBase_;

    public:
        Rep_ (const EQUALS_COMPARER& equalsComparer)
            : fEqualsComparer_{equalsComparer}
        {
        }
        Rep_ (const Rep_& from) = delete;
        Rep_ (const Rep_* from, [[maybe_unused]] IteratorOwnerID forIterableEnvelope)
            : fEqualsComparer_{from->fEqualsComparer_}
            , fData_{from->fData_}
        {
            RequireNotNull (from);
        }

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

    private:
        [[NO_UNIQUE_ADDRESS_ATTR]] const EQUALS_COMPARER fEqualsComparer_;

        // Iterable<T>::_IRep overrides
    public:
        virtual _IterableRepSharedPtr Clone (IteratorOwnerID forIterableEnvelope) const override
        {
            return Iterable<value_type>::template MakeSmartPtr<Rep_> (this, forIterableEnvelope);
        }
        virtual size_t GetLength () const override
        {
            return fData_.GetLength ();
        }
        virtual bool IsEmpty () const override
        {
            return fData_.IsEmpty ();
        }
        virtual Iterator<value_type> MakeIterator (IteratorOwnerID suggestedOwner) const override
        {
            Rep_* NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
            return Iterator<value_type> (Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_));
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement) const override
        {
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

        // MultiSet<T, TRAITS>::_IRep overrides
    public:
        virtual ElementEqualityComparerType GetElementEqualsComparer () const override
        {
            return ElementEqualityComparerType{fEqualsComparer_};
        }
        virtual _MultiSetRepSharedPtr CloneEmpty ([[maybe_unused]] IteratorOwnerID forIterableEnvelope) const override
        {
            return Iterable<value_type>::template MakeSmartPtr<Rep_> (fEqualsComparer_);
        }
        virtual _MultiSetRepSharedPtr CloneAndPatchIterator (Iterator<value_type>* i, IteratorOwnerID obsoleteForIterableEnvelope) const override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            auto                                                      result = Iterable<value_type>::template MakeSmartPtr<Rep_> (this, obsoleteForIterableEnvelope);
            auto&                                                     mir    = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual bool Equals (const typename MultiSet<T, TRAITS>::_IRep& rhs) const override
        {
            return this->_Equals_Reference_Implementation (rhs);
        }
        virtual bool Contains (ArgByValueType<T> item) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            value_type                                                 c = item;
            for (typename DataStructureImplType_::ForwardIterator it{&fData_}; it.More (&c, true);) {
                if (fEqualsComparer_ (c.fValue, item)) {
                    Assert (c.fCount != 0);
                    return true;
                }
            }
            return false;
        }
        virtual void Add (ArgByValueType<T> item, CounterType count) override
        {
            using Traversal::kUnknownIteratorOwnerID;
            if (count != 0) {
                value_type                                                current (item);
                lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                for (typename DataStructureImplType_::ForwardIterator it{&fData_}; it.More (&current, true);) {
                    if (fEqualsComparer_ (current.fValue, item)) {
                        current.fCount += count;
                        fData_.SetAt (it, current);
                        return;
                    }
                }
                fData_.Prepend (value_type{item, count});
            }
        }
        virtual void Remove (ArgByValueType<T> item, CounterType count) override
        {
            using Traversal::kUnknownIteratorOwnerID;
            if (count != 0) {
                value_type                                                current (item);
                lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                for (typename DataStructureImplType_::ForwardIterator it{&fData_}; it.More (&current, true);) {
                    if (fEqualsComparer_ (current.fValue, item)) {
                        if (current.fCount > count) {
                            current.fCount -= count;
                        }
                        else {
                            current.fCount = 0; // Should this be an underflow excpetion, assertion???
                        }
                        if (current.fCount == 0) {
                            fData_.RemoveAt (it);
                        }
                        else {
                            fData_.SetAt (it, current);
                        }
                        break;
                    }
                }
            }
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
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
        virtual void UpdateCount (const Iterator<value_type>& i, CounterType newCount) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            auto&                                                     mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            if (newCount == 0) {
                fData_.RemoveAt (mir.fIterator);
            }
            else {
                value_type c = mir.fIterator.Current ();
                c.fCount     = newCount;
                fData_.SetAt (mir.fIterator, c);
            }
        }
        virtual CounterType OccurrencesOf (ArgByValueType<T> item) const override
        {
            value_type                                                 c = item;
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            for (typename DataStructureImplType_::ForwardIterator it{&fData_}; it.More (&c, true);) {
                if (fEqualsComparer_ (c.fValue, item)) {
                    Ensure (c.fCount != 0);
                    return c.fCount;
                }
            }
            return 0;
        }
        virtual Iterable<T> Elements (const _MultiSetRepSharedPtr& rep) const override
        {
            return this->_Elements_Reference_Implementation (rep);
        }
        virtual Iterable<T> UniqueElements (const _MultiSetRepSharedPtr& rep) const override
        {
            return this->_UniqueElements_Reference_Implementation (rep);
        }
#if qDebug
        virtual void AssertNoIteratorsReferenceOwner ([[maybe_unused]] IteratorOwnerID oBeingDeleted) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            //            fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
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
     ************************ MultiSet_LinkedList<T, TRAITS> ************************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    inline MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList ()
        : MultiSet_LinkedList{equal_to<T>{}}
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <typename EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<T, EQUALS_COMPARER> ()>*>
    inline MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList (const EQUALS_COMPARER& equalsComparer)
        : inherited (inherited::template MakeSmartPtr<Rep_<EQUALS_COMPARER>> (equalsComparer))
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <typename COPY_FROM_ITERATOR>
    inline MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end)
        : MultiSet_LinkedList{}
    {
        this->AddAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    inline MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList (const MultiSet<T, TRAITS>& src)
        : MultiSet_LinkedList{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    inline MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList (const MultiSet_LinkedList<T, TRAITS>& src)
        : inherited (src)
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    inline MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList (const initializer_list<T>& src)
        : MultiSet_LinkedList{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList (const initializer_list<CountedValue<T>>& src)
        : MultiSet_LinkedList{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    inline void MultiSet_LinkedList<T, TRAITS>::AssertRepValidType_ () const
    {
#if qDebug
        typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMemeber
#endif
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_MultiSet_LinkedList_inl_ */
