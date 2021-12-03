/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedKeyedCollection_stdset_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedKeyedCollection_stdset_inl_

#include "../../Configuration/Concepts.h"
#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"
#include "../STL/Compare.h"

#include "../DataStructures/STLContainerWrapper.h"
#include "../Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     ********************************************************************************
     *** SortedKeyedCollection_stdset<KEY_TYPE, MAPPED_VALUE_TYPE>::IImplRepBase_ ***
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    class SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::IImplRepBase_ : public SortedKeyedCollection<T, KEY_TYPE, TRAITS>::_IRep {
    };

    /*
     ********************************************************************************
     ********** SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::Rep_ *************
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER>
    class SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::Rep_ : public IImplRepBase_, public Memory::UseBlockAllocationIfAppropriate<Rep_<KEY_EXTRACTOR, KEY_INORDER_COMPARER>> {
    public:
        static_assert (not is_reference_v<KEY_EXTRACTOR>);
        static_assert (not is_reference_v<KEY_INORDER_COMPARER>);

    private:
        using inherited = IImplRepBase_;

    private:
        [[NO_UNIQUE_ADDRESS_ATTR]] const KEY_EXTRACTOR        fKeyExtractor_;
        [[NO_UNIQUE_ADDRESS_ATTR]] const KEY_INORDER_COMPARER fKeyComparer_;

    public:
        Rep_ (const KEY_EXTRACTOR& keyExtractor, const KEY_INORDER_COMPARER& inorderComparer)
            // clang-format off
            : fKeyExtractor_ {keyExtractor}
            , fKeyComparer_ {  inorderComparer }
            #if qCompilerAndStdLib_deduce_template_arguments_CTOR_Buggy
            , fData_{SetInOrderComparer<KEY_EXTRACTOR,KEY_INORDER_COMPARER>{keyExtractor, inorderComparer}}
            #else
            , fData_{SetInOrderComparer{keyExtractor, inorderComparer}}
            #endif
        // clang-format on
        {
        }
        Rep_ (const Rep_& from) = default;

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

        // Iterable<T>::_IRep overrides
    public:
        virtual _IterableRepSharedPtr Clone () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return Iterable<value_type>::template MakeSmartPtr<Rep_> (*this);
        }
        virtual Iterator<value_type> MakeIterator () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_)};
        }
        virtual size_t GetLength () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return fData_.size ();
        }
        virtual bool IsEmpty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return fData_.empty ();
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            fData_.Apply (doToElement);
        }
        virtual Iterator<T> Find (const function<bool (ArgByValueType<value_type> item)>& that) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return this->_Find (that);
        }
        virtual Iterator<value_type> Find_equal_to (const ArgByValueType<value_type>& v) const override
        {
            // if doing a find by 'equals-to' - we already have this indexed
            auto found = fData_.find (v);
            Ensure ((found == fData_.end () and this->_Find_equal_to_default_implementation (v) == Iterator<value_type>{nullptr}) or (found == Debug::UncheckedDynamicCast<const IteratorRep_&> (this->_Find_equal_to_default_implementation (v).ConstGetRep ()).fIterator.GetUnderlyingIteratorRep ()));
            return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_, found)};
        }

        // KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep overrides
    public:
        virtual KeyExtractorType GetKeyExtractor () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return fKeyExtractor_;
        }
        virtual KeyEqualityComparerType GetKeyEqualityComparer () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            auto                                                        keyComparer = fKeyComparer_;
            return KeyEqualityComparerType{[keyComparer] (const KEY_TYPE& lhs, const KEY_TYPE& rhs) {
                return keyComparer (lhs, rhs) and keyComparer (rhs, lhs);
            }};
        }
        virtual _KeyedCollectionRepSharedPtr CloneEmpty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return Iterable<value_type>::template MakeSmartPtr<Rep_> (this->fKeyExtractor_, this->fKeyComparer_); // keep extractor/comparer but lose data in clone
        }
        virtual _KeyedCollectionRepSharedPtr CloneAndPatchIterator (Iterator<value_type>* i) const override
        {
            RequireNotNull (i);
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            auto                                                        result = Iterable<value_type>::template MakeSmartPtr<Rep_> (*this);
            auto&                                                       mir    = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual Iterable<KEY_TYPE> Keys () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return this->_Keys_Reference_Implementation ();
        }
        virtual bool Lookup (ArgByValueType<KeyType> key, optional<value_type>* item) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            auto                                                        i = fData_.find (key);
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
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            pair<typename DataStructureImplType_::iterator, bool> flagAndI = fData_.insert (item);
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
            Require (not i.Done ());
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            auto&                                                 mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            Assert (mir.fIterator.GetReferredToData () == &fData_);
            auto nextIResult = fData_.erase (mir.fIterator.GetUnderlyingIteratorRep ());
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_, nextIResult)};
            }
        }
        virtual bool RemoveIf (ArgByValueType<KEY_TYPE> key) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            auto                                                  i = fData_.find (key);
            if (i != fData_.end ()) {
                fData_.erase (i);
                fChangeCounts_.PerformedChange ();
                return true;
            }
            return false;
        }

        // SortedKeyedCollection<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep overrides
    public:
        virtual KeyInOrderKeyComparerType GetInOrderKeyComparer () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return KeyInOrderKeyComparerType{fKeyComparer_};
        }

    private:
        using DataStructureImplType_ = DataStructures::STLContainerWrapper<STDSET<KEY_EXTRACTOR, KEY_INORDER_COMPARER>>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                          fData_;
        [[NO_UNIQUE_ADDRESS_ATTR]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     ************ SortedKeyedCollection_stdset<KEY_TYPE,MAPPED_VALUE_TYPE> **********
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_INORDER_COMPARER,
              typename KEY_EXTRACTOR,
              enable_if_t<
                  Common::IsPotentiallyComparerRelation_Old<KEY_TYPE, KEY_INORDER_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>*>
    inline SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_stdset (KEY_INORDER_COMPARER&& keyComparer)
        : SortedKeyedCollection_stdset{KEY_EXTRACTOR{}, forward<KEY_INORDER_COMPARER> (keyComparer)}
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_EXTRACTOR,
              typename KEY_INORDER_COMPARER,
              enable_if_t<
                  Common::IsPotentiallyComparerRelation_Old<KEY_TYPE, KEY_INORDER_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>*>
    SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_stdset (KEY_EXTRACTOR&& keyExtractor, KEY_INORDER_COMPARER&& keyComparer)
        : inherited{inherited::template MakeSmartPtr<Rep_<Configuration::remove_cvref_t<KEY_EXTRACTOR>, Configuration::remove_cvref_t<KEY_INORDER_COMPARER>>> (forward<KEY_EXTRACTOR> (keyExtractor), forward<KEY_INORDER_COMPARER> (keyComparer))}
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename CONTAINER_OF_ADDABLE,
              typename KEY_EXTRACTOR,
              typename KEY_INORDER_COMPARER,
              enable_if_t<
                  Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>> and Common::IsPotentiallyComparerRelation_Old<KEY_TYPE, KEY_INORDER_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>*>
    inline SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_stdset (CONTAINER_OF_ADDABLE&& src)
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename CONTAINER_OF_ADDABLE,
              typename KEY_EXTRACTOR,
              typename KEY_INORDER_COMPARER,
              enable_if_t<
                  Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>> and Common::IsPotentiallyComparerRelation_Old<KEY_TYPE, KEY_INORDER_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>*>
    inline SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_stdset (KEY_INORDER_COMPARER&& keyComparer, CONTAINER_OF_ADDABLE&& src)
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER, typename CONTAINER_OF_ADDABLE,
              enable_if_t<
                  KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsPotentiallyComparerRelation_Old<KEY_TYPE, KEY_INORDER_COMPARER> () and Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T>>*>
    inline SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_stdset (KEY_EXTRACTOR&& keyExtractor, KEY_INORDER_COMPARER&& keyComparer, CONTAINER_OF_ADDABLE&& src)
        : SortedKeyedCollection_stdset{forward<KEY_EXTRACTOR> (keyExtractor), forward<KEY_INORDER_COMPARER> (keyComparer)}
    {
        AssertRepValidType_ ();
        this->AddAll (src);
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE,
              typename KEY_EXTRACTOR,
              typename KEY_INORDER_COMPARER,
              enable_if_t<
                  Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE> and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsPotentiallyComparerRelation_Old<KEY_TYPE, KEY_INORDER_COMPARER> ()>*>
    inline SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_stdset (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        : SortedKeyedCollection_stdset{KEY_EXTRACTOR{}, KEY_INORDER_COMPARER{}}
    {
        this->AddAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE,
              typename KEY_EXTRACTOR,
              typename KEY_INORDER_COMPARER,
              enable_if_t<
                  Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE> and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsPotentiallyComparerRelation_Old<KEY_TYPE, KEY_INORDER_COMPARER> ()>*>
    inline SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_stdset (KEY_INORDER_COMPARER&& keyComparer, COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        : SortedKeyedCollection_stdset{KEY_EXTRACTOR{}, forward<KEY_INORDER_COMPARER> (keyComparer)}
    {
        this->AddAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER, typename COPY_FROM_ITERATOR_OF_ADDABLE,
              enable_if_t<
                  KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsPotentiallyComparerRelation_Old<KEY_TYPE, KEY_INORDER_COMPARER> () and Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE>>*>
    inline SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_stdset (KEY_EXTRACTOR&& keyExtractor, KEY_INORDER_COMPARER&& keyComparer, COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        : SortedKeyedCollection_stdset{forward<KEY_EXTRACTOR> (keyExtractor), forward<KEY_INORDER_COMPARER> (keyComparer)}
    {
        this->AddAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::AssertRepValidType_ () const
    {
#if qDebug
        typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMember
#endif
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_SortedKeyedCollection_stdmap_inl_ */
