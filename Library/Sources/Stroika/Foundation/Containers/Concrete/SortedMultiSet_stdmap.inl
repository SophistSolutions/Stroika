/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedMultiSet_stdmap_inl_
#define _Stroika_Foundation_Containers_SortedMultiSet_stdmap_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <map>

#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"

#include "../STL/Compare.h"

#include "../DataStructures/STLContainerWrapper.h"
#include "../Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    /**
     */
    template <typename T, typename TRAITS>
    class SortedMultiSet_stdmap<T, TRAITS>::IImplRepBase_ : public SortedMultiSet<T, TRAITS>::_IRep {
    };

    /**
     */
    template <typename T, typename TRAITS>
    template <typename INORDER_COMPARER>
    class SortedMultiSet_stdmap<T, TRAITS>::Rep_ : public IImplRepBase_, public Memory::UseBlockAllocationIfAppropriate<Rep_<INORDER_COMPARER>> {
    private:
        using inherited = IImplRepBase_;

    public:
        Rep_ (const INORDER_COMPARER& inorderComparer)
            : fData_{inorderComparer}
        {
        }
        Rep_ (const Rep_& from) = delete;
        Rep_ (const Rep_* from, [[maybe_unused]] IteratorOwnerID forIterableEnvelope)
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
            return Iterable<value_type>::template MakeSmartPtr<Rep_> (this, forIterableEnvelope);
        }
        virtual size_t GetLength () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            return fData_.size ();
        }
        virtual bool IsEmpty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            return fData_.empty ();
        }
        virtual Iterator<value_type> MakeIterator (IteratorOwnerID suggestedOwner) const override
        {
            return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (suggestedOwner, &fData_)};
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement) const override
        {
            this->_Apply (doToElement);
        }
        virtual Iterator<value_type> FindFirstThat (const function<bool (ArgByValueType<value_type> item)>& doToElement, IteratorOwnerID suggestedOwner) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            return this->_FindFirstThat (doToElement, suggestedOwner);
        }

        // MultiSet<T, TRAITS>::_IRep overrides
    public:
        virtual ElementEqualityComparerType GetElementEqualsComparer () const override
        {
            return ElementEqualityComparerType{Common::EqualsComparerAdapter (fData_.key_comp ())};
        }
        virtual _MultiSetRepSharedPtr CloneEmpty ([[maybe_unused]] IteratorOwnerID forIterableEnvelope) const override
        {
            return Iterable<value_type>::template MakeSmartPtr<Rep_> (fData_.key_comp ());
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
            value_type                                                 tmp{item};
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            return fData_.find (item) != fData_.end ();
        }
        virtual void Add (ArgByValueType<T> item, CounterType count) override
        {
            if (count == 0) {
                return;
            }
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            auto                                                      i = fData_.find (item);
            if (i == fData_.end ()) {
                fData_.insert (typename map<T, CounterType>::value_type (item, count));
            }
            else {
                i->second += count;
            }
        }
        virtual void Remove (ArgByValueType<T> item, CounterType count) override
        {
            if (count == 0) {
                return;
            }
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            auto                                                      i = fData_.find (item);
            Require (i != fData_.end ());
            if (i != fData_.end ()) {
                Require (i->second >= count);
                i->second -= count;
                if (i->second == 0) {
                    fData_.erase (i);
                }
            }
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Require (not i.Done ());
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};

            if (nextI != nullptr) {
                *nextI = i;
                ++(*nextI);
            }

            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            Assert (mir.fIterator.fData == &fData_);
            (void)fData_.erase (mir.fIterator.fStdIterator);
            if (nextI != nullptr) {
                nextI->Refresh (); // update to reflect changes made to rep
            }
        }
        virtual void UpdateCount (const Iterator<value_type>& i, CounterType newCount) override
        {
            auto&                                                     mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            if (newCount == 0) {
                mir.fIterator.fStdIterator = fData_.erase (mir.fIterator.fStdIterator);
            }
            else {
                mir.fIterator.fStdIterator->second = newCount;
            }
            // @todo - PATCH
        }
        virtual CounterType OccurrencesOf (ArgByValueType<T> item) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            auto                                                       i = fData_.find (item);
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
        virtual void AssertNoIteratorsReferenceOwner ([[maybe_unused]] IteratorOwnerID oBeingDeleted) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            // fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
        }
#endif

        // SortedMultiSet<T,TRAITS>::_IRep overrides
    public:
        virtual ElementInOrderComparerType GetElementInOrderComparer () const override
        {
            return ElementInOrderComparerType{fData_.key_comp ()};
        }

    private:
        using DataStructureImplType_ = DataStructures::STLContainerWrapper<map<T, CounterType, INORDER_COMPARER>>;
        using IteratorRep_           = Private::IteratorImplHelper_<value_type, DataStructureImplType_, typename DataStructureImplType_::ForwardIterator, pair<T, CounterType>>;

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
        : SortedMultiSet_stdmap{less<T>{}}
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <typename INORDER_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<T, INORDER_COMPARER> ()>*>
    inline SortedMultiSet_stdmap<T, TRAITS>::SortedMultiSet_stdmap (const INORDER_COMPARER& inorderComparer)
        : inherited (inherited::template MakeSmartPtr<Rep_<INORDER_COMPARER>> (inorderComparer))
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    inline SortedMultiSet_stdmap<T, TRAITS>::SortedMultiSet_stdmap (const initializer_list<T>& src)
        : SortedMultiSet_stdmap{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    inline SortedMultiSet_stdmap<T, TRAITS>::SortedMultiSet_stdmap (const initializer_list<value_type>& src)
        : SortedMultiSet_stdmap{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <typename CONTAINER_OF_T, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_T> and not is_convertible_v<const CONTAINER_OF_T*, const SortedMultiSet_stdmap<T, TRAITS>*>>*>
    inline SortedMultiSet_stdmap<T, TRAITS>::SortedMultiSet_stdmap (const CONTAINER_OF_T& src)
        : SortedMultiSet_stdmap{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <typename COPY_FROM_ITERATOR>
    SortedMultiSet_stdmap<T, TRAITS>::SortedMultiSet_stdmap (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end)
        : SortedMultiSet_stdmap{}
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

#endif /* _Stroika_Foundation_Containers_SortedMultiSet_stdmap_inl_ */
