/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Bijection_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Bijection_LinkedList_inl_

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
                 ********** Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>::IImplRepBase_*********
                 ********************************************************************************
                 */
                template <typename DOMAIN_TYPE, typename RANGE_TYPE>
                class Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>::IImplRepBase_ : public Bijection<DOMAIN_TYPE, RANGE_TYPE>::_IRep {
                };

                /*
                 ********************************************************************************
                 *********** Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>::Rep_*****************
                 ********************************************************************************
                 */
                template <typename DOMAIN_TYPE, typename RANGE_TYPE>
                template <typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER>
                class Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>::Rep_ : public IImplRepBase_ {
                private:
                    using inherited = IImplRepBase_;

                public:
                    using _IterableRepSharedPtr  = typename Iterable<pair<DOMAIN_TYPE, RANGE_TYPE>>::_IterableRepSharedPtr;
                    using _BijectionRepSharedPtr = typename inherited::_BijectionRepSharedPtr;
                    using _APPLY_ARGTYPE         = typename inherited::_APPLY_ARGTYPE;
                    using _APPLYUNTIL_ARGTYPE    = typename inherited::_APPLYUNTIL_ARGTYPE;

                public:
                    Rep_ (const DOMAIN_EQUALS_COMPARER& domainEqualsComparer, const RANGE_EQUALS_COMPARER& rangeEqualsComparer)
                        : fDomainEqualsComparer_ (domainEqualsComparer)
                        , fRangeEqualsComparer_ (rangeEqualsComparer)
                    {
                    }
                    Rep_ (const Rep_& from) = delete;
                    Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope)
                        : inherited ()
                        , fDomainEqualsComparer_ (from->fDomainEqualsComparer_)
                        , fRangeEqualsComparer_ (from->fRangeEqualsComparer_)
                        , fData_ (&from->fData_, forIterableEnvelope)
                    {
                        RequireNotNull (from);
                    }

                public:
                    nonvirtual Rep_& operator= (const Rep_&) = delete;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                private:
                    const DOMAIN_EQUALS_COMPARER fDomainEqualsComparer_;
                    const RANGE_EQUALS_COMPARER  fRangeEqualsComparer_;

                    // Iterable<T>::_IRep overrides
                public:
                    virtual _IterableRepSharedPtr Clone (IteratorOwnerID forIterableEnvelope) const override
                    {
                        // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                        return Iterable<pair<DOMAIN_TYPE, RANGE_TYPE>>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                    }
                    virtual Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>> MakeIterator (IteratorOwnerID suggestedOwner) const override
                    {
                        Rep_* NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
                        return Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>> (Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_));
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
                    virtual Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>> FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        using RESULT_TYPE     = Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>;
                        using SHARED_REP_TYPE = Traversal::IteratorBase::SharedPtrImplementationTemplate<IteratorRep_>;
                        auto iLink            = fData_.FindFirstThat (doToElement);
                        if (iLink == nullptr) {
                            return RESULT_TYPE::GetEmptyIterator ();
                        }
                        Rep_*           NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
                        SHARED_REP_TYPE resultRep      = Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                        resultRep->fIterator.SetCurrentLink (iLink);
                        // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
                        return RESULT_TYPE (resultRep);
                    }

                    // Bijection<DOMAIN_TYPE, RANGE_TYPE::BijectionTraitsType>::_IRep overrides
                public:
                    virtual _BijectionRepSharedPtr CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
                    {
                        if (fData_.HasActiveIterators ()) {
                            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                            auto r = Iterable<pair<DOMAIN_TYPE, RANGE_TYPE>>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                            r->fData_.RemoveAll ();
                            return r;
                        }
                        else {
                            return Iterable<pair<DOMAIN_TYPE, RANGE_TYPE>>::template MakeSharedPtr<Rep_> (fDomainEqualsComparer_, fRangeEqualsComparer_);
                        }
                    }
                    virtual bool Equals (const typename Bijection<DomainType, RangeType>::_IRep& rhs) const override
                    {
                        return this->_Equals_Reference_Implementation (rhs);
                    }
                    virtual function<bool(DOMAIN_TYPE, DOMAIN_TYPE)> PeekDomainEqualsComparer () const override
                    {
                        return fDomainEqualsComparer_;
                    }
                    virtual function<bool(RANGE_TYPE, RANGE_TYPE)> PeekRangeEqualsComparer () const override
                    {
                        return fRangeEqualsComparer_;
                    }
                    virtual Iterable<DOMAIN_TYPE> Preimage () const override
                    {
                        return this->_PreImage_Reference_Implementation ();
                    }
                    virtual Iterable<RANGE_TYPE> Image () const override
                    {
                        return this->_Image_Reference_Implementation ();
                    }
                    virtual bool Lookup (ArgByValueType<DOMAIN_TYPE> key, Memory::Optional<RANGE_TYPE>* item) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        for (typename DataStructures::LinkedList<pair<DOMAIN_TYPE, RANGE_TYPE>>::ForwardIterator it (&fData_); it.More (nullptr, true);) {
                            if (fDomainEqualsComparer_ (it.Current ().first, key)) {
                                if (item != nullptr) {
                                    *item = it.Current ().second;
                                }
                                return true;
                            }
                        }
                        if (item != nullptr) {
                            item->clear ();
                        }
                        return false;
                    }
                    virtual bool InverseLookup (ArgByValueType<RANGE_TYPE> key, Memory::Optional<DOMAIN_TYPE>* item) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        for (typename DataStructures::LinkedList<pair<DOMAIN_TYPE, RANGE_TYPE>>::ForwardIterator it (&fData_); it.More (nullptr, true);) {
                            if (fRangeEqualsComparer_ (it.Current ().second, key)) {
                                if (item != nullptr) {
                                    *item = it.Current ().first;
                                }
                                return true;
                            }
                        }
                        if (item != nullptr) {
                            item->clear ();
                        }
                        return false;
                    }
                    virtual void Add (ArgByValueType<DOMAIN_TYPE> key, ArgByValueType<RANGE_TYPE> newElt) override
                    {
                        using Traversal::kUnknownIteratorOwnerID;
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        for (typename DataStructureImplType_::ForwardIterator it (kUnknownIteratorOwnerID, &fData_); it.More (nullptr, true);) {
                            if (fDomainEqualsComparer_ (it.Current ().first, key)) {
                                fData_.SetAt (it, pair<DOMAIN_TYPE, RANGE_TYPE> (key, newElt));
                                return;
                            }
                        }
                        fData_.Append (pair<DOMAIN_TYPE, RANGE_TYPE> (key, newElt));
                    }
                    virtual void RemoveDomainElement (ArgByValueType<DOMAIN_TYPE> d) override
                    {
                        using Traversal::kUnknownIteratorOwnerID;
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        for (typename DataStructureImplType_::ForwardIterator it (kUnknownIteratorOwnerID, &fData_); it.More (nullptr, true);) {
                            if (fDomainEqualsComparer_ (it.Current ().first, d)) {
                                fData_.RemoveAt (it);
                                return;
                            }
                        }
                    }
                    virtual void RemoveRangeElement (ArgByValueType<RANGE_TYPE> r) override
                    {
                        using Traversal::kUnknownIteratorOwnerID;
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        for (typename DataStructureImplType_::ForwardIterator it (kUnknownIteratorOwnerID, &fData_); it.More (nullptr, true);) {
                            if (fRangeEqualsComparer_ (it.Current ().second, r)) {
                                fData_.RemoveAt (it);
                                return;
                            }
                        }
                    }
                    virtual void Remove (const Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>& i) override
                    {
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        const typename Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>::IRep&  ir = i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto& mir = dynamic_cast<const IteratorRep_&> (ir);
                        fData_.RemoveAt (mir.fIterator);
                    }
