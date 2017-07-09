/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedCollection_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedCollection_LinkedList_inl_

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
                 *** SortedCollection_LinkedList<T, TRAITS>::Rep_ ***
                 ********************************************************************************
                 */
                template <typename T, typename TRAITS>
                class SortedCollection_LinkedList<T, TRAITS>::Rep_ : public SortedCollection<T, TRAITS>::_IRep {
                private:
                    using inherited = typename SortedCollection<T, TRAITS>::_IRep;

                public:
                    using _IterableRepSharedPtr   = typename Iterable<T>::_IterableRepSharedPtr;
                    using _CollectionRepSharedPtr = typename Collection<T>::_CollectionRepSharedPtr;
                    using _APPLY_ARGTYPE          = typename inherited::_APPLY_ARGTYPE;
                    using _APPLYUNTIL_ARGTYPE     = typename inherited::_APPLYUNTIL_ARGTYPE;

                public:
                    Rep_ ()                 = default;
                    Rep_ (const Rep_& from) = delete;
                    Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope)
                        : inherited ()
                        , fData_ (&from->fData_, forIterableEnvelope)
                    {
                        RequireNotNull (from);
                    }

                public:
                    nonvirtual Rep_& operator= (const Rep_&) = delete;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual _IterableRepSharedPtr Clone (IteratorOwnerID forIterableEnvelope) const override
                    {
                        // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                        return Iterable<T>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                    }
                    virtual Iterator<T> MakeIterator (IteratorOwnerID suggestedOwner) const override
                    {
                        Rep_* NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
                        return Iterator<T> (Iterator<T>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_));
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
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        using RESULT_TYPE     = Iterator<T>;
                        using SHARED_REP_TYPE = Traversal::IteratorBase::SharedPtrImplementationTemplate<IteratorRep_>;
                        auto iLink            = fData_.FindFirstThat (doToElement);
                        if (iLink == nullptr) {
                            return RESULT_TYPE::GetEmptyIterator ();
                        }
                        Rep_*           NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
                        SHARED_REP_TYPE resultRep      = Iterator<T>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                        resultRep->fIterator.SetCurrentLink (iLink);
                        // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
                        return RESULT_TYPE (resultRep);
                    }

                    // Collection<T>::_IRep overrides
                public:
                    virtual _CollectionRepSharedPtr CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
                    {
                        if (fData_.HasActiveIterators ()) {
                            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                            auto r = Iterable<T>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                            r->fData_.RemoveAll ();
                            return r;
                        }
                        else {
                            return Iterable<T>::template MakeSharedPtr<Rep_> ();
                        }
                    }
                    virtual void Add (ArgByValueType<T> item) override
                    {
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        AddWithoutLocks_ (item);
                    }
                    virtual void Update (const Iterator<T>& i, ArgByValueType<T> newValue) override
                    {
                        const typename Iterator<T>::IRep& ir = i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto&                                                          mir = dynamic_cast<const IteratorRep_&> (ir);
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        // equals might examine a subset of the object and we still want to update the whole object, but
                        // if its not already equal, the sort order could have changed so we must simulate with a remove/add
                        if (TRAITS::EqualsCompareFunctionType::Equals (mir.fIterator.Current (), newValue)) {
                            fData_.SetAt (mir.fIterator, newValue);
                        }
                        else {
                            fData_.RemoveAt (mir.fIterator);
                            AddWithoutLocks_ (newValue);
                        }
                    }
                    virtual void Remove (const Iterator<T>& i) override
                    {
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        const typename Iterator<T>::IRep&                              ir = i.GetRep ();
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

                    // SortedCollection<T>::_IRep overrides
                public:
                    virtual bool Equals (const typename Collection<T>::_IRep& rhs) const override
                    {
                        AssertNotImplemented ();
                        return false;
                        //return this->_Equals_Reference_Implementation (rhs);
                    }
                    virtual bool Contains (T item) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        return fData_.Lookup (item) != nullptr;
                    }
                    virtual void Remove (T item) override
                    {
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        fData_.Remove (item);
                    }

                private:
                    nonvirtual void AddWithoutLocks_ (T item)
                    {
                        using Traversal::kUnknownIteratorOwnerID;
                        typename Rep_::DataStructureImplType_::ForwardIterator it (kUnknownIteratorOwnerID, &fData_);
                        // skip the smaller items
                        while (it.More (nullptr, true) and TRAITS::WellOrderCompareFunctionType::Compare (it.Current (), item) < 0) {
                        }
                        // at this point - we are pointing at the first link >= item, so insert before it
                        fData_.AddBefore (it, item);
                    }

                private:
                    using DataStructureImplType_ = Private::PatchingDataStructures::LinkedList<T, DataStructures::LinkedList_DefaultTraits<T, typename TRAITS::EqualsCompareFunctionType>>;
                    using IteratorRep_           = typename Private::IteratorImplHelper_<T, DataStructureImplType_>;

                private:
                    DataStructureImplType_ fData_;
                };

                /*
                 ********************************************************************************
                 ***************** SortedCollection_LinkedList<T, TRAITS> ***********************
                 ********************************************************************************
                 */
                template <typename T, typename TRAITS>
                inline SortedCollection_LinkedList<T, TRAITS>::SortedCollection_LinkedList ()
                    : inherited (inherited::template MakeSharedPtr<Rep_> ())
                {
                    AssertRepValidType_ ();
                }
                template <typename T, typename TRAITS>
                inline SortedCollection_LinkedList<T, TRAITS>::SortedCollection_LinkedList (const T* start, const T* end)
                    : SortedCollection_LinkedList ()
                {
                    Require ((start == end) or (start != nullptr and end != nullptr));
                    this->AddAll (start, end);
                    AssertRepValidType_ ();
                }
                template <typename T, typename TRAITS>
                SortedCollection_LinkedList<T, TRAITS>::SortedCollection_LinkedList (const SortedCollection<T, TRAITS>& src)
                    : SortedCollection_LinkedList ()
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template <typename T, typename TRAITS>
                SortedCollection_LinkedList<T, TRAITS>::SortedCollection_LinkedList (const SortedCollection_LinkedList<T, TRAITS>& src)
                    : inherited (src)
                {
                    AssertRepValidType_ ();
                }
                template <typename T, typename TRAITS>
                inline void SortedCollection_LinkedList<T, TRAITS>::AssertRepValidType_ () const
                {
#if qDebug
                    typename inherited::template _SafeReadRepAccessor<Rep_> tmp{this}; // for side-effect of AssertMember
#endif
                }
            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_SortedCollection_LinkedList_inl_ */
