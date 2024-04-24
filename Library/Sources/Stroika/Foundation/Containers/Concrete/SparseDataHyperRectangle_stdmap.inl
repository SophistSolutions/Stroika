/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include <map>

#include "Stroika/Foundation/Containers/DataStructures/STLContainerWrapper.h"
#include "Stroika/Foundation/Containers/Private/IteratorImplHelper.h"
#include "Stroika/Foundation/Debug/Cast.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     ********************************************************************************
     *********** SparseDataHyperRectangle_stdmap<T, INDEXES...>::Rep_ ***************
     ********************************************************************************
     */
    template <typename T, typename... INDEXES>
    class SparseDataHyperRectangle_stdmap<T, INDEXES...>::Rep_ : public DataHyperRectangle<T, INDEXES...>::_IRep,
                                                                 public Memory::UseBlockAllocationIfAppropriate<Rep_> {
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
        virtual shared_ptr<typename Iterable<tuple<T, INDEXES...>>::_IRep> Clone () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Memory::MakeSharedPtr<Rep_> (*this);
        }
        virtual Iterator<tuple<T, INDEXES...>> MakeIterator () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_)};
        }
        virtual size_t size () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return fData_.size ();
        }
        virtual bool empty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return fData_.empty ();
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement, [[maybe_unused]] Execution::SequencePolicy seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            fData_.Apply ([&] (const pair<tuple<INDEXES...>, T>& item) { doToElement (tuple_cat (tuple<T>{item.second}, item.first)); });
        }
        virtual Iterator<tuple<T, INDEXES...>> Find (const function<bool (ArgByValueType<value_type> item)>& that,
                                                     [[maybe_unused]] Execution::SequencePolicy              seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            using RESULT_TYPE = Iterator<tuple<T, INDEXES...>>;
            auto iLink        = const_cast<DataStructureImplType_&> (fData_).Find (
                [&] (const pair<tuple<INDEXES...>, T>& item) { return that (tuple_cat (tuple<T>{item.second}, item.first)); });
            if (iLink == fData_.end ()) {
                return RESULT_TYPE::GetEmptyIterator ();
            }
            unique_ptr<IteratorRep_> resultRep = make_unique<IteratorRep_> (&fData_, &fChangeCounts_);
            resultRep->fIterator.SetUnderlyingIteratorRep (iLink);
            return RESULT_TYPE{move (resultRep)};
        }

        // DataHyperRectangle<T, INDEXES...>::_IRep overrides
    public:
        virtual shared_ptr<typename DataHyperRectangle<T, INDEXES...>::_IRep> CloneEmpty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Memory::MakeSharedPtr<Rep_> (fDefaultValue_); // keep default, but lose data
        }
        virtual T GetAt (INDEXES... indexes) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            auto                                                  i = fData_.find (tuple<INDEXES...>{indexes...});
            if (i != fData_.end ()) {
                return i->second;
            }
            return fDefaultValue_;
        }
        virtual void SetAt (INDEXES... indexes, Configuration::ArgByValueType<T> v) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            if (v == fDefaultValue_) {
                auto i = fData_.find (tuple<INDEXES...>{indexes...});
                if (i != fData_.end ()) {
                    fData_.erase (i);
                }
            }
            else {
//  fData_.insert_or_assign (tuple<INDEXES...>{indexes...}, v);        // clang++-17 libstdc++ fails on this...
// fData_.emplace (make_pair(tuple<INDEXES...>{indexes...}, v));    // compiles but wrong semantivcs
//fData_.insert (make_pair(tuple<INDEXES...>{indexes...}, v));// compiles but wrong semantivcs
//fData_[tuple<INDEXES...>{indexes...}] = v;// clang++-17 libstdc++ fails on this...
//fData_[make_tuple (indexes...)] = v;
#if qCompilerAndStdLib_template_map_tuple_insert_Buggy
                if (not fData_.insert (make_pair (tuple<INDEXES...>{indexes...}, v)).second) {
                    // then its there and find works...
                    auto i = fData_.find (tuple<INDEXES...>{indexes...});
                    Assert (i != fData_.end ());
                    i->second = v;
                }
#else
                fData_.insert_or_assign (tuple<INDEXES...>{indexes...}, v);
#endif
            }
            fChangeCounts_.PerformedChange ();
        }

    private:
        // @todo see why we cannot just use Private::IterorImplHelper version of this!!!
        template <typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR = typename PATCHABLE_CONTAINER::ForwardIterator>
        class MyIteratorImplHelper_
            : public Iterator<tuple<T, INDEXES...>>::IRep,
              public Memory::UseBlockAllocationIfAppropriate<MyIteratorImplHelper_<PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR>> {
        private:
            using inherited = typename Iterator<tuple<T, INDEXES...>>::IRep;

            // public:
            //     using RepSmartPtr = typename Iterator<tuple<T, INDEXES...>>::RepSmartPtr;

        public:
            MyIteratorImplHelper_ ()                             = delete;
            MyIteratorImplHelper_ (const MyIteratorImplHelper_&) = default;
            explicit MyIteratorImplHelper_ (const PATCHABLE_CONTAINER*                                   data,
                                            [[maybe_unused]] const Private::ContainerDebugChangeCounts_* changeCounter = nullptr)
                : fIterator{data}
            {
                RequireNotNull (data);
                //fIterator.More (nullptr, true); //tmphack cuz current backend iterators require a first more() - fix that!
            }

        public:
            virtual ~MyIteratorImplHelper_ () = default;

            // Iterator<tuple<T, INDEXES...>>::IRep
        public:
            virtual unique_ptr<inherited> Clone () const override
            {
                return make_unique<MyIteratorImplHelper_> (*this);
            }
            virtual void More (optional<tuple<T, INDEXES...>>* result, bool advance) override
            {
                RequireNotNull (result);
                if (advance) [[likely]] {
                    Require (not fIterator.Done ()); // new requirement since Stroika 2.1b14
                    ++fIterator;
                }
                if (fIterator.Done ()) [[unlikely]] {
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
                return fIterator.Equals (rrhs->fIterator);
            }

        public:
            mutable PATCHABLE_CONTAINER_ITERATOR fIterator;
        };

    private:
        using DataStructureImplType_ = DataStructures::STLContainerWrapper<map<tuple<INDEXES...>, T>>;
        using IteratorRep_           = MyIteratorImplHelper_<DataStructureImplType_>;

    private:
        T                                                          fDefaultValue_;
        DataStructureImplType_                                     fData_;
        [[no_unique_address]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     ************** SparseDataHyperRectangle_stdmap<T, INDEXES...> ******************
     ********************************************************************************
     */
    template <typename T, typename... INDEXES>
    SparseDataHyperRectangle_stdmap<T, INDEXES...>::SparseDataHyperRectangle_stdmap (Configuration::ArgByValueType<T> defaultItem)
        : inherited{Memory::MakeSharedPtr<Rep_> (defaultItem)}
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
    inline SparseDataHyperRectangle_stdmap<T, INDEXES...>&
    SparseDataHyperRectangle_stdmap<T, INDEXES...>::operator= (const SparseDataHyperRectangle_stdmap<T, INDEXES...>& rhs)
    {
        AssertRepValidType_ ();
        inherited::operator= (static_cast<const inherited&> (rhs));
        AssertRepValidType_ ();
        return *this;
    }
    template <typename T, typename... INDEXES>
    inline void SparseDataHyperRectangle_stdmap<T, INDEXES...>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<Rep_> tmp{this}; // for side-effect of AssertMember
        }
    }

}
