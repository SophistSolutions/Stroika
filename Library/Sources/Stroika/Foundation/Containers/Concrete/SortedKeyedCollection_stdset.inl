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

#include <set>

#include "../../Memory/BlockAllocated.h"
#include "../STL/Compare.h"

#include "../Private/IteratorImplHelper.h"
#include "../Private/PatchingDataStructures/STLContainerWrapper.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     ********************************************************************************
     *** SortedKeyedCollection_stdset<KEY_TYPE, MAPPED_VALUE_TYPE>::IImplRepBase_ ***
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    class SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::IImplRepBase_ : public SortedKeyedCollection<T, KEY_TYPE, TRAITS>::_IRep {
    private:
        using inherited = typename SortedKeyedCollection<T, KEY_TYPE, TRAITS>::_IRep;

#if qCompilerAndStdLib_TemplateTypenameReferenceToBaseOfBaseClassMemberNotFound_Buggy
    protected:
        using _IterableRepSharedPtr = typename inherited::_IterableRepSharedPtr;
        using _APPLY_ARGTYPE        = typename inherited::_APPLY_ARGTYPE;
        using _APPLYUNTIL_ARGTYPE   = typename inherited::_APPLYUNTIL_ARGTYPE;
#endif

    protected:
        using _APPLY_ARGTYPE      = typename inherited::_APPLY_ARGTYPE;
        using _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;
    };

    /*
     ********************************************************************************
     ********** SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::Rep_ *************
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER>
    class SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::Rep_ : public IImplRepBase_, public Memory::UseBlockAllocationIfAppropriate<Rep_<KEY_EXTRACTOR, KEY_INORDER_COMPARER>> {
    private:
        using inherited = IImplRepBase_;
        [[NO_UNIQUE_ADDRESS_ATTR]] const KEY_EXTRACTOR        fKeyExtractor_;
        [[NO_UNIQUE_ADDRESS_ATTR]] const KEY_INORDER_COMPARER fKeyComparer_;

    public:
        using KeyExtractorType        = typename KeyedCollection<T, KEY_TYPE, TRAITS>::KeyExtractorType;
        using KeyEqualityComparerType = typename KeyedCollection<T, KEY_TYPE, TRAITS>::KeyEqualityComparerType;
        using KeyType                 = typename KeyedCollection<T, KEY_TYPE, TRAITS>::KeyType;
        using value_type              = typename KeyedCollection<T, KEY_TYPE, TRAITS>::value_type;

        using _IterableRepSharedPtr        = typename inherited::_IterableRepSharedPtr;
        using _KeyedCollectionRepSharedPtr = typename inherited::_IRepSharedPtr;
        using _APPLY_ARGTYPE               = typename inherited::_APPLY_ARGTYPE;
        using _APPLYUNTIL_ARGTYPE          = typename inherited::_APPLYUNTIL_ARGTYPE;

    public:
        Rep_ (const KEY_EXTRACTOR& keyExtractor, const KEY_INORDER_COMPARER& inorderComparer)
            : fKeyExtractor_{keyExtractor}
            , fKeyComparer_{inorderComparer}
            , fData_{SetInOrderComparer_{keyExtractor, inorderComparer}}
        {
        }
        Rep_ (const Rep_& from) = delete;
        Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope)
            : fKeyExtractor_{from->fKeyExtractor_}
            , fKeyComparer_{from->fKeyComparer_}
            , fData_{&from->fData_, forIterableEnvelope}
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
            return fData_.size ();
        }
        virtual bool IsEmpty () const override
        {
            return fData_.empty ();
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
            return this->_FindFirstThat (doToElement, suggestedOwner);
        }

        // KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep overrides
    public:
        virtual KeyExtractorType GetKeyExtractor () const override
        {
            return fKeyExtractor_;
        }
        virtual KeyEqualityComparerType GetKeyEqualityComparer () const override
        {
            return KeyEqualityComparerType{[=] (const KEY_TYPE& lhs, const KEY_TYPE& rhs) {
                return fKeyComparer_ (lhs, rhs) and fKeyComparer_ (rhs, lhs);
            }};
            //return KeyEqualityComparerType{Common::EqualsComparerAdapter{Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eInOrderOrEquals, function<bool (KEY_TYPE, KEY_TYPE)>>{fData_.key_comp ()}}};
        }
        virtual _KeyedCollectionRepSharedPtr CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
        {
            if (fData_.HasActiveIterators ()) {
                // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                auto r = Iterable<T>::template MakeSmartPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                r->fData_.clear_WithPatching ();
                return r;
            }
            else {
                return Iterable<T>::template MakeSmartPtr<Rep_> (this->fKeyExtractor_, this->fKeyComparer_);
            }
        }
        virtual Iterable<KEY_TYPE> Keys () const override
        {
            return this->_Keys_Reference_Implementation ();
        }
        virtual bool Lookup (ArgByValueType<KeyType> key, optional<value_type>* item) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            auto                                                       i = fData_.find (key);
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
        virtual bool Add (ArgByValueType<T> item) override
        {
            pair<typename DataStructureImplType_::iterator, bool> flagAndI = fData_.insert (item);
            if (flagAndI.second) {
                return true;
            }
            else {
                // @todo must patch!!!
                // in case of update, set<> wont update the value so we must remove and re-add, but todo that, use previous iterator as hint
                typename DataStructureImplType_::iterator hint = flagAndI.first;
                hint++;
                fData_.erase (flagAndI.first);
                fData_.insert (hint, item);
                return false;
            }
        }
        virtual void Update (const Iterator<T>& i, ArgByValueType<T> newValue) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            // std::set doesn't appear to let you update an element because it doesn't know what parts go into the key, so you must
            // remove and re-add, but re-adding with a hint of old iterator value is O(1)
            const typename Iterator<T>::IRep& ir = i.ConstGetRep ();
            AssertMember (&ir, IteratorRep_);
            const IteratorRep_&                             mir  = dynamic_cast<const IteratorRep_&> (ir);
            typename DataStructureImplType_::iterator hint = mir.fIterator.fStdIterator;
            hint++;
            mir.fIterator.RemoveCurrent (); //fData_.erase (mir.fIterator.fStdIterator);
            fData_.insert (hint, newValue); // @todo FIX PATCHING!!!! (not sure needed for insert)
        }
        virtual void Remove (const Iterator<T>& i) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            const typename Iterator<T>::IRep&                         ir = i.ConstGetRep ();
            AssertMember (&ir, IteratorRep_);
            auto& mir = dynamic_cast<const IteratorRep_&> (ir);
            mir.fIterator.RemoveCurrent ();
        }
        virtual bool Remove (ArgByValueType<KEY_TYPE> key) override
        {
            auto i = fData_.find (key);
            if (i != fData_.end ()) {
                fData_.erase_WithPatching (i);
                return true;
            }
            return false;
        }
#if qDebug
        virtual void AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
        }
#endif

        // SortedKeyedCollection<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep overrides
    public:
#if 0
        virtual KeyInOrderKeyComparerType GetInOrderKeyComparer () const override
        {
            return KeyInOrderKeyComparerType{fData_.key_comp ()};
        }
#endif

    private:
        struct SetInOrderComparer_ {
            SetInOrderComparer_ (const KEY_EXTRACTOR& keyExtractor, const KEY_INORDER_COMPARER& inorderComparer)
                : fKeyExtractor_{keyExtractor}
                , fKeyComparer_{inorderComparer}
            {
            }
            int operator() (const T& lhs, const KEY_TYPE& rhs) const
            {
                return fKeyComparer_ (fKeyExtractor_ (lhs), rhs);
            };
            int operator() (const KEY_TYPE& lhs, const T& rhs) const
            {
                return fKeyComparer_ (lhs, fKeyExtractor_ (rhs));
            };
            int operator() (const T& lhs, const T& rhs) const
            {
                return fKeyComparer_ (fKeyExtractor_ (lhs), fKeyExtractor_ (rhs));
            };
            [[NO_UNIQUE_ADDRESS_ATTR]] const KEY_EXTRACTOR        fKeyExtractor_;
            [[NO_UNIQUE_ADDRESS_ATTR]] const KEY_INORDER_COMPARER fKeyComparer_;
            using is_transparent = int; // see https://en.cppreference.com/w/cpp/container/set/find - allows overloads to lookup by key
        };

        using DataStructureImplType_ = Private::PatchingDataStructures::STLContainerWrapper<set<T, SetInOrderComparer_>>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<T, DataStructureImplType_>;

    private:
        DataStructureImplType_ fData_;
    };

    /*
     ********************************************************************************
     ************ SortedKeyedCollection_stdset<KEY_TYPE,MAPPED_VALUE_TYPE> **********
     ********************************************************************************
     */

