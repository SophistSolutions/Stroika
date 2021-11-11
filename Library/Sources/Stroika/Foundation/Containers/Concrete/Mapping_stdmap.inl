/*
    * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
    */
#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_stdmap_inl_
#define _Stroika_Foundation_Containers_Concrete_Mapping_stdmap_inl_

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
     ********* Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::IImplRepBase_ ***********
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::IImplRepBase_ : public Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep {
    };

    /*
     ********************************************************************************
     ************* Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_ ****************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename KEY_INORDER_COMPARER>
    class Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_ : public IImplRepBase_, public Memory::UseBlockAllocationIfAppropriate<Rep_<KEY_INORDER_COMPARER>> {
    private:
        using inherited = IImplRepBase_;

    public:
        Rep_ (const KEY_INORDER_COMPARER& inorderComparer)
            : fData_{inorderComparer}
        {
        }
        Rep_ (map<KEY_TYPE, MAPPED_VALUE_TYPE>&& src)
            : fData_{move (src)}
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
            // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
            // use iterator (which currently implies lots of locks) with this->_Apply ()
            fData_.Apply (doToElement);
        }
        virtual Iterator<value_type> FindFirstThat (const function<bool (ArgByValueType<value_type> item)>& doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return this->_FindFirstThat (doToElement);
        }

        // Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep overrides
    public:
        virtual KeyEqualsCompareFunctionType GetKeyEqualsComparer () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return KeyEqualsCompareFunctionType{Common::EqualsComparerAdapter (fData_.key_comp ())};
        }
        virtual _MappingRepSharedPtr CloneEmpty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return Iterable<value_type>::template MakeSmartPtr<Rep_> (fData_.key_comp ()); // keep comparer, but lose data
        }
        virtual _MappingRepSharedPtr CloneAndPatchIterator (Iterator<value_type>* i) const override
        {
            RequireNotNull (i);
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            auto                                                        result = Iterable<value_type>::template MakeSmartPtr<Rep_> (*this);
            auto&                                                       mir    = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual Iterable<KEY_TYPE> Keys () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return this->_Keys_Reference_Implementation ();
        }
        virtual Iterable<MAPPED_VALUE_TYPE> MappedValues () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return this->_Values_Reference_Implementation ();
        }
        virtual bool Lookup (ArgByValueType<KEY_TYPE> key, optional<MAPPED_VALUE_TYPE>* item) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            auto                                                        i = fData_.find (key);
            if (i == fData_.end ()) {
                if (item != nullptr) {
                    *item = nullopt;
                }
                return false;
            }
            else {
                if (item != nullptr) {
                    *item = i->second;
                }
                return true;
            }
        }
        virtual bool Add (ArgByValueType<KEY_TYPE> key, ArgByValueType<MAPPED_VALUE_TYPE> newElt, AddReplaceMode addReplaceMode) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            fData_.Invariant ();
            bool result{};
            switch (addReplaceMode) {
                case AddReplaceMode::eAddReplaces:
                    result = fData_.insert_or_assign (key, newElt).second; // according to https://en.cppreference.com/w/cpp/container/map/insert_or_assign - no iterator references are invalidated
                    break;
                case AddReplaceMode::eAddIfMissing:
                    result = fData_.insert ({key, newElt}).second; // according to https://en.cppreference.com/w/cpp/container/map/insert no iterator references are invalidated
                    break;
                default:
                    AssertNotReached ();
            }
            fChangeCounts_.PerformedChange ();
            fData_.Invariant ();
            return result;
        }
        virtual void Remove (ArgByValueType<KEY_TYPE> key) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            fData_.Invariant ();
            auto i = fData_.find (key);
            if (i != fData_.end ()) {
                fData_.erase (i);
                fChangeCounts_.PerformedChange ();
            }
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Require (not i.Done ());
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            if (nextI != nullptr) {
                *nextI = i;
                ++(*nextI); // advance to next item if deleting current one
            }
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            Assert (mir.fIterator.GetReferredToData () == &fData_);
            (void)fData_.erase (mir.fIterator.GetCurrentSTLIterator ());
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                Debug::UncheckedDynamicCast<IteratorRep_&> (nextI->GetRep ()).UpdateChangeCount ();
                nextI->Refresh (); // update to reflect changes made to rep
            }
        }
        virtual void Update (const Iterator<value_type>& i, ArgByValueType<mapped_type> newValue, Iterator<value_type>* nextI) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            fData_.remove_constness (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator.GetCurrentSTLIterator ())->second = newValue;
            if (nextI != nullptr) {
                *nextI = i;
                Debug::UncheckedDynamicCast<IteratorRep_&> (nextI->GetRep ()).UpdateChangeCount ();
            }
        }

    private:
        using DataStructureImplType_ = DataStructures::STLContainerWrapper<map<KEY_TYPE, MAPPED_VALUE_TYPE, KEY_INORDER_COMPARER>>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                          fData_;
        [[NO_UNIQUE_ADDRESS_ATTR]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     ******************* Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE> ****************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_stdmap ()
        : Mapping_stdmap{less<KEY_TYPE>{}}
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_stdmap (map<KEY_TYPE, MAPPED_VALUE_TYPE>&& src)
        : inherited (inherited::template MakeSmartPtr<Rep_<typename map<KEY_TYPE, MAPPED_VALUE_TYPE>::key_compare>> (move (src)))
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename KEY_INORDER_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_INORDER_COMPARER> ()>*>
    inline Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_stdmap (const KEY_INORDER_COMPARER& inorderComparer)
        : inherited (inherited::template MakeSmartPtr<Rep_<KEY_INORDER_COMPARER>> (inorderComparer))
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_ADDABLE> and not is_convertible_v<const CONTAINER_OF_ADDABLE*, const Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>*>>*>
    inline Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_stdmap (const CONTAINER_OF_ADDABLE& src)
        : Mapping_stdmap{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename COPY_FROM_ITERATOR_KEYVALUE>
    Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_stdmap (COPY_FROM_ITERATOR_KEYVALUE start, COPY_FROM_ITERATOR_KEYVALUE end)
        : Mapping_stdmap{}
    {
        this->AddAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::AssertRepValidType_ () const
    {
#if qDebug
        typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMemeber
#endif
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_Mapping_stdmap_inl_ */
