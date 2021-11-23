/*
* Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
*/
#ifndef _Stroika_Foundation_Containers_Concrete_Association_Array_inl_
#define _Stroika_Foundation_Containers_Concrete_Association_Array_inl_

/*
********************************************************************************
***************************** Implementation Details ***************************
********************************************************************************
*/
#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"

#include "../DataStructures/Array.h"

namespace Stroika ::Foundation::Containers ::Concrete {

    /*
     ********************************************************************************
     *********** Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_ ***************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_ : public Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
    private:
        using inherited = typename Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep;

    public:
        Rep_ ()                 = default;
        Rep_ (const Rep_& from) = default;

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

        // Iterable<T>::_IRep overrides
    public:
        virtual _IterableRepSharedPtr Clone () const override
        {
            return Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSmartPtr<Rep_> (*this);
        }
        virtual Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> MakeIterator () const override
        {
            return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_)};
        }
        virtual size_t GetLength () const override
        {
            return fData_.GetLength ();
        }
        virtual bool IsEmpty () const override
        {
            return fData_.GetLength () == 0;
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> critSec{fData_};
            fData_.Apply (doToElement);
        }
        virtual Iterator<value_type> Find (const function<bool (ArgByValueType<value_type> item)>& that) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> critSec{fData_};
            size_t                                                      i = fData_.Find (that);
            if (i == fData_.GetLength ()) {
                return nullptr;
            }
            return Iterator<value_type>{Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSmartPtr<IteratorRep_> (&fData_, i)};
        }
        virtual Iterator<value_type> Find_equal_to ([[maybe_unused]] const ArgByValueType<value_type>& v) const override
        {
            return this->_Find_equal_to_default_implementation (v);
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
            shared_lock<const Debug::AssertExternallySynchronizedMutex> critSec{fData_};
            for (typename DataStructureImplType_::ForwardIterator it (&fData_); not it.Done (); ++it) {
                if (KeyEqualsCompareFunctionType::Equals (it.Current ().fKey, key)) {
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
            lock_guard<const Debug::AssertExternallySynchronizedMutex> critSec{fData_};
            for (typename DataStructureImplType_::ForwardIterator it (&fData_); not it.Done (); ++it) {
                if (KeyEqualsCompareFunctionType::Equals (it.Current ().fKey, key)) {
                    fData_[it.CurrentIndex ()].fValue = newElt;
                    return;
                }
            }
            fData_.InsertAt (fData_.GetLength (), KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE> (key, newElt));
        }
        virtual void Remove (ArgByValueType<KEY_TYPE> key) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedMutex> critSec{fData_};
            for (typename DataStructureImplType_::ForwardIterator it (&fData_); not it.Done (); ++it) {
                if (KeyEqualsCompareFunctionType::Equals (it.Current ().fKey, key)) {
                    fData_.RemoveAt (it.CurrentIndex ());
                    return;
                }
            }
        }
        virtual void Remove (const Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& i) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedMutex>                critSec{fData_};
            const typename Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::IRep& ir = i.GetRep ();
            AssertMember (&ir, IteratorRep_);
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (ir);
            fData_.RemoveAt (mir.fIterator);
        }

    public:
        using KeyEqualsCompareFunctionType = typename Association<KEY_TYPE, MAPPED_VALUE_TYPE>::KeyEqualsCompareFunctionType;

    private:
        using DataStructureImplType_ = DataStructures::Array<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>;

    private:
        using IteratorRep_ = typename Private::IteratorImplHelper_<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>, DataStructureImplType_>;

    private:
        DataStructureImplType_ fData_;
    };

    /*
     ********************************************************************************
     ************** Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE> ******************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_Array ()
        : inherited (inherited::template MakeSmartPtr<Rep_> ())
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_ADDABLE> and not is_base_of_v<Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>*>
    inline Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_Array (CONTAINER_OF_ADDABLE&& src)
        : Association_Array ()
    {
        this->AddAll (forward<CONTAINER_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename COPY_FROM_ITERATOR_KEY_T>
    Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_Array (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end)
        : Association_Array ()
    {
        this->AddAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::shrink_to_fit ()
    {
        using _SafeReadWriteRepAccessor = typename Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template _SafeReadWriteRepAccessor<Rep_>;
        _SafeReadWriteRepAccessor                                   accessor{this};
        shared_lock<const Debug::AssertExternallySynchronizedMutex> critSec{accessor._ConstGetRep ().fData_};
        accessor._GetWriteableRep ().fData_.shrink_to_fit ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline size_t Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::GetCapacity () const
    {
        using _SafeReadRepAccessor = typename Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template _SafeReadRepAccessor<Rep_>;
        _SafeReadRepAccessor                                        accessor{this};
        shared_lock<const Debug::AssertExternallySynchronizedMutex> critSec{accessor._ConstGetRep ().fData_};
        return accessor._ConstGetRep ().fData_.GetCapacity ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::SetCapacity (size_t slotsAlloced)
    {
        using _SafeReadWriteRepAccessor = typename Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template _SafeReadWriteRepAccessor<Rep_>;
        _SafeReadWriteRepAccessor                                   accessor{this};
        shared_lock<const Debug::AssertExternallySynchronizedMutex> critSec{accessor._ConstGetRep ().fData_};
        accessor._GetWriteableRep ().fData_.SetCapacity (slotsAlloced);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline size_t Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::capacity () const
    {
        return GetCapacity ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::reserve (size_t slotsAlloced)
    {
        SetCapacity (slotsAlloced);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::AssertRepValidType_ () const
    {
#if qDebug
        typename inherited::template _SafeReadRepAccessor<Rep_> tmp{this}; // for side-effect of AssertMemeber
#endif
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Association_Array_inl_ */
