/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Set_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Set_LinkedList_inl_

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
                 */
                template <typename T, typename TRAITS>
                class Set_LinkedList<T, TRAITS>::IImplRep_ : public Set<T, TRAITS>::_IRep {
                private:
                    using inherited = typename Set<T, TRAITS>::_IRep;

                protected:
                    using _SetRepSharedPtr    = typename inherited::_SetRepSharedPtr;
                    using _APPLY_ARGTYPE      = typename inherited::_APPLY_ARGTYPE;
                    using _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;
                };

                /*
                 */
                template <typename T, typename TRAITS>
                class Set_LinkedList<T, TRAITS>::Rep_ : public IImplRep_ {
                private:
                    using inherited = IImplRep_;

                public:
                    using _IterableRepSharedPtr = typename Iterable<T>::_IterableRepSharedPtr;
                    using _SetRepSharedPtr      = typename inherited::_SetRepSharedPtr;
                    using _APPLY_ARGTYPE        = typename inherited::_APPLY_ARGTYPE;
                    using _APPLYUNTIL_ARGTYPE   = typename inherited::_APPLYUNTIL_ARGTYPE;

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
                    virtual _IterableRepSharedPtr Clone (IteratorOwnerID forIterableEnvelope) const override
                    {
                        // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                        return Iterable<T>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                    }
                    virtual Iterator<T> MakeIterator (IteratorOwnerID suggestedOwner) const override
                    {
                        Rep_* NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
                        return Iterator<T> (Iterator<T>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_));
                    }
                    virtual size_t GetLength () const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        return fData_.GetLength ();
                    }
                    virtual bool IsEmpty () const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        return fData_.IsEmpty ();
                    }
                    virtual void Apply (_APPLY_ARGTYPE doToElement) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
                        // use iterator (which currently implies lots of locks) with this->_Apply ()
                        fData_.Apply (doToElement);
                    }
                    virtual Iterator<T> FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        using RESULT_TYPE     = Iterator<T>;
                        using SHARED_REP_TYPE = Traversal::IteratorBase::SharedPtrImplementationTemplate<IteratorRep_>;
                        auto iLink            = fData_.FindFirstThat (doToElement);
                        if (iLink == nullptr) {
                            return RESULT_TYPE::GetEmptyIterator ();
                        }
                        Rep_*           NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
                        SHARED_REP_TYPE resultRep      = Iterator<T>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                        resultRep->fIterator.SetCurrentLink (iLink);
                        // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
                        return RESULT_TYPE (resultRep);
                    }

                    // Set<T, TRAITS>::_IRep overrides
                public:
                    virtual _SetRepSharedPtr CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
                    {
                        if (fData_.HasActiveIterators ()) {
                            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                            auto r = Iterable<T>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                            r->fData_.RemoveAll ();
                            return r;
                        }
                        else {
                            return Iterable<T>::template MakeSharedPtr<Rep_> ();
                        }
                    }
                    virtual bool Equals (const typename Set<T, TRAITS>::_IRep& rhs) const override
                    {
                        return this->_Equals_Reference_Implementation (rhs);
                    }
                    virtual bool Contains (ArgByValueType<T> item) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        return fData_.Lookup (item, Common::NEW_EQUALS_COMPARER<typename TRAITS::EqualsCompareFunctionType>{}) != nullptr;
                    }
                    virtual Memory::Optional<T> Lookup (ArgByValueType<T> item) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        const T*                                                        l = fData_.Lookup (item, Common::NEW_EQUALS_COMPARER<typename TRAITS::EqualsCompareFunctionType>{});
                        return (l == nullptr) ? Memory::Optional<T> () : Memory::Optional<T> (*l);
                    }
                    virtual void Add (ArgByValueType<T> item) override
                    {
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> lg{fData_};
                        // safe to use UnpatchedForwardIterator cuz locked and no updates
                        for (typename DataStructureImplType_::UnpatchedForwardIterator it (&fData_); it.More (nullptr, true);) {
                            if (TRAITS::EqualsCompareFunctionType::Equals (it.Current (), item)) {
                                return;
                            }
                        }
                        fData_.Prepend (item);
                    }
                    virtual void Remove (ArgByValueType<T> item) override
                    {
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> lg{fData_};
                        using Traversal::kUnknownIteratorOwnerID;
                        for (typename DataStructureImplType_::ForwardIterator it (kUnknownIteratorOwnerID, &fData_); it.More (nullptr, true);) {
                            if (TRAITS::EqualsCompareFunctionType::Equals (it.Current (), item)) {
                                fData_.RemoveAt (it);
                                return;
                            }
                        }
                    }
                    virtual void Remove (const Iterator<T>& i) override
                    {
                        const typename Iterator<T>::IRep& ir = i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto&                                                          mir = dynamic_cast<const IteratorRep_&> (ir);
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> lg{fData_};
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
                    using DataStructureImplType_ = Private::PatchingDataStructures::LinkedList<T, DataStructures::LinkedList_DefaultTraits<T, typename TRAITS::EqualsCompareFunctionType>>;
                    using IteratorRep_           = typename Private::IteratorImplHelper_<T, DataStructureImplType_>;

                private:
                    DataStructureImplType_ fData_;
                };

                /*
                 ********************************************************************************
                 ************************ Set_LinkedList<T, TRAITS> *****************************
                 ********************************************************************************
                 */
                template <typename T, typename TRAITS>
                inline Set_LinkedList<T, TRAITS>::Set_LinkedList ()
                    : inherited (inherited::template MakeSharedPtr<Rep_> ())
                {
                    AssertRepValidType_ ();
                }
                template <typename T, typename TRAITS>
                inline Set_LinkedList<T, TRAITS>::Set_LinkedList (const Set_LinkedList<T, TRAITS>& src)
                    : inherited (src)
                {
                    AssertRepValidType_ ();
                }
                template <typename T, typename TRAITS>
                inline Set_LinkedList<T, TRAITS>::Set_LinkedList (const initializer_list<T>& src)
                    : Set_LinkedList ()
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template <typename T, typename TRAITS>
                template <typename CONTAINER_OF_T, typename ENABLE_IF>
                inline Set_LinkedList<T, TRAITS>::Set_LinkedList (const CONTAINER_OF_T& src)
                    : Set_LinkedList ()
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template <typename T, typename TRAITS>
                template <typename COPY_FROM_ITERATOR_OF_T>
                inline Set_LinkedList<T, TRAITS>::Set_LinkedList (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                    : Set_LinkedList ()
                {
                    AddAll (start, end);
                    AssertRepValidType_ ();
                }
                template <typename T, typename TRAITS>
                inline Set_LinkedList<T, TRAITS>& Set_LinkedList<T, TRAITS>::operator= (const Set_LinkedList<T, TRAITS>& rhs)
                {
                    AssertRepValidType_ ();
                    inherited::operator= (static_cast<const inherited&> (rhs));
                    AssertRepValidType_ ();
                    return *this;
                }
                template <typename T, typename TRAITS>
                inline void Set_LinkedList<T, TRAITS>::AssertRepValidType_ () const
                {
#if qDebug
                    typename inherited::template _SafeReadRepAccessor<IImplRep_> tmp{this}; // for side-effect of AssertMember
#endif
                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Set_LinkedList_inl_ */
