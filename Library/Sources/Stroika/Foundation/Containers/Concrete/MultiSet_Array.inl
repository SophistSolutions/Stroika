/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_MultiSet_Array_inl_
#define _Stroika_Foundation_Containers_MultiSet_Array_inl_ 1

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

                using Traversal::IteratorOwnerID;

                /*
                 ********************************************************************************
                 ********************* MultiSet_Array<T, TRAITS>::IImplRepBase_ *****************
                 ********************************************************************************
                 */
                template <typename T, typename TRAITS>
                class MultiSet_Array<T, TRAITS>::IImplRepBase_ : public MultiSet<T, TRAITS>::_IRep {
                public:
                    virtual size_t GetCapacity () const              = 0;
                    virtual void   SetCapacity (size_t slotsAlloced) = 0;
                    virtual void   Compact ()                        = 0;
                };

                /*
                 ********************************************************************************
                 ************************** MultiSet_Array<T, TRAITS>::Rep_ *********************
                 ********************************************************************************
                 */
                template <typename T, typename TRAITS>
                template <typename EQUALS_COMPARER>
                class MultiSet_Array<T, TRAITS>::Rep_ : public IImplRepBase_ {
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
                    virtual function<bool(T, T)> GetEqualsComparer () const override
                    {
                        return fEqualsComparer_;
                    }
                    virtual _IterableRepSharedPtr Clone (IteratorOwnerID forIterableEnvelope) const override
                    {
                        // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                        return Iterable<CountedValue<T>>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                    }
                    virtual size_t GetLength () const override
                    {
                        return fData_.GetLength ();
                    }
                    virtual bool IsEmpty () const override
                    {
                        return fData_.GetLength () == 0;
                    }
                    virtual Iterator<CountedValue<T>> MakeIterator (IteratorOwnerID suggestedOwner) const override
                    {
                        // const cast cuz this mutator won't really be used to change anything - except stuff like
                        // link list of owned iterators
                        Rep_* NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
                        return Iterator<CountedValue<T>> (Iterator<CountedValue<T>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_));
                    }
                    virtual void Apply (_APPLY_ARGTYPE doToElement) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
                        // use iterator (which currently implies lots of locks) with this->_Apply ()
                        fData_.Apply (doToElement);
                    }
                    virtual Iterator<CountedValue<T>> FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        using RESULT_TYPE     = Iterator<CountedValue<T>>;
                        using SHARED_REP_TYPE = Traversal::IteratorBase::SharedPtrImplementationTemplate<IteratorRep_>;
                        size_t i              = fData_.FindFirstThat (doToElement);
                        if (i == fData_.GetLength ()) {
                            return RESULT_TYPE::GetEmptyIterator ();
                        }
                        Rep_*           NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
                        SHARED_REP_TYPE resultRep      = Iterator<CountedValue<T>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                        resultRep->fIterator.SetIndex (i);
                        // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
                        return RESULT_TYPE (resultRep);
                    }

                    // MultiSet<T, TRAITS>::_IRep overrides
                public:
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
                        CountedValue<T>                                                 tmp (item);
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        return (bool(Find_ (tmp) != kNotFound_));
                    }
                    virtual void Add (ArgByValueType<T> item, CounterType count) override
                    {
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        CountedValue<T>                                                tmp (item, count);
                        size_t                                                         index = Find_ (tmp);
                        if (index == kNotFound_) {
                            fData_.InsertAt (fData_.GetLength (), tmp);
                        }
                        else {
                            tmp.fCount += count;
                            fData_.SetAt (index, tmp);
                        }
                    }
                    virtual void Remove (ArgByValueType<T> item, CounterType count) override
                    {
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        CountedValue<T>                                                tmp (item);
                        size_t                                                         index = Find_ (tmp);
                        if (index != kNotFound_) {
                            Assert (index < fData_.GetLength ());
                            Assert (tmp.fCount >= count);
                            tmp.fCount -= count;
                            if (tmp.fCount == 0) {
                                fData_.RemoveAt (index);
                            }
                            else {
                                fData_.SetAt (index, tmp);
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
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        CountedValue<T>                                                 tmp (item);
                        size_t                                                          index = Find_ (tmp);
                        if (index == kNotFound_) {
                            return 0;
                        }
                        Assert (index >= 0);
                        Assert (index < fData_.GetLength ());
                        return fData_[index].fCount;
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

                    // MultiSet_Array<T, TRAITS>::_IRep overrides
                public:
                    virtual size_t GetCapacity () const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        return fData_.GetCapacity ();
                    }
                    virtual void SetCapacity (size_t slotsAlloced) override
                    {
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        fData_.SetCapacity (slotsAlloced);
                    }
                    virtual void Compact () override
                    {
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        fData_.Compact ();
                    }

                private:
                    using DataStructureImplType_ = Private::PatchingDataStructures::Array<CountedValue<T>>;
                    using IteratorRep_           = typename Private::IteratorImplHelper_<CountedValue<T>, DataStructureImplType_>;

                private:
                    DataStructureImplType_ fData_;

                private:
                    static constexpr size_t kNotFound_ = (size_t)-1;

                private:
                    nonvirtual size_t Find_ (CountedValue<T>& item) const
                    {
                        // this code assumes locking done by callers
                        size_t length = fData_.GetLength ();
                        for (size_t i = 0; i < length; i++) {
                            if (fEqualsComparer_ (fData_.GetAt (i).fValue, item.fValue)) {
                                item = fData_.GetAt (i);
                                return i;
                            }
                        }
                        return kNotFound_;
                    }

                private:
                    friend class MultiSet_Array<T, TRAITS>;
                };

                /*
                 ********************************************************************************
                 ***************************** MultiSet_Array<T, TRAITS> ************************
                 ********************************************************************************
                 */
                template <typename T, typename TRAITS>
                inline MultiSet_Array<T, TRAITS>::MultiSet_Array ()
                    : inherited (std::equal_to<T>{})
                {
                    AssertRepValidType_ ();
                }
                template <typename T, typename TRAITS>
                template <typename EQUALS_COMPARER, typename ENABLE_IF_IS_COMPARER>
                inline MultiSet_Array<T, TRAITS>::MultiSet_Array (const EQUALS_COMPARER& equalsComparer, ENABLE_IF_IS_COMPARER*)
                    : inherited (inherited::template MakeSharedPtr<Rep_<EQUALS_COMPARER>> (equalsComparer))
                {
                    static_assert (Common::IsEqualsComparer<EQUALS_COMPARER> (), "Equals comparer required with MultiSet_Array");
                    AssertRepValidType_ ();
                }
                template <typename T, typename TRAITS>
                template <typename COPY_FROM_ITERATOR>
                MultiSet_Array<T, TRAITS>::MultiSet_Array (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end)
                    : MultiSet_Array ()
                {
                    SetCapacity (end - start);
                    this->AddAll (start, end);
                    AssertRepValidType_ ();
                }
                template <typename T, typename TRAITS>
                inline MultiSet_Array<T, TRAITS>::MultiSet_Array (const MultiSet<T, TRAITS>& src)
                    : MultiSet_Array ()
                {
                    SetCapacity (src.GetLength ());
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template <typename T, typename TRAITS>
                MultiSet_Array<T, TRAITS>::MultiSet_Array (const initializer_list<T>& src)
                    : MultiSet_Array ()
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template <typename T, typename TRAITS>
                MultiSet_Array<T, TRAITS>::MultiSet_Array (const initializer_list<CountedValue<T>>& src)
                    : MultiSet_Array ()
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template <typename T, typename TRAITS>
                inline size_t MultiSet_Array<T, TRAITS>::GetCapacity () const
                {
                    using _SafeReadRepAccessor = typename inherited::template _SafeReadRepAccessor<IImplRepBase_>;
                    _SafeReadRepAccessor accessor{this};
                    return accessor._ConstGetRep ().GetCapacity ();
                }
                template <typename T, typename TRAITS>
                inline void MultiSet_Array<T, TRAITS>::SetCapacity (size_t slotsAlloced)
                {
                    using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<IImplRepBase_>;
                    _SafeReadWriteRepAccessor accessor{this};
                    accessor._GetWriteableRep ().SetCapacity (slotsAlloced);
                }
                template <typename T, typename TRAITS>
                inline void MultiSet_Array<T, TRAITS>::Compact ()
                {
                    using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<IImplRepBase_>;
                    _SafeReadWriteRepAccessor accessor{this};
                    accessor._GetWriteableRep ().Compact ();
                }
                template <typename T, typename TRAITS>
                inline size_t MultiSet_Array<T, TRAITS>::capacity () const
                {
                    return GetCapacity ();
                }
                template <typename T, typename TRAITS>
                inline void MultiSet_Array<T, TRAITS>::reserve (size_t slotsAlloced)
                {
                    SetCapacity (slotsAlloced);
                }
                template <typename T, typename TRAITS>
                inline void MultiSet_Array<T, TRAITS>::AssertRepValidType_ () const
                {
#if qDebug
                    typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMemeber
#endif
                }
            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_MultiSet_Array_inl_ */
