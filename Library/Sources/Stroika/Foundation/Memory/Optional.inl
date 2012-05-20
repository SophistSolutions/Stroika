/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Optional_inl_
#define _Stroika_Foundation_Memory_Optional_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
 #include	"../Debug/Assertions.h"
 
namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {

            //  class   Optional::Optional<T>
            template    <typename T>
            inline  Optional<T>::Optional ()
                : fValue_ (nullptr)
            {
            }
            template    <typename T>
            inline  Optional<T>::Optional (const T& from)
                : fValue_ (new T (from))
            {
            }
            template    <typename T>
            inline  Optional<T>::Optional (const Optional<T>& from)
                : fValue_ (from.fValue_)
            {
            }
            template    <typename T>
            inline  void    Optional<T>::clear ()
            {
                fValue_.clear ();
            }
            template    <typename T>
            inline  const T*    Optional<T>::get () const
            {
                return fValue_.get ();
            }
            template    <typename T>
            inline  bool    Optional<T>::empty () const
            {
                return fValue_.get () == nullptr;
            }
            template    <typename T>
            inline  const T* Optional<T>::operator-> () const
            {
                Require (not empty ())
                return fValue_.get ();
            }
            template    <typename T>
            inline  T* Optional<T>::operator-> ()
            {
                Require (not empty ())
                return fValue_.get ();
            }
            template    <typename T>
            inline  const T& Optional<T>::operator* () const
            {
                Require (not empty ())
                return *fValue_.get ();
            }
            template    <typename T>
            inline  T& Optional<T>::operator* ()
            {
                Require (not empty ())
                return *fValue_.get ();
            }
            template    <typename T>
            inline  Optional<T>::operator T () const
            {
                Require (not empty ())
                return *fValue_;
            }
            template    <typename T>
            bool    Optional<T>::operator< (const Optional<T>& rhs) const
            {
                if (fValue_.get () == nullptr) {
                    return (rhs.fValue_.get () == nullptr) ? false : true; // arbitrary choice - but assume if lhs is empty thats less than any T value
                }
                if (rhs.fValue_.get () == nullptr) {
                    return false;
                }
                return *fValue_ < rhs.fValue_;
            }
            template    <typename T>
            bool    Optional<T>::operator<= (const Optional<T>& rhs) const
            {
                return *this < rhs or * this == rhs;
            }
            template    <typename T>
            inline  bool    Optional<T>::operator> (const Optional<T>& rhs) const
            {
                return rhs < *this;
            }
            template    <typename T>
            bool    Optional<T>::operator>= (const Optional<T>& rhs) const
            {
                return *this > rhs or * this == rhs;
            }
            template    <typename T>
            bool    Optional<T>::operator== (const Optional<T>& rhs) const
            {
                if (fValue_.get () == nullptr) {
                    return rhs.fValue_.get () == nullptr;
                }
                if (rhs.fValue_.get () == nullptr) {
                    return false;
                }
                return *fValue_ == *rhs.fValue_;
            }
            template    <typename T>
            inline  bool    Optional<T>::operator!= (const Optional<T>& rhs) const
            {
                return not (*this == rhs);
            }

        }
    }
}
#endif  /*_Stroika_Foundation_Memory_Optional_inl_*/
