/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Sequence_inl_
#define _Stroika_Foundation_Containers_Sequence_inl_

#include    "../Debug/Assertions.h"

#include    "Concrete/Sequence_Factory.h"
#include    "Private/IterableUtils.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             ******************************** Sequence<T> ***********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  Sequence<T>::Sequence ()
                : inherited (move (Concrete::Sequence_Factory<T>::mk ()))
            {
            }
            template    <typename T>
            inline  Sequence<T>::Sequence (const Sequence<T>& s)
                : inherited (static_cast<const inherited&> (s))
            {
            }
            template    <typename T>
            inline  Sequence<T>::Sequence (const initializer_list<T>& s)
                : inherited (move (Concrete::Sequence_Factory<T>::mk ()))
            {
                AppendAll (s);
            }
            template    <typename T>
            inline  Sequence<T>::Sequence (const vector<T>& s)
                : inherited (move (Concrete::Sequence_Factory<T>::mk ()))
            {
                AppendAll (s);
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  Sequence<T>::Sequence (const CONTAINER_OF_T& s)
                : inherited (move (Concrete::Sequence_Factory<T>::mk ()))
            {
                AppendAll (s);
            }
            template    <typename T>
            inline  Sequence<T>::Sequence (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                RequireNotNull (rep);
            }
            template    <typename T>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            inline Sequence<T>::Sequence (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                : inherited (move (Concrete::Sequence_Factory<T>::mk ()))
            {
                AppendAll (start, end);
            }
#if     qDebug
            template    <typename T>
            Sequence<T>::~Sequence ()
            {
                if (this->_GetSharingState () != Memory::SharedByValue_State::eNull) {
                    _GetRep ().AssertNoIteratorsReferenceOwner (this);
                }
            }
#endif
            template    <typename T>
            inline  Sequence<T>& Sequence<T>::operator= (const Sequence<T>& rhs)
            {
                inherited::operator= (rhs);
                return *this;
            }
            template    <typename T>
            inline  const typename  Sequence<T>::_IRep&    Sequence<T>::_GetRep () const
            {
                EnsureMember (&inherited::_GetRep (), _IRep);       // static_cast more efficient, but assert to verify safe
                return *static_cast<const _IRep*> (&inherited::_GetRep ());
            }
            template    <typename T>
            inline  typename    Sequence<T>::_IRep&  Sequence<T>::_GetRep ()
            {
                EnsureMember (&inherited::_GetRep (), _IRep);       // static_cast more efficient, but assert to verify safe
                return *static_cast<_IRep*> (&inherited::_GetRep ());
            }
            template    <typename T>
            template    <typename EQUALS_COMPARER>
            inline  bool    Sequence<T>::Contains (T item) const
            {
                return Private::Contains_<EQUALS_COMPARER, T> (*this, item);
            }
            template    <typename T>
            template    <typename ELEMENT_COMPARER>
            inline  int    Sequence<T>::Compare (const Iterable<T>& rhs) const
            {
                return Private::Compare_<T, ELEMENT_COMPARER> (*this, rhs);
            }
            template    <typename T>
            Sequence<T>    Sequence<T>::EachWith (const function<bool(const T& item)>& doToElement) const
            {
                Sequence<T>  result;
                for (T i : *this) {
                    if (doToElement (i)) {
                        result.Append (i);
                    }
                }
                return result;
            }
            template    <typename T>
            template    <typename EQUALS_COMPARER>
            inline  bool    Sequence<T>::Equals (const Sequence<T>& rhs) const
            {
                return Private::Equals_<T, EQUALS_COMPARER> (*this, rhs);
            }
            template    <typename T>
            inline  void    Sequence<T>::RemoveAll ()
            {
                _GetRep ().Remove (0, this->GetLength ());
            }
            template    <typename T>
            inline  T    Sequence<T>::GetAt (size_t i) const
            {
                Require (i < this->GetLength ());
                return _GetRep ().GetAt (i);
            }
            template    <typename T>
            inline  void    Sequence<T>::SetAt (size_t i, T item)
            {
                Require (i < this->GetLength ());
                _GetRep ().SetAt (i, item);
            }
            template    <typename T>
            inline  T    Sequence<T>::operator[] (size_t i) const
            {
                Require (i < this->GetLength ());
                return _GetRep ().GetAt (i);
            }
            template    <typename T>
            template    <typename EQUALS_COMPARER>
            inline  size_t    Sequence<T>::IndexOf (T item) const
            {
                return Private::IndexOf_<T, EQUALS_COMPARER> (*this, item, kBadSequenceIndex);
            }
            template    <typename T>
            template    <typename EQUALS_COMPARER>
            inline  size_t    Sequence<T>::IndexOf (const Sequence<T>& s) const
            {
                return Private::IndexOf_<T, EQUALS_COMPARER> (*this, s, kBadSequenceIndex);
            }
            template    <typename T>
            template    <typename IGNORED>
            inline  size_t    Sequence<T>::IndexOf (const Iterator<T>& i) const
            {
                return _GetRep ().IndexOf (i);
            }
            template    <typename T>
            inline  void    Sequence<T>::Insert (size_t i, T item)
            {
                Require (i <= this->GetLength ());
                return _GetRep ().Insert (i, &item, &item + 1);
            }
            template    <typename T>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            void    Sequence<T>::InsertAll (size_t i, COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
            {
                Require (i <= this->GetLength ());
                size_t  insertAt = i;
                for (auto i = start; i != end; ++i) {
                    Insert (insertAt++, *i);
                }
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  void    Sequence<T>::InsertAll (size_t i, const CONTAINER_OF_T& s)
            {
                Require (i <= this->GetLength ());
                InsertAll (i, s.begin (), s.end ());
            }
            template    <typename T>
            inline  void    Sequence<T>::Prepend (T item)
            {
                Insert (0, item);
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  void    Sequence<T>::PrependAll (const CONTAINER_OF_T& s)
            {
                InsertAll (0, s);
            }
            template    <typename T>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            inline void Sequence<T>::PrependAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
            {
                InsertAll (0, start, end);
            }
            template    <typename T>
            inline  void    Sequence<T>::Append (T item)
            {
                _GetRep ().Insert (kBadSequenceIndex, &item, &item + 1);
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  void    Sequence<T>::AppendAll (const CONTAINER_OF_T& s)
            {
                for (auto i : s) {
                    _GetRep ().Insert (kBadSequenceIndex, &i, &i + 1);
                }
            }
            template    <typename T>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            inline void Sequence<T>::AppendAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
            {
                for (auto i = start; i != end; ++i) {
                    T tmp = *i;
                    _GetRep ().Insert (kBadSequenceIndex, &tmp, &tmp + 1);
                }
            }
            template    <typename T>
            inline  void    Sequence<T>::Update (const Iterator<T>& i, T newValue)
            {
                _GetRep ().Update (i, newValue);
            }
            template    <typename T>
            inline  void    Sequence<T>::Remove (size_t i)
            {
                Require (i < this->GetLength ());
                _GetRep ().Remove (i, i + 1);
            }
            template    <typename T>
            inline  void    Sequence<T>::Remove (size_t start, size_t end)
            {
                Require (start <= end and end <= this->GetLength ());
                _GetRep ().Remove (start, end);
            }
            template    <typename T>
            inline  void    Sequence<T>::Remove (const Iterator<T>& i)
            {
                _GetRep ().Remove (i);
            }
            template    <typename T>
            template    <typename   CONTAINER_OF_T>
            inline  CONTAINER_OF_T  Sequence<T>::As () const
            {
                return CONTAINER_OF_T (this->begin (), this->end ());
            }
            template    <typename T>
            template    <typename   CONTAINER_OF_T>
            inline  void    Sequence<T>::As (CONTAINER_OF_T* into) const
            {
                RequireNotNull (into);
                *into = CONTAINER_OF_T (this->begin (), this->end ());
            }
            template    <typename T>
            inline  T    Sequence<T>::GetFirst () const
            {
                Require (not this->IsEmpty ());
                return GetAt (0);
            }
            template    <typename T>
            inline  T    Sequence<T>::GetLast () const
            {
                Require (not this->IsEmpty ());
                // IRep::GetAt() defined to allow special kBadSequenceIndex
                return _GetRep ().GetAt (kBadSequenceIndex);
            }
            template    <typename T>
            inline  void    Sequence<T>::push_back (T item)
            {
                Append (item);
            }
            template    <typename T>
            inline  T    Sequence<T>::back () const
            {
                return GetLast ();
            }
            template    <typename T>
            inline  T    Sequence<T>::front () const
            {
                return GetFirst ();
            }
            template    <typename T>
            inline  void    Sequence<T>::clear ()
            {
                RemoveAll ();
            }
            template    <typename T>
            inline  Sequence<T>& Sequence<T>::operator+= (T item)
            {
                Append (item);
                return *this;
            }
            template    <typename T>
            inline  Sequence<T>& Sequence<T>::operator+= (const Sequence<T>& items)
            {
                AppendAll (items);
                return *this;
            }
            template    <typename T>
            inline  bool Sequence<T>::operator< (const Sequence<T>& rhs) const
            {
                return Compare (rhs) < 0;
            }
            template    <typename T>
            inline  bool Sequence<T>::operator<= (const Sequence<T>& rhs) const
            {
                return Compare (rhs) <= 0;
            }
            template    <typename T>
            inline  bool Sequence<T>::operator> (const Sequence<T>& rhs) const
            {
                return Compare (rhs) > 0;
            }
            template    <typename T>
            inline  bool Sequence<T>::operator>= (const Sequence<T>& rhs) const
            {
                return Compare (rhs) >= 0;
            }
            template    <typename T>
            inline  bool Sequence<T>::operator== (const Sequence<T>& rhs) const
            {
                return Equals (rhs);
            }
            template    <typename T>
            inline  bool Sequence<T>::operator!= (const Sequence<T>& rhs) const
            {
                return not Equals (rhs);
            }


            /*
             ********************************************************************************
             *************************** Sequence<T>::_IRep *********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  Sequence<T>::_IRep::_IRep ()
            {
            }
            template    <typename T>
            inline  Sequence<T>::_IRep::~_IRep ()
            {
            }


        }
    }
}
#endif /* _Stroika_Foundation_Containers_Sequence_inl_ */
