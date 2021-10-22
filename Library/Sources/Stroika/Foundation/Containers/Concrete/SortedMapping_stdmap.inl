/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedMapping_stdmap_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedMapping_stdmap_inl_

#include <map>

#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"
#include "../STL/Compare.h"

#include "../DataStructures/STLContainerWrapper.h"
#include "../Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     ********************************************************************************
     ******* SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::IImplRepBase_ *******
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::IImplRepBase_ : public SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep {
    private:
        using inherited = typename SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep;

#if qCompilerAndStdLib_TemplateTypenameReferenceToBaseOfBaseClassMemberNotFound_Buggy
    protected:
        using _IterableRepSharedPtr = typename inherited::_IterableRepSharedPtr;
        using _APPLY_ARGTYPE        = typename inherited::_APPLY_ARGTYPE;
        using _APPLYUNTIL_ARGTYPE   = typename inherited::_APPLYUNTIL_ARGTYPE;
#endif

    protected:
        using _APPLY_ARGTYPE      = typename inherited::_APPLY_ARGTYPE;
        using _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;
    };

    /*
     ********************************************************************************
     ********** SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_ *************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename KEY_INORDER_COMPARER>
    class SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_ : public IImplRepBase_, public Memory::UseBlockAllocationIfAppropriate<Rep_<KEY_INORDER_COMPARER>> {
    private:
        using inherited = IImplRepBase_;

    public:
        using _IterableRepSharedPtr        = typename inherited::_IterableRepSharedPtr;
        using _MappingRepSharedPtr         = typename inherited::_IRepSharedPtr;
        using _APPLY_ARGTYPE               = typename inherited::_APPLY_ARGTYPE;
        using _APPLYUNTIL_ARGTYPE          = typename inherited::_APPLYUNTIL_ARGTYPE;
        using KeyEqualsCompareFunctionType = typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::KeyEqualsCompareFunctionType;
        using KeyInOrderKeyComparerType    = typename SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::KeyInOrderKeyComparerType;

    public:
        Rep_ (const KEY_INORDER_COMPARER& inorderComparer)
            : fData_{inorderComparer}
        {
        }
        Rep_ (const Rep_& from) = delete;
        Rep_ (Rep_* from, [[maybe_unused]] IteratorOwnerID forIterableEnvelope)
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
            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
            return Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSmartPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
        }
        virtual Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> MakeIterator (IteratorOwnerID suggestedOwner) const override
        {
            Rep_* NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
            return Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> (Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSmartPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_));
        }
        virtual size_t GetLength () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            fData_.Invariant ();
            return fData_.size ();
        }
        virtual bool IsEmpty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            fData_.Invariant ();
            return fData_.empty ();
        }
        virtual void Apply (_APPLY_ARGTYPE doToElement) const override
        {
            // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
            // use iterator (which currently implies lots of locks) with this->_Apply ()
            fData_.Apply (doToElement);
        }
        virtual Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            return this->_FindFirstThat (doToElement, suggestedOwner);
        }

        // Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep overrides
    public:
        virtual KeyEqualsCompareFunctionType GetKeyEqualsComparer () const override
        {
            return KeyEqualsCompareFunctionType{Common::EqualsComparerAdapter (fData_.key_comp ())};
        }
        virtual _MappingRepSharedPtr CloneEmpty ([[maybe_unused]] IteratorOwnerID forIterableEnvelope) const override
        {
            return Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSmartPtr<Rep_> (fData_.key_comp ());
        }
        virtual _MappingRepSharedPtr CloneAndPatchIterator ([[maybe_unused]] Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>* i, IteratorOwnerID obsoleteForIterableEnvelope) const
        {
            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
            auto                                                      result = Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSmartPtr<Rep_> (const_cast<Rep_*> (this), obsoleteForIterableEnvelope);
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            auto&                                                     mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            return result;
        }
        virtual Iterable<KEY_TYPE> Keys () const override
        {
            return this->_Keys_Reference_Implementation ();
        }
        virtual Iterable<MAPPED_VALUE_TYPE> MappedValues () const override
        {
            return this->_Values_Reference_Implementation ();
        }
        virtual bool Lookup (ArgByValueType<KEY_TYPE> key, optional<MAPPED_VALUE_TYPE>* item) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            auto                                                       i = fData_.find (key);
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
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
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
            fData_.Invariant ();
            return result;
        }
        virtual void Remove (ArgByValueType<KEY_TYPE> key) override
        {
            // @todo require not oustsanding iterators
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            fData_.Invariant ();
            auto i = fData_.find (key);
            if (i != fData_.end ()) {
                fData_.erase (i);
            }
        }
        virtual void Remove (const Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& i) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            (void)fData_.erase (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator.fStdIterator);
        }
        virtual void PatchIteratorBeforeRemove (const optional<Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>>& adjustmentAt, Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>* i) const override
        {
            RequireNotNull (i);
            if (adjustmentAt == *i) {
                ++(*i); // advance to next item if deleting current one
            }
            else {
                // nothing needed for other links
            }
        }
#if qDebug
        virtual void AssertNoIteratorsReferenceOwner ([[maybe_unused]] IteratorOwnerID oBeingDeleted) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            // fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
        }
#endif

        // SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep overrides
    public:
        virtual KeyInOrderKeyComparerType GetInOrderKeyComparer () const override
        {
            return KeyInOrderKeyComparerType{fData_.key_comp ()};
        }

    private:
        using DataStructureImplType_ = DataStructures::STLContainerWrapper<map<KEY_TYPE, MAPPED_VALUE_TYPE, KEY_INORDER_COMPARER>>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>, DataStructureImplType_>;

    private:
        DataStructureImplType_ fData_;
    };

    /*
     ********************************************************************************
     ************* SortedMapping_stdmap<KEY_TYPE,MAPPED_VALUE_TYPE> *****************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping_stdmap ()
        : SortedMapping_stdmap{less<KEY_TYPE>{}}
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename KEY_INORDER_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_INORDER_COMPARER> ()>*>
    inline SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping_stdmap (const KEY_INORDER_COMPARER& inorderComparer)
        : inherited (inherited::template MakeSmartPtr<Rep_<KEY_INORDER_COMPARER>> (inorderComparer))
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_ADDABLE> and not is_convertible_v<const CONTAINER_OF_ADDABLE*, const SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>*>>*>
    inline SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping_stdmap (const CONTAINER_OF_ADDABLE& src)
        : SortedMapping_stdmap{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename COPY_FROM_ITERATOR_KEYVALUE>
    inline SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping_stdmap (COPY_FROM_ITERATOR_KEYVALUE start, COPY_FROM_ITERATOR_KEYVALUE end)
        : SortedMapping_stdmap{}
    {
        this->AddAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::AssertRepValidType_ () const
    {
#if qDebug
        typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMember
#endif
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_SortedMapping_stdmap_inl_ */
