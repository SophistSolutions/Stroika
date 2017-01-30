/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedMapping_stdmap_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedMapping_stdmap_inl_

#include <map>

#include "../../Memory/BlockAllocated.h"
#include "../STL/Compare.h"

#include "../Private/IteratorImplHelper.h"
#include "../Private/PatchingDataStructures/STLContainerWrapper.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Concrete {

                /*
                 */
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE, typename TRAITS>
                class SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>::IImplRep_ : public SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>::_IRep {
                private:
                    using inherited = typename SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>::_IRep;

                protected:
                    using _APPLY_ARGTYPE      = typename inherited::_APPLY_ARGTYPE;
                    using _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;
                };

                /*
                 */
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE, typename TRAITS>
                class SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>::Rep_ : public IImplRep_ {
                private:
                    using inherited = IImplRep_;

                public:
                    using _IterableSharedPtrIRep = typename Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::_SharedPtrIRep;
                    using _MappingSharedPtrIRep  = typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE, typename TRAITS::MappingTraitsType>::_SharedPtrIRep;
                    using _APPLY_ARGTYPE         = typename inherited::_APPLY_ARGTYPE;
                    using _APPLYUNTIL_ARGTYPE    = typename inherited::_APPLYUNTIL_ARGTYPE;

                public:
                    Rep_ ()                 = default;
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
                    virtual _IterableSharedPtrIRep Clone (IteratorOwnerID forIterableEnvelope) const override
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
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        fData_.Invariant ();
                        return fData_.size ();
                    }
                    virtual bool IsEmpty () const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
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
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        return this->_FindFirstThat (doToElement, suggestedOwner);
                    }

                    // Mapping<KEY_TYPE, MAPPED_VALUE_TYPE, typename TRAITS::MappingTraitsType>::_IRep overrides
                public:
                    virtual _MappingSharedPtrIRep CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
                    {
                        if (fData_.HasActiveIterators ()) {
                            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                            auto r = Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                            r->fData_.clear_WithPatching ();
                            return r;
                        }
                        else {
                            return Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSharedPtr<Rep_> ();
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
                    virtual bool Lookup (ArgByValueType<KEY_TYPE> key, Memory::Optional<MAPPED_VALUE_TYPE>* item) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        auto                                                            i = fData_.find (key);
                        if (i == fData_.end ()) {
                            if (item != nullptr) {
                                item->clear ();
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
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
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
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        fData_.Invariant ();
                        auto i = fData_.find (key);
                        if (i != fData_.end ()) {
                            fData_.erase_WithPatching (i);
                        }
                    }
                    virtual void Remove (const Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& i) override
                    {
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        const typename Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::IRep& ir = i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto& mir = dynamic_cast<const IteratorRep_&> (ir);
                        mir.fIterator.RemoveCurrent ();
                    }
#if qDebug
                    virtual void AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
                    }
#endif

                private:
                    using DataStructureImplType_ = Private::PatchingDataStructures::STLContainerWrapper<map<KEY_TYPE, MAPPED_VALUE_TYPE, Common::STL::less<KEY_TYPE, typename TRAITS::KeyWellOrderCompareFunctionType>>>;
                    using IteratorRep_           = typename Private::IteratorImplHelper_<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>, DataStructureImplType_>;

                private:
                    DataStructureImplType_ fData_;
                };

                /*
                 ********************************************************************************
                 ************* SortedMapping_stdmap<KEY_TYPE,MAPPED_VALUE_TYPE> *****************
                 ********************************************************************************
                 */
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE, typename TRAITS>
                inline SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>::SortedMapping_stdmap ()
                    : inherited (inherited::template MakeSharedPtr<Rep_> ())
                {
                    AssertRepValidType_ ();
                }
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE, typename TRAITS>
                inline SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>::SortedMapping_stdmap (const SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>& src)
                    : inherited (src)
                {
                    AssertRepValidType_ ();
                }
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE, typename TRAITS>
                template <typename CONTAINER_OF_PAIR_KEY_T, typename ENABLE_IF>
                inline SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>::SortedMapping_stdmap (const CONTAINER_OF_PAIR_KEY_T& src)
                    : SortedMapping_stdmap ()
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE, typename TRAITS>
                inline void SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>::AssertRepValidType_ () const
                {
#if qDebug
                    typename inherited::template _SafeReadRepAccessor<IImplRep_> tmp{this}; // for side-effect of AssertMember
#endif
                }
            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_SortedMapping_stdmap_inl_ */
