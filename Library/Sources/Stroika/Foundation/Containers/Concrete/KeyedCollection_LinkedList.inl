/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_KeyedCollection_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_KeyedCollection_LinkedList_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"

#include "../DataStructures/LinkedList.h"
#include "../Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    using Traversal::IteratorOwnerID;

    /*
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    class KeyedCollection_LinkedList<T, KEY_TYPE, TRAITS>::IImplRep_ : public KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep {
    };

    /*
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_EXTRACTOR, typename KEY_EQUALS_COMPARER>
    class KeyedCollection_LinkedList<T, KEY_TYPE, TRAITS>::Rep_ : public IImplRep_, public Memory::UseBlockAllocationIfAppropriate<Rep_<KEY_EXTRACTOR, KEY_EQUALS_COMPARER>> {
    private:
        using inherited = IImplRep_;
        [[NO_UNIQUE_ADDRESS_ATTR]] const KEY_EXTRACTOR       fKeyExtractor_;
        [[NO_UNIQUE_ADDRESS_ATTR]] const KEY_EQUALS_COMPARER fKeyComparer_;

    public:
        using _IterableRepSharedPtr        = typename Iterable<T>::_IterableRepSharedPtr;
        using _KeyedCollectionRepSharedPtr = typename inherited::_IRepSharedPtr;
        using _APPLY_ARGTYPE               = typename inherited::_APPLY_ARGTYPE;
        using _APPLYUNTIL_ARGTYPE          = typename inherited::_APPLYUNTIL_ARGTYPE;

    public:
        Rep_ (const KEY_EXTRACTOR& keyExtractor, const KEY_EQUALS_COMPARER& keyComparer)
            : fKeyExtractor_{keyExtractor}
            , fKeyComparer_{keyComparer}
        {
        }
        Rep_ (const Rep_& from) = delete;
        Rep_ (Rep_* from, [[maybe_unused]] IteratorOwnerID forIterableEnvelope)
            : fKeyExtractor_{from->fKeyExtractor_}
            , fKeyComparer_{from->fKeyComparer_}
            , fData_{from->fData_}
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
            return fData_.GetLength ();
        }
        virtual bool IsEmpty () const override
        {
            return fData_.IsEmpty ();
        }
        virtual void Apply (_APPLY_ARGTYPE doToElement) const override
        {
            // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
            // use iterator (which currently implies lots of locks) with this->_Apply ()
            fData_.Apply (doToElement);
        }
        virtual Iterator<T> FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            using RESULT_TYPE     = Iterator<T>;
            using SHARED_REP_TYPE = Traversal::IteratorBase::PtrImplementationTemplate<IteratorRep_>;
            auto iLink            = fData_.FindFirstThat (doToElement);
            if (iLink == nullptr) {
                return RESULT_TYPE::GetEmptyIterator ();
            }
            Rep_*           NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
            SHARED_REP_TYPE resultRep      = Iterator<T>::template MakeSmartPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
            resultRep->fIterator.SetCurrentLink (iLink);
            // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
            return RESULT_TYPE (move (resultRep));
        }

        // KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep overrides
    public:
        virtual KeyExtractorType GetKeyExtractor () const override
        {
            return fKeyExtractor_;
        }
        virtual KeyEqualityComparerType GetKeyEqualityComparer () const override
        {
            return KeyEqualityComparerType{fKeyComparer_};
        }
        virtual _KeyedCollectionRepSharedPtr CloneEmpty ([[maybe_unused]] IteratorOwnerID forIterableEnvelope) const override
        {
            return Iterable<T>::template MakeSmartPtr<Rep_> (this->fKeyExtractor_, this->fKeyComparer_);
        }
        virtual Iterable<KEY_TYPE> Keys () const override
        {
            return this->_Keys_Reference_Implementation ();
        }
        virtual bool Lookup (ArgByValueType<KeyType> key, optional<value_type>* item) const override
        {
            if (auto i = this->FindFirstThat ([this, &key] (ArgByValueType<T> item) { return fKeyComparer_ (fKeyExtractor_ (item), key); }, nullptr)) {
                if (item != nullptr) {
                    *item = *i;
                }
                return true;
            }
            return false;
        }
        virtual bool Add (ArgByValueType<T> item) override
        {
            KEY_TYPE key{fKeyExtractor_ (item)};
            if (auto i = this->FindFirstThat ([this, &key] (ArgByValueType<T> item) { return fKeyComparer_ (fKeyExtractor_ (item), key); }, nullptr)) {
                auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
                fData_.SetAt (mir.fIterator, item);
                return false;
            }
            else {
                fData_.Prepend (item);
                return true;
            }
        }
        virtual void Remove (const Iterator<T>& i) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            auto&                                                     mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            fData_.RemoveAt (mir.fIterator);
        }
        virtual bool Remove (ArgByValueType<KEY_TYPE> key) override
        {
            if (auto i = this->FindFirstThat ([this, &key] (ArgByValueType<T> item) { return fKeyComparer_ (fKeyExtractor_ (item), key); }, nullptr)) {
                Remove (i);
                return true;
            }
            return false;
        }
#if qDebug
        virtual void AssertNoIteratorsReferenceOwner ([[maybe_unused]] IteratorOwnerID oBeingDeleted) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            //            fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
        }
#endif

    private:
        using DataStructureImplType_ = DataStructures::LinkedList<T>;
        using IteratorRep_           = typename Private::IteratorImplHelper2_<T, DataStructureImplType_>;

    private:
        DataStructureImplType_ fData_;
    };

    /*
     ********************************************************************************
     ************************** KeyedCollection_LinkedList<T> ***********************
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_EQUALS_COMPARER, typename KEY_EXTRACTOR, enable_if_t<Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>*>
    inline KeyedCollection_LinkedList<T, KEY_TYPE, TRAITS>::KeyedCollection_LinkedList (KEY_EQUALS_COMPARER&& keyComparer)
        : KeyedCollection_LinkedList{KEY_EXTRACTOR{}, keyComparer}
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_EXTRACTOR,
              typename KEY_EQUALS_COMPARER,
              enable_if_t<
                  Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>*>
    inline KeyedCollection_LinkedList<T, KEY_TYPE, TRAITS>::KeyedCollection_LinkedList (KEY_EXTRACTOR&& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer)
        : inherited (inherited::template MakeSmartPtr<Rep_<KEY_EXTRACTOR, KEY_EQUALS_COMPARER>> (forward<KEY_EXTRACTOR> (keyExtractor), forward<KEY_EQUALS_COMPARER> (keyComparer)))
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename CONTAINER_OF_ADDABLE,
              typename KEY_EXTRACTOR,
              typename KEY_EQUALS_COMPARER,
              enable_if_t<
                  Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<KeyedCollection_LinkedList<T, KEY_TYPE, TRAITS>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>> and Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>*>
    inline KeyedCollection_LinkedList<T, KEY_TYPE, TRAITS>::KeyedCollection_LinkedList (CONTAINER_OF_ADDABLE&& src)
        : KeyedCollection_LinkedList{typename TRAITS::DefaultKeyExtractor{}, equal_to<KEY_TYPE>{}}
    {
        AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename CONTAINER_OF_ADDABLE,
              typename KEY_EXTRACTOR,
              typename KEY_EQUALS_COMPARER,
              enable_if_t<
                  Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<KeyedCollection_LinkedList<T, KEY_TYPE, TRAITS>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>> and Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>*>
    inline KeyedCollection_LinkedList<T, KEY_TYPE, TRAITS>::KeyedCollection_LinkedList (KEY_EQUALS_COMPARER&& keyComparer, CONTAINER_OF_ADDABLE&& src)
        : KeyedCollection_LinkedList{typename TRAITS::DefaultKeyExtractor{}, forward<KEY_EQUALS_COMPARER> (keyComparer)}
    {
        AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_EXTRACTOR, typename KEY_EQUALS_COMPARER, typename CONTAINER_OF_ADDABLE, enable_if_t<KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> () and Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T>>*>
    inline KeyedCollection_LinkedList<T, KEY_TYPE, TRAITS>::KeyedCollection_LinkedList (KEY_EXTRACTOR&& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer, CONTAINER_OF_ADDABLE&& src)
        : KeyedCollection_LinkedList{forward<KEY_EXTRACTOR> (keyExtractor), forward<KEY_EQUALS_COMPARER> (keyComparer)}
    {
        AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE,
              typename KEY_EXTRACTOR,
              typename KEY_EQUALS_COMPARER,
              enable_if_t<
                  Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE> and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> ()>*>
    KeyedCollection_LinkedList<T, KEY_TYPE, TRAITS>::KeyedCollection_LinkedList (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        : KeyedCollection_LinkedList{KEY_EXTRACTOR{}, KEY_EQUALS_COMPARER{}}
    {
        AddAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE,
              typename KEY_EXTRACTOR,
              typename KEY_EQUALS_COMPARER,
              enable_if_t<
                  Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE> and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> ()>*>
    KeyedCollection_LinkedList<T, KEY_TYPE, TRAITS>::KeyedCollection_LinkedList (KEY_EQUALS_COMPARER&& keyComparer, COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        : KeyedCollection_LinkedList{KEY_EXTRACTOR{}, forward<KEY_EQUALS_COMPARER> (keyComparer)}
    {
        AddAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_EXTRACTOR, typename KEY_EQUALS_COMPARER, typename COPY_FROM_ITERATOR_OF_ADDABLE,
              enable_if_t<
                  KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> () and Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE>>*>
    KeyedCollection_LinkedList<T, KEY_TYPE, TRAITS>::KeyedCollection_LinkedList (KEY_EXTRACTOR&& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer, COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        : KeyedCollection_LinkedList{forward<KEY_EXTRACTOR> (keyExtractor), forward<KEY_EQUALS_COMPARER> (keyComparer)}
    {
        AddAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void KeyedCollection_LinkedList<T, KEY_TYPE, TRAITS>::AssertRepValidType_ () const
    {
#if qDebug
        typename inherited::template _SafeReadRepAccessor<IImplRep_> tmp{this}; // for side-effect of AssertMemeber
#endif
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_KeyedCollection_LinkedList_inl_ */
