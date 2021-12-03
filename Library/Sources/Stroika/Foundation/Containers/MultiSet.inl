/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_MultiSet_inl_
#define _Stroika_Foundation_Containers_MultiSet_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../Debug/Assertions.h"
#include "../Traversal/IterableFromIterator.h"
#include "Factory/MultiSet_Factory.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     ********* MultiSet<T, TRAITS>::_IRep::ElementsIteratorHelperContext_ ***********
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    struct MultiSet<T, TRAITS>::_IRep::ElementsIteratorHelperContext_ {
        ElementsIteratorHelperContext_ (const typename Iterable<value_type>::_IterableRepSharedPtr& tally, const Iterator<value_type>& delegateTo, size_t countMoreTimesToGoBeforeAdvance = 0, optional<T> saved2Return = optional<T> ())
            : fMultiSet{tally}
            , fMultiSetIterator{delegateTo}
            , fCountMoreTimesToGoBeforeAdvance{countMoreTimesToGoBeforeAdvance}
            , fSaved2Return{saved2Return}
        {
        }
        typename Iterable<value_type>::_IterableRepSharedPtr fMultiSet;
        Iterator<value_type>                                 fMultiSetIterator;
        size_t                                               fCountMoreTimesToGoBeforeAdvance;
        optional<T>                                          fSaved2Return;
    };

    template <typename T, typename TRAITS>
    struct MultiSet<T, TRAITS>::_IRep::ElementsIteratorHelper_ : public Iterator<T> {
        struct Rep : public Iterator<T>::IRep, public Memory::UseBlockAllocationIfAppropriate<Rep> {
            using inherited = typename Iterator<T>::IRep;
            ElementsIteratorHelperContext_ fContext;
            Rep (const ElementsIteratorHelperContext_& context)
                : fContext{context}
            {
                if (not fContext.fMultiSetIterator.Done ()) {
                    fContext.fSaved2Return = fContext.fMultiSetIterator->fValue;
                    if (fContext.fSaved2Return.has_value ()) {
                        fContext.fCountMoreTimesToGoBeforeAdvance = fContext.fMultiSetIterator->fCount - 1;
                    }
                }
            }
            virtual void More (optional<T>* result, bool advance) override
            {
                RequireNotNull (result);
                if (fContext.fCountMoreTimesToGoBeforeAdvance > 0) {
                    *result = fContext.fSaved2Return;
                    if (advance) {
                        fContext.fCountMoreTimesToGoBeforeAdvance--;
                    }
                }
                else {
                    bool done = fContext.fMultiSetIterator.Done ();
                    if (not done and advance) {
                        ++fContext.fMultiSetIterator;
                        done = fContext.fMultiSetIterator.Done ();
                    }
                    if (done) {
                        *result = nullopt;
                    }
                    else {
                        *result = fContext.fMultiSetIterator->fValue;
                    }
                    if (advance) {
                        fContext.fSaved2Return = *result;
                        if (fContext.fSaved2Return.has_value ()) {
                            fContext.fCountMoreTimesToGoBeforeAdvance = fContext.fMultiSetIterator->fCount - 1;
                        }
                    }
                }
            }
            virtual typename Iterator<T>::RepSmartPtr Clone () const override
            {
                return Iterator<T>::template MakeSmartPtr<Rep> (*this);
            }
            virtual bool Equals (const typename Iterator<T>::IRep* /*rhs*/) const override
            {
                AssertNotImplemented ();
                return false;
            }
        };
        ElementsIteratorHelper_ (const ElementsIteratorHelperContext_& context)
            : Iterator<T>{Iterator<T>::template MakeSmartPtr<Rep> (context)}
        {
        }
    };

    /**
     *  Protected helper class to convert from an iterator of MultiSetEntries
     *  to an iterator over individual items - repeating items the appropriate number of times
     *  depending on its count.
     */
    template <typename T, typename TRAITS>
    struct MultiSet<T, TRAITS>::_IRep::_ElementsIterableHelper : public Iterable<T> {
        using MyIteratorRep_ = typename ElementsIteratorHelper_::Rep;
        using MyDataBLOB_    = ElementsIteratorHelperContext_;
        struct MyIterableRep_ : Traversal::IterableFromIterator<T, MyIteratorRep_, MyDataBLOB_>::_Rep, public Memory::UseBlockAllocationIfAppropriate<MyIterableRep_> {
            using inherited = typename Traversal::IterableFromIterator<T, MyIteratorRep_, MyDataBLOB_>::_Rep;
            MyIterableRep_ (const ElementsIteratorHelperContext_& context)
                : inherited{context}
            {
            }
            virtual size_t GetLength () const override
            {
                size_t n = 0;
                for (Iterator<CountedValue<T>> i = this->_fContextForEachIterator.fMultiSet->MakeIterator (); not i.Done (); ++i) {
                    n += i->fCount;
                }
                return n;
            }
            virtual bool IsEmpty () const override
            {
                return this->_fContextForEachIterator.fMultiSet->IsEmpty ();
            }
            virtual typename Iterable<T>::_IterableRepSharedPtr Clone () const override
            {
                return Iterable<T>::template MakeSmartPtr<MyIterableRep_> (*this);
            }
        };
        _ElementsIterableHelper (const typename Iterable<CountedValue<T>>::_IterableRepSharedPtr& iterateOverMultiSet)
            : Iterable<T>{Iterable<T>::template MakeSmartPtr<MyIterableRep_> (ElementsIteratorHelperContext_ (iterateOverMultiSet, iterateOverMultiSet->MakeIterator ()))}
        {
        }
    };

    template <typename T, typename TRAITS>
    struct MultiSet<T, TRAITS>::_IRep::UniqueElementsIteratorHelperContext_ {
        UniqueElementsIteratorHelperContext_ (const typename Iterable<value_type>::_IterableRepSharedPtr& tally, const Iterator<value_type>& delegateTo)
            : fMultiSet{tally}
            , fMultiSetIterator{delegateTo}
        {
        }
        typename Iterable<value_type>::_IterableRepSharedPtr fMultiSet;
        Iterator<value_type>                                 fMultiSetIterator;
    };

    template <typename T, typename TRAITS>
    struct MultiSet<T, TRAITS>::_IRep::UniqueElementsIteratorHelper_ : public Iterator<T> {
        struct Rep : public Iterator<T>::IRep, public Memory::UseBlockAllocationIfAppropriate<Rep> {
            using inherited = typename Iterator<T>::IRep;
            UniqueElementsIteratorHelperContext_ fContext;
            Rep (const UniqueElementsIteratorHelperContext_& context)
                : fContext{context}
            {
            }
            virtual void More (optional<T>* result, bool advance) override
            {
                RequireNotNull (result);
                bool done = fContext.fMultiSetIterator.Done ();
                if (not done and advance) {
                    ++fContext.fMultiSetIterator;
                    done = fContext.fMultiSetIterator.Done ();
                }
                if (done) {
                    *result = nullopt;
                }
                else {
                    *result = fContext.fMultiSetIterator->fValue;
                }
            }
            virtual typename Iterator<T>::RepSmartPtr Clone () const override
            {
                return Iterator<T>::template MakeSmartPtr<Rep> (*this);
            }
            virtual bool Equals (const typename Iterator<T>::IRep* /*rhs*/) const override
            {
                AssertNotImplemented ();
                return false;
            }
        };
        UniqueElementsIteratorHelper_ (const typename Iterable<CountedValue<T>>::_IterableRepSharedPtr& tally)
            : Iterator<T>{Iterator<T>::template MakeSmartPtr<Rep> (UniqueElementsIteratorHelperContext_{tally, tally->MakeIterator (tally.get ())})}
        {
        }
    };

    /**
     *  Protected helper class to convert from an iterator of MultiSetEntries
     *  to an iterator over unique individual items.
     */
    template <typename T, typename TRAITS>
    struct MultiSet<T, TRAITS>::_IRep::_UniqueElementsHelper : public Iterable<T> {
        using MyIteratorRep_ = typename UniqueElementsIteratorHelper_::Rep;
        using MyDataBLOB_    = UniqueElementsIteratorHelperContext_;
        struct MyIterableRep_ : Traversal::IterableFromIterator<T, MyIteratorRep_, MyDataBLOB_>::_Rep, public Memory::UseBlockAllocationIfAppropriate<MyIterableRep_> {
            using inherited = typename Traversal::IterableFromIterator<T, MyIteratorRep_, MyDataBLOB_>::_Rep;
            MyIterableRep_ (const UniqueElementsIteratorHelperContext_& context)
                : inherited{context}
            {
            }
            virtual size_t GetLength () const override
            {
                return this->_fContextForEachIterator.fMultiSet->GetLength ();
            }
            virtual bool IsEmpty () const override
            {
                return this->_fContextForEachIterator.fMultiSet->IsEmpty ();
            }
            virtual typename Iterable<T>::_IterableRepSharedPtr Clone () const override
            {
                return Iterable<T>::template MakeSmartPtr<MyIterableRep_> (*this);
            }
        };
        _UniqueElementsHelper (const typename Iterable<CountedValue<T>>::_IterableRepSharedPtr& tally)
            : Iterable<T>{Iterable<T>::template MakeSmartPtr<MyIterableRep_> (UniqueElementsIteratorHelperContext_{tally, tally->MakeIterator ()})}
        {
        }
    };

    /*
     ********************************************************************************
     ************************* MultiSet<T, TRAITS>::_IRep ***************************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    bool MultiSet<T, TRAITS>::_IRep::_Equals_Reference_Implementation (const _IRep& rhs) const
    {
        if (this == &rhs) {
            return true;
        }
        if (this->GetLength () != rhs.GetLength ()) {
            return false;
        }
        for (auto i = this->MakeIterator (); not i.Done (); ++i) {
            if (i->fCount != rhs.OccurrencesOf (i->fValue)) {
                return false;
            }
        }
        return true;
    }
    template <typename T, typename TRAITS>
    Iterable<T> MultiSet<T, TRAITS>::_IRep::_Elements_Reference_Implementation (const _IRepSharedPtr& rep) const
    {
        Require (rep.get () == this); // allows reference counting but without using enable_shared_from_this (so cheap!)
        return _ElementsIterableHelper (rep);
    }
    template <typename T, typename TRAITS>
    Iterable<T> MultiSet<T, TRAITS>::_IRep::_UniqueElements_Reference_Implementation (const _IRepSharedPtr& rep) const
    {
        Require (rep.get () == this); // allows reference counting but without using enable_shared_from_this (so cheap!)
        return _UniqueElementsHelper (rep);
    }

    /*
     ********************************************************************************
     ***************************** MultiSet<T, TRAITS> ******************************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    MultiSet<T, TRAITS>::MultiSet ()
        : MultiSet{equal_to<T>{}}
    {
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <typename EQUALS_COMPARER, enable_if_t<Common::IsEqualsComparer<EQUALS_COMPARER, T> ()>*>
    inline MultiSet<T, TRAITS>::MultiSet (EQUALS_COMPARER&& equalsComparer)
        : inherited{Factory::MultiSet_Factory<T, TRAITS, EQUALS_COMPARER> (forward<EQUALS_COMPARER> (equalsComparer)) ()}
    {
        static_assert (Common::IsEqualsComparer<EQUALS_COMPARER> (), "MultiSet constructor with EQUALS_COMPARER - comparer not valid EqualsComparer- see ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals, function<bool(T, T)>");
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<MultiSet<T, TRAITS>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>*>
    inline MultiSet<T, TRAITS>::MultiSet (CONTAINER_OF_ADDABLE&& src)
        : MultiSet{}
    {
        AddAll (forward<CONTAINER_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <typename EQUALS_COMPARER, typename CONTAINER_OF_ADDABLE, enable_if_t<Common::IsEqualsComparer<EQUALS_COMPARER, T> () and Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T>>*>
    inline MultiSet<T, TRAITS>::MultiSet (EQUALS_COMPARER&& equalsComparer, const CONTAINER_OF_ADDABLE& src)
        : MultiSet{forward<EQUALS_COMPARER> (equalsComparer)}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    inline MultiSet<T, TRAITS>::MultiSet (const _IRepSharedPtr& rep) noexcept
        : inherited{(RequireNotNull (rep), rep)}
    {
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    inline MultiSet<T, TRAITS>::MultiSet (_IRepSharedPtr&& rep) noexcept
        : inherited{(RequireNotNull (rep), move (rep))}
    {
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    MultiSet<T, TRAITS>::MultiSet (const initializer_list<T>& src)
        : MultiSet{}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <typename EQUALS_COMPARER, enable_if_t<Common::IsEqualsComparer<EQUALS_COMPARER, T> ()>*>
    MultiSet<T, TRAITS>::MultiSet (EQUALS_COMPARER&& equalsComparer, const initializer_list<T>& src)
        : MultiSet{forward<EQUALS_COMPARER> (equalsComparer)}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    MultiSet<T, TRAITS>::MultiSet (const initializer_list<value_type>& src)
        : MultiSet{}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <typename EQUALS_COMPARER, enable_if_t<Common::IsEqualsComparer<EQUALS_COMPARER, T> ()>*>
    MultiSet<T, TRAITS>::MultiSet (EQUALS_COMPARER&& equalsComparer, const initializer_list<value_type>& src)
        : MultiSet{forward<EQUALS_COMPARER> (equalsComparer)}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE, enable_if_t<Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE>>*>
    MultiSet<T, TRAITS>::MultiSet (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        : MultiSet{}
    {
        AddAll (start, end);
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <typename EQUALS_COMPARER, typename COPY_FROM_ITERATOR_OF_ADDABLE, enable_if_t<Common::IsEqualsComparer<EQUALS_COMPARER, T> () and Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE>>*>
    MultiSet<T, TRAITS>::MultiSet (EQUALS_COMPARER&& equalsComparer, COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        : MultiSet{forward<EQUALS_COMPARER> (equalsComparer)}
    {
        AddAll (start, end);
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    auto MultiSet<T, TRAITS>::TotalOccurrences () const -> CounterType
    {
        CounterType sum = 0;
        for (value_type i : *this) {
            sum += i.fCount;
        }
        return sum;
    }
    template <typename T, typename TRAITS>
    inline auto MultiSet<T, TRAITS>::erase (const Iterator<value_type>& i) -> Iterator<value_type>
    {
        Iterator<value_type> result{nullptr};
        this->Remove (i, &result);
        return result;
    }
    template <typename T, typename TRAITS>
    inline void MultiSet<T, TRAITS>::clear ()
    {
        RemoveAll ();
    }
    template <typename T, typename TRAITS>
    inline Iterable<T> MultiSet<T, TRAITS>::Elements () const
    {
        _SafeReadRepAccessor<_IRep> accessor{this};
#if qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
        _IRepSharedPtr ss = dynamic_pointer_cast<typename _IRepSharedPtr::element_type> (const_cast<_IRep&> (accessor._ConstGetRep ()).shared_from_this ());
#else
        _IRepSharedPtr ss = const_cast<_IRep&> (accessor._ConstGetRep ()).shared_from_this ();
#endif
        AssertNotNull (ss.get ());
        return ss->Elements (ss);
    }
    template <typename T, typename TRAITS>
    inline Iterable<T> MultiSet<T, TRAITS>::UniqueElements () const
    {
        _SafeReadRepAccessor<_IRep> accessor{this};
#if qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
        _IRepSharedPtr ss = dynamic_pointer_cast<typename _IRepSharedPtr::element_type> (const_cast<_IRep&> (accessor._ConstGetRep ()).shared_from_this ());
#else
        _IRepSharedPtr ss = const_cast<_IRep&> (accessor._ConstGetRep ()).shared_from_this ();
#endif
        AssertNotNull (ss.get ());
        return ss->UniqueElements (ss);
    }
    template <typename T, typename TRAITS>
    inline auto MultiSet<T, TRAITS>::GetElementEqualsComparer () const -> ElementEqualityComparerType
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetElementEqualsComparer ();
    }
    template <typename T, typename TRAITS>
    inline bool MultiSet<T, TRAITS>::Contains (ArgByValueType<T> item) const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Contains (item);
    }
    template <typename T, typename TRAITS>
    inline void MultiSet<T, TRAITS>::RemoveAll ()
    {
        _SafeReadRepAccessor<_IRep> tmp{this}; // important to use READ not WRITE accessor, because write accessor would have already cloned the data
        if (not tmp._ConstGetRep ().IsEmpty ()) {
            this->_fRep = tmp._ConstGetRep ().CloneEmpty ();
        }
    }
    template <typename T, typename TRAITS>
    inline size_t MultiSet<T, TRAITS>::RemoveAll (ArgByValueType<T> item)
    {
        return RemoveIf (item, OccurrencesOf (item));
    }
    template <typename T, typename TRAITS>
    inline void MultiSet<T, TRAITS>::Add (ArgByValueType<T> item)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (item, 1);
    }
    template <typename T, typename TRAITS>
    inline void MultiSet<T, TRAITS>::Add (ArgByValueType<T> item, CounterType count)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (item, count);
    }
    template <typename T, typename TRAITS>
    inline void MultiSet<T, TRAITS>::Add (const value_type& item)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (item.fValue, item.fCount);
    }
    template <typename T, typename TRAITS>
    template <typename COPY_FROM_ITERATOR>
    void MultiSet<T, TRAITS>::AddAll (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end)
    {
        for (COPY_FROM_ITERATOR i = start; i != end; ++i) {
            Add (*i);
        }
    }
    template <typename T, typename TRAITS>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_ADDABLE>>*>
    void MultiSet<T, TRAITS>::AddAll (CONTAINER_OF_ADDABLE&& items)
    {
        // see https://stroika.atlassian.net/browse/STK-645
        if constexpr (std::is_convertible_v<decay_t<CONTAINER_OF_ADDABLE>*, const MultiSet<T, TRAITS>*>) {
            if (static_cast<const Iterable<value_type>*> (this) == static_cast<const Iterable<value_type>*> (&items)) [[UNLIKELY_ATTR]] {
                // very rare corner case
                vector<typename decay_t<CONTAINER_OF_ADDABLE>::value_type> copy{std::begin (items), std::end (items)}; // because you can not iterate over a container while modifying it
                for (auto&& i : copy) {
                    Add (i);
                }
                return;
            }
        }
        for (auto&& i : items) {
            Add (i);
        }
    }
    template <typename T, typename TRAITS>
    inline void MultiSet<T, TRAITS>::Remove (ArgByValueType<T> item, CounterType count)
    {
        Verify (_SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().RemoveIf (item, count) == count);
    }
    template <typename T, typename TRAITS>
    inline void MultiSet<T, TRAITS>::Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI)
    {
        Require (not i.Done ());
        auto [writerRep, patchedIterator] = _GetWritableRepAndPatchAssociatedIterator (i);
        writerRep->Remove (patchedIterator, nextI);
    }
    template <typename T, typename TRAITS>
    inline size_t MultiSet<T, TRAITS>::RemoveIf (ArgByValueType<T> item, CounterType count)
    {
        return _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().RemoveIf (item, count);
    }
    template <typename T, typename TRAITS>
    inline void MultiSet<T, TRAITS>::UpdateCount (const Iterator<value_type>& i, CounterType newCount, Iterator<value_type>* nextI)
    {
        Require (not i.Done ());
        auto [writerRep, patchedIterator] = _GetWritableRepAndPatchAssociatedIterator (i);
        writerRep->UpdateCount (patchedIterator, newCount, nextI);
    }
    template <typename T, typename TRAITS>
    inline void MultiSet<T, TRAITS>::SetCount (ArgByValueType<T> i, CounterType newCount)
    {
        CounterType cnt = OccurrencesOf (i);
        if (newCount > cnt) {
            Add (i, newCount - cnt);
        }
        else if (newCount < cnt) {
            Remove (i, cnt - newCount);
        }
    }
    template <typename T, typename TRAITS>
    inline auto MultiSet<T, TRAITS>::OccurrencesOf (ArgByValueType<T> item) const -> CounterType
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().OccurrencesOf (item);
    }
    template <typename T, typename TRAITS>
    inline MultiSet<T, TRAITS>& MultiSet<T, TRAITS>::operator+= (ArgByValueType<T> item)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (item, 1);
        return *this;
    }
    template <typename T, typename TRAITS>
    MultiSet<T, TRAITS>& MultiSet<T, TRAITS>::operator+= (const MultiSet<T, TRAITS>& items)
    {
        AddAll (items);
        return *this;
    }
    template <typename T, typename TRAITS>
    auto MultiSet<T, TRAITS>::_GetWritableRepAndPatchAssociatedIterator (const Iterator<value_type>& i) -> tuple<_IRep*, Iterator<value_type>>
    {
        Require (not i.Done ());
        using element_type                   = typename inherited::_SharedByValueRepType::element_type;
        Iterator<value_type> patchedIterator = i;
        element_type*        writableRep     = this->_fRep.rwget (
            [&] (const element_type& prevRepPtr) -> typename inherited::_SharedByValueRepType::shared_ptr_type {
                return Debug::UncheckedDynamicCast<const _IRep&> (prevRepPtr).CloneAndPatchIterator (&patchedIterator);
            });
        AssertNotNull (writableRep);
        return make_tuple (Debug::UncheckedDynamicCast<_IRep*> (writableRep), patchedIterator);
    }
    template <typename T, typename TRAITS>
    inline void MultiSet<T, TRAITS>::_AssertRepValidType () const
    {
#if qDebug
        _SafeReadRepAccessor<_IRep>{this};
#endif
    }
#if __cpp_impl_three_way_comparison >= 201907
    template <typename T, typename TRAITS>
    inline bool MultiSet<T, TRAITS>::operator== (const MultiSet& rhs) const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Equals (_SafeReadRepAccessor<_IRep>{&rhs}._ConstGetRep ());
    }
#endif

#if __cpp_impl_three_way_comparison < 201907
    /*
     ********************************************************************************
     **************************** MultiSet operators ********************************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    inline bool operator== (const MultiSet<T, TRAITS>& lhs, const MultiSet<T, TRAITS>& rhs)
    {
        using M        = MultiSet<T, TRAITS>;
        using ACCESSOR = typename M::template _SafeReadRepAccessor<typename M::_IRep>;
        return ACCESSOR{&lhs}._ConstGetRep ().Equals (ACCESSOR{&rhs}._ConstGetRep ());
    }
    template <typename T, typename TRAITS>
    inline bool operator!= (const MultiSet<T, TRAITS>& lhs, const MultiSet<T, TRAITS>& rhs)
    {
        return not(lhs == rhs);
    }
#endif

}

#endif /* _Stroika_Foundation_Containers_MultiSet_inl_ */