#if 0
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_INORDER_COMPARER ,
              typename DEFAULT_KEY_EXTRACTOR,
              enable_if_t<
                  Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_INORDER_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, DEFAULT_KEY_EXTRACTOR> ()>*>
    SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_stdset (KEY_INORDER_COMPARER&& keyComparer)
    {
    }
#endif

    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_EXTRACTOR,
              typename KEY_INORDER_COMPARER,
              enable_if_t<
                  Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_INORDER_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>*>
    SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_stdset (KEY_EXTRACTOR&& keyExtractor, KEY_INORDER_COMPARER&& keyComparer)
        : inherited (inherited::template MakeSmartPtr<Rep_<KEY_EXTRACTOR, KEY_INORDER_COMPARER>> (forward<KEY_EXTRACTOR> (keyExtractor), forward<KEY_INORDER_COMPARER> (keyComparer)))
    {
        AssertRepValidType_ ();
    }

#if 0
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_stdset ()
        : SortedKeyedCollection_stdset{less<KEY_TYPE>{}}
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_INORDER_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_INORDER_COMPARER> ()>*>
    inline SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_stdset (const KEY_INORDER_COMPARER& inorderComparer)
        : inherited (inherited::template MakeSmartPtr<Rep_<KEY_INORDER_COMPARER>> (inorderComparer))
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_ADDABLE> and not is_convertible_v<const CONTAINER_OF_ADDABLE*, const SortedKeyedCollection_stdmap<T, KEY_TYPE, TRAITS>*>>*>
    inline SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_stdmap (const CONTAINER_OF_ADDABLE& src)
        : SortedKeyedCollection_stdset{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename COPY_FROM_ITERATOR_KEYVALUE>
    inline SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_stdmap (COPY_FROM_ITERATOR_KEYVALUE start, COPY_FROM_ITERATOR_KEYVALUE end)
        : SortedKeyedCollection_stdset{}
    {
        this->AddAll (start, end);
        AssertRepValidType_ ();
    }
#endif
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::AssertRepValidType_ () const
    {
#if qDebug
        typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMember
#endif
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_SortedKeyedCollection_stdmap_inl_ */
