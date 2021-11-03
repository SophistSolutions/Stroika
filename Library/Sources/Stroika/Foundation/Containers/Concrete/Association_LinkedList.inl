/*
* Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
*/
#ifndef _Stroika_Foundation_Containers_Concrete_Association_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Association_LinkedList_inl_

/*
********************************************************************************
***************************** Implementation Details ***************************
********************************************************************************
*/
#include "../../Memory/BlockAllocated.h"

#include "../DataStructures/LinkedList.h"
#include "../Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
    ********************************************************************************
    *********** Association_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_***********
    ********************************************************************************
    */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class Association_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_ : public Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
    private:
        using inherited = typename Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep;

    public:
        Rep_ ()                 = default;
        Rep_ (const Rep_& from) = default;

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

    private:
        equal_to<KEY_TYPE> fKeyEqualsComparer_;

        // Iterable<T>::_IRep overrides
    public:
        virtual _IterableRepSharedPtr Clone () const override
        {
            return Iterable<value_type>::template MakeSmartPtr<Rep_> (*this);
        }
        virtual Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> MakeIterator () const override
        {
            return Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> (Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSmartPtr<IteratorRep_> (&fData_));
        }
        virtual size_t GetLength () const override
        {
            return fData_.GetLength ();
        }
        virtual bool IsEmpty () const override
        {
            return fData_.IsEmpty ();
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement) const override
        {
            // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
            // use iterator (which currently implies lots of locks) with this->_Apply ()
            fData_.Apply (doToElement);
        }
        virtual Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> FindFirstThat (const function<bool (ArgByValueType<value_type> item)>& doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            using RESULT_TYPE = Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>;
            auto iLink        = fData_.FindFirstThat (doToElement);
            if (iLink == nullptr) {
                return RESULT_TYPE::GetEmptyIterator ();
            }
            Traversal::IteratorBase::PtrImplementationTemplate<IteratorRep_> resultRep = Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSmartPtr<IteratorRep_> (&fData_);
            resultRep->fIterator.SetCurrentLink (iLink);
            return RESULT_TYPE (move (resultRep));
        }

        // Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep overrides
    public:
        virtual _AssociationRepSharedPtr CloneEmpty () const override
        {
            return Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSmartPtr<Rep_> ();
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
            for (typename DataStructures::LinkedList<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::ForwardIterator it (&fData_); not it.Done (); ++it) {
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
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            for (typename DataStructureImplType_::ForwardIterator it (&fData_); not it.Done (); ++it) {
                if (fKeyEqualsComparer_ (it.Current ().fKey, key)) {
                    fData_.SetAt (it, KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE> (key, newElt));
                    return;
                }
            }
            fData_.Append (KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE> (key, newElt));
        }
        virtual void Remove (ArgByValueType<KEY_TYPE> key) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            for (typename DataStructureImplType_::ForwardIterator it (&fData_); not it.Done (); ++it) {
                if (fKeyEqualsComparer_ (it.Current ().fKey, key)) {
                    fData_.RemoveAt (it);
                    return;
                }
            }
        }
        virtual void Remove (const Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& i) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock>                 critSec{fData_};
            const typename Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::IRep& ir = i.ConstGetRep ();
            AssertMember (&ir, IteratorRep_);
            auto& mir = dynamic_cast<const IteratorRep_&> (ir);
            fData_.RemoveAt (mir.fIterator);
        }

    public:
        using KeyEqualsCompareFunctionType = typename Association<KEY_TYPE, MAPPED_VALUE_TYPE>::KeyEqualsCompareFunctionType;

    private:
        using DataStructureImplType_ = DataStructures::LinkedList<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>;
        using IteratorRep_           = Private::IteratorImplHelper_<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>, DataStructureImplType_>;

    private:
        DataStructureImplType_ fData_;
    };

    /*
        ********************************************************************************
        *********** Association_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE> ****************
        ********************************************************************************
        */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Association_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_LinkedList ()
        : inherited (inherited::template MakeSmartPtr<Rep_> ())
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_ADDABLE> and not is_base_of_v<Association_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>*>
    inline Association_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_LinkedList (CONTAINER_OF_ADDABLE&& src)
        : Association_LinkedList ()
    {
        this->AddAll (forward<CONTAINER_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename COPY_FROM_ITERATOR_KEY_T>
    Association_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_LinkedList (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end)
        : Association_LinkedList ()
    {
        this->AddAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Association_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>::AssertRepValidType_ () const
    {
#if qDebug
        typename inherited::template _SafeReadRepAccessor<Rep_> tmp{this}; // for side-effect of AssertMemeber
#endif
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_Association_LinkedList_inl_ */
