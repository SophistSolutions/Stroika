/*
    * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
    */
#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_stdmap_inl_
#define _Stroika_Foundation_Containers_Concrete_Mapping_stdmap_inl_

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

namespace Stroika::Foundation::Containers::Concrete {

    /*
     ********************************************************************************
     ********* Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::IImplRepBase_ ***********
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::IImplRepBase_ : public Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep {
    private:
        using inherited = typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep;

#if qCompilerAndStdLib_TemplateTypenameReferenceToBaseOfBaseClassMemberNotFound_Buggy
    protected:
        using _APPLY_ARGTYPE      = typename inherited::_APPLY_ARGTYPE;
        using _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;
#endif
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
        using _IterableRepSharedPtr        = typename Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::_IterableRepSharedPtr;
        using _MappingRepSharedPtr         = typename inherited::_MappingRepSharedPtr;
        using _APPLY_ARGTYPE               = typename inherited::_APPLY_ARGTYPE;
        using _APPLYUNTIL_ARGTYPE          = typename inherited::_APPLYUNTIL_ARGTYPE;
        using KeyEqualsCompareFunctionType = typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::KeyEqualsCompareFunctionType;

    public:
        Rep_ (const KEY_INORDER_COMPARER& inorderComparer)
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

        // Iterable<T>::_IRep overrides
    public:
        virtual _IterableRepSharedPtr Clone (IteratorOwnerID forIterableEnvelope) const override
        {
            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
            return Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
        }
        virtual Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> MakeIterator (IteratorOwnerID suggestedOwner) const override
        {
            Rep_* NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
            return Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> (Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_));
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
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
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
            return KeyEqualsCompareFunctionType{Common::mkEqualsComparerAdapter (fData_.key_comp ())};
        }
        virtual _MappingRepSharedPtr CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
        {
            if (fData_.HasActiveIterators ()) {
                // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                auto r = Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                r->fData_.clear_WithPatching ();
                return r;
            }
            else {
                return Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSharedPtr<Rep_> (fData_.key_comp ());
            }
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
        virtual void Add (ArgByValueType<KEY_TYPE> key, ArgByValueType<MAPPED_VALUE_TYPE> newElt) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            fData_.Invariant ();
            auto i = fData_.find (key);
            if (i == fData_.end ()) {
                i = fData_.insert (pair<KEY_TYPE, MAPPED_VALUE_TYPE> (key, newElt)).first;
                // no need to patch map<>
            }
            else {
                i->second = newElt;
            }
            fData_.Invariant ();
        }
        virtual void Remove (ArgByValueType<KEY_TYPE> key) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            fData_.Invariant ();
            auto i = fData_.find (key);
            if (i != fData_.end ()) {
                fData_.erase_WithPatching (i);
            }
        }
        virtual void Remove (const Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& i) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock>                 critSec{fData_};
            const typename Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::IRep& ir = i.GetRep ();
            AssertMember (&ir, IteratorRep_);
            auto& mir = dynamic_cast<const IteratorRep_&> (ir);
            mir.fIterator.RemoveCurrent ();
        }
#if qDebug
        virtual void AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
        }
#endif

    private:
        using DataStructureImplType_ = Private::PatchingDataStructures::STLContainerWrapper<map<KEY_TYPE, MAPPED_VALUE_TYPE, KEY_INORDER_COMPARER>>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>, DataStructureImplType_>;

    private:
        DataStructureImplType_ fData_;
    };

    /*
     ********************************************************************************
     ******************* Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE> ****************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_stdmap ()
        : Mapping_stdmap (less<KEY_TYPE>{})
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename KEY_INORDER_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_INORDER_COMPARER> ()>*>
    inline Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_stdmap (const KEY_INORDER_COMPARER& inorderComparer)
        : inherited (inherited::template MakeSharedPtr<Rep_<KEY_INORDER_COMPARER>> (inorderComparer))
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_ADDABLE> and not is_convertible_v<const CONTAINER_OF_ADDABLE*, const Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>*>>*>
    inline Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_stdmap (const CONTAINER_OF_ADDABLE& src)
        : Mapping_stdmap ()
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename COPY_FROM_ITERATOR_KEYVALUE>
    Mapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_stdmap (COPY_FROM_ITERATOR_KEYVALUE start, COPY_FROM_ITERATOR_KEYVALUE end)
        : Mapping_stdmap ()
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
