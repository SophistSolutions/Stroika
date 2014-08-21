/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Optional_inl_
#define _Stroika_Foundation_Containers_Optional_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    <mutex>

#include   "../Debug/Assertions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


#if 0

            /*
             ********************************************************************************
             **************************** Optional<T, TRAITS> *******************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional ()
                : fMutex_ ()
                , fValue_ ()
            {
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional (const T& from)
                : fMutex_ ()
                , fValue_ (from)
            {
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional (T&& from)
                : fMutex_ ()
                , fValue_ (std::move (from))
            {
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional (const Optional<T, TRAITS>& from)
                : fMutex_ ()
                , fValue_ ()
            {
                unique_lock<std::mutex> l1 (fMutex_, std::defer_lock);
                unique_lock<std::mutex> l2 (from.fMutex_, std::defer_lock);
                lock (l1, l2);
                fValue_ = from.fValue_;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional (Optional<T, TRAITS>&& from)
                : fMutex_ ()
                , fValue_ (std::move (from.fValue_))
            {
                // We dont need to lock this cuz we are constructing. We assume we dont need to lock from because
                // its assumed std::move() - no lock is needed
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional (const Memory::Optional<T, TRAITS>& from)
                : fMutex_ ()
                , fValue_ (from)
            {
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (const T& rhs)
            {
                lock_guard<mutex>   critSec (fMutex_);
                fValue_ = rhs;
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (T && rhs)
            {
                lock_guard<mutex>   critSec (fMutex_);
                fValue_ = std::move (rhs);
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (const Optional<T, TRAITS>& rhs)
            {
                unique_lock<std::mutex> l1 (fMutex_, std::defer_lock);
                unique_lock<std::mutex> l2 (rhs.fMutex_, std::defer_lock);
                lock (l1, l2);
                fValue_ = rhs.fValue_;
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (Optional<T, TRAITS> && rhs)
            {
                // We assume we dont need to lock from because its assumed std::move() - no lock is needed
                lock_guard<mutex>   critSec (fMutex_);
                fValue_ = std::move (rhs.fValue_);
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (const Memory::Optional<T, TRAITS>& rhs)
            {
                lock_guard<mutex>   critSec (fMutex_);
                fValue_ = rhs;
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  T   Optional<T, TRAITS>::operator* () const
            {
                Require (IsPresent ());
                lock_guard<mutex>   critSec (fMutex_);
                Require (fValue_.IsPresent ());
                return *fValue_;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::operator Memory::Optional<T, TRAITS> () const
            {
                lock_guard<mutex>   critSec (fMutex_);
                return fValue_;
            }
            template    <typename T, typename TRAITS>
            inline  void    Optional<T, TRAITS>::clear ()
            {
                lock_guard<mutex>   critSec (fMutex_);
                fValue_.clear ();
            }
            template    <typename T, typename TRAITS>
            inline  bool    Optional<T, TRAITS>::IsMissing () const
            {
                lock_guard<mutex>   critSec (fMutex_);
                return fValue_.IsMissing ();
            }
            template    <typename T, typename TRAITS>
            inline  bool    Optional<T, TRAITS>::IsPresent () const
            {
                lock_guard<mutex>   critSec (fMutex_);
                return fValue_.IsPresent ();
            }
            template    <typename T, typename TRAITS>
            inline  T Optional<T, TRAITS>::Value (T defaultValue) const
            {
                lock_guard<mutex>   critSec (fMutex_);
                return fValue_.Value (defaultValue);
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&    Optional<T, TRAITS>::operator+= (const T& rhs)
            {
                lock_guard<mutex>   critSec (fMutex_);
                fValue_ += rhs;
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&    Optional<T, TRAITS>::operator-= (const T& rhs)
            {
                lock_guard<mutex>   critSec (fMutex_);
                fValue_ -= rhs;
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&    Optional<T, TRAITS>::operator*= (const T& rhs)
            {
                lock_guard<mutex>   critSec (fMutex_);
                fValue_ *= rhs;
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&    Optional<T, TRAITS>::operator/= (const T& rhs)
            {
                lock_guard<mutex>   critSec (fMutex_);
                fValue_ /= rhs;
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  int Optional<T, TRAITS>::Compare (const Optional<T, TRAITS>& rhs) const
            {
                unique_lock<std::mutex> l1 (fMutex_, std::defer_lock);
                unique_lock<std::mutex> l2 (rhs.fMutex_, std::defer_lock);
                lock (l1, l2);
                return fValue_.Compare (rhs.fValue_);
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
#endif

        }
    }
}
#endif  /*_Stroika_Foundation_Containers_Optional_inl_*/
