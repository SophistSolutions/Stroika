/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Iterable_inl_
#define _Stroika_Foundation_Containers_Iterable_inl_

#include    "../Debug/Assertions.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {

            // Class Iterable<T>::_IRep
            template    <typename T>
            inline  Iterable<T>::_IRep::_IRep ()
            {
            }
            template    <typename T>
            inline  Iterable<T>::_IRep::~_IRep ()
            {
            }
            template    <typename T>
            inline  bool    Iterable<T>::_IRep::_IsEmpty () const
            {
                RequireNotNull (fRep_.get ());
                return GetLength () == 0;
            }
            template    <typename T>
            inline  void    Iterable<T>::_IRep::_Apply (void (*doToElement) (const T& item)) const
            {
                RequireNotNull (doToElement);
                for (Iterator<T> i = MakeIterator (); i != Iterable<T>::end (); ++i) {
                    (doToElement) (*i);
                }
            }
            template    <typename T>
            inline  Iterator<T>    Iterable<T>::_IRep::_ApplyUntilTrue (bool (*doToElement) (const T& item)) const
            {
                RequireNotNull (doToElement);
                for (Iterator<T> i = MakeIterator (); i != Iterable<T>::end (); ++i) {
                    if ((doToElement) (*i)) {
                        return i;
                    }
                }
                return end ();
            }





            // Class Iterable<T>
            template    <typename T>
            inline  Iterable<T>::Iterable (const _SharedByValueRepType& rep)
                : fRep_ (rep)
            {
                RequireNotNull (fRep_.get ());
            }
            template    <typename T>
            inline  Iterable<T>::Iterable (const Iterable<T>& from)
                : fRep_ (from.fRep_)
            {
                RequireNotNull (fRep_.get ());
            }
            template    <typename T>
            inline  Iterable<T>::~Iterable ()
            {
            }
            template    <typename T>
            inline  Iterable<T>&    Iterable<T>::operator= (const Iterable<T>& rhs)
            {
                if (this != &rhs) {
                    RequireNotNull (rhs.fRep_.get ());
                    fRep_ = rhs.fRep_;
                }
                return *this;
            }
            template    <typename T>
            inline	typename Iterable<T>::_IRep*  Iterable<T>::Clone_ (const typename Iterable<T>::_IRep& rep)
            {
                return (rep.Clone ());
            }
            template    <typename T>
            inline  typename Iterable<T>::_IRep&         Iterable<T>::_GetRep ()
            {
                EnsureNotNull (fRep_.get ());
                return *fRep_;
            }
            template    <typename T>
            inline  const typename Iterable<T>::_IRep&   Iterable<T>::_GetRep () const
            {
                EnsureNotNull (fRep_.get ());
                return *fRep_;
            }
            template    <typename T>
            inline  Iterator<T>     Iterable<T>::MakeIterator () const
            {
                return _GetRep ().MakeIterator ();
            }
            template    <typename T>
            inline  size_t  Iterable<T>::GetLength () const
            {
                return _GetRep ().GetLength ();
            }
            template    <typename T>
            inline  bool    Iterable<T>::IsEmpty () const
            {
                return _GetRep ().IsEmpty ();
            }
            template    <typename T>
            inline  bool    Iterable<T>::empty () const
            {
                return IsEmpty ();
            }
            template    <typename T>
            inline  size_t  Iterable<T>::length () const
            {
                return GetLength ();
            }
            template    <typename T>
            inline  size_t  Iterable<T>::size () const
            {
                return GetLength ();
            }
            template    <typename T>
            inline  Iterator<T> Iterable<T>::begin () const
            {
                return MakeIterator ();
            }
            template    <typename T>
            inline  Iterator<T>    Iterable<T>::end ()
            {
                return (Iterator<T>::GetSentinal ());
            }
            template    <typename T>
            inline  void    Iterable<T>::Apply (void (*doToElement) (const T& item)) const
            {
                RequireNotNull (doToElement);
                return _GetRep ().Apply (doToElement);
            }
            template    <typename T>
            inline  Iterator<T>    Iterable<T>::ApplyUntilTrue (bool (*doToElement) (const T& item)) const
            {
                RequireNotNull (doToElement);
                return _GetRep ().ApplyUntilTrue (doToElement);
            }
            template    <typename T>
            void    Iterable<T>::_SetRep (_SharedByValueRepType rep)
            {
                RequireNotNull (rep.get ());
                if (rep.get () != fRep_.get ()) {
                    fRep_ = rep;
                }
                EnsureNotNull (rep.get ());
            }
        }
    }
}



#endif /* _Stroika_Foundation_Containers_Iterable_inl_ */