#if qDebug
                    virtual void AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
                    }
#endif
                private:
                    using DataStructureImplType_ = Private::PatchingDataStructures::LinkedList<pair<DOMAIN_TYPE, RANGE_TYPE>>;
                    using IteratorRep_           = Private::IteratorImplHelper_<pair<DOMAIN_TYPE, RANGE_TYPE>, DataStructureImplType_>;

                private:
                    DataStructureImplType_ fData_;
                };

                /*
                 ********************************************************************************
                 **************** Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE> *****************
                 ********************************************************************************
                 */
                template <typename DOMAIN_TYPE, typename RANGE_TYPE>
                inline Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>::Bijection_LinkedList ()
                    : Bijection_LinkedList (std::equal_to<DOMAIN_TYPE>{}, std::equal_to<RANGE_TYPE>{})
                {
                    AssertRepValidType_ ();
                }
                template <typename DOMAIN_TYPE, typename RANGE_TYPE>
                template <typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER, typename ENABLE_IF_IS_COMPARER>
                inline Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>::Bijection_LinkedList (const DOMAIN_EQUALS_COMPARER& domainEqualsComparer, const RANGE_EQUALS_COMPARER& rangeEqualsComparer, ENABLE_IF_IS_COMPARER*)
                    : inherited (inherited::template MakeSharedPtr<Rep_<DOMAIN_EQUALS_COMPARER, RANGE_EQUALS_COMPARER>> (domainEqualsComparer, rangeEqualsComparer))
                {
                }
                template <typename DOMAIN_TYPE, typename RANGE_TYPE>
                template <typename CONTAINER_OF_PAIR_KEY_T>
                inline Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>::Bijection_LinkedList (const CONTAINER_OF_PAIR_KEY_T& src)
                    : Bijection_LinkedList ()
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template <typename DOMAIN_TYPE, typename RANGE_TYPE>
                template <typename COPY_FROM_ITERATOR_KVP_T, typename ENABLE_IF>
                Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>::Bijection_LinkedList (COPY_FROM_ITERATOR_KVP_T start, COPY_FROM_ITERATOR_KVP_T end)
                    : Bijection_LinkedList ()
                {
                    this->AddAll (start, end);
                    AssertRepValidType_ ();
                }
                template <typename DOMAIN_TYPE, typename RANGE_TYPE>
                inline void Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>::AssertRepValidType_ () const
                {
#if qDebug
                    typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMemeber
#endif
                }
            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Bijection_LinkedList_inl_ */
