/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Containers_Concrete_KeyedCollection_stdset_inl_
#define _Stroika_Foundation_Containers_Concrete_KeyedCollection_stdset_inl_

#include "../../Configuration/Concepts.h"
#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"
#include "../STL/Compare.h"

#include "../DataStructures/STLContainerWrapper.h"
#include "../Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     ********************************************************************************
     ****** KeyedCollection_stdset<KEY_TYPE, MAPPED_VALUE_TYPE>::IImplRepBase_ ******
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    class KeyedCollection_stdset<T, KEY_TYPE, TRAITS>::IImplRepBase_ : public KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep {};

    /*
     ********************************************************************************
     ************ KeyedCollection_stdset<T, KEY_TYPE, TRAITS>::Rep_ *****************
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER>
    class KeyedCollection_stdset<T, KEY_TYPE, TRAITS>::Rep_
        : public IImplRepBase_,
          public Memory::UseBlockAllocationIfAppropriate<Rep_<KEY_EXTRACTOR, KEY_INORDER_COMPARER>> {
    public:
        static_assert (not is_reference_v<KEY_EXTRACTOR>);
        static_assert (not is_reference_v<KEY_INORDER_COMPARER>);

    private:
        using inherited = IImplRepBase_;

    private:
        [[no_unique_address]] const KEY_EXTRACTOR        fKeyExtractor_;
        [[no_unique_address]] const KEY_INORDER_COMPARER fKeyComparer_;

    public:
        Rep_ (const KEY_EXTRACTOR& keyExtractor, const KEY_INORDER_COMPARER& inorderComparer)
            : fKeyExtractor_{keyExtractor}
            , fKeyComparer_{inorderComparer}
#if qCompilerAndStdLib_deduce_template_arguments_CTOR_Buggy
            , fData_{SetInOrderComparer<KEY_EXTRACTOR, KEY_INORDER_COMPARER> {
                keyExtractor,
                inorderComparer
            }}
#else
            , fData_{SetInOrderComparer{keyExtractor, inorderComparer}}
#endif
        {
        }
        Rep_ (const Rep_& from) = default;

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

        // Iterable<T>::_IRep overrides
    public:
        virtual _IterableRepSharedPtr Clone () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Iterable<value_type>::template MakeSmartPtr<Rep_> (*this);
        }
        virtual Iterator<value_type> MakeIterator ([[maybe_unused]] const _IterableRepSharedPtr& thisSharedPtr) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_)};
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
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            fData_.Apply (doToElement);
        }
        virtual Iterator<T> Find (const _IterableRepSharedPtr& thisSharedPtr, const function<bool (ArgByValueType<value_type> item)>& that) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return this->_Find (thisSharedPtr, that);
        }
        virtual Iterator<value_type> Find_equal_to (const _IterableRepSharedPtr& thisSharedPtr, const ArgByValueType<value_type>& v) const override
        {
            // if doing a find by 'equals-to' - we already have this indexed
            auto found = fData_.find (v);
            Ensure (
                (found == fData_.end () and this->_Find_equal_to_default_implementation (thisSharedPtr, v) == Iterator<value_type>{nullptr}) or
                (found == Debug::UncheckedDynamicCast<const IteratorRep_&> (this->_Find_equal_to_default_implementation (thisSharedPtr, v).ConstGetRep ())
                              .fIterator.GetUnderlyingIteratorRep ()));
            return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_, found)};
        }

        // KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep overrides
    public:
        virtual KeyExtractorType GetKeyExtractor () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return fKeyExtractor_;
        }
        virtual KeyEqualityComparerType GetKeyEqualityComparer () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            auto                                                  keyComparer = fKeyComparer_;
            return KeyEqualityComparerType{
                [keyComparer] (const KEY_TYPE& lhs, const KEY_TYPE& rhs) { return keyComparer (lhs, rhs) and keyComparer (rhs, lhs); }};
        }
        virtual _KeyedCollectionRepSharedPtr CloneEmpty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Iterable<value_type>::template MakeSmartPtr<Rep_> (this->fKeyExtractor_, this->fKeyComparer_); // lose data but keep compare/extractor functions
        }
        virtual _KeyedCollectionRepSharedPtr CloneAndPatchIterator (Iterator<value_type>* i) const override
        {
            RequireNotNull (i);
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            auto                                                  result = Iterable<value_type>::template MakeSmartPtr<Rep_> (*this);
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual Iterable<KEY_TYPE> Keys () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return this->_Keys_Reference_Implementation ();
        }
        virtual bool Lookup (ArgByValueType<KeyType> key, optional<value_type>* item) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            auto                                                  i = fData_.find (key);
            if (i == fData_.end ()) {
                if (item != nullptr) {
                    *item = nullopt;
                }
                return false;
            }
            else {
                if (item != nullptr) {
                    *item = *i;
                }
                return true;
            }
        }
        virtual bool Add (ArgByValueType<value_type> item) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            pair<typename DataStructureImplType_::iterator, bool>  flagAndI = fData_.insert (item);
            if (flagAndI.second) {
                return true;
            }
            else {
                // @todo must patch!!!
                // in case of update, set<> wont update the value so we must remove and re-add, but todo that, use previous iterator as hint
                typename DataStructureImplType_::iterator hint = flagAndI.first;
                ++hint;
                fData_.erase (flagAndI.first);
                fData_.insert (hint, item);
                return false;
            }
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            auto& mir      = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            auto  nextStdI = fData_.erase (mir.fIterator.GetUnderlyingIteratorRep ());
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_, nextStdI)};
            }
        }
        virtual bool RemoveIf (ArgByValueType<KEY_TYPE> key) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            auto                                                   i = fData_.find (key);
            if (i != fData_.end ()) {
                fData_.erase (i);
                return true;
            }
            return false;
        }

    private:
        using DataStructureImplType_ = DataStructures::STLContainerWrapper<STDSET<KEY_EXTRACTOR, KEY_INORDER_COMPARER>>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                     fData_;
        [[no_unique_address]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     **************** KeyedCollection_stdset<KEY_TYPE,MAPPED_VALUE_TYPE> ************
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_INORDER_COMPARER, typename KEY_EXTRACTOR,
              enable_if_t<Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>*>
    inline KeyedCollection_stdset<T, KEY_TYPE, TRAITS>::KeyedCollection_stdset (KEY_INORDER_COMPARER&& keyComparer)
        : KeyedCollection_stdset{KEY_EXTRACTOR{}, forward<KEY_INORDER_COMPARER> (keyComparer)}
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER,
              enable_if_t<Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>*>
    inline KeyedCollection_stdset<T, KEY_TYPE, TRAITS>::KeyedCollection_stdset (KEY_EXTRACTOR&& keyExtractor, KEY_INORDER_COMPARER&& keyComparer)
        : inherited{inherited::template MakeSmartPtr<Rep_<remove_cvref_t<KEY_EXTRACTOR>, remove_cvref_t<KEY_INORDER_COMPARER>>> (
              forward<KEY_EXTRACTOR> (keyExtractor), forward<KEY_INORDER_COMPARER> (keyComparer))}
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename ITERABLE_OF_ADDABLE, typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER,
              enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<KeyedCollection_stdset<T, KEY_TYPE, TRAITS>, decay_t<ITERABLE_OF_ADDABLE>> and
                          Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>*>
    inline KeyedCollection_stdset<T, KEY_TYPE, TRAITS>::KeyedCollection_stdset (ITERABLE_OF_ADDABLE&& src)
        : KeyedCollection_stdset{typename TRAITS::DefaultKeyExtractor{}, less<KEY_TYPE>{}}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename ITERABLE_OF_ADDABLE, typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER,
              enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<KeyedCollection_stdset<T, KEY_TYPE, TRAITS>, decay_t<ITERABLE_OF_ADDABLE>> and
                          Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>*>
    inline KeyedCollection_stdset<T, KEY_TYPE, TRAITS>::KeyedCollection_stdset (KEY_INORDER_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src)
        : KeyedCollection_stdset{typename TRAITS::DefaultKeyExtractor{}, forward<KEY_INORDER_COMPARER> (keyComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER, typename ITERABLE_OF_ADDABLE,
              enable_if_t<KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and
                          Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () and Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>*>
    inline KeyedCollection_stdset<T, KEY_TYPE, TRAITS>::KeyedCollection_stdset (KEY_EXTRACTOR&& keyExtractor,
                                                                                KEY_INORDER_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src)
        : KeyedCollection_stdset{forward<KEY_EXTRACTOR> (keyExtractor), forward<KEY_INORDER_COMPARER> (keyComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename ITERATOR_OF_ADDABLE, typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER,
              enable_if_t<Configuration::IsIterator_v<ITERATOR_OF_ADDABLE> and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and
                          Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> ()>*>
    KeyedCollection_stdset<T, KEY_TYPE, TRAITS>::KeyedCollection_stdset (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : KeyedCollection_stdset{KEY_EXTRACTOR{}, KEY_INORDER_COMPARER{}}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename ITERATOR_OF_ADDABLE, typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER,
              enable_if_t<Configuration::IsIterator_v<ITERATOR_OF_ADDABLE> and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and
                          Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> ()>*>
    KeyedCollection_stdset<T, KEY_TYPE, TRAITS>::KeyedCollection_stdset (KEY_INORDER_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start,
                                                                         ITERATOR_OF_ADDABLE&& end)
        : KeyedCollection_stdset{KEY_EXTRACTOR{}, forward<KEY_INORDER_COMPARER> (keyComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER, typename ITERATOR_OF_ADDABLE,
              enable_if_t<KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and
                          Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () and Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>*>
    KeyedCollection_stdset<T, KEY_TYPE, TRAITS>::KeyedCollection_stdset (KEY_EXTRACTOR&& keyExtractor, KEY_INORDER_COMPARER&& keyComparer,
                                                                         ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : KeyedCollection_stdset{forward<KEY_EXTRACTOR> (keyExtractor), forward<KEY_INORDER_COMPARER> (keyComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void KeyedCollection_stdset<T, KEY_TYPE, TRAITS>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMember
        }
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_KeyedCollection_stdset_inl_ */
