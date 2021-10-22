/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedAssociation_stdmultimap_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedAssociation_stdmultimap_inl_

#include <map>

#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"
#include "../STL/Compare.h"

#include "../DataStructures/STLContainerWrapper.h"
#include "../Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class SortedAssociation_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::IImplRep_ : public SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep {
    private:
        using inherited = typename SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep;

    protected:
        using _APPLY_ARGTYPE      = typename inherited::_APPLY_ARGTYPE;
        using _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;
    };

    /*
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class SortedAssociation_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_ : public IImplRep_, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
    private:
        using inherited = IImplRep_;

    public:
        using _IterableRepSharedPtr    = typename inherited::_IterableRepSharedPtr;
        using _AssociationRepSharedPtr = typename inherited::_AssociationRepSharedPtr;
        using _APPLY_ARGTYPE           = typename inherited::_APPLY_ARGTYPE;
        using _APPLYUNTIL_ARGTYPE      = typename inherited::_APPLYUNTIL_ARGTYPE;

    public:
        Rep_ ()                 = default;
        Rep_ (const Rep_& from) = delete;
        Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope)
            : fData_{&from->fData_, forIterableEnvelope}
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

        // Association<KEY_TYPE, MAPPED_VALUE_TYPE, typename TRAITS::AssociationTraitsType>::_IRep overrides
    public:
        virtual _AssociationRepSharedPtr CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
        {
            if (fData_.HasActiveIterators ()) {
                // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                auto r = Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSmartPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                r->fData_.clear_WithPatching ();
                return r;
            }
            else {
                return Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSmartPtr<Rep_> ();
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
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            auto&                                                     mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
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
        using DataStructureImplType_ = DataStructures::STLContainerWrapper<map<KEY_TYPE, MAPPED_VALUE_TYPE, less<KEY_TYPE>>>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>, DataStructureImplType_>;

    private:
        DataStructureImplType_ fData_;
    };

    /*
     ********************************************************************************
     ******** SortedAssociation_stdmultimap<KEY_TYPE,MAPPED_VALUE_TYPE> *************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline SortedAssociation_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation_stdmultimap ()
        : inherited (inherited::template MakeSmartPtr<Rep_> ())
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_ADDABLE> and not is_convertible_v<const CONTAINER_OF_ADDABLE*, const SortedAssociation_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>*>>*>
    inline SortedAssociation_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation_stdmultimap (const CONTAINER_OF_ADDABLE& src)
        : SortedAssociation_stdmultimap ()
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void SortedAssociation_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::AssertRepValidType_ () const
    {
#if qDebug
        typename inherited::template _SafeReadRepAccessor<IImplRep_> tmp{this}; // for side-effect of AssertMember
#endif
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_SortedAssociation_stdmultimap_inl_ */
