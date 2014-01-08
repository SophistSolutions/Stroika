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
             **************************** Sequence<T, TRAITS> *******************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  Sequence<T, TRAITS>::Sequence ()
                : inherited (Concrete::Sequence_Factory<T, TRAITS>::mk ())
            {
            }
            template    <typename T, typename TRAITS>
            inline  Sequence<T, TRAITS>::Sequence (const Sequence<T, TRAITS>& s)
                : inherited (static_cast<const inherited&> (s))
            {
            }
            template    <typename T, typename TRAITS>
            inline  Sequence<T, TRAITS>::Sequence (const initializer_list<T>& s)
                : inherited (Concrete::Sequence_Factory<T, TRAITS>::mk ())
            {
                AppendAll (s);
            }
            template    <typename T, typename TRAITS>
            inline  Sequence<T, TRAITS>::Sequence (const vector<T>& s)
                : inherited (Concrete::Sequence_Factory<T, TRAITS>::mk ())
            {
                AppendAll (s);
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  Sequence<T, TRAITS>::Sequence (const CONTAINER_OF_T& s)
                : inherited (Concrete::Sequence_Factory<T, TRAITS>::mk ())
            {
                AppendAll (s);
            }
            template    <typename T, typename TRAITS>
            inline  Sequence<T, TRAITS>::Sequence (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                RequireNotNull (rep);
            }
            template    <typename T, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            inline Sequence<T, TRAITS>::Sequence (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                : inherited (Concrete::Sequence_Factory<T, TRAITS>::mk ())
            {
                AppendAll (start, end);
            }
            template    <typename T, typename TRAITS>
            inline  Sequence<T, TRAITS>& Sequence<T, TRAITS>::operator= (const Sequence<T, TRAITS>& rhs)
            {
                inherited::operator= (rhs);
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  const typename  Sequence<T, TRAITS>::_IRep&    Sequence<T, TRAITS>::_GetRep () const
            {
                EnsureMember (&inherited::_GetRep (), _IRep);       // static_cast more efficient, but assert to verify safe
                return *static_cast<const _IRep*> (&inherited::_GetRep ());
            }
            template    <typename T, typename TRAITS>
            inline  typename    Sequence<T, TRAITS>::_IRep&  Sequence<T, TRAITS>::_GetRep ()
            {
                EnsureMember (&inherited::_GetRep (), _IRep);       // static_cast more efficient, but assert to verify safe
                return *static_cast<_IRep*> (&inherited::_GetRep ());
            }
            template    <typename T, typename TRAITS>
            inline  bool    Sequence<T, TRAITS>::Contains (T item) const
            {
                RequireConceptAppliesToTypeInFunction(Concept_EqualsCompareFunctionType, EqualsCompareFunctionType);
                return Private::Contains_<T, EqualsCompareFunctionType> (*this, item);
            }
            template    <typename T, typename TRAITS>
            template    <typename ELEMENT_COMPARER>
            inline  int    Sequence<T, TRAITS>::Compare (const Iterable<T>& rhs) const
            {
                return Private::Compare_<T, ELEMENT_COMPARER> (*this, rhs);
            }
            template    <typename T, typename TRAITS>
            Sequence<T, TRAITS>    Sequence<T, TRAITS>::EachWith (const std::function<bool(const T& item)>& doToElement) const
            {
                Sequence<T, TRAITS>  result;
                for (T i : *this) {
                    if (doToElement (i)) {
                        result.Append (i);
                    }
                }
                return result;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Sequence<T, TRAITS>::Equals (const Sequence<T, TRAITS>& rhs) const
            {
                RequireConceptAppliesToTypeInFunction(Concept_EqualsCompareFunctionType, EqualsCompareFunctionType);
                return Private::Equals_<T, EqualsCompareFunctionType> (*this, rhs);
            }
            template    <typename T, typename TRAITS>
            inline  void    Sequence<T, TRAITS>::RemoveAll ()
            {
                _GetRep ().Remove (0, this->GetLength ());
            }
            template    <typename T, typename TRAITS>
            inline  T    Sequence<T, TRAITS>::GetAt (size_t i) const
            {
                Require (i < this->GetLength ());
                return _GetRep ().GetAt (i);
            }
            template    <typename T, typename TRAITS>
            inline  void    Sequence<T, TRAITS>::SetAt (size_t i, T item)
            {
                Require (i < this->GetLength ());
                _GetRep ().SetAt (i, item);
            }
            template    <typename T, typename TRAITS>
            inline  T    Sequence<T, TRAITS>::operator[] (size_t i) const
            {
                Require (i < this->GetLength ());
                return _GetRep ().GetAt (i);
            }
            template    <typename T, typename TRAITS>
            inline  size_t    Sequence<T, TRAITS>::IndexOf (T item) const
            {
                RequireConceptAppliesToTypeInFunction(Concept_EqualsCompareFunctionType, EqualsCompareFunctionType);
                return Private::IndexOf_<T, EqualsCompareFunctionType> (*this, item, kBadSequenceIndex);
            }
            template    <typename T, typename TRAITS>
            inline  size_t    Sequence<T, TRAITS>::IndexOf (const Sequence<T, TRAITS>& s) const
            {
                RequireConceptAppliesToTypeInFunction(Concept_EqualsCompareFunctionType, EqualsCompareFunctionType);
                return Private::IndexOf_<T, EqualsCompareFunctionType> (*this, s, kBadSequenceIndex);
            }
            template    <typename T, typename TRAITS>
            inline  size_t    Sequence<T, TRAITS>::IndexOf (const Iterator<T>& i) const
            {
                return _GetRep ().IndexOf (i);
            }
            template    <typename T, typename TRAITS>
            inline  void    Sequence<T, TRAITS>::Insert (size_t i, T item)
            {
                Require (i <= this->GetLength ());
                return _GetRep ().Insert (i, &item, &item + 1);
            }
            template    <typename T, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            void    Sequence<T, TRAITS>::InsertAll (size_t i, COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
            {
                Require (i <= this->GetLength ());
                size_t  insertAt = i;
                for (auto i = start; i != end; ++i) {
                    Insert (insertAt++, *i);
                }
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  void    Sequence<T, TRAITS>::InsertAll (size_t i, const CONTAINER_OF_T& s)
            {
                Require (i <= this->GetLength ());
                InsertAll (i, s.begin (), s.end ());
            }
            template    <typename T, typename TRAITS>
            inline  void    Sequence<T, TRAITS>::Prepend (T item)
            {
                Insert (0, item);
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  void    Sequence<T, TRAITS>::PrependAll (const CONTAINER_OF_T& s)
            {
                InsertAll (0, s);
            }
            template    <typename T, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            inline void Sequence<T, TRAITS>::PrependAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
            {
                InsertAll (0, start, end);
            }
            template    <typename T, typename TRAITS>
            inline  void    Sequence<T, TRAITS>::Append (T item)
            {
                _GetRep ().Insert (kBadSequenceIndex, &item, &item + 1);
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  void    Sequence<T, TRAITS>::AppendAll (const CONTAINER_OF_T& s)
            {
                for (auto i : s) {
                    _GetRep ().Insert (kBadSequenceIndex, &i, &i + 1);
                }
            }
            template    <typename T, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            inline void Sequence<T, TRAITS>::AppendAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
            {
                for (auto i = start; i != end; ++i) {
                    auto tmp = *i;
                    _GetRep ().Insert (kBadSequenceIndex, &tmp, &tmp + 1);
                }
            }
            template    <typename T, typename TRAITS>
            inline  void    Sequence<T, TRAITS>::Update (const Iterator<T>& i, T newValue)
            {
                _GetRep ().Update (i, newValue);
            }
            template    <typename T, typename TRAITS>
            inline  void    Sequence<T, TRAITS>::Remove (size_t i)
            {
                Require (i < this->GetLength ());
                _GetRep ().Remove (i, i + 1);
            }
            template    <typename T, typename TRAITS>
            inline  void    Sequence<T, TRAITS>::Remove (size_t start, size_t end)
            {
                Require (start <= end and end <= this->GetLength ());
                _GetRep ().Remove (start, end);
            }
            template    <typename T, typename TRAITS>
            inline  void    Sequence<T, TRAITS>::Remove (const Iterator<T>& i)
            {
                _GetRep ().Remove (i);
            }
            template    <typename T, typename TRAITS>
            template    <typename   CONTAINER_OF_T>
            inline  CONTAINER_OF_T  Sequence<T, TRAITS>::As () const
            {
                return CONTAINER_OF_T (this->begin (), this->end ());
            }
            template    <typename T, typename TRAITS>
            template    <typename   CONTAINER_OF_T>
            inline  void    Sequence<T, TRAITS>::As (CONTAINER_OF_T* into) const
            {
                RequireNotNull (into);
                *into = CONTAINER_OF_T (this->begin (), this->end ());
            }
            template    <typename T, typename TRAITS>
            inline  T    Sequence<T, TRAITS>::GetFirst () const
            {
                Require (not this->IsEmpty ());
                return GetAt (0);
            }
            template    <typename T, typename TRAITS>
            inline  T    Sequence<T, TRAITS>::GetLast () const
            {
                Require (not this->IsEmpty ());
                // IRep::GetAt() defined to allow special kBadSequenceIndex
                return _GetRep ().GetAt (kBadSequenceIndex);
            }
            template    <typename T, typename TRAITS>
            inline  void    Sequence<T, TRAITS>::push_back (T item)
            {
                Append (item);
            }
            template    <typename T, typename TRAITS>
            inline  T    Sequence<T, TRAITS>::back () const
            {
                return GetLast ();
            }
            template    <typename T, typename TRAITS>
            inline  T    Sequence<T, TRAITS>::front () const
            {
                return GetFirst ();
            }
            template    <typename T, typename TRAITS>
            inline  void    Sequence<T, TRAITS>::clear ()
            {
                RemoveAll ();
            }
            template    <typename T, typename TRAITS>
            inline  Sequence<T, TRAITS>& Sequence<T, TRAITS>::operator+= (T item)
            {
                Append (item);
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Sequence<T, TRAITS>& Sequence<T, TRAITS>::operator+= (const Sequence<T, TRAITS>& items)
            {
                AppendAll (items);
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  bool Sequence<T, TRAITS>::operator< (const Sequence<T, TRAITS>& rhs) const
            {
                return Compare (rhs) < 0;
            }
            template    <typename T, typename TRAITS>
            inline  bool Sequence<T, TRAITS>::operator<= (const Sequence<T, TRAITS>& rhs) const
            {
                return Compare (rhs) <= 0;
            }
            template    <typename T, typename TRAITS>
            inline  bool Sequence<T, TRAITS>::operator> (const Sequence<T, TRAITS>& rhs) const
            {
                return Compare (rhs) > 0;
            }
            template    <typename T, typename TRAITS>
            inline  bool Sequence<T, TRAITS>::operator>= (const Sequence<T, TRAITS>& rhs) const
            {
                return Compare (rhs) >= 0;
            }
            template    <typename T, typename TRAITS>
            inline  bool Sequence<T, TRAITS>::operator== (const Sequence<T, TRAITS>& rhs) const
            {
                return Equals (rhs);
            }
            template    <typename T, typename TRAITS>
            inline  bool Sequence<T, TRAITS>::operator!= (const Sequence<T, TRAITS>& rhs) const
            {
                return not Equals (rhs);
            }


            /*
             ********************************************************************************
             *************************** Sequence<T>::_IRep *********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  Sequence<T, TRAITS>::_IRep::_IRep ()
            {
            }
            template    <typename T, typename TRAITS>
            inline  Sequence<T, TRAITS>::_IRep::~_IRep ()
            {
            }


        }
    }
}
#endif /* _Stroika_Foundation_Containers_Sequence_inl_ */
