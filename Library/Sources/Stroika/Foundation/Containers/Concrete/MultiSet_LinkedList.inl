/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_MultiSet_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_MultiSet_LinkedList_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Memory/BlockAllocated.h"

#include "../Private/IteratorImplHelper.h"
#include "../Private/PatchingDataStructures/LinkedList.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Concrete {

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
                class MultiSet_LinkedList<T, TRAITS>::Rep_ : public IImplRepBase_ {
                private:
                    using inherited = IImplRepBase_;

                public:
                    using _IterableRepSharedPtr = typename Iterable<CountedValue<T>>::_IterableRepSharedPtr;
                    using _MultiSetRepSharedPtr = typename inherited::_MultiSetRepSharedPtr;
                    using _APPLY_ARGTYPE        = typename inherited::_APPLY_ARGTYPE;
                    using _APPLYUNTIL_ARGTYPE   = typename inherited::_APPLYUNTIL_ARGTYPE;
                    using CounterType           = typename inherited::CounterType;
                    using EqualityComparerType  = typename MultiSet<T, TRAITS>::EqualityComparerType;

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

                public:
                    nonvirtual Rep_& operator= (const Rep_&) = delete;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                private:
                    const EQUALS_COMPARER fEqualsComparer_;

                    // Iterable<T>::_IRep overrides
                public:
                    virtual _IterableRepSharedPtr Clone (IteratorOwnerID forIterableEnvelope) const override
                    {
                        // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                        return Iterable<CountedValue<T>>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                    }
                    virtual size_t GetLength () const override
                    {
                        return (fData_.GetLength ());
                    }
                    virtual bool IsEmpty () const override
                    {
                        return fData_.IsEmpty ();
                    }
                    virtual Iterator<CountedValue<T>> MakeIterator (IteratorOwnerID suggestedOwner) const override
                    {
                        Rep_* NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
                        return Iterator<CountedValue<T>> (Iterator<CountedValue<T>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_));
                    }
                    virtual void Apply (_APPLY_ARGTYPE doToElement) const override
                    {
                        // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
                        // use iterator (which currently implies lots of locks) with this->_Apply ()
                        fData_.Apply (doToElement);
                    }
                    virtual Iterator<CountedValue<T>> FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        using RESULT_TYPE     = Iterator<CountedValue<T>>;
                        using SHARED_REP_TYPE = Traversal::IteratorBase::SharedPtrImplementationTemplate<IteratorRep_>;
                        auto iLink            = fData_.FindFirstThat (doToElement);
                        if (iLink == nullptr) {
                            return RESULT_TYPE::GetEmptyIterator ();
                        }
                        Rep_*           NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
                        SHARED_REP_TYPE resultRep      = Iterator<CountedValue<T>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                        resultRep->fIterator.SetCurrentLink (iLink);
                        // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
                        return RESULT_TYPE (resultRep);
                    }

                    // MultiSet<T, TRAITS>::_IRep overrides
                public:
                    virtual function<bool(T, T)> GetEqualsComparer () const override
                    {
                        return fEqualsComparer_;
                    }
                    virtual _MultiSetRepSharedPtr CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
                    {
                        if (fData_.HasActiveIterators ()) {
                            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                            auto r = Iterable<CountedValue<T>>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                            r->fData_.RemoveAll ();
                            return r;
                        }
                        else {
                            return Iterable<CountedValue<T>>::template MakeSharedPtr<Rep_> (fEqualsComparer_);
                        }
                    }
                    virtual bool Equals (const typename MultiSet<T, TRAITS>::_IRep& rhs) const override
                    {
                        return this->_Equals_Reference_Implementation (rhs);
                    }
                    virtual bool Contains (ArgByValueType<T> item) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        CountedValue<T>                                                 c = item;
                        for (typename NonPatchingDataStructureImplType_::ForwardIterator it (&fData_); it.More (&c, true);) {
                            if (fEqualsComparer_ (c.fValue, item)) {
                                Assert (c.fCount != 0);
                                return (true);
                            }
                        }
                        return false;
                    }
                    virtual void Add (ArgByValueType<T> item, CounterType count) override
                    {
                        using Traversal::kUnknownIteratorOwnerID;
                        if (count != 0) {
                            CountedValue<T>                                                current (item);
                            std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                            for (typename DataStructureImplType_::ForwardIterator it (kUnknownIteratorOwnerID, &fData_); it.More (&current, true);) {
                                if (fEqualsComparer_ (current.fValue, item)) {
                                    current.fCount += count;
                                    fData_.SetAt (it, current);
                                    return;
                                }
                            }
                            fData_.Prepend (CountedValue<T> (item, count));
                        }
                    }
                    virtual void Remove (ArgByValueType<T> item, CounterType count) override
                    {
                        using Traversal::kUnknownIteratorOwnerID;
                        if (count != 0) {
                            CountedValue<T>                                                current (item);
                            std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                            for (typename DataStructureImplType_::ForwardIterator it (kUnknownIteratorOwnerID, &fData_); it.More (&current, true);) {
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
                    virtual void Remove (const Iterator<CountedValue<T>>& i) override
                    {
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        const typename Iterator<CountedValue<T>>::IRep&                ir = i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto& mir = dynamic_cast<const IteratorRep_&> (ir);
                        fData_.RemoveAt (mir.fIterator);
                    }
                    virtual void UpdateCount (const Iterator<CountedValue<T>>& i, CounterType newCount) override
                    {
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        const typename Iterator<CountedValue<T>>::IRep&                ir = i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto& mir = dynamic_cast<const IteratorRep_&> (ir);
                        if (newCount == 0) {
                            fData_.RemoveAt (mir.fIterator);
                        }
                        else {
                            CountedValue<T> c = mir.fIterator.Current ();
                            c.fCount          = newCount;
                            fData_.SetAt (mir.fIterator, c);
                        }
                    }
                    virtual CounterType OccurrencesOf (ArgByValueType<T> item) const override
                    {
                        CountedValue<T>                                                 c = item;
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        for (typename NonPatchingDataStructureImplType_::ForwardIterator it (&fData_); it.More (&c, true);) {
                            if (fEqualsComparer_ (c.fValue, item)) {
                                Ensure (c.fCount != 0);
                                return c.fCount;
                            }
                        }
                        return 0;
                    }
                    virtual Iterable<T> Elements (const typename MultiSet<T, TRAITS>::_MultiSetRepSharedPtr& rep) const override
                    {
                        return this->_Elements_Reference_Implementation (rep);
                    }
                    virtual Iterable<T> UniqueElements (const typename MultiSet<T, TRAITS>::_MultiSetRepSharedPtr& rep) const override
                    {
                        return this->_UniqueElements_Reference_Implementation (rep);
                    }
#if qDebug
                    virtual void AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
                    }
#endif

                private:
                    using NonPatchingDataStructureImplType_ = DataStructures::LinkedList<CountedValue<T>>;
                    using DataStructureImplType_            = Private::PatchingDataStructures::LinkedList<CountedValue<T>>;
                    using IteratorRep_                      = typename Private::IteratorImplHelper_<CountedValue<T>, DataStructureImplType_>;

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
                    : MultiSet_LinkedList (std::equal_to<T>{})
                {
                    AssertRepValidType_ ();
                }
                template <typename T, typename TRAITS>
                template <typename EQUALS_COMPARER, typename ENABLE_IF_IS_COMPARER>
                inline MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList (const EQUALS_COMPARER& equalsComparer, ENABLE_IF_IS_COMPARER*)
                    : inherited (inherited::template MakeSharedPtr<Rep_<EQUALS_COMPARER>> (equalsComparer))
                {
                    AssertRepValidType_ ();
                }
                template <typename T, typename TRAITS>
                template <typename COPY_FROM_ITERATOR>
                inline MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end)
                    : MultiSet_LinkedList ()
                {
                    this->AddAll (start, end);
                    AssertRepValidType_ ();
                }
                template <typename T, typename TRAITS>
                inline MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList (const MultiSet<T, TRAITS>& src)
                    : MultiSet_LinkedList ()
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
                    : MultiSet_LinkedList ()
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template <typename T, typename TRAITS>
                MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList (const initializer_list<CountedValue<T>>& src)
                    : MultiSet_LinkedList ()
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
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_MultiSet_LinkedList_inl_ */
