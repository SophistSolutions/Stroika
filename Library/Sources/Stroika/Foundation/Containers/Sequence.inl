/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Sequence_inl_
#define _Stroika_Foundation_Containers_Sequence_inl_

#include "../Debug/Assertions.h"
#include "Concrete/Sequence_stdvector.h"
#include "Factory/Sequence_Factory.h"
#include "Private/IterableUtils.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     ******************** Sequence<T>::TemporaryElementReference_ *******************
     ********************************************************************************
     */
    /*
     * TemporaryElementReference_ is a private implementation detail, so we can do:
     *      Sequence<int> x;    // and initialize with several items, and then
     *      x[3] = 4;
     *
     *  We need two templated variants - one inheriting and one not - to handle the fact that some people want to call a method
     *  on T, as in:
     *
     *      Sequence<String> x;
     *      size_t a = x[3].length ();      // wont work if we use aggregating variant of TemporaryElementReference_
     *                                      // e.g: error: �struct Stroika::Foundation::Containers::Sequence<Stroika::Foundation::Characters::String>::TemporaryElementReference_� has no member named �Trim�
     */
    template <typename T>
    template <typename X, typename ENABLE>
    struct Sequence<T>::TemporaryElementReference_ { // for 'X' non-class (e.g == int which we cannot subclass from)
        static_assert (is_same_v<T, X>, "constructed so this is so - just use second template so we can do enable_if_t");
        Sequence<X>* fV;
        size_t       fIndex;
        TemporaryElementReference_ (const TemporaryElementReference_&) = default;
        TemporaryElementReference_ (TemporaryElementReference_&& from) = default;
        TemporaryElementReference_ (Sequence<X>* s, size_t i)
            : fV{(RequireNotNull (s), s)}
            , fIndex{i}
        {
        }
        TemporaryElementReference_& operator= (const TemporaryElementReference_&) = delete;
        TemporaryElementReference_& operator= (ArgByValueType<X> v)
        {
            RequireNotNull (fV);
            fV->SetAt (fIndex, v);
            return *this;
        }
        operator X () const
        {
            RequireNotNull (fV);
            return fV->GetAt (fIndex);
        }
    };
    template <typename T>
    template <typename X>
    struct Sequence<T>::TemporaryElementReference_<X, enable_if_t<is_class_v<X> or is_union_v<X>>> : X {
        static_assert (is_same_v<T, X>, "constructed so this is so - just use second template so we can do enable_if_t");
        Sequence* fV;
        size_t    fIndex;
        TemporaryElementReference_ (const TemporaryElementReference_&) = default;
        TemporaryElementReference_ (TemporaryElementReference_&&)      = default;
        TemporaryElementReference_ (Sequence<X>* s, size_t i)
            : X{(RequireNotNull (s), s->GetAt (i))}
            , fV{s}
            , fIndex{i}
        {
        }
        TemporaryElementReference_& operator= (const TemporaryElementReference_&) = delete;
        TemporaryElementReference_& operator= (ArgByValueType<X> v)
        {
            RequireNotNull (fV);
            *((X*)this) = v;
            return *this;
        }
        operator X&& () = delete; // didn't help -- https://stroika.atlassian.net/browse/STK-582
        ~TemporaryElementReference_ ()
        {
            // now remaining problem with this strategy is that if we have
            // String a = sequence[i] = the temporary may get MOVE()d to 'a', and so *this is now invalid, and cannot be used in a set.
            // We don't need to set in that case, but we have no way to reliably tell that we got moved.

            // needed cuz modifications CAN come from from something like Sequence<String> x; x[1].clear ();
            if (fV != nullptr) {
                IgnoreExceptionsForCall (fV->SetAt (fIndex, *((X*)this)));
            }
        }
    };

    /*
     ********************************************************************************
     ******************************** Sequence<T> ***********************************
     ********************************************************************************
     */
    template <typename T>
    inline Sequence<T>::Sequence ()
        : inherited{Factory::Sequence_Factory<T>{}()}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    inline Sequence<T>::Sequence (const initializer_list<value_type>& src)
        : Sequence{}
    {
        AppendAll (src);
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<Sequence<T>, decay_t<ITERABLE_OF_ADDABLE>>>*>
    inline Sequence<T>::Sequence (ITERABLE_OF_ADDABLE&& src)
        : Sequence{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        AppendAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename T>
    inline Sequence<T>::Sequence (const _IRepSharedPtr& rep) noexcept
        : inherited{(RequireNotNull (rep), rep)}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    inline Sequence<T>::Sequence (_IRepSharedPtr&& rep) noexcept
        : inherited{(RequireNotNull (rep), move (rep))}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename ITERATOR_OF_ADDABLE>
    inline Sequence<T>::Sequence (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : Sequence{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        AppendAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename T>
    inline auto Sequence<T>::Where (const function<bool (ArgByValueType<value_type>)>& includeIfTrue) const -> Sequence
    {
#if 1
        Sequence<T> result;
        for (const auto& i : Iterable<T>::Where (includeIfTrue)) {
            //result.Add (i);
            result.push_back (i);
        }
        return result;
#else
        return Iterable<T>::Where (doToElement, Sequence<T>{});
#endif
    }
    template <typename T>
    template <typename INORDER_COMPARER_TYPE>
    auto Sequence<T>::OrderBy (const INORDER_COMPARER_TYPE& inorderComparer) const -> Sequence
    {
        vector<T> tmp (this->begin (), this->end ()); // due to Sequence_stdvector move constructor, a not very expensive implementation
        stable_sort (tmp.begin (), tmp.end (), inorderComparer);
        return Concrete::Sequence_stdvector<T>{move (tmp)};
    }
    template <typename T>
    inline void Sequence<T>::RemoveAll ()
    {
        _SafeReadRepAccessor<_IRep> tmp{this}; // important to use READ not WRITE accessor, because write accessor would have already cloned the data
        if (not tmp._ConstGetRep ().empty ()) {
            this->_fRep = tmp._ConstGetRep ().CloneEmpty ();
        }
    }
    template <typename T>
    template <typename PREDICATE, enable_if_t<Configuration::IsTPredicate<T, PREDICATE> ()>*>
    size_t Sequence<T>::RemoveAll (const PREDICATE& p)
    {
        size_t nRemoved{};
        for (Iterator<T> i = this->begin (); i != this->end ();) {
            if (p (*i)) {
                Remove (i, &i);
                ++nRemoved;
            }
            else {
                ++i;
            }
        }
        return nRemoved;
    }
    template <typename T>
    inline auto Sequence<T>::GetAt (size_t i) const -> value_type
    {
        _SafeReadRepAccessor<_IRep> accessor{this};
        Require (i < accessor._ConstGetRep ().size ());
        return accessor._ConstGetRep ().GetAt (i);
    }
    template <typename T>
    inline void Sequence<T>::SetAt (size_t i, ArgByValueType<value_type> item)
    {
        _SafeReadWriteRepAccessor<_IRep> accessor{this};
        Require (i < accessor._ConstGetRep ().size ());
        accessor._GetWriteableRep ().SetAt (i, item);
    }
    template <typename T>
    inline auto Sequence<T>::operator[] (size_t i) const -> value_type
    {
        _SafeReadRepAccessor<_IRep> accessor{this};
        Require (i < accessor._ConstGetRep ().size ());
        return accessor._ConstGetRep ().GetAt (i);
    }
#if Stroika_Foundation_Containers_Sequence_SupportProxyModifiableOperatorBracket
    template <typename T>
    inline auto Sequence<T>::operator[] (size_t i) -> TemporaryElementReference_<T>
    {
        return TemporaryElementReference_<value_type>{this, i};
    }
#endif
#if Stroika_Foundation_Containers_Sequence_SupportProxyModifiableOperatorOpenCloseParens
    template <typename T>
    inline auto Sequence<T>::operator() (size_t i) -> TemporaryElementReference_<value_type>
    {
        return TemporaryElementReference_<value_type>{this, i};
    }
#endif
    template <typename T>
    template <typename EQUALS_COMPARER>
    inline optional<size_t> Sequence<T>::IndexOf (ArgByValueType<value_type> item, const EQUALS_COMPARER& equalsComparer) const
    {
        return Private::IndexOf_<T, EQUALS_COMPARER> (*this, item, equalsComparer);
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    inline optional<size_t> Sequence<T>::IndexOf (const Sequence& s, const EQUALS_COMPARER& equalsComparer) const
    {
        return Private::IndexOf_<T, EQUALS_COMPARER> (*this, s, equalsComparer);
    }
    template <typename T>
    template <typename IGNORED>
    inline size_t Sequence<T>::IndexOf (const Iterator<value_type>& i) const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().IndexOf (i);
    }
    template <typename T>
    inline void Sequence<T>::Insert (size_t i, ArgByValueType<value_type> item)
    {
        _SafeReadWriteRepAccessor<_IRep> accessor{this};
        Require (i <= accessor._ConstGetRep ().size ());
        return accessor._GetWriteableRep ().Insert (i, &item, &item + 1);
    }
    template <typename T>
    inline void Sequence<T>::Insert (const Iterator<value_type>& i, ArgByValueType<value_type> item)
    {
        _SafeReadWriteRepAccessor<_IRep> accessor{this};
        size_t                           idx = accessor._ConstGetRep ().IndexOf (i);
        Require (idx <= accessor._ConstGetRep ().size ()); //  if equals end, we append
        return accessor._GetWriteableRep ().Insert (idx, &item, &item + 1);
    }
    template <typename T>
    template <typename ITERATOR_OF_ADDABLE>
    void Sequence<T>::InsertAll (size_t i, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        Require (i <= this->size ());
        size_t insertAt = i;
        for (auto ii = forward<ITERATOR_OF_ADDABLE> (start); ii != end; ++ii) {
            Insert (insertAt++, *ii);
        }
    }
    template <typename T>
    template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>*>
    inline void Sequence<T>::InsertAll (size_t i, ITERABLE_OF_ADDABLE&& s)
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        Require (i <= this->size ());
        InsertAll (i, s.begin (), s.end ());
    }
    template <typename T>
    inline void Sequence<T>::Prepend (ArgByValueType<value_type> item)
    {
        Insert (0, item);
    }
    template <typename T>
    template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>*>
    inline void Sequence<T>::PrependAll (ITERABLE_OF_ADDABLE&& s)
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        InsertAll (0, forward<ITERABLE_OF_ADDABLE> (s));
    }
    template <typename T>
    template <typename ITERATOR_OF_ADDABLE>
    inline void Sequence<T>::PrependAll (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        InsertAll (0, forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
    }
    template <typename T>
    inline void Sequence<T>::Append (ArgByValueType<value_type> item)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Insert (_IRep::_kSentinalLastItemIndex, &item, &item + 1);
    }
    template <typename T>
    template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>*>
    inline void Sequence<T>::AppendAll (ITERABLE_OF_ADDABLE&& s)
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        AppendAll (s.begin (), s.end ());
    }
    template <typename T>
    template <typename ITERATOR_OF_ADDABLE>
    inline void Sequence<T>::AppendAll (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        _SafeReadWriteRepAccessor<_IRep> accessor = {this};
        for (auto i = forward<ITERATOR_OF_ADDABLE> (start); i != end; ++i) {
            const T& tmp = *i;
            accessor._GetWriteableRep ().Insert (_IRep::_kSentinalLastItemIndex, &tmp, &tmp + 1);
        }
    }
    template <typename T>
    inline void Sequence<T>::Update (const Iterator<value_type>& i, ArgByValueType<value_type> newValue, Iterator<value_type>* nextI)
    {
        Require (not i.Done ());
        auto [writerRep, patchedIterator] = _GetWritableRepAndPatchAssociatedIterator (i);
        writerRep->Update (patchedIterator, newValue, nextI);
    }
    template <typename T>
    inline void Sequence<T>::Remove (size_t i)
    {
        Require (i < this->size ());
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Remove (i, i + 1);
    }
    template <typename T>
    inline void Sequence<T>::Remove (size_t start, size_t end)
    {
        Require (start <= end and end <= this->size ());
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Remove (start, end);
    }
    template <typename T>
    inline void Sequence<T>::Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI)
    {
        Require (not i.Done ());
        auto [writerRep, patchedIterator] = _GetWritableRepAndPatchAssociatedIterator (i);
        writerRep->Remove (patchedIterator, nextI);
    }
    template <typename T>
    template <typename CONTAINER_OF_ADDABLE>
    inline CONTAINER_OF_ADDABLE Sequence<T>::As () const
    {
        return CONTAINER_OF_ADDABLE{this->begin (), this->end ()};
    }
    template <typename T>
    template <typename CONTAINER_OF_ADDABLE>
    inline void Sequence<T>::As (CONTAINER_OF_ADDABLE* into) const
    {
        RequireNotNull (into);
        *into = CONTAINER_OF_ADDABLE{this->begin (), this->end ()};
    }
    template <typename T>
    inline auto Sequence<T>::First () const -> optional<value_type>
    {
        return this->empty () ? optional<T>{} : GetAt (0);
    }
    template <typename T>
    inline auto Sequence<T>::First (const function<bool (ArgByValueType<value_type>)>& that) const -> optional<value_type>
    {
        return inherited::First (that);
    }
    template <typename T>
    inline auto Sequence<T>::FirstValue (ArgByValueType<value_type> defaultValue) const -> value_type
    {
        return this->empty () ? defaultValue : GetAt (0);
    }
    template <typename T>
    inline auto Sequence<T>::Last () const -> optional<value_type>
    {
        // IRep::GetAt() defined to allow special _IRep::_kSentinalLastItemIndex
        return this->empty () ? optional<T>{} : _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetAt (_IRep::_kSentinalLastItemIndex);
    }
    template <typename T>
    inline auto Sequence<T>::Last (const function<bool (ArgByValueType<value_type>)>& that) const -> optional<value_type>
    {
        // @todo when we have reverse iterators - we could implement this more efficiently by walking the sequence backwards
        return inherited::Last (that);
    }
    template <typename T>
    inline auto Sequence<T>::LastValue (ArgByValueType<value_type> defaultValue) const -> value_type
    {
        // IRep::GetAt() defined to allow special _IRep::_kSentinalLastItemIndex
        return this->empty () ? defaultValue : _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetAt (_IRep::_kSentinalLastItemIndex);
    }
    template <typename T>
    inline void Sequence<T>::push_back (ArgByValueType<value_type> item)
    {
        Append (item);
    }
    template <typename T>
    inline auto Sequence<T>::back () const -> value_type
    {
        return *Last ();
    }
    template <typename T>
    inline auto Sequence<T>::front () const -> value_type
    {
        return *First ();
    }
    template <typename T>
    inline void Sequence<T>::clear ()
    {
        RemoveAll ();
    }
    template <typename T>
    inline void Sequence<T>::erase (size_t i)
    {
        this->Remove (i);
    }
    template <typename T>
    inline auto Sequence<T>::erase (const Iterator<value_type>& i) -> Iterator<value_type>
    {
        Iterator<T> nextI{nullptr};
        this->Remove (i, &nextI);
        return nextI;
    }
    template <typename T>
    inline auto Sequence<T>::operator+= (ArgByValueType<value_type> item) -> Sequence&
    {
        Append (item);
        return *this;
    }
    template <typename T>
    inline auto Sequence<T>::operator+= (const Sequence& items) -> Sequence&
    {
        AppendAll (items);
        return *this;
    }
    template <typename T>
    auto Sequence<T>::_GetWritableRepAndPatchAssociatedIterator (const Iterator<value_type>& i) -> tuple<_IRep*, Iterator<value_type>>
    {
        Require (not i.Done ());
        using element_type                   = typename inherited::_SharedByValueRepType::element_type;
        Iterator<value_type> patchedIterator = i;
        element_type*        writableRep     = this->_fRep.rwget (
            [&] (const element_type& prevRepPtr) -> typename inherited::_SharedByValueRepType::shared_ptr_type {
                return Debug::UncheckedDynamicCast<const _IRep&> (prevRepPtr).CloneAndPatchIterator (&patchedIterator);
            });
        AssertNotNull (writableRep);
        return make_tuple (Debug::UncheckedDynamicCast<_IRep*> (writableRep), move (patchedIterator));
    }
    template <typename T>
    inline void Sequence<T>::_AssertRepValidType () const
    {
        if constexpr (qDebug) {
            _SafeReadRepAccessor<_IRep>{this};
        }
    }
    template <typename T>
    inline bool Sequence<T>::operator== (const Sequence& rhs) const
    {
        return EqualsComparer<>{}(*this, rhs);
    }
    template <typename T>
    inline auto Sequence<T>::operator<=> (const Sequence& rhs) const
    {
        return ThreeWayComparer<>{}(*this, rhs);
    }

    /*
     ********************************************************************************
     ********************************* operator+ ************************************
     ********************************************************************************
     */
    template <typename T>
    Sequence<T> operator+ (const Iterable<T>& lhs, const Sequence<T>& rhs)
    {
        Sequence<T> result{lhs};
        result += rhs;
        return result;
    }
    template <typename T>
    Sequence<T> operator+ (const Sequence<T>& lhs, const Iterable<T>& rhs)
    {
        Sequence<T> result{lhs};
        result += rhs;
        return result;
    }
    template <typename T>
    Sequence<T> operator+ (const Sequence<T>& lhs, const Sequence<T>& rhs)
    {
        Sequence<T> result{lhs};
        result += rhs;
        return result;
    }

}

#endif /* _Stroika_Foundation_Containers_Sequence_inl_ */
