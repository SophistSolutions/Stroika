/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedMultiSet_stdmap_inl_
#define _Stroika_Foundation_Containers_SortedMultiSet_stdmap_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <map>

#include "../../Memory/BlockAllocated.h"

#include "../STL/Compare.h"

#include "../Private/IteratorImplHelper.h"
#include "../Private/PatchingDataStructures/STLContainerWrapper.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Concrete {

                /**
                 */
                template <typename T, typename TRAITS>
                class SortedMultiSet_stdmap<T, TRAITS>::IImplRepBase_ : public SortedMultiSet<T, TRAITS>::_IRep {
                };

                /**
                 */
                template <typename T, typename TRAITS>
                template <typename INORDER_COMPARER>
                class SortedMultiSet_stdmap<T, TRAITS>::Rep_ : public IImplRepBase_ {
                private:
                    using inherited = IImplRepBase_;

                public:
                    using _IterableRepSharedPtr = typename Iterable<CountedValue<T>>::_IterableRepSharedPtr;
                    using _MultiSetRepSharedPtr = typename inherited::_MultiSetRepSharedPtr;
                    using _APPLY_ARGTYPE        = typename inherited::_APPLY_ARGTYPE;
                    using _APPLYUNTIL_ARGTYPE   = typename inherited::_APPLYUNTIL_ARGTYPE;
                    using CounterType           = typename inherited::CounterType;
                    using EqualityComparerType  = typename MultiSet<T, TRAITS>::EqualityComparerType;
                    using InOrderComparerType   = typename SortedMultiSet<T, TRAITS>::InOrderComparerType;

                public:
                    Rep_ (const INORDER_COMPARER& inorderComparer)
                        : fData_ (inorderComparer)
                    {
                    }
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
                        return Iterable<CountedValue<T>>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                    }
                    virtual size_t GetLength () const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        return fData_.size ();
                    }
                    virtual bool IsEmpty () const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        return fData_.empty ();
                    }
                    virtual Iterator<CountedValue<T>> MakeIterator (IteratorOwnerID suggestedOwner) const override
                    {
                        Rep_* NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
                        return Iterator<CountedValue<T>> (Iterator<CountedValue<T>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_));
                    }
                    virtual void Apply (_APPLY_ARGTYPE doToElement) const override
                    {
                        this->_Apply (doToElement);
                    }
                    virtual Iterator<CountedValue<T>> FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        return this->_FindFirstThat (doToElement, suggestedOwner);
                    }

                    // MultiSet<T, TRAITS>::_IRep overrides
                public:
                    virtual EqualityComparerType GetEqualsComparer () const override
                    {
                        return EqualityComparerType{Common::mkEqualsComparerAdapter (fData_.key_comp ())};
                    }
                    virtual _MultiSetRepSharedPtr CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
                    {
                        if (fData_.HasActiveIterators ()) {
                            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                            auto r = Iterable<CountedValue<T>>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                            r->fData_.clear_WithPatching ();
                            return r;
                        }
                        else {
                            return Iterable<CountedValue<T>>::template MakeSharedPtr<Rep_> (fData_.key_comp ());
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
                        return fData_.find (item) != fData_.end ();
                    }
                    virtual void Add (ArgByValueType<T> item, CounterType count) override
                    {
                        if (count == 0) {
                            return;
                        }
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        auto                                                           i = fData_.find (item);
                        if (i == fData_.end ()) {
                            fData_.insert (typename map<T, CounterType>::value_type (item, count));
                        }
                        else {
                            i->second += count;
                        }
                        // MUST PATCH
                    }
                    virtual void Remove (ArgByValueType<T> item, CounterType count) override
                    {
                        if (count == 0) {
                            return;
                        }
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        auto                                                           i = fData_.find (item);
                        Require (i != fData_.end ());
                        if (i != fData_.end ()) {
                            Require (i->second >= count);
                            i->second -= count;
                            if (i->second == 0) {
                                fData_.erase_WithPatching (i);
                            }
                        }
                    }
                    virtual void Remove (const Iterator<CountedValue<T>>& i) override
                    {
                        const typename Iterator<CountedValue<T>>::IRep& ir = i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto&                                                          mir = dynamic_cast<const IteratorRep_&> (ir);
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        mir.fIterator.RemoveCurrent ();
                    }
                    virtual void UpdateCount (const Iterator<CountedValue<T>>& i, CounterType newCount) override
                    {
                        const typename Iterator<CountedValue<T>>::IRep& ir = i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto&                                                          mir = dynamic_cast<const IteratorRep_&> (ir);
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        if (newCount == 0) {
                            mir.fIterator.RemoveCurrent ();
                        }
                        else {
                            mir.fIterator.fStdIterator->second = newCount;
                        }
                        // TODO - PATCH
                    }
                    virtual CounterType OccurrencesOf (ArgByValueType<T> item) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        auto                                                            i = fData_.find (item);
                        if (i == fData_.end ()) {
                            return 0;
                        }
                        return i->second;
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
                    virtual void AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
                    }
