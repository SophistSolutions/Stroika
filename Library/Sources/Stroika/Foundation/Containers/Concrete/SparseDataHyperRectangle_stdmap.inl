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
        Rep_ (const Rep_& from) = default;

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

        // Iterable<tuple<T, INDEXES...>>::_IRep overrides
    public:
        virtual _IterableRepSharedPtr Clone () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return Iterable<tuple<T, INDEXES...>>::template MakeSmartPtr<Rep_> (*this);
        }
        virtual Iterator<tuple<T, INDEXES...>> MakeIterator () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_)};
        }
        virtual size_t size () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return fData_.size ();
        }
        virtual bool empty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return fData_.empty ();
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            fData_.Apply (
                [&] (const pair<tuple<INDEXES...>, T>& item) {
                    doToElement (tuple_cat (tuple<T>{item.second}, item.first));
                });
        }
        virtual Iterator<tuple<T, INDEXES...>> Find (const function<bool (ArgByValueType<value_type> item)>& that) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            using RESULT_TYPE = Iterator<tuple<T, INDEXES...>>;
            auto iLink        = const_cast<DataStructureImplType_&> (fData_).Find (
                [&] (const pair<tuple<INDEXES...>, T>& item) {
                    return that (tuple_cat (tuple<T>{item.second}, item.first));
                });
            if (iLink == fData_.end ()) {
                return RESULT_TYPE::GetEmptyIterator ();
            }
            Traversal::IteratorBase::PtrImplementationTemplate<IteratorRep_> resultRep = Iterator<T>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_);
            resultRep->fIterator.SetUnderlyingIteratorRep (iLink);
            return RESULT_TYPE{move (resultRep)};
        }
        virtual Iterator<tuple<T, INDEXES...>> Find_equal_to (const ArgByValueType<value_type>& v) const override
        {
            return this->_Find_equal_to_default_implementation (v);
        }

        // DataHyperRectangle<T, INDEXES...>::_IRep overrides
    public:
        virtual _DataHyperRectangleRepSharedPtr CloneEmpty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return Iterable<tuple<T, INDEXES...>>::template MakeSmartPtr<Rep_> (fDefaultValue_); // keep default, but lose data
        }
        virtual T GetAt (INDEXES... indexes) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> critSec{fData_};
            auto                                                        i = fData_.find (tuple<INDEXES...>{indexes...});
            if (i != fData_.end ()) {
                return i->second;
            }
            return fDefaultValue_;
        }
        virtual void SetAt (INDEXES... indexes, Configuration::ArgByValueType<T> v) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
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
            fChangeCounts_.PerformedChange ();
        }

    private:
        // @todo see why we cannot just use Private::IterorImplHelper version of this!!!
        template <typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR = typename PATCHABLE_CONTAINER::ForwardIterator>
        class MyIteratorImplHelper_ : public Iterator<tuple<T, INDEXES...>>::IRep, public Memory::UseBlockAllocationIfAppropriate<MyIteratorImplHelper_<PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR>> {
        private:
            using inherited = typename Iterator<tuple<T, INDEXES...>>::IRep;

        public:
            using RepSmartPtr = typename Iterator<tuple<T, INDEXES...>>::RepSmartPtr;

        public:
            MyIteratorImplHelper_ ()                             = delete;
            MyIteratorImplHelper_ (const MyIteratorImplHelper_&) = default;
            explicit MyIteratorImplHelper_ (const PATCHABLE_CONTAINER* data, [[maybe_unused]] const Private::ContainerDebugChangeCounts_* changeCounter = nullptr)
                : fIterator{data}
            {
                RequireNotNull (data);
                //fIterator.More (nullptr, true); //tmphack cuz current backend iterators require a first more() - fix that!
            }

        public:
            virtual ~MyIteratorImplHelper_ () = default;

            // Iterator<tuple<T, INDEXES...>>::IRep
        public:
            virtual RepSmartPtr Clone () const override
            {
                return Iterator<tuple<T, INDEXES...>>::template MakeSmartPtr<MyIteratorImplHelper_> (*this);
            }
            virtual void More (optional<tuple<T, INDEXES...>>* result, bool advance) override
            {
                RequireNotNull (result);
                if (advance) [[LIKELY_ATTR]] {
                    Require (not fIterator.Done ()); // new requirement since Stroika 2.1b14
                    ++fIterator;
                }
                if (fIterator.Done ()) [[UNLIKELY_ATTR]] {
                    *result = nullopt;
                }
                else {
                    auto tmp = fIterator.Current ();
                    *result  = tuple_cat (tuple<T>{tmp.second}, tmp.first);
                }
            }
            virtual bool Equals (const typename Iterator<tuple<T, INDEXES...>>::IRep* rhs) const override
            {
                RequireNotNull (rhs);
                using ActualIterImplType_       = MyIteratorImplHelper_<PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR>;
                const ActualIterImplType_* rrhs = Debug::UncheckedDynamicCast<const ActualIterImplType_*> (rhs);
                AssertNotNull (rrhs);
                //          shared_lock<const Debug::AssertExternallySynchronizedMutex> critSec1 (*fIterator.GetPatchableContainerHelper ());
                //          shared_lock<const Debug::AssertExternallySynchronizedMutex> critSec2 (*rrhs->fIterator.GetPatchableContainerHelper ());
                return fIterator.Equals (rrhs->fIterator);
            }

        public:
            mutable PATCHABLE_CONTAINER_ITERATOR fIterator;
        };

    private:
        using DataStructureImplType_ = DataStructures::STLContainerWrapper<map<tuple<INDEXES...>, T>>;
        using IteratorRep_           = MyIteratorImplHelper_<DataStructureImplType_>;

    private:
        T                                                               fDefaultValue_;
        DataStructureImplType_                                          fData_;
        [[NO_UNIQUE_ADDRESS_ATTR]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     ************** SparseDataHyperRectangle_stdmap<T, INDEXES...> ******************
     ********************************************************************************
     */
    template <typename T, typename... INDEXES>
    SparseDataHyperRectangle_stdmap<T, INDEXES...>::SparseDataHyperRectangle_stdmap (Configuration::ArgByValueType<T> defaultItem)
        : inherited{inherited::template MakeSmartPtr<Rep_> (defaultItem)}
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename... INDEXES>
    inline SparseDataHyperRectangle_stdmap<T, INDEXES...>::SparseDataHyperRectangle_stdmap (const SparseDataHyperRectangle_stdmap<T, INDEXES...>& src)
        : inherited{static_cast<const inherited&> (src)}
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
