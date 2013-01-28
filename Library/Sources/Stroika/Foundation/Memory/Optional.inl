/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Optional_inl_
#define _Stroika_Foundation_Memory_Optional_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include   "../Debug/Assertions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


            /*
             ********************************************************************************
             ********************************** Optional<T> *********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  Optional<T>::Optional ()
                : fValue_ (nullptr)
            {
            }
            template    <typename T>
            inline  Optional<T>::Optional (const T& from)
                : fValue_ (new BlockAllocated<T> (from))
            {
            }
            template    <typename T>
            inline  Optional<T>::Optional (T && from)
                : fValue_ (new BlockAllocated<T> (std::move (from)))
            {
            }
            template    <typename T>
            inline  Optional<T>::Optional (const Optional<T>& from)
                : fValue_ (from.fValue_ == nullptr ? nullptr : new BlockAllocated<T> (*from.fValue_))
            {
            }
            template    <typename T>
            inline  Optional<T>::Optional (Optional<T> && from)
                : fValue_ (from.fValue_)
            {
                from.fValue_ = nullptr;
            }
            template    <typename T>
            inline  Optional<T>::~Optional ()
            {
                delete fValue_;
            }
            template    <typename T>
            inline  Optional<T>&   Optional<T>::operator= (const T& from)
            {
                delete fValue_;
                fValue_ = nullptr;
                fValue_ = new BlockAllocated<T> (from);
                return *this;
            }
            template    <typename T>
            inline  Optional<T>&   Optional<T>::operator= (T && from)
            {
                fValue_ = new BlockAllocated<T> (std::move (from));
                return *this;
            }
            template    <typename T>
            inline  Optional<T>&   Optional<T>::operator= (const Optional<T>& from)
            {
                delete fValue_;
                fValue_ = nullptr;
                if (from.fValue_ != nullptr) {
                    fValue_ = new BlockAllocated<T> (*from.fValue_);
                }
                return *this;
            }
            template    <typename T>
            inline  Optional<T>&   Optional<T>::operator= (Optional<T> && from)
            {
                fValue_ = from.fValue_;
                from.fValue_ = nullptr;
                return *this;
            }
            template    <typename T>
            inline  void    Optional<T>::clear ()
            {
                delete fValue_;
                fValue_ = nullptr;
            }
            template    <typename T>
            inline  const T*    Optional<T>::get () const
            {
                return fValue_;
            }
            template    <typename T>
            inline  bool    Optional<T>::empty () const
            {
                return fValue_ == nullptr;
            }
            template    <typename T>
            inline  const T* Optional<T>::operator-> () const
            {
                Require (not empty ())
                return fValue_;
            }
            template    <typename T>
            inline  T* Optional<T>::operator-> ()
            {
                Require (not empty ())
                return fValue_;
            }
            template    <typename T>
            inline  const T& Optional<T>::operator* () const
            {
                Require (not empty ())
                return *fValue_;
            }
            template    <typename T>
            inline  T& Optional<T>::operator* ()
            {
                Require (not empty ())
                return *fValue_->get ();
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
                if (fValue_ == nullptr) {
                    return (rhs.fValue_ == nullptr) ? false : true; // arbitrary choice - but assume if lhs is empty thats less than any T value
                }
                if (rhs.fValue_ == nullptr) {
                    return false;
                }
                return *fValue_ < *rhs.fValue_;
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
                if (fValue_ == nullptr) {
                    return rhs.fValue_ == nullptr;
                }
                if (rhs.fValue_ == nullptr) {
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
