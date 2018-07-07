/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Sequence_inl_
#define _Stroika_Foundation_Containers_Sequence_inl_

#include "../Debug/Assertions.h"
#include "Factory/Sequence_Factory.h"
#include "Private/IterableUtils.h"

namespace Stroika::Foundation {
    namespace Containers {

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
         *                                      // e.g: error: ‘struct Stroika::Foundation::Containers::Sequence<Stroika::Foundation::Characters::String>::TemporaryElementReference_’ has no member named ‘Trim’
         */
        template <typename T>
        template <typename X, typename ENABLE>
        struct Sequence<T>::TemporaryElementReference_ { // for 'X' non-class (e.g == int which we cannot subclass from)
            static_assert (is_same<T, X>::value, "constructed so this is so - just use second template so we can do enable_if_t");
            Sequence<X>* fV;
            size_t       fIndex;
            TemporaryElementReference_ (const TemporaryElementReference_&) = default;
            TemporaryElementReference_ (TemporaryElementReference_&& from) = default;
            TemporaryElementReference_ (Sequence<X>* s, size_t i)
                : fV ((RequireNotNull (s), s))
                , fIndex (i)
            {
            }
            TemporaryElementReference_& operator= (const TemporaryElementReference_&) = delete;
            TemporaryElementReference_& operator                                      = (ArgByValueType<X> v)
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
        struct Sequence<T>::TemporaryElementReference_<X, enable_if_t<is_class<X>::value or is_union<X>::value>> : X {
            static_assert (is_same<T, X>::value, "constructed so this is so - just use second template so we can do enable_if_t");
            Sequence<T>* fV;
            size_t       fIndex;
            TemporaryElementReference_ (const TemporaryElementReference_&) = default;
            TemporaryElementReference_ (TemporaryElementReference_&&)      = default;
            TemporaryElementReference_ (Sequence<X>* s, size_t i)
                : X ((RequireNotNull (s), s->GetAt (i)))
                , fV (s)
                , fIndex (i)
            {
            }
            TemporaryElementReference_& operator= (const TemporaryElementReference_&) = delete;
            TemporaryElementReference_& operator                                      = (ArgByValueType<X> v)
            {
                RequireNotNull (fV);
                *((X*)this) = v;
                return *this;
            }
            operator X && () = delete; // didn't help -- https://stroika.atlassian.net/browse/STK-582
            ~TemporaryElementReference_ ()
            {
                // now remaining problem with this strategy is that if we have
                // String a = sequence[i] = the temporary may get MOVE()d to 'a', and so *this is now invalid, and cannot be used in a set.
                // We dont need to set in that case, but we have no way to reliably tell that we got moved.

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
            : inherited (move (Factory::Sequence_Factory<T> () ()))
        {
            _AssertRepValidType ();
        }
        template <typename T>
        inline Sequence<T>::Sequence (const initializer_list<T>& src)
            : Sequence ()
        {
            AppendAll (src);
            _AssertRepValidType ();
        }
        template <typename T>
        template <typename CONTAINER_OF_ADDABLE, typename ENABLE_IF>
        inline Sequence<T>::Sequence (const CONTAINER_OF_ADDABLE& src)
            : Sequence ()
        {
            AppendAll (src);
            _AssertRepValidType ();
        }
        template <typename T>
        inline Sequence<T>::Sequence (const _SequenceRepSharedPtr& rep) noexcept
            : inherited ((RequireNotNull (rep), rep))
        {
            _AssertRepValidType ();
        }
        template <typename T>
        inline Sequence<T>::Sequence (_SequenceRepSharedPtr&& rep) noexcept
            : inherited (move (rep))
        {
            //RequireNotNull (rep); -- logically required, but we cannot test here, must test before mem-initializers
            _AssertRepValidType ();
        }
        template <typename T>
        template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
        inline Sequence<T>::Sequence (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
            : Sequence ()
        {
            AppendAll (start, end);
            _AssertRepValidType ();
        }
#if qDebug
        template <typename T>
        Sequence<T>::~Sequence ()
        {
            if (this->_GetSharingState () != Memory::SharedByValue_State::eNull) {
                // SharingState can be NULL because of MOVE semantics
                _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().AssertNoIteratorsReferenceOwner (this);
            }
        }
#endif
        template <typename T>
        template <typename ELEMENT_COMPARER>
        inline int Sequence<T>::Compare (const Iterable<T>& rhs, const ELEMENT_COMPARER& comparer) const
        {
            return Private::ThreeWayCompare_<T, ELEMENT_COMPARER> (*this, rhs, comparer);
        }
        template <typename T>
        inline Sequence<T> Sequence<T>::Where (const function<bool(ArgByValueType<T>)>& doToElement) const
        {
            return Iterable<T>::Where (doToElement, Sequence<T>{});
        }
        template <typename T>
        template <typename EQUALS_COMPARER>
        inline bool Sequence<T>::Equals (const Sequence<T>& rhs, const EQUALS_COMPARER& equalsComparer) const
        {
            return Private::Equals_<T, EQUALS_COMPARER> (*this, rhs, equalsComparer);
        }
        template <typename T>
        inline void Sequence<T>::RemoveAll ()
        {
            _SafeReadWriteRepAccessor<_IRep> tmp{this};
            if (not tmp._ConstGetRep ().IsEmpty ()) {
                tmp._UpdateRep (tmp._ConstGetRep ().CloneEmpty (this));
            }
        }
        template <typename T>
        inline T Sequence<T>::GetAt (size_t i) const
        {
            _SafeReadRepAccessor<_IRep> accessor{this};
            Require (i < accessor._ConstGetRep ().GetLength ());
            return accessor._ConstGetRep ().GetAt (i);
        }
        template <typename T>
        inline void Sequence<T>::SetAt (size_t i, ArgByValueType<T> item)
        {
            _SafeReadWriteRepAccessor<_IRep> accessor{this};
            Require (i < accessor._ConstGetRep ().GetLength ());
            accessor._GetWriteableRep ().SetAt (i, item);
        }
        template <typename T>
        inline T Sequence<T>::operator[] (size_t i) const
        {
            _SafeReadRepAccessor<_IRep> accessor{this};
            Require (i < accessor._ConstGetRep ().GetLength ());
            return accessor._ConstGetRep ().GetAt (i);
        }
#if Stroika_Foundation_Containers_Sequence_SupportProxyModifiableOperatorBracket
        template <typename T>
        inline auto Sequence<T>::operator[] (size_t i) -> TemporaryElementReference_<T>
        {
            return TemporaryElementReference_<T>{this, i};
        }
#endif
#if Stroika_Foundation_Containers_Sequence_SupportProxyModifiableOperatorOpenCloseParens
        template <typename T>
        inline auto Sequence<T>::operator() (size_t i) -> TemporaryElementReference_<T>
        {
            return TemporaryElementReference_<T>{this, i};
        }
#endif
        template <typename T>
        template <typename EQUALS_COMPARER>
        inline optional<size_t> Sequence<T>::IndexOf (ArgByValueType<T> item, const EQUALS_COMPARER& equalsComparer) const
        {
            return Private::IndexOf_<T, EQUALS_COMPARER> (*this, item, equalsComparer);
        }
        template <typename T>
        template <typename EQUALS_COMPARER>
        inline optional<size_t> Sequence<T>::IndexOf (const Sequence<T>& s, const EQUALS_COMPARER& equalsComparer) const
        {
            return Private::IndexOf_<T, EQUALS_COMPARER> (*this, s, equalsComparer);
        }
        template <typename T>
        template <typename IGNORED>
        inline size_t Sequence<T>::IndexOf (const Iterator<T>& i) const
        {
            return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().IndexOf (i);
        }
        template <typename T>
        inline void Sequence<T>::Insert (size_t i, ArgByValueType<T> item)
        {
            _SafeReadWriteRepAccessor<_IRep> accessor{this};
            Require (i <= accessor._ConstGetRep ().GetLength ());
            return accessor._GetWriteableRep ().Insert (i, &item, &item + 1);
        }
        template <typename T>
        inline void Sequence<T>::Insert (const Iterator<T>& i, ArgByValueType<T> item)
        {
            _SafeReadWriteRepAccessor<_IRep> accessor{this};
            size_t                           idx = accessor._ConstGetRep ().IndexOf (i);
            Require (idx <= accessor._ConstGetRep ().GetLength ()); //  if equals end, we append
            return accessor._GetWriteableRep ().Insert (idx, &item, &item + 1);
        }
        template <typename T>
        template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
        void Sequence<T>::InsertAll (size_t i, COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        {
            Require (i <= this->GetLength ());
            size_t insertAt = i;
            for (auto ii = start; ii != end; ++ii) {
                Insert (insertAt++, *ii);
            }
        }
        template <typename T>
        template <typename CONTAINER_OF_ADDABLE, typename ENABLE_IF>
        inline void Sequence<T>::InsertAll (size_t i, const CONTAINER_OF_ADDABLE& s)
        {
            Require (i <= this->GetLength ());
            InsertAll (i, s.begin (), s.end ());
        }
        template <typename T>
        inline void Sequence<T>::Prepend (ArgByValueType<T> item)
        {
            Insert (0, item);
        }
        template <typename T>
        template <typename CONTAINER_OF_ADDABLE, typename ENABLE_IF>
        inline void Sequence<T>::PrependAll (const CONTAINER_OF_ADDABLE& s)
        {
            InsertAll (0, s);
        }
        template <typename T>
        template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
        inline void Sequence<T>::PrependAll (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        {
            InsertAll (0, start, end);
        }
        template <typename T>
        inline void Sequence<T>::Append (ArgByValueType<T> item)
        {
            _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Insert (_IRep::_kBadSequenceIndex, &item, &item + 1);
        }
        template <typename T>
        template <typename CONTAINER_OF_ADDABLE, typename ENABLE_IF>
        void Sequence<T>::AppendAll (const CONTAINER_OF_ADDABLE& s)
        {
            _SafeReadWriteRepAccessor<_IRep> tmp = {this};
            _IRep*                           ww  = nullptr; // lazy _GetWriteableRep... in case we dont need
#if 0
                // faster but doesn't work if container doesn't have 'Apply' method - like vector
                s.Apply ([&ww] (const T & item) {
                    if (ww == nullptr) {
                        ww = &tmp._GetWriteableRep ();
                    }
                    ww->Insert (_IRep::_kBadSequenceIndex, &item, &item + 1);
                });
#else
            for (auto i : s) {
                if (ww == nullptr) {
                    ww = &tmp._GetWriteableRep ();
                }
                ww->Insert (_IRep::_kBadSequenceIndex, &i, &i + 1);
            }
#endif
        }
        template <typename T>
        template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
        inline void Sequence<T>::AppendAll (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        {
            _SafeReadWriteRepAccessor<_IRep> accessor = {this};
            for (auto i = start; i != end; ++i) {
                T tmp = *i;
                accessor._GetWriteableRep ().Insert (_IRep::_kBadSequenceIndex, &tmp, &tmp + 1);
            }
        }
        template <typename T>
        inline void Sequence<T>::Update (const Iterator<T>& i, ArgByValueType<T> newValue)
        {
            _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Update (i, newValue);
        }
        template <typename T>
        inline void Sequence<T>::Remove (size_t i)
        {
            Require (i < this->GetLength ());
            _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Remove (i, i + 1);
        }
        template <typename T>
        inline void Sequence<T>::Remove (size_t start, size_t end)
        {
            Require (start <= end and end <= this->GetLength ());
            _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Remove (start, end);
        }
        template <typename T>
        inline void Sequence<T>::Remove (const Iterator<T>& i)
        {
            _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Remove (i);
        }
        template <typename T>
        template <typename CONTAINER_OF_ADDABLE>
        inline CONTAINER_OF_ADDABLE Sequence<T>::As () const
        {
            return CONTAINER_OF_ADDABLE (this->begin (), this->end ());
        }
        template <typename T>
        template <typename CONTAINER_OF_ADDABLE>
        inline void Sequence<T>::As (CONTAINER_OF_ADDABLE* into) const
        {
            RequireNotNull (into);
            *into = CONTAINER_OF_ADDABLE (this->begin (), this->end ());
        }
        template <typename T>
        inline optional<T> Sequence<T>::First () const
        {
            return this->IsEmpty () ? optional<T>{} : GetAt (0);
        }
        template <typename T>
        inline optional<T> Sequence<T>::First (const function<bool(ArgByValueType<T>)>& that) const
        {
            return inherited::First (that);
        }
        template <typename T>
        inline T Sequence<T>::FirstValue (ArgByValueType<T> defaultValue) const
        {
            return this->IsEmpty () ? defaultValue : GetAt (0);
        }
        template <typename T>
        inline optional<T> Sequence<T>::Last () const
        {
            // IRep::GetAt() defined to allow special _IRep::_kBadSequenceIndex
            return this->IsEmpty () ? optional<T>{} : _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetAt (_IRep::_kBadSequenceIndex);
        }
        template <typename T>
        inline optional<T> Sequence<T>::Last (const function<bool(ArgByValueType<T>)>& that) const
        {
            // @todo when we have reverse iterators - we could implement this more efficiently by walking the sequence backwards
            return inherited::Last (that);
        }
        template <typename T>
        inline T Sequence<T>::LastValue (ArgByValueType<T> defaultValue) const
        {
            // IRep::GetAt() defined to allow special _IRep::_kBadSequenceIndex
            return this->IsEmpty () ? defaultValue : _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetAt (_IRep::_kBadSequenceIndex);
        }
        template <typename T>
        inline void Sequence<T>::push_back (ArgByValueType<T> item)
        {
            Append (item);
        }
        template <typename T>
        inline T Sequence<T>::back () const
        {
            return *Last ();
        }
        template <typename T>
        inline T Sequence<T>::front () const
        {
            return *First ();
        }
        template <typename T>
        inline void Sequence<T>::clear ()
        {
            RemoveAll ();
        }
        template <typename T>
        inline Sequence<T>& Sequence<T>::operator+= (ArgByValueType<T> item)
        {
            Append (item);
            return *this;
        }
        template <typename T>
        inline Sequence<T>& Sequence<T>::operator+= (const Sequence<T>& items)
        {
            AppendAll (items);
            return *this;
        }
        template <typename T>
        inline void Sequence<T>::_AssertRepValidType () const
        {
#if qDebug
            _SafeReadRepAccessor<_IRep>{this};
#endif
        }

        /*
         ********************************************************************************
         ************************** Sequence<T> operators *******************************
         ********************************************************************************
         */
        template <typename T>
        inline bool operator< (const Sequence<T>& lhs, const Sequence<T>& rhs)
        {
            return lhs.Compare (rhs) < 0;
        }
        template <typename T>
        inline bool operator<= (const Sequence<T>& lhs, const Sequence<T>& rhs)
        {
            return lhs.Compare (rhs) <= 0;
        }
        template <typename T>
        inline bool operator== (const Sequence<T>& lhs, const Sequence<T>& rhs)
        {
            return lhs.Equals (rhs);
        }
        template <typename T>
        inline bool operator!= (const Sequence<T>& lhs, const Sequence<T>& rhs)
        {
            return not lhs.Equals (rhs);
        }
        template <typename T>
        inline bool operator>= (const Sequence<T>& lhs, const Sequence<T>& rhs)
        {
            return lhs.Compare (rhs) >= 0;
        }
        template <typename T>
        inline bool operator> (const Sequence<T>& lhs, const Sequence<T>& rhs)
        {
            return lhs.Compare (rhs) > 0;
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Sequence_inl_ */
