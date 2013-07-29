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
             *********************** Optional_DefaultTraits<T> ******************************
             ********************************************************************************
             */
            template    <typename T>
            inline  int Optional_DefaultTraits<T>::Compare (T lhs, T rhs)
            {
                if (lhs < rhs) {
                    return -1;
                }
                else if (rhs < lhs) {
                    return 1;
                }
                Assert (lhs == rhs);
                return 0;
            }


            /*
             ********************************************************************************
             ********************************** Optional<T> *********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional ()
                : fValue_ (nullptr)
            {
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional (const T& from)
                : fValue_ (new BlockAllocated<T> (from))
            {
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional (T && from)
                : fValue_ (new BlockAllocated<T> (std::move (from)))
            {
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional (const Optional<T, TRAITS>& from)
                : fValue_ (from.fValue_ == nullptr ? nullptr : new BlockAllocated<T> (*from.fValue_))
            {
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional (Optional<T, TRAITS> && from)
                : fValue_ (from.fValue_)
            {
                from.fValue_ = nullptr;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::~Optional ()
            {
                delete fValue_;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (const T& from)
            {
                if (fValue_->get () == &from) {
                    // No need to copy in this case and would be bad to try
                    //  Optional<T> x;
                    //  x = *x;
                }
                else {
                    if (fValue_ == nullptr) {
                        fValue_ = new BlockAllocated<T> (from);
                    }
                    else {
                        *fValue_ = from;
                    }
                }
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (T && from)
            {
                if (fValue_->get () == &from) {
                    // No need to copy in this case and would be bad to try
                    //  Optional<T> x;
                    //  x = *x;
                }
                else {
                    if (fValue_ == nullptr) {
                        fValue_ = new BlockAllocated<T> (std::move (from));
                    }
                    else {
                        *fValue_ = std::move (from);
                    }
                }
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (const Optional<T, TRAITS>& from)
            {
                if (fValue_->get () != from.fValue_->get ()) {
                    delete fValue_;
                    fValue_ = nullptr;
                    if (from.fValue_ != nullptr) {
                        fValue_ = new BlockAllocated<T> (*from.fValue_);
                    }
                }
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (Optional<T, TRAITS> && from)
            {
                if (fValue_->get () != from.fValue_->get ()) {
                    delete fValue_;
                    fValue_ = from.fValue_;
                    from.fValue_ = nullptr;
                }
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  void    Optional<T, TRAITS>::clear ()
            {
                delete fValue_;
                fValue_ = nullptr;
            }
            template    <typename T, typename TRAITS>
            inline  T*    Optional<T, TRAITS>::get ()
            {
                return fValue_ == nullptr ? nullptr : fValue_->get ();
            }
            template    <typename T, typename TRAITS>
            inline  const T*    Optional<T, TRAITS>::get () const
            {
                return fValue_ == nullptr ? nullptr : fValue_->get ();
            }
            template    <typename T, typename TRAITS>
            inline  bool    Optional<T, TRAITS>::empty () const
            {
                return fValue_ == nullptr;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Optional<T, TRAITS>::IsPresent () const
            {
                return fValue_ != nullptr;
            }
            template    <typename T, typename TRAITS>
            inline  T Optional<T, TRAITS>::Value (T defaultValue) const
            {
                return empty () ? defaultValue : *fValue_->get ();
            }
            template    <typename T, typename TRAITS>
            inline  const T* Optional<T, TRAITS>::operator-> () const
            {
                Require (IsPresent ());
                AssertNotNull (fValue_);
                EnsureNotNull (fValue_->get ());
                return fValue_->get ();
            }
            template    <typename T, typename TRAITS>
            inline  T* Optional<T, TRAITS>::operator-> ()
            {
                Require (IsPresent ());
                AssertNotNull (fValue_);
                EnsureNotNull (fValue_->get ());
                return fValue_->get ();
            }
            template    <typename T, typename TRAITS>
            inline  const T& Optional<T, TRAITS>::operator* () const
            {
                Require (IsPresent ());
                AssertNotNull (fValue_);
                EnsureNotNull (fValue_->get ());
                return *fValue_->get ();
            }
            template    <typename T, typename TRAITS>
            inline  T& Optional<T, TRAITS>::operator* ()
            {
                Require (IsPresent ());
                AssertNotNull (fValue_);
                EnsureNotNull (fValue_->get ());
                return *fValue_->get ();
            }
            template    <typename T, typename TRAITS>
            inline  int Optional<T, TRAITS>::Compare (const Optional<T>& rhs) const
            {
                if (fValue_ == nullptr) {
                    return (rhs.fValue_ == nullptr) ? 0 : 1; // arbitrary choice - but assume if lhs is empty thats less than any T value
                }
                if (rhs.fValue_ == nullptr) {
                    AssertNotNull (fValue_);
                    return -1;
                }
                AssertNotNull (fValue_);
                AssertNotNull (rhs.fValue_);
                return TRAITS::Compare (static_cast<T> (*fValue_), static_cast<T> (*rhs.fValue_));
            }
            template    <typename T, typename TRAITS>
            inline  bool    Optional<T, TRAITS>::operator< (const Optional<T, TRAITS>& rhs) const
            {
                return Compare (rhs) < 0;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Optional<T, TRAITS>::operator<= (const Optional<T, TRAITS>& rhs) const
            {
                return Compare (rhs) <= 0;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Optional<T, TRAITS>::operator> (const Optional<T, TRAITS>& rhs) const
            {
                return Compare (rhs) > 0;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Optional<T, TRAITS>::operator>= (const Optional<T, TRAITS>& rhs) const
            {
                return Compare (rhs) >= 0;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Optional<T, TRAITS>::operator== (const Optional<T, TRAITS>& rhs) const
            {
                return Compare (rhs) == 0;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Optional<T, TRAITS>::operator!= (const Optional<T, TRAITS>& rhs) const
            {
                return Compare (rhs) != 0;
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Memory_Optional_inl_*/
