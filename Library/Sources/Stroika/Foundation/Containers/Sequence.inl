/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Sequence_inl_
#define _Stroika_Foundation_Containers_Sequence_inl_

#include    "../Debug/Assertions.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             ****************************** Sequence<T> *************************************
             ********************************************************************************
             */
#if 0
            template    <typename T>
            Sequence<T>::Sequence ()
                : Iterable<T> (Concrete::Sequence_Array<T> ())
            {
            }
#endif
            template    <typename T>
            inline  Sequence<T>::Sequence (const Sequence<T>& s)
                : Iterable<T> (s)
            {
            }
            template    <typename T>
            inline  Sequence<T>::Sequence (const _SharedPtrIRep& rep)
                : Iterable<T> (typename Iterable<T>::_SharedByValueRepType (rep))
            {
                RequireNotNull (rep);
            }
#if 0
            template    <typename T>
            Sequence<T>::Sequence (const T* start, const T* end)
                : Iterable<T> (Concrete::Sequence_Array<T> (start, end))
            {
            }
#endif
#if 1
            template    <typename T>
            bool    Sequence<T>::Contains (T item) const
            {
                for (T i : *this) {
                    if (i == item) {
                        return true;
                    }
                }
            }
            template    <typename T>
            int    Sequence<T>::Compare (const Sequence<T>& rhs) const
            {
#if 0
                auto i =
                for (T i : *this) {
                    int cmp = i.Compare (
                    if (i.Compare () {
                    return true;
                }
            }
#endif
            return 0;
        }
#endif
                  template    <typename T>
        inline  void    Sequence<T>::RemoveAll ()
            {
                _GetRep ().RemoveAll ();
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
            void  Bag<T>::Add (const T* begin, const T* end)
            {
                for (const T* i = begin; i != end; i++) {
                    Add (*i);
                }
            }
            template    <typename T>
            inline  void    Bag<T>::Update (const Iterator<T>& i, T newValue)
            {
                _GetRep ().Update (i, newValue);
            }
            template    <typename T>
            inline  void  Bag<T>::Remove (T item)
            {
                _GetRep ().Remove (item);
            }
            template    <typename T>
            void  Bag<T>::Remove (const Bag<T>& items)
            {
                if (&_GetRep () == &items._GetRep ()) {
                    RemoveAll ();
                }
                else {
                    for (T i : items) {
                        _GetRep ().Remove (i);
                    }
                }
            }
            template    <typename T>
            inline  void    Bag<T>::Remove (const Iterator<T>& i)
            {
                _GetRep ().Remove (i);
            }
            template    <typename T>
            size_t    Bag<T>::TallyOf (T item) const
            {
                size_t  count = 0;
                for (T i : *this) {
                    if (i == item) {
                        count++;
                    }
                }
                return (count);
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


#if 0
            /*
             ********************************************************************************
             ******************************** Bag<T>::_IRep *********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  Bag<T>::_IRep::_IRep ()
            {
            }
            template    <typename T>
            inline  Bag<T>::_IRep::~_IRep ()
            {
            }
            template    <typename T>
            inline  const typename  Bag<T>::_IRep&    Bag<T>::_GetRep () const
            {
                // Unsure - MAY need to use dynamic_cast here - but I think static cast performs better, so try...
                EnsureMember (&Iterable<T>::_GetRep (), Bag<T>::_IRep);
                return *static_cast<const Bag<T>::_IRep*> (&Iterable<T>::_GetRep ());
            }
            template    <typename T>
            inline  typename    Bag<T>::_IRep&  Bag<T>::_GetRep ()
            {
                // Unsure - MAY need to use dynamic_cast here - but I think static cast performs better, so try...
                EnsureMember (&Iterable<T>::_GetRep (), Bag<T>::_IRep);
                return *static_cast<Bag<T>::_IRep*> (&Iterable<T>::_GetRep ());
            }
#endif


        }
    }
}



#endif /* _Stroika_Foundation_Containers_Sequence_inl_ */