#endif

                    // SortedMultiSet<T,TRAITS>::_IRep overrides
                public:
                    virtual InOrderComparerType GetInOrderComparer () const override
                    {
                        return InOrderComparerType{fData_.key_comp ()};
                    }

                private:
                    using DataStructureImplType_ = Private::PatchingDataStructures::STLContainerWrapper<map<T, CounterType, INORDER_COMPARER>>;
                    using IteratorRep_           = Private::IteratorImplHelper_<CountedValue<T>, DataStructureImplType_, typename DataStructureImplType_::ForwardIterator, pair<T, CounterType>>;

                private:
                    DataStructureImplType_ fData_;
                };

                /*
                 ********************************************************************************
                 ************************ SortedMultiSet_stdmap<T, TRAITS> **********************
                 ********************************************************************************
                 */
                template <typename T, typename TRAITS>
                inline SortedMultiSet_stdmap<T, TRAITS>::SortedMultiSet_stdmap ()
                    : SortedMultiSet_stdmap (std::less<T>{})
                {
                    AssertRepValidType_ ();
                }
                template <typename T, typename TRAITS>
                template <typename INORDER_COMPARER, typename ENABLE_IF_IS_COMPARER>
                inline SortedMultiSet_stdmap<T, TRAITS>::SortedMultiSet_stdmap (const INORDER_COMPARER& inorderComparer, ENABLE_IF_IS_COMPARER*)
                    : inherited (inherited::template MakeSharedPtr<Rep_<INORDER_COMPARER>> (inorderComparer))
                {
                    AssertRepValidType_ ();
                }
                template <typename T, typename TRAITS>
                inline SortedMultiSet_stdmap<T, TRAITS>::SortedMultiSet_stdmap (const initializer_list<T>& src)
                    : SortedMultiSet_stdmap ()
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template <typename T, typename TRAITS>
                inline SortedMultiSet_stdmap<T, TRAITS>::SortedMultiSet_stdmap (const initializer_list<CountedValue<T>>& src)
                    : SortedMultiSet_stdmap ()
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template <typename T, typename TRAITS>
                template <typename CONTAINER_OF_T, typename ENABLE_IF>
                inline SortedMultiSet_stdmap<T, TRAITS>::SortedMultiSet_stdmap (const CONTAINER_OF_T& src)
                    : SortedMultiSet_stdmap ()
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template <typename T, typename TRAITS>
                template <typename COPY_FROM_ITERATOR>
                SortedMultiSet_stdmap<T, TRAITS>::SortedMultiSet_stdmap (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end)
                    : SortedMultiSet_stdmap ()
                {
                    AddAll (start, end);
                    AssertRepValidType_ ();
                }
                template <typename T, typename TRAITS>
                inline void SortedMultiSet_stdmap<T, TRAITS>::AssertRepValidType_ () const
                {
#if qDebug
                    typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMember
#endif
                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_SortedMultiSet_stdmap_inl_ */
