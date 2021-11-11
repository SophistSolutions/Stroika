/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_MultiSet_stdmap_inl_
#define _Stroika_Foundation_Containers_Concrete_MultiSet_stdmap_inl_

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

    /*
     ********************************************************************************
     ******************** MultiSet_stdmap<T, TRAITS>::IImplRepBase_ *****************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    class MultiSet_stdmap<T, TRAITS>::IImplRepBase_ : public MultiSet<T, TRAITS>::_IRep {
    };

    /*
     ********************************************************************************
     ********************* MultiSet_stdmap<T, TRAITS>::Rep_ *************************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    template <typename INORDER_COMPARER>
    class MultiSet_stdmap<T, TRAITS>::Rep_ : public IImplRepBase_, public Memory::UseBlockAllocationIfAppropriate<Rep_<INORDER_COMPARER>> {
    private:
        using inherited = IImplRepBase_;

    public:
        Rep_ (const INORDER_COMPARER& inorderComparer)
            : fData_{inorderComparer}
        {
        }
        Rep_ (const Rep_& from) = default;

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

        // Iterable<T>::_IRep overrides
    public:
        virtual _IterableRepSharedPtr Clone () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return Iterable<value_type>::template MakeSmartPtr<Rep_> (*this);
        }
        virtual size_t GetLength () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return fData_.size ();
        }
        virtual bool IsEmpty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return fData_.empty ();
        }
        virtual Iterator<value_type> MakeIterator () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_)};
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            this->_Apply (doToElement);
        }
        virtual Iterator<value_type> FindFirstThat (const function<bool (ArgByValueType<value_type> item)>& doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return this->_FindFirstThat (doToElement);
        }

        // MultiSet<T, TRAITS>::_IRep overrides
    public:
        virtual ElementEqualityComparerType GetElementEqualsComparer () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return ElementEqualityComparerType{Common::EqualsComparerAdapter (fData_.key_comp ())};
        }
        virtual _MultiSetRepSharedPtr CloneEmpty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return Iterable<value_type>::template MakeSmartPtr<Rep_> (fData_.key_comp ()); // same comparer, but no data
        }
        virtual _MultiSetRepSharedPtr CloneAndPatchIterator (Iterator<value_type>* i) const override
        {
            RequireNotNull (i);
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            auto                                                        result = Iterable<value_type>::template MakeSmartPtr<Rep_> (*this);
            auto&                                                       mir    = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual bool Equals (const typename MultiSet<T, TRAITS>::_IRep& rhs) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return this->_Equals_Reference_Implementation (rhs);
        }
        virtual bool Contains (ArgByValueType<T> item) const override
        {
            value_type                                                  tmp{item};
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return fData_.find (item) != fData_.end ();
        }
        virtual void Add (ArgByValueType<T> item, CounterType count) override
        {
            if (count == 0) {
                return;
            }
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            auto                                                  i = fData_.find (item);
            if (i == fData_.end ()) {
                fData_.insert (typename map<T, CounterType, INORDER_COMPARER>::value_type (item, count));
            }
            else {
                i->second += count;
            }
            fChangeCounts_.PerformedChange ();
        }
        virtual void Remove (ArgByValueType<T> item, CounterType count) override
        {
            if (count == 0) {
                return;
            }
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            auto                                                  i = fData_.find (item);
            Require (i != fData_.end ());
            if (i != fData_.end ()) {
                Require (i->second >= count);
                i->second -= count;
                if (i->second == 0) {
                    fData_.erase (i);
                }
                fChangeCounts_.PerformedChange ();
            }
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            if (nextI != nullptr) {
                *nextI = i;
                ++(*nextI);
            }
            (void)fData_.erase (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator.GetCurrentSTLIterator ());
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                Debug::UncheckedDynamicCast<IteratorRep_&> (nextI->GetRep ()).UpdateChangeCount ();
                nextI->Refresh (); // update to reflect changes made to rep
            }
        }
        virtual void UpdateCount (const Iterator<value_type>& i, CounterType newCount, Iterator<value_type>* nextI) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            auto&                                                 mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            if (newCount == 0) {
                if (nextI != nullptr) {
                    *nextI = i;
                    ++(*nextI);
                }
                (void)fData_.erase (mir.fIterator.GetCurrentSTLIterator ());
            }
            else {
                fData_.remove_constness (mir.fIterator.GetCurrentSTLIterator ())->second = newCount;
                if (nextI != nullptr) {
                    *nextI = i;
                }
            }
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                Debug::UncheckedDynamicCast<IteratorRep_&> (nextI->GetRep ()).UpdateChangeCount ();
                nextI->Refresh (); // update to reflect changes made to rep
            }
        }
        virtual CounterType OccurrencesOf (ArgByValueType<T> item) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            auto                                                        i = fData_.find (item);
            if (i == fData_.end ()) {
                return 0;
            }
            return i->second;
        }
        virtual Iterable<T> Elements (const _MultiSetRepSharedPtr& rep) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return this->_Elements_Reference_Implementation (rep);
        }
        virtual Iterable<T> UniqueElements (const _MultiSetRepSharedPtr& rep) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return this->_UniqueElements_Reference_Implementation (rep);
        }

    private:
        using DataStructureImplType_ = DataStructures::STLContainerWrapper<map<T, CounterType, INORDER_COMPARER>>;
        using IteratorRep_           = Private::IteratorImplHelper_<value_type, DataStructureImplType_, typename DataStructureImplType_::ForwardIterator, pair<T, CounterType>>;

    private:
        DataStructureImplType_                                          fData_;
        [[NO_UNIQUE_ADDRESS_ATTR]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     ************************ MultiSet_stdmap<T, TRAITS> ****************************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    inline MultiSet_stdmap<T, TRAITS>::MultiSet_stdmap ()
        : MultiSet_stdmap{less<T>{}}
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <typename INORDER_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<T, INORDER_COMPARER> ()>*>
    inline MultiSet_stdmap<T, TRAITS>::MultiSet_stdmap (const INORDER_COMPARER& inorderComparer)
        : inherited (inherited::template MakeSmartPtr<Rep_<INORDER_COMPARER>> (inorderComparer))
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <typename CONTAINER_OF_T, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_T> and not is_convertible_v<const CONTAINER_OF_T*, const MultiSet_stdmap<T>*>>*>
    inline MultiSet_stdmap<T, TRAITS>::MultiSet_stdmap (const CONTAINER_OF_T& src)
        : MultiSet_stdmap{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    MultiSet_stdmap<T, TRAITS>::MultiSet_stdmap (const initializer_list<T>& src)
        : MultiSet_stdmap{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    MultiSet_stdmap<T, TRAITS>::MultiSet_stdmap (const initializer_list<value_type>& src)
        : MultiSet_stdmap ()
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <typename COPY_FROM_ITERATOR>
    MultiSet_stdmap<T, TRAITS>::MultiSet_stdmap (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end)
        : MultiSet_stdmap{}
    {
        this->AddAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    inline void MultiSet_stdmap<T, TRAITS>::AssertRepValidType_ () const
    {
#if qDebug
        typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMemeber
#endif
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_MultiSet_stdmap_inl_ */
