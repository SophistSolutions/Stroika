/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Sequence_inl_
#define _Stroika_Foundation_Containers_Sequence_inl_

#include    "../Debug/Assertions.h"

#include    "Concrete/Sequence_Array.h"  // needed for default constructor
#include    "Private/IterableUtils.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             ********************************* Sequence<T> **********************************
             ********************************************************************************
             */
            template    <typename T>
            Sequence<T>::Sequence ()
                : Iterable<T> (Concrete::Sequence_Array<T> ())
            {
            }
            template    <typename T>
            inline  Sequence<T>::Sequence (const Sequence<T>& s)
                : Iterable<T> (s)
            {
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  Sequence<T>::Sequence (const CONTAINER_OF_T& s)
                : Iterable<T> (Concrete::Sequence_Array<T> ())
            {
                InsertAll (0, s);
            }
            template    <typename T>
            inline  Sequence<T>::Sequence (const _SharedPtrIRep& rep)
                : Iterable<T> (typename Iterable<T>::_SharedByValueRepType (rep))
            {
                RequireNotNull (rep);
            }
            template    <typename T>
            template    <typename COPY_FROM_ITERATOR>
            inline Sequence<T>::Sequence (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end)
                : Iterable<T> (Concrete::Sequence_Array<T> ())
            {
                Append (start, end);
            }
            template    <typename T>
            inline  const typename  Sequence<T>::_IRep&    Sequence<T>::_GetRep () const
            {
                // Unsure - MAY need to use dynamic_cast here - but I think static cast performs better, so try...
                EnsureMember (&Iterable<T>::_GetRep (), Sequence<T>::_IRep);
                return *static_cast<const Sequence<T>::_IRep*> (&Iterable<T>::_GetRep ());
            }
            template    <typename T>
            inline  typename    Sequence<T>::_IRep&  Sequence<T>::_GetRep ()
            {
                // Unsure - MAY need to use dynamic_cast here - but I think static cast performs better, so try...
                EnsureMember (&Iterable<T>::_GetRep (), Sequence<T>::_IRep);
                return *static_cast<Sequence<T>::_IRep*> (&Iterable<T>::_GetRep ());
            }
            template    <typename T>
            inline  bool    Sequence<T>::Contains (T item) const
            {
                return Private::Contains_ (*this, item);
            }
            template    <typename T>
            inline  int    Sequence<T>::Compare (const Iterable<T>& rhs) const
            {
                return Private::Compare_ (*this, rhs);
            }
            template    <typename T>
            inline  bool    Sequence<T>::Equals (const Iterable<T>& rhs) const
            {
                return Private::Equals_ (*this, rhs);
            }
            template    <typename T>
            inline  void    Sequence<T>::RemoveAll ()
            {
                _GetRep ().Remove (0, this->GetLength ());
            }
            template    <typename T>
            inline  T    Sequence<T>::GetAt (size_t i) const
            {
                return _GetRep ().GetAt (i);
            }
            template    <typename T>
            inline  void    Sequence<T>::SetAt (size_t i, T item)
            {
                _GetRep ().SetAt (i, item);
            }
            template    <typename T>
            inline  T    Sequence<T>::operator[] (size_t i) const
            {
                return _GetRep ().GetAt (i);
            }
            template    <typename T>
            inline  size_t    Sequence<T>::IndexOf (T item) const
            {
#if     qCompilerAndStdLib_Supports_constexpr
                static_assert (Private::kBadIndex == kBadSequenceIndex, "Private::kBadIndex == kBadSequenceIndex");
#else
                Assert (Private::kBadIndex == kBadSequenceIndex);
#endif
                return Private::IndexOf_ (*this, item);
            }
            template    <typename T>
            inline  size_t    Sequence<T>::IndexOf (const Sequence<T>& s) const
            {
#if     qCompilerAndStdLib_Supports_constexpr
                static_assert (Private::kBadIndex == kBadSequenceIndex, "Private::kBadIndex == kBadSequenceIndex");
#else
                Assert (Private::kBadIndex == kBadSequenceIndex);
#endif
                return Private::IndexOf_ (*this, s);
            }
            template    <typename T>
            inline  size_t    Sequence<T>::IndexOf (const Iterator<T>& i) const
            {
                return _GetRep ().IndexOf (i);
            }
            template    <typename T>
            inline  void    Sequence<T>::Insert (size_t index, T item)
            {
                return _GetRep ().Insert (index, &item, &item + 1);
            }
            template    <typename T>
            template    <typename COPY_FROM_ITERATOR>
            void    Sequence<T>::InsertAll (size_t i, COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end)
            {
                size_t  insertAt = i;
                for (auto i = start; i != end; ++i) {
                    Insert (insertAt++, *i);
                }
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  void    Sequence<T>::InsertAll (size_t i, const CONTAINER_OF_T& s)
            {
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
            inline  void    Sequence<T>::Append (T item)
            {
                Insert (this->GetLength (), item);
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  void    Sequence<T>::AppendAll (const CONTAINER_OF_T& s)
            {
                InsertAll (this->GetLength (), s);
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
            inline  void    Sequence<T>::push_back (T item)
            {
                Append (item);
            }
            template    <typename   T>
            template    <typename   CONTAINER_OF_T>
            inline  CONTAINER_OF_T  Sequence<T>:: As () const
            {
                return CONTAINER_OF_T (this->begin (), this->end ());
            }
            template    <typename   T>
            template    <typename   CONTAINER_OF_T>
            inline  void    Sequence<T>::As (CONTAINER_OF_T* into) const
            {
                RequireNotNull (into);
                *into = CONTAINER_OF_T (this->begin (), this->end ());
            }
            template    <typename T>
            inline  T    Sequence<T>::back () const
            {
                Require (not this->IsEmpty ());
                return GetAt (this->GetLength () - 1);
            }
            template    <typename T>
            inline  T    Sequence<T>::front () const
            {
                Require (not this->IsEmpty ());
                return GetAt (0);
            }
            template    <typename T>
            inline  void    Sequence<T>::clear ()
            {
                RemoveAll ();
            }
#if 0
            template    <typename T>
            inline  Sequence<T>& Sequence<T>::operator+= (T item)
            {
                Add (item);
                return (*this);
            }
            template    <typename T>
            inline  Bag<T>& Bag<T>::operator+= (const Bag<T>& items)
            {
                Add (items);
                return (*this);
            }
            template    <typename T>
            inline  Bag<T>& Bag<T>::operator-= (T item)
            {
                Remove (item);
                return (*this);
            }
            template    <typename T>
            inline  Bag<T>& Bag<T>::operator-= (const Bag<T>& items)
            {
                Remove (items);
                return (*this);
            }
            template    <typename T>
            inline  void    Bag<T>::Add (T item)
            {
                _GetRep ().Add (item);
                // this-> required for gcc 4.6.3. I don't THINK this is a bug because I think its cuz its an inherite dmethod.
                // I must re-read C++ template docs for clarificaiton...
                //      -- LGP 2012-07-28
                Ensure (not this->IsEmpty ());
            }
            template    <typename T>
            void  Bag<T>::Add (const Bag<T>& items)
            {
                if (&_GetRep () == &items._GetRep ()) {
                    // Copy - so we don't update this while we are copying from it...
                    Bag<T>  copiedItems =   items;
                    for (T i : copiedItems) {
                        _GetRep ().Add (i);
                    }
                }
                else {
                    for (T i : items) {
                        _GetRep ().Add (i);
                    }
                }
            }
            template    <typename T>
            bool  Bag<T>::operator== (const Bag<T>& rhs) const
            {
                if (&this->_GetRep () == &rhs._GetRep ()) {
                    return (true);
                }
                if (this->GetLength () != rhs.GetLength ()) {
                    return (false);
                }

                // n^2 algorithm!!!
                for (T i : *this) {
                    if (this->TallyOf (i) != rhs.TallyOf (i)) {
                        return (false);
                    }
                }
                return (true);
            }
            template    <typename T>
            inline  bool    Bag<T>::operator!= (const Bag<T>& rhs) const
            {
                return (not (*this == rhs));
            }
#endif


#if 0
            /*
             ********************************************************************************
             ************************************ operators *********************************
             ********************************************************************************
             */
            template    <typename T>
            Bag<T>    operator+ (const Bag<T>& lhs, const Bag<T>& rhs)
            {
                Bag<T>  temp    = lhs;
                temp += rhs;
                return (temp);
            }
            template    <typename T>
            Bag<T>    operator- (const Bag<T>& lhs, const Bag<T>& rhs)
            {
                Bag<T>  temp    = lhs;
                temp -= rhs;
                return (temp);
            }
#endif



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
