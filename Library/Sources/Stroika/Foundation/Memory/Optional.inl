/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Optional_inl_
#define _Stroika_Foundation_Memory_Optional_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"
#include    "../Execution/Common.h"

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
             **************************** Optional<T, TRAITS> *******************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline
#if     !qCompilerAndStdLib_constexpr_Buggy
            constexpr
#endif
            Optional<T, TRAITS>::Optional ()
                : fValue_ (nullptr)
            {
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional (const T& from)
                : fValue_ (new AutomaticallyBlockAllocated<T> (from))
            {
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional (T&& from)
                : fValue_ (new AutomaticallyBlockAllocated<T> (std::move (from)))
            {
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional (const Optional<T, TRAITS>& from)
                : fValue_ (from.fValue_ == nullptr ? nullptr : new AutomaticallyBlockAllocated<T> (*from))
            {
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional (Optional<T, TRAITS>&& from)
                : fValue_ (from.fValue_)
            {
                from.fValue_ = nullptr;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::Optional (const T* from)
                : fValue_ (from == nullptr ? nullptr : new AutomaticallyBlockAllocated<T> (*from))
            {
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>::~Optional ()
            {
                delete fValue_;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (const T& rhs)
            {
                if (fValue_->get () == &rhs) {
                    // No need to copy in this case and would be bad to try
                    //  Optional<T, TRAITS> x;
                    //  x = *x;
                }
                else {
                    if (fValue_ == nullptr) {
                        fValue_ = new AutomaticallyBlockAllocated<T> (rhs);
                    }
                    else {
                        *fValue_ = rhs;
                    }
                }
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (T && rhs)
            {
                if (fValue_->get () == &rhs) {
                    // No need to copy in this case and would be bad to try
                    //  Optional<T, TRAITS> x;
                    //  x = *x;
                }
                else {
                    if (fValue_ == nullptr) {
                        fValue_ = new AutomaticallyBlockAllocated<T> (std::move (rhs));
                    }
                    else {
                        *fValue_ = std::move (rhs);
                    }
                }
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (const Optional<T, TRAITS>& rhs)
            {
                if (fValue_->get () != rhs.fValue_->get ()) {
                    delete fValue_;
                    fValue_ = nullptr;
                    if (rhs.fValue_ != nullptr) {
                        fValue_ = new AutomaticallyBlockAllocated<T> (*rhs);
                    }
                }
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (Optional<T, TRAITS> && rhs)
            {
                if (fValue_->get () != rhs.fValue_->get ()) {
                    delete fValue_;
                    fValue_ = rhs.fValue_;
                    rhs.fValue_ = nullptr;
                }
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&   Optional<T, TRAITS>::operator= (const T* rhs)
            {
                if (fValue_->get () != rhs) {
                    delete fValue_;
                    fValue_ = nullptr;
                    if (rhs != nullptr) {
                        fValue_ = new AutomaticallyBlockAllocated<T> (*rhs);
                    }
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
            inline  constexpr   bool    Optional<T, TRAITS>::IsMissing () const
            {
                return fValue_ == nullptr;
            }
            template    <typename T, typename TRAITS>
            inline  constexpr   bool    Optional<T, TRAITS>::IsPresent () const
            {
                return fValue_ != nullptr;
            }
            template    <typename T, typename TRAITS>
            inline  T Optional<T, TRAITS>::Value (T defaultValue) const
            {
                return IsMissing () ? defaultValue : *fValue_->get ();
            }
            template    <typename T, typename TRAITS>
            template    <typename   CONVERTABLE_TO_TYPE>
            nonvirtual  void    Optional<T, TRAITS>::AssignIf (CONVERTABLE_TO_TYPE* to) const
            {
                RequireNotNull (to);
                if (IsPresent ()) {
                    *to = *fValue_->get ();
                }
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
            inline  Optional<T, TRAITS>&    Optional<T, TRAITS>::operator+= (const T& rhs)
            {
                (*fValue_->get ()) += rhs;
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&    Optional<T, TRAITS>::operator-= (const T& rhs)
            {
                (*fValue_->get ()) -= rhs;
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&    Optional<T, TRAITS>::operator*= (const T& rhs)
            {
                (*fValue_->get ()) *= rhs;
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Optional<T, TRAITS>&    Optional<T, TRAITS>::operator/= (const T& rhs)
            {
                (*fValue_->get ()) /= rhs;
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  int Optional<T, TRAITS>::Compare (const Optional<T, TRAITS>& rhs) const
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
            template    <typename   RHS_CONVERTIBLE_TO_OPTIONAL_OF_T_TRAITS>
            inline  int Optional<T, TRAITS>::Compare (const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T_TRAITS& rhs) const
            {
                return Compare (static_cast<Optional<T, TRAITS>> (rhs));
            }
            template    <typename T, typename TRAITS>
            template    <typename   RHS_CONVERTIBLE_TO_OPTIONAL_OF_T_TRAITS>
            inline  bool    Optional<T, TRAITS>::operator< (const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T_TRAITS& rhs) const
            {
                return Compare (rhs) < 0;
            }
            template    <typename T, typename TRAITS>
            template    <typename   RHS_CONVERTIBLE_TO_OPTIONAL_OF_T_TRAITS>
            inline  bool    Optional<T, TRAITS>::operator<= (const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T_TRAITS& rhs) const
            {
                return Compare (rhs) <= 0;
            }
            template    <typename T, typename TRAITS>
            template    <typename   RHS_CONVERTIBLE_TO_OPTIONAL_OF_T_TRAITS>
            inline  bool    Optional<T, TRAITS>::operator> (const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T_TRAITS& rhs) const
            {
                return Compare (rhs) > 0;
            }
            template    <typename T, typename TRAITS>
            template    <typename   RHS_CONVERTIBLE_TO_OPTIONAL_OF_T_TRAITS>
            inline  bool    Optional<T, TRAITS>::operator>= (const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T_TRAITS& rhs) const
            {
                return Compare (rhs) >= 0;
            }
            template    <typename T, typename TRAITS>
            template    <typename   RHS_CONVERTIBLE_TO_OPTIONAL_OF_T_TRAITS>
            inline  bool    Optional<T, TRAITS>::operator== (const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T_TRAITS& rhs) const
            {
                return Compare (rhs) == 0;
            }
            template    <typename T, typename TRAITS>
            template    <typename   RHS_CONVERTIBLE_TO_OPTIONAL_OF_T_TRAITS>
            nonvirtual  bool    Optional<T, TRAITS>::operator!= (const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T_TRAITS& rhs) const
            {
                return Compare (rhs) != 0;
            }


        }


        namespace   Execution {


            /*
             ********************************************************************************
             **************** Synchronized<Memory::Optional<T, TRAITS>> *********************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  Synchronized<Memory::Optional<T, TRAITS>>::Synchronized ()
                        : fDelegate_ ()
                        , fMutex_ ()
            {
            }
            template    <typename T, typename TRAITS>
            inline  Synchronized<Memory::Optional<T, TRAITS>>::Synchronized (const T& from)
                        : fDelegate_ (from)
                        , fMutex_ ()
            {
            }
            template    <typename T, typename TRAITS>
            inline  Synchronized<Memory::Optional<T, TRAITS>>::Synchronized (T&& from)
                        : fDelegate_ (move (from))
                        , fMutex_ ()
            {
            }
            template    <typename T, typename TRAITS>
            inline  Synchronized<Memory::Optional<T, TRAITS>>::Synchronized (const Synchronized<Memory::Optional<T, TRAITS>>& from)
                        : fDelegate_ ()
                        , fMutex_ ()
            {
                unique_lock<decltype (fMutex_)> l1 (fMutex_, std::defer_lock);
                unique_lock<decltype (fMutex_)> l2 (from.fMutex_, std::defer_lock);
                lock (l1, l2);
                fDelegate_ = from.fDelegate_;
            }
            template    <typename T, typename TRAITS>
            inline  Synchronized<Memory::Optional<T, TRAITS>>::Synchronized (Synchronized<Memory::Optional<T, TRAITS>>&& from)
                        : fDelegate_ ()
                        , fMutex_ ()
            {
                unique_lock<decltype (fMutex_)> l1 (fMutex_, std::defer_lock);
                unique_lock<decltype (fMutex_)> l2 (from.fMutex_, std::defer_lock);
                lock (l1, l2);
                fDelegate_ = move (from.fDelegate_);
            }
            template    <typename T, typename TRAITS>
            inline  Synchronized<Memory::Optional<T, TRAITS>>::Synchronized (const Memory::Optional<T, TRAITS>& from)
                        : fDelegate_ (from)
                        , fMutex_ ()
            {
            }
            template    <typename T, typename TRAITS>
            inline  Synchronized<Memory::Optional<T, TRAITS>>::Synchronized (Memory::Optional<T, TRAITS>&& from)
                        : fDelegate_ (move (from))
                        , fMutex_ ()
            {
                // We dont need to lock this cuz we are constructing. We assume we dont need to lock from because
                // its assumed std::move() - no lock is needed
            }
            template    <typename T, typename TRAITS>
            inline  Synchronized<Memory::Optional<T, TRAITS>>&   Synchronized<Memory::Optional<T, TRAITS>>::operator= (const T& rhs)
            {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                MACRO_LOCK_GUARD_CONTEXT (fMutex_);
#else
                auto    critSec { make_unique_lock (fMutex_) };
#endif
                fDelegate_ = rhs;
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Synchronized<Memory::Optional<T, TRAITS>>&   Synchronized<Memory::Optional<T, TRAITS>>::operator= (T && rhs)
            {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                MACRO_LOCK_GUARD_CONTEXT (fMutex_);
#else
                auto    critSec { make_unique_lock (fMutex_) };
#endif
                fDelegate_ = std::move (rhs);
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Synchronized<Memory::Optional<T, TRAITS>>&   Synchronized<Memory::Optional<T, TRAITS>>::operator= (const Synchronized<Memory::Optional<T, TRAITS>>& rhs)
            {
                unique_lock<decltype (fMutex_)> l1 (fMutex_, std::defer_lock);
                unique_lock<decltype (fMutex_)> l2 (rhs.fMutex_, std::defer_lock);
                lock (l1, l2);
                fDelegate_ = rhs;
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Synchronized<Memory::Optional<T, TRAITS>>&   Synchronized<Memory::Optional<T, TRAITS>>::operator= (Synchronized<Memory::Optional<T, TRAITS>> && rhs)
            {
                // We assume we dont need to lock from because its assumed std::move() - no lock is needed
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                MACRO_LOCK_GUARD_CONTEXT (fMutex_);
#else
                auto    critSec { make_unique_lock (fMutex_) };
#endif
                fDelegate_ = move (rhs);
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Synchronized<Memory::Optional<T, TRAITS>>&   Synchronized<Memory::Optional<T, TRAITS>>::operator= (const Memory::Optional<T, TRAITS>& rhs)
            {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                MACRO_LOCK_GUARD_CONTEXT (fMutex_);
#else
                auto    critSec { make_unique_lock (fMutex_) };
#endif
                fDelegate_ = rhs;
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  T   Synchronized<Memory::Optional<T, TRAITS>>::operator* () const
            {
                Require (IsPresent ());
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                MACRO_LOCK_GUARD_CONTEXT (fMutex_);
#else
                auto    critSec { make_unique_lock (fMutex_) };
#endif
                Require (fDelegate_.IsPresent ());
                return *fDelegate_;
            }
            template    <typename T, typename TRAITS>
            inline  Synchronized<Memory::Optional<T, TRAITS>>::operator Memory::Optional<T, TRAITS> () const
            {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                MACRO_LOCK_GUARD_CONTEXT (fMutex_);
#else
                auto    critSec { make_unique_lock (fMutex_) };
#endif
                return fDelegate_;
            }
            template    <typename T, typename TRAITS>
            inline  void    Synchronized<Memory::Optional<T, TRAITS>>::clear ()
            {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                MACRO_LOCK_GUARD_CONTEXT (fMutex_);
#else
                auto    critSec { make_unique_lock (fMutex_) };
#endif
                fDelegate_.clear ();
            }
            template    <typename T, typename TRAITS>
            inline  bool    Synchronized<Memory::Optional<T, TRAITS>>::IsMissing () const
            {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                MACRO_LOCK_GUARD_CONTEXT (fMutex_);
#else
                auto    critSec { make_unique_lock (fMutex_) };
#endif
                return fDelegate_.IsMissing ();
            }
            template    <typename T, typename TRAITS>
            inline  bool    Synchronized<Memory::Optional<T, TRAITS>>::IsPresent () const
            {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                MACRO_LOCK_GUARD_CONTEXT (fMutex_);
#else
                auto    critSec { make_unique_lock (fMutex_) };
#endif
                return fDelegate_.IsPresent ();
            }
            template    <typename T, typename TRAITS>
            inline  T Synchronized<Memory::Optional<T, TRAITS>>::Value (T defaultValue) const
            {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                MACRO_LOCK_GUARD_CONTEXT (fMutex_);
#else
                auto    critSec { make_unique_lock (fMutex_) };
#endif
                return fDelegate_.Value (defaultValue);
            }
            template    <typename T, typename TRAITS>
            template    <typename   CONVERTABLE_TO_TYPE>
            inline  void    Synchronized<Memory::Optional<T, TRAITS>>::AssignIf (CONVERTABLE_TO_TYPE* to) const
            {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                MACRO_LOCK_GUARD_CONTEXT (fMutex_);
#else
                auto    critSec { make_unique_lock (fMutex_) };
#endif
                return fDelegate_.AssignIf (to);
            }
            template    <typename T, typename TRAITS>
            inline  Synchronized<Memory::Optional<T, TRAITS>>&    Synchronized<Memory::Optional<T, TRAITS>>::operator+= (const T& rhs)
            {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                MACRO_LOCK_GUARD_CONTEXT (fMutex_);
#else
                auto    critSec { make_unique_lock (fMutex_) };
#endif
                fDelegate_ += rhs;
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Synchronized<Memory::Optional<T, TRAITS>>&    Synchronized<Memory::Optional<T, TRAITS>>::operator-= (const T& rhs)
            {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                MACRO_LOCK_GUARD_CONTEXT (fMutex_);
#else
                auto    critSec { make_unique_lock (fMutex_) };
#endif
                fDelegate_ -= rhs;
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Synchronized<Memory::Optional<T, TRAITS>>&    Synchronized<Memory::Optional<T, TRAITS>>::operator*= (const T& rhs)
            {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                MACRO_LOCK_GUARD_CONTEXT (fMutex_);
#else
                auto    critSec { make_unique_lock (fMutex_) };
#endif
                fDelegate_ *= (rhs);
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Synchronized<Memory::Optional<T, TRAITS>>&    Synchronized<Memory::Optional<T, TRAITS>>::operator/= (const T& rhs)
            {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                MACRO_LOCK_GUARD_CONTEXT (fMutex_);
#else
                auto    critSec { make_unique_lock (fMutex_) };
#endif
                fDelegate_ /= rhs;
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  int Synchronized<Memory::Optional<T, TRAITS>>::Compare (const T& rhs) const
            {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                MACRO_LOCK_GUARD_CONTEXT (fMutex_);
#else
                auto    critSec { make_unique_lock (fMutex_) };
#endif
                return fDelegate_.Compare (rhs);
            }
            template    <typename T, typename TRAITS>
            inline  int Synchronized<Memory::Optional<T, TRAITS>>::Compare (const Memory::Optional<T, TRAITS>& rhs) const
            {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                MACRO_LOCK_GUARD_CONTEXT (fMutex_);
#else
                auto    critSec { make_unique_lock (fMutex_) };
#endif
                return fDelegate_.Compare (rhs);
            }
            template    <typename T, typename TRAITS>
            inline  int Synchronized<Memory::Optional<T, TRAITS>>::Compare (const Synchronized<Memory::Optional<T, TRAITS>>& rhs) const
            {
                if (this == &rhs) {
                    return 0;   // avoid double mutex lock
                }
                unique_lock<decltype (fMutex_)> l1 (fMutex_, std::defer_lock);
                unique_lock<decltype (fMutex_)> l2 (rhs.fMutex_, std::defer_lock);
                lock (l1, l2);
                return fDelegate_.Compare (rhs.fDelegate_);
            }
            template    <typename T, typename TRAITS>
            inline  bool    Synchronized<Memory::Optional<T, TRAITS>>::operator< (const T& rhs) const
            {
                return Compare (rhs) < 0;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Synchronized<Memory::Optional<T, TRAITS>>::operator< (const Memory::Optional<T, TRAITS>& rhs) const
            {
                return Compare (rhs) < 0;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Synchronized<Memory::Optional<T, TRAITS>>::operator< (const Synchronized<Memory::Optional<T, TRAITS>>& rhs) const
            {
                return Compare (rhs) < 0;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Synchronized<Memory::Optional<T, TRAITS>>::operator<= (const T& rhs) const
            {
                return Compare (rhs) <= 0;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Synchronized<Memory::Optional<T, TRAITS>>::operator<= (const Memory::Optional<T, TRAITS>& rhs) const
            {
                return Compare (rhs) <= 0;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Synchronized<Memory::Optional<T, TRAITS>>::operator<= (const Synchronized<Memory::Optional<T, TRAITS>>& rhs) const
            {
                return Compare (rhs) <= 0;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Synchronized<Memory::Optional<T, TRAITS>>::operator> (const T& rhs) const
            {
                return Compare (rhs) > 0;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Synchronized<Memory::Optional<T, TRAITS>>::operator> (const Memory::Optional<T, TRAITS>& rhs) const
            {
                return Compare (rhs) > 0;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Synchronized<Memory::Optional<T, TRAITS>>::operator> (const Synchronized<Memory::Optional<T, TRAITS>>& rhs) const
            {
                return Compare (rhs) > 0;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Synchronized<Memory::Optional<T, TRAITS>>::operator>= (const T& rhs) const
            {
                return Compare (rhs) >= 0;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Synchronized<Memory::Optional<T, TRAITS>>::operator>= (const Memory::Optional<T, TRAITS>& rhs) const
            {
                return Compare (rhs) >= 0;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Synchronized<Memory::Optional<T, TRAITS>>::operator>= (const Synchronized<Memory::Optional<T, TRAITS>>& rhs) const
            {
                return Compare (rhs) >= 0;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Synchronized<Memory::Optional<T, TRAITS>>::operator== (const T& rhs) const
            {
                return Compare (rhs) == 0;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Synchronized<Memory::Optional<T, TRAITS>>::operator== (const Memory::Optional<T, TRAITS>& rhs) const
            {
                return Compare (rhs) == 0;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Synchronized<Memory::Optional<T, TRAITS>>::operator== (const Synchronized<Memory::Optional<T, TRAITS>>& rhs) const
            {
                return Compare (rhs) == 0;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Synchronized<Memory::Optional<T, TRAITS>>::operator!= (const T& rhs) const
            {
                return Compare (rhs) != 0;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Synchronized<Memory::Optional<T, TRAITS>>::operator!= (const Synchronized<Memory::Optional<T, TRAITS>>& rhs) const
            {
                return Compare (rhs) != 0;
            }
            template    <typename T, typename TRAITS>
            inline  bool    Synchronized<Memory::Optional<T, TRAITS>>::operator!= (const Memory::Optional<T, TRAITS>& rhs) const
            {
                return Compare (rhs) != 0;
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Memory_Optional_inl_*/
