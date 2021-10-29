/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SparseDataHyperRectangle_stdmap_inl_
#define _Stroika_Foundation_Containers_Concrete_SparseDataHyperRectangle_stdmap_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <map>

#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"

#include "../DataStructures/STLContainerWrapper.h"
#include "../Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    using Traversal::IteratorOwnerID;

    /*
     ********************************************************************************
     *********** SparseDataHyperRectangle_stdmap<T, INDEXES...>::Rep_ ***************
     ********************************************************************************
     */
    template <typename T, typename... INDEXES>
    class SparseDataHyperRectangle_stdmap<T, INDEXES...>::Rep_ : public DataHyperRectangle<T, INDEXES...>::_IRep, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
    private:
        using inherited = typename DataHyperRectangle<T, INDEXES...>::_IRep;

    public:
        Rep_ (Configuration::ArgByValueType<T> defaultItem)
            : fDefaultValue_{defaultItem}
        {
        }
        Rep_ (const Rep_& from) = delete;
        Rep_ (const Rep_* from, [[maybe_unused]] IteratorOwnerID forIterableEnvelope)
            : fData_{from->fData_}
        {
            RequireNotNull (from);
        }

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

        // Iterable<tuple<T, INDEXES...>>::_IRep overrides
    public:
        virtual _IterableRepSharedPtr Clone (IteratorOwnerID forIterableEnvelope) const override
        {
            return Iterable<tuple<T, INDEXES...>>::template MakeSmartPtr<Rep_> (this, forIterableEnvelope);
        }
        virtual Iterator<tuple<T, INDEXES...>> MakeIterator (IteratorOwnerID suggestedOwner) const override
        {
            return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (suggestedOwner, &fData_)};
            //return Iterator<tuple<T, INDEXES...>> (Iterator<tuple<T, INDEXES...>>::template MakeSmartPtr<IteratorRep_> (suggestedOwner, &fData_));
        }
        virtual size_t GetLength () const override
        {
            return fData_.size ();
        }
        virtual bool IsEmpty () const override
        {
            return fData_.empty ();
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement) const override
        {
            fData_.Apply (
                [&] (const pair<tuple<INDEXES...>, T>& item) {
                    doToElement (tuple_cat (tuple<T>{item.second}, item.first));
                });
        }
        virtual Iterator<tuple<T, INDEXES...>> FindFirstThat (const function<bool (ArgByValueType<value_type> item)>& doToElement, IteratorOwnerID suggestedOwner) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            using RESULT_TYPE = Iterator<tuple<T, INDEXES...>>;
            auto iLink        = const_cast<DataStructureImplType_&> (fData_).FindFirstThat (
                [&] (const pair<tuple<INDEXES...>, T>& item) {
                    return doToElement (tuple_cat (tuple<T>{item.second}, item.first));
                });
            if (iLink == fData_.end ()) {
                return RESULT_TYPE::GetEmptyIterator ();
            }
            Traversal::IteratorBase::PtrImplementationTemplate<IteratorRep_> resultRep = Iterator<T>::template MakeSmartPtr<IteratorRep_> (suggestedOwner, &fData_);
            resultRep->fIterator.SetCurrentLink (iLink);
            return RESULT_TYPE (move (resultRep));
        }

        // DataHyperRectangle<T, INDEXES...>::_IRep overrides
    public:
        virtual _DataHyperRectangleRepSharedPtr CloneEmpty ([[maybe_unused]] IteratorOwnerID forIterableEnvelope) const override
        {
            return Iterable<tuple<T, INDEXES...>>::template MakeSmartPtr<Rep_> (fDefaultValue_);
        }
        virtual T GetAt (INDEXES... indexes) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            auto                                                       i = fData_.find (tuple<INDEXES...>{indexes...});
            if (i != fData_.end ()) {
                return i->second;
            }
            return fDefaultValue_;
        }
        virtual void SetAt (INDEXES... indexes, Configuration::ArgByValueType<T> v) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            if (v == fDefaultValue_) {
                auto i = fData_.find (tuple<INDEXES...> (indexes...));
                if (i != fData_.end ()) {
                    fData_.erase (i);
                }
            }
            else {
// @todo - add patching...
#if qCompilerAndStdLib_insert_or_assign_Buggy
                fData_.erase (tuple<INDEXES...> (indexes...));
                fData_.insert (typename map<tuple<INDEXES...>, T>::value_type (tuple<INDEXES...> (indexes...), v));
#else
                fData_.insert_or_assign (tuple<INDEXES...> (indexes...), v);
#endif
            }
        }

    private:
        template <typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR = typename PATCHABLE_CONTAINER::ForwardIterator>
        class MyIteratorImplHelper_ : public Iterator<tuple<T, INDEXES...>>::IRep, public Memory::UseBlockAllocationIfAppropriate<MyIteratorImplHelper_<PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR>> {
        private:
            using inherited = typename Iterator<tuple<T, INDEXES...>>::IRep;

        public:
            using RepSmartPtr = typename Iterator<tuple<T, INDEXES...>>::RepSmartPtr;

        public:
            MyIteratorImplHelper_ ()                             = delete;
            MyIteratorImplHelper_ (const MyIteratorImplHelper_&) = default;
            explicit MyIteratorImplHelper_ (IteratorOwnerID /*owner*/, const PATCHABLE_CONTAINER* data)
                : fIterator{data}
            {
                RequireNotNull (data);
                fIterator.More (static_cast<pair<tuple<INDEXES...>, T>*> (nullptr), true); //tmphack cuz current backend iterators require a first more() - fix that!
            }

        public:
            virtual ~MyIteratorImplHelper_ () = default;

            // Iterator<tuple<T, INDEXES...>>::IRep
        public:
            virtual RepSmartPtr Clone () const override
            {
                return Iterator<tuple<T, INDEXES...>>::template MakeSmartPtr<MyIteratorImplHelper_> (*this);
            }
            virtual IteratorOwnerID GetOwner () const override
            {
                return nullptr;
                //return fIterator.GetOwner ();
            }
            virtual void More (optional<tuple<T, INDEXES...>>* result, bool advance) override
            {
                RequireNotNull (result);
                // NOTE: the reason this is Debug::AssertExternallySynchronizedLock, is because we only modify data on the newly cloned (breakreferences)
                // iterator, and that must be in the thread (so externally synchronized) of the modifier
                // shared_lock<const Debug::AssertExternallySynchronizedLock> lg (*fIterator.GetPatchableContainerHelper ());
                More_SFINAE_ (result, advance);
            }
            virtual bool Equals (const typename Iterator<tuple<T, INDEXES...>>::IRep* rhs) const override
            {
                RequireNotNull (rhs);
                using ActualIterImplType_       = MyIteratorImplHelper_<PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR>;
                const ActualIterImplType_* rrhs = Debug::UncheckedDynamicCast<const ActualIterImplType_*> (rhs);
                AssertNotNull (rrhs);
                //          shared_lock<const Debug::AssertExternallySynchronizedLock> critSec1 (*fIterator.GetPatchableContainerHelper ());
                //          shared_lock<const Debug::AssertExternallySynchronizedLock> critSec2 (*rrhs->fIterator.GetPatchableContainerHelper ());
                return fIterator.Equals (rrhs->fIterator);
            }

        private:
            /*
             *  More_SFINAE_ () trick is cuz if types are the same, we can just pass pointer, but if they differ, we need
             *  a temporary, and to copy.
             */
            template <typename CHECK_KEY = typename PATCHABLE_CONTAINER::value_type>
            nonvirtual void More_SFINAE_ (optional<tuple<T, INDEXES...>>* result, bool advance, enable_if_t<is_same_v<T, CHECK_KEY>>* = 0)
            {
                RequireNotNull (result);
                fIterator.More (result, advance);
            }
            template <typename CHECK_KEY = typename PATCHABLE_CONTAINER::value_type>
            nonvirtual void More_SFINAE_ (optional<tuple<T, INDEXES...>>* result, bool advance, enable_if_t<!is_same_v<T, CHECK_KEY>>* = 0)
            {
                RequireNotNull (result);
                optional<pair<tuple<INDEXES...>, T>> tmp;
                fIterator.More (&tmp, advance);
                if (tmp.has_value ()) {
                    *result = tuple_cat (tuple<T>{tmp->second}, tmp->first);
                }
                else {
                    *result = nullopt;
                }
            }

        public:
            mutable PATCHABLE_CONTAINER_ITERATOR fIterator;
        };

    private:
        using DataStructureImplType_ = DataStructures::STLContainerWrapper<map<tuple<INDEXES...>, T>>;
        using IteratorRep_           = MyIteratorImplHelper_<DataStructureImplType_>;

    private:
        T                      fDefaultValue_;
        DataStructureImplType_ fData_;
    };

    /*
     ********************************************************************************
     ************** SparseDataHyperRectangle_stdmap<T, INDEXES...> ******************
     ********************************************************************************
     */
    template <typename T, typename... INDEXES>
    SparseDataHyperRectangle_stdmap<T, INDEXES...>::SparseDataHyperRectangle_stdmap (Configuration::ArgByValueType<T> defaultItem)
        : inherited (inherited::template MakeSmartPtr<Rep_> (defaultItem))
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename... INDEXES>
    inline SparseDataHyperRectangle_stdmap<T, INDEXES...>::SparseDataHyperRectangle_stdmap (const SparseDataHyperRectangle_stdmap<T, INDEXES...>& src)
        : inherited (static_cast<const inherited&> (src))
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename... INDEXES>
    inline SparseDataHyperRectangle_stdmap<T, INDEXES...>& SparseDataHyperRectangle_stdmap<T, INDEXES...>::operator= (const SparseDataHyperRectangle_stdmap<T, INDEXES...>& rhs)
    {
        AssertRepValidType_ ();
        inherited::operator= (static_cast<const inherited&> (rhs));
        AssertRepValidType_ ();
        return *this;
    }
    template <typename T, typename... INDEXES>
    inline void SparseDataHyperRectangle_stdmap<T, INDEXES...>::AssertRepValidType_ () const
    {
#if qDebug
        typename inherited::template _SafeReadRepAccessor<Rep_> tmp{this}; // for side-effect of AssertMember
#endif
    }

}
#endif /* _Stroika_Foundation_Containers_Concrete_SparseDataHyperRectangle_stdmap_inl_ */
