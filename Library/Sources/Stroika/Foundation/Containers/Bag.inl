/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Bag_inl_
#define _Stroika_Foundation_Containers_Bag_inl_

#include    "../Debug/Assertions.h"

#include    "Concrete/Bag_Array.h"  // needed for default constructor



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {

            // class Bag<T>
            template    <typename T>
            inline  Bag<T>::Bag (const Bag<T>& bag)
                : Iterable<T> (bag)
            {
            }
            template    <typename T>
            inline  Bag<T>::Bag (typename Bag<T>::_IRep* rep)
                : Iterable<T> (typename Iterable<T>::_SharedByValueRepType (rep))
            {
                RequireNotNull (rep);
            }
            template    <typename T>
            Bag<T>::Bag ()
                : Iterable<T> (Concrete::Bag_Array<T> ())
            {
            }
            template    <typename T>
            Bag<T>::Bag (const T* start, const T* end)
                : Iterable<T> (Concrete::Bag_Array<T> (start, end))
            {
            }
            template    <typename T>
            inline  bool    Bag<T>::Contains (T item) const
            {
                return (_GetRep ().Contains (item));
            }
            template    <typename T>
            inline  void    Bag<T>::RemoveAll ()
            {
                _GetRep ().RemoveAll ();
            }
            template    <typename T>
            inline  void    Bag<T>::clear ()
            {
                RemoveAll ();
            }
            template    <typename T>
            inline  void    Bag<T>::Compact ()
            {
                _GetRep ().Compact ();
            }
            template    <typename T>
            inline  Bag<T>& Bag<T>::operator+= (T item)
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





            /*
             ********************************************************************************
             ************************************ operators *********************************
             ********************************************************************************
             */
            template    <typename T>
            bool  operator== (const Bag<T>& lhs, const Bag<T>& rhs)
            {
                if (&lhs._GetRep () == &rhs._GetRep ()) {
                    return (true);
                }
                if (lhs.GetLength () != rhs.GetLength ()) {
                    return (false);
                }

                // n^2 algorithm!!!
for (T i : lhs) {
                    if (lhs.TallyOf (i) != rhs.TallyOf (i)) {
                        return (false);
                    }
                }
                return (true);
            }
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
            template    <typename T>
            inline  bool    operator!= (const Bag<T>& lhs, const Bag<T>& rhs)
            {
                return (not operator== (lhs, rhs));
            }






            // class _IRep<T>
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


        }
    }
}

#endif /* _Stroika_Foundation_Containers_Bag_inl_ */
