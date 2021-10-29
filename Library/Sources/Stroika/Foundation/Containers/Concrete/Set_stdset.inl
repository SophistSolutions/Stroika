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
#include <set>

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
            return Iterable<T>::template MakeSmartPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
        }
        virtual Iterator<T> MakeIterator (IteratorOwnerID suggestedOwner) const override
        {
            Rep_* NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
            return Iterator<T> (Iterator<T>::template MakeSmartPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_));
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
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
            // use iterator (which currently implies lots of locks) with this->_Apply ()
            fData_.Apply (doToElement);
        }
        virtual Iterator<T> FindFirstThat (const function<bool (ArgByValueType<value_type> item)>& doToElement, IteratorOwnerID suggestedOwner) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            return this->_FindFirstThat (doToElement, suggestedOwner);
        }

        // Set<T>::_IRep overrides
    public:
        virtual ElementEqualityComparerType GetElementEqualsComparer () const override
        {
            return ElementEqualityComparerType{Common::EqualsComparerAdapter (fData_.key_comp ())};
        }
        virtual _SetRepSharedPtr CloneEmpty ([[maybe_unused]] IteratorOwnerID forIterableEnvelope) const override
        {
            return Iterable<T>::template MakeSmartPtr<Rep_> (fData_.key_comp ());
        }
        virtual _SetRepSharedPtr CloneAndPatchIterator (Iterator<value_type>* i, IteratorOwnerID obsoleteForIterableEnvelope) const override
        {
            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
            auto                                                      result = Iterable<value_type>::template MakeSmartPtr<Rep_> (const_cast<Rep_*> (this), obsoleteForIterableEnvelope);
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            auto&                                                     mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual bool Equals (const typename Iterable<value_type>::_IRep& rhs) const override
        {
            return this->_Equals_Reference_Implementation (rhs);
        }
        virtual bool Contains (ArgByValueType<value_type> item) const override
        {
            return fData_.Contains (item);
        }
        virtual optional<T> Lookup (ArgByValueType<T> item) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            auto                                                       i = fData_.find (item);
            return (i == fData_.end ()) ? optional<T> () : optional<T> (*i);
        }
        virtual void Add (ArgByValueType<value_type> item) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            fData_.insert (item);
        }
        virtual void Remove (ArgByValueType<value_type> item) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            fData_.Invariant ();
            auto i = fData_.find (item);
            if (i != fData_.end ()) {
                fData_.erase (i);
            }
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Require (not i.Done ());
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            auto&                                                     mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            Assert (mir.fIterator.fData == &fData_);
            auto nextIR = fData_.erase (mir.fIterator.fStdIterator);
            if (nextI != nullptr) {
                auto resultRep = Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (i.GetOwner (), &fData_);
                resultRep->fIterator.SetCurrentLink (nextIR);
                *nextI = Iterator<value_type>{move (resultRep)};
            }
        }
#if qDebug
        virtual void AssertNoIteratorsReferenceOwner ([[maybe_unused]] IteratorOwnerID oBeingDeleted) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            //  fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
        }
#endif

    private:
        using DataStructureImplType_ = DataStructures::STLContainerWrapper<set<T, INORDER_COMPARER>>;
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
        : Set_stdset{less<T>{}}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename INORDER_COMPARER>
    inline Set_stdset<T>::Set_stdset (const INORDER_COMPARER& inorderComparer)
        : inherited (inherited::template MakeSmartPtr<Rep_<INORDER_COMPARER>> (inorderComparer))
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
        : Set_stdset (equalsComparer)
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
        : Set_stdset (equalsComparer)
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

#endif /* _Stroika_Foundation_Containers_Concrete_Set_stdset_inl_ */
