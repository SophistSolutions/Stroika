/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Collection_stdmultiset_inl_
#define _Stroika_Foundation_Containers_Concrete_Collection_stdmultiset_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"

#include "../DataStructures/STLContainerWrapper.h"
#include "../Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     ********************************************************************************
     ******************* Collection_stdmultiset<T>::IImplRepBase_ *************
     ********************************************************************************
     */
    template <typename T>
    class Collection_stdmultiset<T>::IImplRepBase_ : public Collection<T>::_IRep {
    };

    /*
     ********************************************************************************
     ******************** Collection_stdmultiset<T>::Rep_ *********************
     ********************************************************************************
     */
    template <typename T>
    template <typename INORDER_COMPARER>
    class Collection_stdmultiset<T>::Rep_ : public IImplRepBase_, public Memory::UseBlockAllocationIfAppropriate<Rep_<INORDER_COMPARER>> {
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
        virtual Iterator<value_type> MakeIterator () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_)};
        }
        virtual size_t size () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return fData_.size ();
        }
        virtual bool empty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return fData_.empty ();
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            fData_.Apply (doToElement);
        }
        virtual Iterator<value_type> Find (const function<bool (ArgByValueType<value_type> item)>& that) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return this->_Find (that);
        }
        virtual Iterator<value_type> Find_equal_to (const ArgByValueType<value_type>& v) const override
        {
            // if doing a find by 'equals-to' - we already have this indexed
            auto found = fData_.find (v);
            Ensure ((found == fData_.end () and this->_Find_equal_to_default_implementation (v) == Iterator<value_type>{nullptr}) or (found == Debug::UncheckedDynamicCast<const IteratorRep_&> (this->_Find_equal_to_default_implementation (v).ConstGetRep ()).fIterator.GetUnderlyingIteratorRep ()));
            return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_, found)};
        }

        // Collection<T>::_IRep overrides
    public:
        virtual _CollectionRepSharedPtr CloneEmpty () const override
        {
            return Iterable<T>::template MakeSmartPtr<Rep_> (fData_.key_comp ()); // keep comparer, but lose data in clone
        }
        virtual _CollectionRepSharedPtr CloneAndPatchIterator (Iterator<value_type>* i) const override
        {
            RequireNotNull (i);
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            auto                                                        result = Iterable<value_type>::template MakeSmartPtr<Rep_> (*this);
            auto&                                                       mir    = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual void Add (ArgByValueType<value_type> item) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            fData_.insert (item);
            fChangeCounts_.PerformedChange ();
        }
        virtual void Update (const Iterator<value_type>& i, ArgByValueType<value_type> newValue, Iterator<value_type>* nextI) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            auto                                                  nextIR = fData_.erase (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator.GetUnderlyingIteratorRep ());
            fData_.insert (newValue);
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_, nextIR)};
            }
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            auto&                                                 mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            Assert (mir.fIterator.GetReferredToData () == &fData_);
            auto nextIR = fData_.erase (mir.fIterator.GetUnderlyingIteratorRep ());
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_, nextIR)};
            }
        }

    private:
        using DataStructureImplType_ = DataStructures::STLContainerWrapper<STDMULTISET<INORDER_COMPARER>>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                     fData_;
        [[no_unique_address]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     ************************* Collection_stdmultiset<T> ****************************
     ********************************************************************************
     */
    template <typename T>
    inline Collection_stdmultiset<T>::Collection_stdmultiset ()
        : Collection_stdmultiset{less<T>{}}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename INORDER_COMPARER>
    inline Collection_stdmultiset<T>::Collection_stdmultiset (INORDER_COMPARER&& inorderComparer)
        : inherited{inherited::template MakeSmartPtr<Rep_<INORDER_COMPARER>> (forward<INORDER_COMPARER> (inorderComparer))}
    {
        static_assert (Common::IsStrictInOrderComparer<INORDER_COMPARER> (), "StrictInOrder comparer required with Collection_stdmultiset");
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename ITERATOR_OF_ADDABLE>
    inline Collection_stdmultiset<T>::Collection_stdmultiset (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : Collection_stdmultiset{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T>
    inline Collection_stdmultiset<T>::Collection_stdmultiset (const initializer_list<value_type>& src)
        : Collection_stdmultiset{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<Collection_stdmultiset<T>, decay_t<ITERABLE_OF_ADDABLE>>>*>
    inline Collection_stdmultiset<T>::Collection_stdmultiset (ITERABLE_OF_ADDABLE&& src)
        : Collection_stdmultiset{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename T>
    inline void Collection_stdmultiset<T>::AssertRepValidType_ () const
    {
#if qDebug
        typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMember
#endif
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_Collection_stdmultiset_inl_ */
