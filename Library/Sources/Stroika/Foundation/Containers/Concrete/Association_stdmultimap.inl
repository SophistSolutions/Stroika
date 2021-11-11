/*
* Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
*/
#ifndef _Stroika_Foundation_Containers_Concrete_Association_stdmultimap_inl_
#define _Stroika_Foundation_Containers_Concrete_Association_stdmultimap_inl_

/*
********************************************************************************
***************************** Implementation Details ***************************
********************************************************************************
*/
#include <map>

#include "../../Memory/BlockAllocated.h"
#include "../STL/Compare.h"

#include "../DataStructures/STLContainerWrapper.h"
#include "../Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
    */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class Association_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::IImplRep_ : public Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep {
    private:
        using inherited = typename Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep;
    };

    /*
    */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class Association_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_ : public IImplRep_, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
    private:
        using inherited = IImplRep_;

    public:
        Rep_ ()                 = default;
        Rep_ (const Rep_& from) = default;

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

        // Iterable<T>::_IRep overrides
    public:
        virtual _IterableRepSharedPtr Clone () const override
        {
            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
            return Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSmartPtr<Rep_> (*this);
        }
        virtual Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> MakeIterator () const override
        {
            Rep_* NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
            return Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> (Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSmartPtr<IteratorRep_> (&NON_CONST_THIS->fData_));
        }
        virtual size_t GetLength () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> critSec{fData_};
            fData_.Invariant ();
            return fData_.size ();
        }
        virtual bool IsEmpty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> critSec{fData_};
            fData_.Invariant ();
            return fData_.empty ();
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> critSec{fData_};
            // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
            // use iterator (which currently implies lots of locks) with this->_Apply ()
            fData_.Apply (doToElement);
        }
        virtual Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> FindFirstThat (const function<bool (ArgByValueType<value_type> item)>& doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> critSec{fData_};
            return this->_FindFirstThat (doToElement);
        }

        // Association<KEY_TYPE, MAPPED_VALUE_TYPE, typename TRAITS::AssociationTraitsType>::_IRep overrides
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
            auto                                                        i = fData_.find (key);
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
            lock_guard<const Debug::AssertExternallySynchronizedMutex> critSec{fData_};
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
            lock_guard<const Debug::AssertExternallySynchronizedMutex> critSec{fData_};
            fData_.Invariant ();
            auto i = fData_.find (key);
            if (i != fData_.end ()) {
                fData_.erase_WithPatching (i);
            }
        }
        virtual void Remove (const Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& i) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedMutex>                critSec{fData_};
            const typename Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::IRep& ir = i.GetRep ();
            AssertMember (&ir, IteratorRep_);
            auto& mir = dynamic_cast<const IteratorRep_&> (ir);
            mir.fIterator.RemoveCurrent ();
        }

    private:
        using DataStructureImplType_ = DataStructures::STLContainerWrapper<map<KEY_TYPE, MAPPED_VALUE_TYPE, less<KEY_TYPE>>>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>, DataStructureImplType_>;

    private:
        DataStructureImplType_ fData_;
    };

    /*
    ********************************************************************************
    ************* Association_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE> *************
    ********************************************************************************
    */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Association_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_stdmultimap ()
        : inherited (inherited::template MakeSmartPtr<Rep_> ())
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_ADDABLE> and not is_convertible_v<const CONTAINER_OF_ADDABLE*, const Association_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>*>>*>
    inline Association_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_stdmultimap (const CONTAINER_OF_ADDABLE& src)
        : Association_stdmultimap ()
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Association_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::AssertRepValidType_ () const
    {
#if qDebug
        typename inherited::template _SafeReadRepAccessor<IImplRep_> tmp{this}; // for side-effect of AssertMemeber
#endif
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_Association_stdmultimap_inl_ */
