/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Iterable_inl_
#define _Stroika_Foundation_Containers_Iterable_inl_

#include    "../Debug/Assertions.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {

            // Class Iterable<T>::IRep
            template    <typename T>
            inline  Iterable<T>::IRep::IRep ()
            {
            }
            template    <typename T>
            inline  Iterable<T>::IRep::~IRep ()
            {
            }
            template    <typename T>
            inline  bool    Iterable<T>::IRep::_IsEmpty () const
            {
                RequireNotNull (_fRep.get ());
                return GetLength () == 0;
            }
            template    <typename T>
            inline  void    Iterable<T>::IRep::_Apply (void (*doToElement) (const T& item)) const
            {
                RequireNotNull (doToElement);
                for (Iterator<T> i = begin (); i != end (); ++i) {
                    (doToElement) (*i);
                }
            }
            template    <typename T>
            inline  Iterator<T>    Iterable<T>::IRep::_ApplyUntilTrue (bool (*doToElement) (const T& item)) const
            {
                RequireNotNull (doToElement);
                for (Iterator<T> i = begin (); i != end (); ++i) {
                    if ((doToElement) (*i)) {
                        return i;
                    }
                }
                return end ();
            }





            // Class Iterable<T>
            template    <typename T>
            inline  Iterable<T>::Iterable (const Memory::SharedByValue<IRep>& rep)
                : _fRep (rep)
            {
            }
            template    <typename T>
            inline  Iterable<T>::Iterable (const Iterable<T>& from)
                : _fRep (from._fRep)
            {
            }
            template    <typename T>
            inline  Iterable<T>::~Iterable ()
            {
            }
            template    <typename T>
            inline  Iterable<T>&    Iterable<T>::operator= (const Iterable<T>& rhs)
            {
                if (this != &rhs) {
                    _fRep = rhs._fRep;
                }
                return *this;
            }
            template    <typename T>
            inline  Iterator<T>     Iterable<T>::MakeIterator () const
            {
                RequireNotNull (_fRep.get ());
                return _fRep.get ()->MakeIterator ();
            }
            template    <typename T>
            inline  size_t  Iterable<T>::GetLength () const
            {
                RequireNotNull (_fRep.get ());
                return _fRep.get ()->GetLength ();
            }
            template    <typename T>
            inline  bool    Iterable<T>::IsEmpty () const
            {
                RequireNotNull (_fRep.get ());
                return _fRep.get ()->IsEmpty ();
            }
            template    <typename T>
            inline  bool    Iterable<T>::empty () const
            {
                RequireNotNull (_fRep.get ());
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
                RequireNotNull (_fRep.get ());
                return MakeIterator ();
            }
            template    <typename T>
            inline  Iterator<T>    Iterable<T>::end () const
            {
                return (Iterator<T>::GetSentinal ());
            }
            template    <typename T>
            inline  void    Iterable<T>::Apply (void (*doToElement) (const T& item)) const
            {
                RequireNotNull (_fRep.get ());
                RequireNotNull (doToElement);
                return _fRep.get ()->Apply (doToElement);
            }
            template    <typename T>
            inline  Iterator<T>    Iterable<T>::ApplyUntilTrue (bool (*doToElement) (const T& item)) const
            {
                RequireNotNull (_fRep.get ());
                RequireNotNull (doToElement);
                return _fRep.get ()->ApplyUntilTrue (doToElement);
            }


        }
    }
}



#endif /* _Stroika_Foundation_Containers_Iterable_inl_ */

