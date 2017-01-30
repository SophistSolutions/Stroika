/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Sequence_inl_
#define _Stroika_Foundation_Containers_Sequence_inl_

#include "../Debug/Assertions.h"
#include "Factory/Sequence_Factory.h"
#include "Private/IterableUtils.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            /*
             ********************************************************************************
             ******************************** Sequence<T> ***********************************
             ********************************************************************************
             */
            template <typename T>
            inline Sequence<T>::Sequence ()
                : inherited (move (Concrete::Sequence_Factory<T>::mk ()))
            {
                _AssertRepValidType ();
            }
            template <typename T>
            inline Sequence<T>::Sequence (const Sequence<T>& src) noexcept
                : inherited (src)
            {
                _AssertRepValidType ();
            }
            template <typename T>
            inline Sequence<T>::Sequence (Sequence<T>&& src) noexcept
                : inherited (move (src))
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
            template <typename CONTAINER_OF_T, typename ENABLE_IF>
            inline Sequence<T>::Sequence (const CONTAINER_OF_T& src)
                : Sequence ()
            {
                AppendAll (src);
                _AssertRepValidType ();
            }
            template <typename T>
            inline Sequence<T>::Sequence (const _SharedPtrIRep& rep) noexcept
                : inherited ((RequireNotNull (rep), rep))
            {
                _AssertRepValidType ();
            }
            template <typename T>
            inline Sequence<T>::Sequence (_SharedPtrIRep&& rep) noexcept
                : inherited (move (rep))
            {
                //RequireNotNull (rep); -- logically required, but we cannot test here, must test before mem-initializers
                _AssertRepValidType ();
            }
            template <typename T>
            template <typename COPY_FROM_ITERATOR_OF_T>
            inline Sequence<T>::Sequence (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
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
            template <typename EQUALS_COMPARER>
            inline bool Sequence<T>::Contains (T item) const
            {
                return Private::Contains_<EQUALS_COMPARER, T> (*this, item);
            }
            template <typename T>
            template <typename ELEMENT_COMPARER>
            inline int Sequence<T>::Compare (const Iterable<T>& rhs) const
            {
                return Private::Compare_<T, ELEMENT_COMPARER> (*this, rhs);
            }
            template <typename T>
            inline Sequence<T> Sequence<T>::Where (const function<bool(ArgByValueType<T>)>& doToElement) const
            {
                return Iterable<T>::Where (doToElement, Sequence<T>{});
            }
            template <typename T>
            template <typename EQUALS_COMPARER>
            inline bool Sequence<T>::Equals (const Sequence<T>& rhs) const
            {
                return Private::Equals_<T, EQUALS_COMPARER> (*this, rhs);
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
            inline void Sequence<T>::SetAt (size_t i, T item)
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
            template <typename T>
            template <typename EQUALS_COMPARER>
            inline size_t Sequence<T>::IndexOf (ArgByValueType<T> item) const
            {
                return Private::IndexOf_<T, EQUALS_COMPARER> (*this, item, kBadSequenceIndex);
            }
            template <typename T>
            template <typename EQUALS_COMPARER>
            inline size_t Sequence<T>::IndexOf (const Sequence<T>& s) const
            {
                return Private::IndexOf_<T, EQUALS_COMPARER> (*this, s, kBadSequenceIndex);
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
            template <typename COPY_FROM_ITERATOR_OF_T>
            void Sequence<T>::InsertAll (size_t i, COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
            {
                Require (i <= this->GetLength ());
                size_t insertAt = i;
                for (auto i = start; i != end; ++i) {
                    Insert (insertAt++, *i);
                }
            }
            template <typename T>
            template <typename CONTAINER_OF_T, typename ENABLE_IF>
            inline void Sequence<T>::InsertAll (size_t i, const CONTAINER_OF_T& s)
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
            template <typename CONTAINER_OF_T, typename ENABLE_IF>
            inline void Sequence<T>::PrependAll (const CONTAINER_OF_T& s)
            {
                InsertAll (0, s);
            }
            template <typename T>
            template <typename COPY_FROM_ITERATOR_OF_T>
            inline void Sequence<T>::PrependAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
            {
                InsertAll (0, start, end);
            }
            template <typename T>
            inline void Sequence<T>::Append (ArgByValueType<T> item)
            {
                _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Insert (kBadSequenceIndex, &item, &item + 1);
            }
            template <typename T>
            template <typename CONTAINER_OF_T, typename ENABLE_IF>
            void Sequence<T>::AppendAll (const CONTAINER_OF_T& s)
            {
                _SafeReadWriteRepAccessor<_IRep> tmp = {this};
                _IRep*                           ww  = nullptr; // lazy _GetWriteableRep... in case we dont need
#if 0
                // faster but doesn't work if container doesnt have 'Apply' method - like vector
                s.Apply ([&ww] (const T & item) {
                    if (ww == nullptr) {
                        ww = &tmp._GetWriteableRep ();
                    }
                    ww->Insert (kBadSequenceIndex, &item, &item + 1);
                });
#else
                for (auto i : s) {
                    if (ww == nullptr) {
                        ww = &tmp._GetWriteableRep ();
                    }
                    ww->Insert (kBadSequenceIndex, &i, &i + 1);
                }
#endif
            }
            template <typename T>
            template <typename COPY_FROM_ITERATOR_OF_T>
            inline void Sequence<T>::AppendAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
            {
                _SafeReadWriteRepAccessor<_IRep> accessor = {this};
                for (auto i = start; i != end; ++i) {
                    T tmp = *i;
                    accessor._GetWriteableRep ().Insert (kBadSequenceIndex, &tmp, &tmp + 1);
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
            template <typename CONTAINER_OF_T>
            inline CONTAINER_OF_T Sequence<T>::As () const
            {
                return CONTAINER_OF_T (this->begin (), this->end ());
            }
            template <typename T>
            template <typename CONTAINER_OF_T>
            inline void Sequence<T>::As (CONTAINER_OF_T* into) const
            {
                RequireNotNull (into);
                *into = CONTAINER_OF_T (this->begin (), this->end ());
            }
            template <typename T>
            inline Memory::Optional<T> Sequence<T>::First () const
            {
                return this->IsEmpty () ? Memory::Optional<T>{} : GetAt (0);
            }
            template <typename T>
            inline Memory::Optional<T> Sequence<T>::First (const function<bool(ArgByValueType<T>)>& that) const
            {
                return inherited::First (that);
            }
            template <typename T>
            inline T Sequence<T>::FirstValue (ArgByValueType<T> defaultValue) const
            {
                return this->IsEmpty () ? defaultValue : GetAt (0);
            }
            template <typename T>
            inline Memory::Optional<T> Sequence<T>::Last () const
            {
                // IRep::GetAt() defined to allow special kBadSequenceIndex
                return this->IsEmpty () ? Memory::Optional<T>{} : _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetAt (kBadSequenceIndex);
            }
            template <typename T>
            inline Memory::Optional<T> Sequence<T>::Last (const function<bool(ArgByValueType<T>)>& that) const
            {
                // @todo when we have reverse iterators - we could implement this more efficiently by walking the sequence backwards
                return inherited::Last (that);
            }
            template <typename T>
            inline T Sequence<T>::LastValue (ArgByValueType<T> defaultValue) const
            {
                // IRep::GetAt() defined to allow special kBadSequenceIndex
                return this->IsEmpty () ? defaultValue : _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetAt (kBadSequenceIndex);
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
}
#endif /* _Stroika_Foundation_Containers_Sequence_inl_ */
