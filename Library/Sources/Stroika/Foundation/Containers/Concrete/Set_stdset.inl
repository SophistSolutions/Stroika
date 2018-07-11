/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Set_stdset_inl_
#define _Stroika_Foundation_Containers_Concrete_Set_stdset_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <set>

#include "../../Common/Compare.h"
#include "../../Memory/BlockAllocated.h"

#include "../Private/IteratorImplHelper.h"
#include "../Private/PatchingDataStructures/STLContainerWrapper.h"
#include "../STL/Compare.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Concrete {

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
                    using _IterableRepSharedPtr = typename Iterable<T>::_IterableRepSharedPtr;
                    using _SetRepSharedPtr      = typename inherited::_SetRepSharedPtr;
                    using _APPLY_ARGTYPE        = typename inherited::_APPLY_ARGTYPE;
                    using _APPLYUNTIL_ARGTYPE   = typename inherited::_APPLYUNTIL_ARGTYPE;
                    using EqualityComparerType  = typename Set<T>::EqualityComparerType;

                public:
                    Rep_ (const INORDER_COMPARER& inorderComparer)
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
                        return Iterable<T>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                    }
                    virtual Iterator<T> MakeIterator (IteratorOwnerID suggestedOwner) const override
                    {
                        Rep_* NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
                        return Iterator<T> (Iterator<T>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_));
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
                    virtual Iterator<T> FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override
                    {
                        shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        return this->_FindFirstThat (doToElement, suggestedOwner);
                    }

                    // Set<T>::_IRep overrides
                public:
                    virtual EqualityComparerType GetEqualsComparer () const override
                    {
                        return EqualityComparerType{Common::mkEqualsComparerAdapter (fData_.key_comp ())};
                    }
                    virtual _SetRepSharedPtr CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
                    {
                        if (fData_.HasActiveIterators ()) {
                            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                            auto r = Iterable<T>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                            r->fData_.clear_WithPatching ();
                            return r;
                        }
                        else {
                            return Iterable<T>::template MakeSharedPtr<Rep_> (fData_.key_comp ());
                        }
                    }
                    virtual bool Equals (const typename Set<T>::_IRep& rhs) const override
                    {
                        return this->_Equals_Reference_Implementation (rhs);
                    }
                    virtual bool Contains (ArgByValueType<T> item) const override
                    {
                        return fData_.Contains (item);
                    }
                    virtual optional<T> Lookup (ArgByValueType<T> item) const override
                    {
                        shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        auto                                                       i = fData_.find (item);
                        return (i == fData_.end ()) ? optional<T> () : optional<T> (*i);
                    }
                    virtual void Add (ArgByValueType<T> item) override
                    {
                        lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        fData_.insert (item);
                        // must patch!!!
                    }
                    virtual void Remove (ArgByValueType<T> item) override
                    {
                        lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        fData_.Invariant ();
                        auto i = fData_.find (item);
                        if (i != fData_.end ()) {
                            fData_.erase_WithPatching (i);
                        }
                    }
                    virtual void Remove (const Iterator<T>& i) override
                    {
                        lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        const typename Iterator<T>::IRep&                         ir = i.GetRep ();
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
                    using DataStructureImplType_ = Private::PatchingDataStructures::STLContainerWrapper<set<T, INORDER_COMPARER>>;
                    using IteratorRep_           = typename Private::IteratorImplHelper_<T, DataStructureImplType_>;

                private:
                    DataStructureImplType_ fData_;
                };

                /*
                 ********************************************************************************
                 ********************************** Set_setset<T> *******************************
                 ********************************************************************************
                 */
                template <typename T>
                inline Set_stdset<T>::Set_stdset ()
                    : Set_stdset (less<T>{})
                {
                    AssertRepValidType_ ();
                }
                template <typename T>
                template <typename INORDER_COMPARER>
                inline Set_stdset<T>::Set_stdset (const INORDER_COMPARER& inorderComparer)
                    : inherited (inherited::template MakeSharedPtr<Rep_<INORDER_COMPARER>> (inorderComparer))
                {
                    AssertRepValidType_ ();
                }
                template <typename T>
                inline Set_stdset<T>::Set_stdset (const initializer_list<T>& src)
                    : Set_stdset ()
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template <typename T>
                inline Set_stdset<T>::Set_stdset (const EqualityComparerType& equalsComparer, const initializer_list<T>& src)
                    : Set_stdset (equalsComparer)
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template <typename T>
                template <typename CONTAINER_OF_T, enable_if_t<Configuration::has_beginend<CONTAINER_OF_T>::value and !is_convertible_v<const CONTAINER_OF_T*, const Set_stdset<T>*>>*>
                inline Set_stdset<T>::Set_stdset (const CONTAINER_OF_T& src)
                    : Set_stdset ()
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template <typename T>
                template <typename CONTAINER_OF_T, enable_if_t<Configuration::has_beginend<CONTAINER_OF_T>::value and !is_convertible_v<const CONTAINER_OF_T*, const Set_stdset<T>*>>*>
                inline Set_stdset<T>::Set_stdset (const EqualityComparerType& equalsComparer, const CONTAINER_OF_T& src)
                    : Set_stdset (equalsComparer)
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template <typename T>
                template <typename COPY_FROM_ITERATOR_T>
                inline Set_stdset<T>::Set_stdset (COPY_FROM_ITERATOR_T start, COPY_FROM_ITERATOR_T end)
                    : Set_stdset ()
                {
                    this->AddAll (start, end);
                    AssertRepValidType_ ();
                }
                template <typename T>
                template <typename COPY_FROM_ITERATOR_T>
                inline Set_stdset<T>::Set_stdset (const EqualityComparerType& equalsComparer, COPY_FROM_ITERATOR_T start, COPY_FROM_ITERATOR_T end)
                    : Set_stdset (equalsComparer)
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
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Set_stdset_inl_ */
