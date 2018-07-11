/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Collection_Array_inl_
#define _Stroika_Foundation_Containers_Concrete_Collection_Array_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Memory/BlockAllocated.h"

#include "../Private/IteratorImplHelper.h"
#include "../Private/PatchingDataStructures/Array.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Concrete {

                /*
                 ********************************************************************************
                 ********************** Collection_Array<T,TRAITS>::Rep_ ************************
                 ********************************************************************************
                 */
                template <typename T>
                class Collection_Array<T>::Rep_ : public Collection<T>::_IRep, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
                private:
                    using inherited = typename Collection<T>::_IRep;

                public:
                    using _IterableRepSharedPtr   = typename Iterable<T>::_IterableRepSharedPtr;
                    using _CollectionRepSharedPtr = typename Collection<T>::_CollectionRepSharedPtr;
                    using _APPLY_ARGTYPE          = typename inherited::_APPLY_ARGTYPE;
                    using _APPLYUNTIL_ARGTYPE     = typename inherited::_APPLYUNTIL_ARGTYPE;

                public:
                    Rep_ ()                 = default;
                    Rep_ (const Rep_& from) = delete;
                    Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope)
                        : fData_ (&from->fData_, forIterableEnvelope)
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
                        return fData_.GetLength () == 0;
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
                        using SHARED_REP_TYPE = Traversal::IteratorBase::SharedPtrImplementationTemplate<IteratorRep_>;
                        size_t i              = fData_.FindFirstThat (doToElement);
                        if (i == fData_.GetLength ()) {
                            return RESULT_TYPE::GetEmptyIterator ();
                        }
                        Rep_*           NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
                        SHARED_REP_TYPE resultRep      = Iterator<T>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                        resultRep->fIterator.SetIndex (i);
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
                        shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        // Appending is fastest
                        fData_.InsertAt (fData_.GetLength (), item);
                    }
                    virtual void Update (const Iterator<T>& i, ArgByValueType<T> newValue) override
                    {
                        const typename Iterator<T>::IRep& ir = i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto&                                                      mir = dynamic_cast<const IteratorRep_&> (ir);
                        shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        fData_.SetAt (mir.fIterator, newValue);
                    }
                    virtual void Remove (const Iterator<T>& i) override
                    {
                        const typename Iterator<T>::IRep& ir = i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto&                                                      mir = dynamic_cast<const IteratorRep_&> (ir);
                        shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
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
                    using DataStructureImplType_ = Private::PatchingDataStructures::Array<T>;

                private:
                    using IteratorRep_ = typename Private::IteratorImplHelper_<T, DataStructureImplType_>;

                private:
                    DataStructureImplType_ fData_;
                };

                /*
                 ********************************************************************************
                 ************************* Collection_Array<T,TRAITS> ***************************
                 ********************************************************************************
                 */
                template <typename T>
                inline Collection_Array<T>::Collection_Array ()
                    : inherited (inherited::template MakeSharedPtr<Rep_> ())
                {
                    AssertRepValidType_ ();
                }
                template <typename T>
                inline Collection_Array<T>::Collection_Array (const Collection<T>& src)
                    : Collection_Array ()
                {
                    SetCapacity (src.GetLength ());
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template <typename T>
                inline Collection_Array<T>::Collection_Array (const T* start, const T* end)
                    : Collection_Array ()
                {
                    Require ((start == end) or (start != nullptr and end != nullptr));
                    AssertRepValidType_ ();
                    if (start != end) {
                        SetCapacity (end - start);
                        this->AddAll (start, end);
                    }
                    AssertRepValidType_ ();
                }
                template <typename T>
                inline void Collection_Array<T>::Compact ()
                {
                    using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<Rep_>;
                    _SafeReadWriteRepAccessor{this}._GetWriteableRep ().fData_.Compact ();
                }
                template <typename T>
                inline size_t Collection_Array<T>::GetCapacity () const
                {
                    using _SafeReadRepAccessor = typename inherited::template _SafeReadRepAccessor<Rep_>;
                    return _SafeReadRepAccessor{this}._ConstGetRep ().fData_.GetCapacity ();
                }
                template <typename T>
                inline void Collection_Array<T>::SetCapacity (size_t slotsAlloced)
                {
                    using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<Rep_>;
                    _SafeReadWriteRepAccessor{this}._GetWriteableRep ().fData_.SetCapacity (slotsAlloced);
                }
                template <typename T>
                inline size_t Collection_Array<T>::capacity () const
                {
                    return GetCapacity ();
                }
                template <typename T>
                inline void Collection_Array<T>::reserve (size_t slotsAlloced)
                {
                    SetCapacity (slotsAlloced);
                }
                template <typename T>
                inline void Collection_Array<T>::AssertRepValidType_ () const
                {
#if qDebug
                    typename inherited::template _SafeReadRepAccessor<Rep_> tmp{this}; // for side-effect of AssertMemeber
#endif
                }
            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Collection_Array_inl_ */
