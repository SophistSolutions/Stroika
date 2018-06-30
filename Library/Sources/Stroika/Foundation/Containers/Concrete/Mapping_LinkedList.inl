/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Mapping_LinkedList_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Memory/BlockAllocated.h"

#include "../Private/IteratorImplHelper.h"
#include "../Private/PatchingDataStructures/LinkedList.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Concrete {

                using Traversal::IteratorOwnerID;

                /*
                 ********************************************************************************
                 ******** Mapping_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>::IImplRepBase_ ********
                 ********************************************************************************
                 */
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
                class Mapping_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>::IImplRepBase_ : public Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep {
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
                 *********** Mapping_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_***************
                 ********************************************************************************
                 */
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
                template <typename KEY_EQUALS_COMPARER>
                class Mapping_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_ : public IImplRepBase_ {
                private:
                    using inherited = IImplRepBase_;

                public:
                    using _IterableRepSharedPtr        = typename Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::_IterableRepSharedPtr;
                    using _MappingRepSharedPtr         = typename inherited::_MappingRepSharedPtr;
                    using _APPLY_ARGTYPE               = typename inherited::_APPLY_ARGTYPE;
                    using _APPLYUNTIL_ARGTYPE          = typename inherited::_APPLYUNTIL_ARGTYPE;
                    using KeyEqualsCompareFunctionType = typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::KeyEqualsCompareFunctionType;

                public:
                    Rep_ (const KEY_EQUALS_COMPARER& keyEqualsComparer)
                        : fKeyEqualsComparer_ (keyEqualsComparer)
                    {
                    }
                    Rep_ (const Rep_& from) = delete;
                    Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope)
                        : inherited ()
                        , fKeyEqualsComparer_ (from->fKeyEqualsComparer_)
                        , fData_ (&from->fData_, forIterableEnvelope)
                    {
                        RequireNotNull (from);
                    }

                public:
                    nonvirtual Rep_& operator= (const Rep_&) = delete;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                private:
                    KEY_EQUALS_COMPARER fKeyEqualsComparer_;

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
                        return fData_.GetLength ();
                    }
                    virtual bool IsEmpty () const override
                    {
                        return fData_.IsEmpty ();
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
                        using RESULT_TYPE     = Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>;
                        using SHARED_REP_TYPE = Traversal::IteratorBase::SharedPtrImplementationTemplate<IteratorRep_>;
                        auto iLink            = fData_.FindFirstThat (doToElement);
                        if (iLink == nullptr) {
                            return RESULT_TYPE::GetEmptyIterator ();
                        }
                        Rep_*           NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
                        SHARED_REP_TYPE resultRep      = Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                        resultRep->fIterator.SetCurrentLink (iLink);
                        // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
                        return RESULT_TYPE (resultRep);
                    }

                    // Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep overrides
                public:
                    virtual KeyEqualsCompareFunctionType GetKeyEqualsComparer () const override
                    {
                        return KeyEqualsCompareFunctionType{fKeyEqualsComparer_};
                    }
                    virtual _MappingRepSharedPtr CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
                    {
                        if (fData_.HasActiveIterators ()) {
                            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                            auto r = Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                            r->fData_.RemoveAll ();
                            return r;
                        }
                        else {
                            return Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSharedPtr<Rep_> (fKeyEqualsComparer_);
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
                        for (typename DataStructures::LinkedList<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::ForwardIterator it (&fData_); it.More (nullptr, true);) {
                            if (fKeyEqualsComparer_ (it.Current ().fKey, key)) {
                                if (item != nullptr) {
                                    *item = it.Current ().fValue;
                                }
                                return true;
                            }
                        }
                        if (item != nullptr) {
                            *item = nullopt;
                        }
                        return false;
                    }
                    virtual void Add (ArgByValueType<KEY_TYPE> key, ArgByValueType<MAPPED_VALUE_TYPE> newElt) override
                    {
                        using Traversal::kUnknownIteratorOwnerID;
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        for (typename DataStructureImplType_::ForwardIterator it (kUnknownIteratorOwnerID, &fData_); it.More (nullptr, true);) {
                            if (fKeyEqualsComparer_ (it.Current ().fKey, key)) {
                                fData_.SetAt (it, KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE> (key, newElt));
                                return;
                            }
                        }
                        fData_.Append (KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE> (key, newElt));
                    }
                    virtual void Remove (ArgByValueType<KEY_TYPE> key) override
                    {
                        using Traversal::kUnknownIteratorOwnerID;
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        for (typename DataStructureImplType_::ForwardIterator it (kUnknownIteratorOwnerID, &fData_); it.More (nullptr, true);) {
                            if (fKeyEqualsComparer_ (it.Current ().fKey, key)) {
                                fData_.RemoveAt (it);
                                return;
                            }
                        }
                    }
                    virtual void Remove (const Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& i) override
                    {
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock>            critSec{fData_};
                        const typename Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::IRep& ir = i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto& mir = dynamic_cast<const IteratorRep_&> (ir);
                        fData_.RemoveAt (mir.fIterator);
                    }
#if qDebug
                    virtual void AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
                    }
#endif

                private:
                    using DataStructureImplType_ = Private::PatchingDataStructures::LinkedList<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>;
                    using IteratorRep_           = Private::IteratorImplHelper_<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>, DataStructureImplType_>;

                private:
                    DataStructureImplType_ fData_;
                };

                /*
                 ********************************************************************************
                 *************** Mapping_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE> ****************
                 ********************************************************************************
                 */
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
                inline Mapping_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_LinkedList ()
                    : Mapping_LinkedList (std::equal_to<KEY_TYPE>{})
                {
                    AssertRepValidType_ ();
                }
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
                template <typename KEY_EQUALS_COMPARER, typename ENABLE_IF_IS_COMPARER>
                Mapping_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_LinkedList (const KEY_EQUALS_COMPARER& keyEqualsComparer, ENABLE_IF_IS_COMPARER*)
                    : inherited (inherited::template MakeSharedPtr<Rep_<KEY_EQUALS_COMPARER>> (keyEqualsComparer))
                {
                    AssertRepValidType_ ();
                }
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
                template <typename CONTAINER_OF_ADDABLE, typename ENABLE_IF>
                inline Mapping_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_LinkedList (const CONTAINER_OF_ADDABLE& src)
                    : Mapping_LinkedList ()
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
                template <typename COPY_FROM_ITERATOR_KEYVALUE>
                Mapping_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_LinkedList (COPY_FROM_ITERATOR_KEYVALUE start, COPY_FROM_ITERATOR_KEYVALUE end)
                    : Mapping_LinkedList ()
                {
                    this->AddAll (start, end);
                    AssertRepValidType_ ();
                }
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
                inline void Mapping_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>::AssertRepValidType_ () const
                {
#if qDebug
                    typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMemeber
#endif
                }
            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Mapping_LinkedList_inl_ */
