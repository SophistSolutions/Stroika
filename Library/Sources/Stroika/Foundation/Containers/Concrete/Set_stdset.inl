/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Set_stdset_inl_
#define _Stroika_Foundation_Containers_Concrete_Set_stdset_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Common/Compare.h"
#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"

#include "../DataStructures/STLContainerWrapper.h"
#include "../Private/IteratorImplHelper.h"
#include "../STL/Compare.h"

namespace Stroika::Foundation::Containers::Concrete {

    /**
     */
    template <typename T>
    class Set_stdset<T>::IImplRepBase_ : public Set<T>::_IRep {
    };

    /**
     */
    template <typename T>
    template <typename INORDER_COMPARER>
    class Set_stdset<T>::Rep_ : public IImplRepBase_, public Memory::UseBlockAllocationIfAppropriate<Rep_<INORDER_COMPARER>> {
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
        virtual size_t GetLength () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            fData_.Invariant ();
            return fData_.size ();
        }
        virtual bool IsEmpty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            fData_.Invariant ();
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

        // Set<T>::_IRep overrides
    public:
        virtual ElementEqualityComparerType GetElementEqualsComparer () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return ElementEqualityComparerType{Common::EqualsComparerAdapter (fData_.key_comp ())};
        }
        virtual _SetRepSharedPtr CloneEmpty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return Iterable<value_type>::template MakeSmartPtr<Rep_> (fData_.key_comp ()); // keep comparer, but lose data
        }
        virtual _SetRepSharedPtr CloneAndPatchIterator (Iterator<value_type>* i) const override
        {
            RequireNotNull (i);
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            auto                                                        result = Iterable<value_type>::template MakeSmartPtr<Rep_> (*this);
            auto&                                                       mir    = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual bool Equals (const typename Iterable<value_type>::_IRep& rhs) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return this->_Equals_Reference_Implementation (rhs);
        }
        virtual bool Lookup (ArgByValueType<value_type> item, optional<value_type>* oResult, Iterator<value_type>* iResult) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            auto                                                        i       = fData_.find (item);
            bool                                                        notDone = i != fData_.end ();
            if (oResult != nullptr and notDone) {
                *oResult = *i;
            }
            if (iResult != nullptr and notDone) {
                *iResult = Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_, i)};
            }
            return notDone;
        }
        virtual void Add (ArgByValueType<value_type> item) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            fData_.insert (item);
            fChangeCounts_.PerformedChange ();
        }
        virtual void Remove (ArgByValueType<value_type> item) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            fData_.Invariant ();
            auto i = fData_.find (item);
            if (i != fData_.end ()) {
                fData_.erase (i);
                fChangeCounts_.PerformedChange ();
            }
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Require (not i.Done ());
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
        using DataStructureImplType_ = DataStructures::STLContainerWrapper<STDSET<INORDER_COMPARER>>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                          fData_;
        [[NO_UNIQUE_ADDRESS_ATTR]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     ********************************** Set_setset<T> *******************************
     ********************************************************************************
     */
    template <typename T>
    inline Set_stdset<T>::Set_stdset ()
        : Set_stdset{less<value_type>{}}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename INORDER_COMPARER>
    inline Set_stdset<T>::Set_stdset (const INORDER_COMPARER& inorderComparer)
        : inherited{inherited::template MakeSmartPtr<Rep_<INORDER_COMPARER>> (inorderComparer)}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    inline Set_stdset<T>::Set_stdset (const initializer_list<value_type>& src)
        : Set_stdset{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T>
    inline Set_stdset<T>::Set_stdset (const ElementEqualityComparerType& equalsComparer, const initializer_list<T>& src)
        : Set_stdset{equalsComparer}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_ADDABLE> and not is_base_of_v<Set_stdset<T>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>*>
    inline Set_stdset<T>::Set_stdset (CONTAINER_OF_ADDABLE&& src)
        : Set_stdset{}
    {
        this->AddAll (forward<CONTAINER_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_ADDABLE>>*>
    inline Set_stdset<T>::Set_stdset (const ElementEqualityComparerType& equalsComparer, CONTAINER_OF_ADDABLE&& src)
        : Set_stdset{equalsComparer}
    {
        this->AddAll (forward<CONTAINER_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename COPY_FROM_ITERATOR_T>
    inline Set_stdset<T>::Set_stdset (COPY_FROM_ITERATOR_T start, COPY_FROM_ITERATOR_T end)
        : Set_stdset{}
    {
        this->AddAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename COPY_FROM_ITERATOR_T>
    inline Set_stdset<T>::Set_stdset (const ElementEqualityComparerType& equalsComparer, COPY_FROM_ITERATOR_T start, COPY_FROM_ITERATOR_T end)
        : Set_stdset{equalsComparer}
    {
        this->AddAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename T>
    inline void Set_stdset<T>::AssertRepValidType_ () const
    {
#if qDebug
        typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMember
#endif
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_Set_stdset_inl_ */
