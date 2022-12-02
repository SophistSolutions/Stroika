/*
    * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
    */
#ifndef _Stroika_Foundation_Containers_Concrete_Association_stdmultimap_inl_
#define _Stroika_Foundation_Containers_Concrete_Association_stdmultimap_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"
#include "../STL/Compare.h"

#include "../DataStructures/STLContainerWrapper.h"
#include "../Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     ********************************************************************************
     **** Association_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::IImplRepBase_ *******
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class Association_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::IImplRepBase_ : public Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep {
    };

    /*
     ********************************************************************************
     ********* Association_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_ ***********
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename KEY_INORDER_COMPARER>
    class Association_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_ : public IImplRepBase_, public Memory::UseBlockAllocationIfAppropriate<Rep_<KEY_INORDER_COMPARER>> {
    public:
        static_assert (not is_reference_v<KEY_INORDER_COMPARER>);

    private:
        using inherited = IImplRepBase_;

    public:
        Rep_ (const KEY_INORDER_COMPARER& inorderComparer)
            : fData_{inorderComparer}
        {
        }
        Rep_ (STDMULTIMAP<>&& src)
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
            Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fData_};
            return Iterable<value_type>::template MakeSmartPtr<Rep_> (*this);
        }
        virtual Iterator<value_type> MakeIterator () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fData_};
            return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_)};
        }
        virtual size_t size () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fData_};
            fData_.Invariant ();
            return fData_.size ();
        }
        virtual bool empty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fData_};
            fData_.Invariant ();
            return fData_.empty ();
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fData_};
            fData_.Apply (doToElement);
        }
        virtual Iterator<value_type> Find (const function<bool (ArgByValueType<value_type> item)>& that) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fData_};
            return this->_Find (that);
        }
        virtual Iterator<value_type> Find_equal_to (const ArgByValueType<value_type>& v) const override
        {
            // std::map indexes on key, not full value_type (though maybe we could use that as a hint to start a search?)
            return this->_Find_equal_to_default_implementation (v);
        }

        // Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep overrides
    public:
        virtual KeyEqualsCompareFunctionType GetKeyEqualsComparer () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fData_};
            return KeyEqualsCompareFunctionType{Common::EqualsComparerAdapter (fData_.key_comp ())};
        }
        virtual _AssociationRepSharedPtr CloneEmpty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fData_};
            return Iterable<value_type>::template MakeSmartPtr<Rep_> (fData_.key_comp ()); // keep comparer, but lose data
        }
        virtual _AssociationRepSharedPtr CloneAndPatchIterator (Iterator<value_type>* i) const override
        {
            RequireNotNull (i);
            Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fData_};
            auto                                               result = Iterable<value_type>::template MakeSmartPtr<Rep_> (*this);
            result->fData_.MoveIteratorHereAfterClone (&Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ()).fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual Iterable<KEY_TYPE> Keys () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fData_};
            return this->_Keys_Reference_Implementation ();
        }
        virtual Iterable<MAPPED_VALUE_TYPE> MappedValues () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fData_};
            return this->_Values_Reference_Implementation ();
        }
        virtual Iterable<mapped_type> Lookup (ArgByValueType<KEY_TYPE> key) const override
        {
            // @todo consider doing custom class here, or using CreateGenerator
            Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fData_};
            auto                                               i = fData_.find (key);
            vector<mapped_type>                                result;
            if (i != fData_.end ()) {
                Assert (GetKeyEqualsComparer () (key, i->first));
                result.push_back (i->second);
                // the items in a multimap are all in order so we know the current i->key is not less
                Assert (not fData_.key_comp () (key, i->first));
                Assert (not fData_.key_comp () (i->first, key));
                for (++i; i != fData_.end () and not fData_.key_comp () (key, i->first); ++i) {
                    result.push_back (i->second);
                }
            }
            return Iterable<mapped_type>{move (result)};
        }
        virtual void Add (ArgByValueType<KEY_TYPE> key, ArgByValueType<MAPPED_VALUE_TYPE> newElt) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteLock writeLock{fData_};
            fData_.Invariant ();
            (void)fData_.insert ({key, newElt});
            fChangeCounts_.PerformedChange ();
            fData_.Invariant ();
        }
        virtual bool RemoveIf (ArgByValueType<KEY_TYPE> key) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteLock writeLock{fData_};
            fData_.Invariant ();
            auto i = fData_.find (key);
            if (i != fData_.end ()) {
                fData_.erase (i);
                fChangeCounts_.PerformedChange ();
                return true;
            }
            return false;
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Require (not i.Done ());
            Debug::AssertExternallySynchronizedMutex::WriteLock writeLock{fData_};
            if (nextI != nullptr) {
                *nextI = i;
                ++(*nextI); // advance to next item if deleting current one
            }
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            Assert (mir.fIterator.GetReferredToData () == &fData_);
            (void)fData_.erase (mir.fIterator.GetUnderlyingIteratorRep ());
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                Debug::UncheckedDynamicCast<IteratorRep_&> (nextI->GetRep ()).UpdateChangeCount ();
                nextI->Refresh (); // update to reflect changes made to rep
            }
        }
        virtual void Update (const Iterator<value_type>& i, ArgByValueType<mapped_type> newValue, Iterator<value_type>* nextI) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteLock              writeLock{fData_};
            optional<typename DataStructureImplType_::UnderlyingIteratorRep> savedUnderlyingIndex;
            if (nextI != nullptr) {
                savedUnderlyingIndex = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator.GetUnderlyingIteratorRep ();
            }
            fData_.remove_constness (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator.GetUnderlyingIteratorRep ())->second = newValue;
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_, *savedUnderlyingIndex)};
            }
        }

    private:
        using DataStructureImplType_ = DataStructures::STLContainerWrapper<STDMULTIMAP<KEY_INORDER_COMPARER>>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                     fData_;
        [[no_unique_address]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     *************** Association_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE> ***********
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Association_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_stdmultimap ()
        : Association_stdmultimap{less<KEY_TYPE>{}}
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Association_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_stdmultimap (STDMULTIMAP<>&& src)
        : inherited{inherited::template MakeSmartPtr<Rep_<typename map<KEY_TYPE, MAPPED_VALUE_TYPE>::key_compare>> (move (src))}
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename KEY_INORDER_COMPARER, enable_if_t<Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> ()>*>
    inline Association_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_stdmultimap (KEY_INORDER_COMPARER&& keyComparer)
        : inherited{inherited::template MakeSmartPtr<Rep_<Configuration::remove_cvref_t<KEY_INORDER_COMPARER>>> (keyComparer)}
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Association_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_stdmultimap (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        : Association_stdmultimap{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename KEY_INORDER_COMPARER, enable_if_t<Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> ()>*>
    inline Association_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_stdmultimap (KEY_INORDER_COMPARER&& keyComparer, const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        : Association_stdmultimap{forward<KEY_INORDER_COMPARER> (keyComparer)}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<Association_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>, decay_t<ITERABLE_OF_ADDABLE>>>*>
    inline Association_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_stdmultimap (ITERABLE_OF_ADDABLE&& src)
        : Association_stdmultimap{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename KEY_INORDER_COMPARER, typename ITERABLE_OF_ADDABLE, enable_if_t<Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () and Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>*>
    inline Association_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_stdmultimap (KEY_INORDER_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src)
        : Association_stdmultimap{forward<KEY_INORDER_COMPARER> (keyComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename ITERATOR_OF_ADDABLE, enable_if_t<Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>*>
    Association_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_stdmultimap (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : Association_stdmultimap{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename KEY_INORDER_COMPARER, typename ITERATOR_OF_ADDABLE, enable_if_t<Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () and Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>*>
    Association_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_stdmultimap (KEY_INORDER_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : Association_stdmultimap{forward<KEY_INORDER_COMPARER> (keyComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Association_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMemeber
        }
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_Association_stdmultimap_inl_ */
